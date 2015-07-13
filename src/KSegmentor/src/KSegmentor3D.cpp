#include "KSegmentor3D.h"
#include <QDebug>
#include "llist.h"
#include "sfm_local_chanvese_mex.h"
#include "interactive_kurvolver.h"
#include "vtkImageData.h"
#include "KSandbox.h"
#include <omp.h>
#include <string>
#include <sstream>
#include <opencv2/highgui/highgui.hpp>
#include "vtkImageGaussianSmooth.h"
#include <cstdlib>
#include <ctime>
#include <cstring>

#define THREE_DIGITS(X)  std::setw(3) << std::setfill('0') << X
#define FIVE_DIGITS(X)   std::setw(5) << std::setfill('0') << X


using std::string;
using cv::Mat;

namespace vrcl
{

    double getDefaultDrawLabelMaxVal() { return 1000.0; }

    struct KSegmentorBase::SFM_vars
    {
        //formerly global variables, for energy3c.cpp
        double ain, aout, auser; // means
        double *pdfin, *pdfout, *pdfuser;
        long numdims;
        double engEval;
        bool UseInitContour;
        double *Ain, *Aout, *Sin, *Sout; //local means
    };

    KSegmentor3D* KSegmentor3D::CreateSegmentor(vtkImageData *image, vtkImageData *label, bool contInit)
    {
        KSegmentor3D* seg3DPointer = new KSegmentor3D;
        seg3DPointer->m_EnergyName = GetSupportedEnergyNames( )[1];
        seg3DPointer->InitializeVariables(seg3DPointer, image, label, contInit);

        if (contInit)
        { /** an input label was provided;
                       then, assume intent is for U initially large then */
            std::cout << "Initializing user input using label data" << std::endl;
            seg3DPointer->initializeUserInputImageWithContour( );
        }
        assert(0 < seg3DPointer->GetUmax( ));
        seg3DPointer->m_SFM_vars = std::shared_ptr<SFM_vars>(new SFM_vars);
        seg3DPointer->initializeData( );
        seg3DPointer->CreateLLs(seg3DPointer->LL3D);

        seg3DPointer->intializeLevelSet3D( );
        return seg3DPointer;
    }

    void KSegmentor3D::accumulateCurrentUserInput(double value, const unsigned int element,
        double weight /*=1.0 default */)
    {
        double Umax = 1.0;
        Umax = this->GetUmax( ); assert(Umax > 0);
        double Ustep = weight * (Umax) / 2.0;
        if (fabs(Ustep) < 0.01)
            assert(1);

        double user_input = -Ustep * (value > 0.5) +
            Ustep * (value <= 0.5);

        this->ptrU_t_Image[element] = user_input;

        if (ptrIntegral_Image[element] < -Umax) {
            ptrIntegral_Image[element] = -Umax;
        }
        else if (ptrIntegral_Image[element] > Umax) {
            ptrIntegral_Image[element] = Umax;
        }

        this->OnUserPaintsLabel( ); // Ivan: consider OnUserPaintsLabel the "on label changed" entry point
    }

    void KSegmentor3D::integrateUserInput( )
    {
        ptrIntegral_Image = static_cast<double*>(this->U_Integral_image->GetScalarPointer( ));
        ptrU_t_Image = static_cast<double*>(this->U_t_image->GetScalarPointer( ));

        int pos = 0;
        int Nelements = this->m_UpdateVector.size( );
        cout << " Integrating:  KSegmentor3D::integrateUserInput(), N= "
            << Nelements << endl;
        for (int element = 0; element < Nelements; element++)
        {
            pos = this->m_UpdateVector[element];
            this->ptrIntegral_Image[pos] += this->ptrU_t_Image[pos];
            this->ptrU_t_Image[pos] = 0;
        }
    }


    void KSegmentor3D::UpdateArraysAfterTransform( )
    {
        this->mdims = U_t_image->GetDimensions( );

        //Set dimensions
        dimz = (int)mdims[2];
        dimy = (int)mdims[1];
        dimx = (int)mdims[0];

        dims[2] = dimz;
        dims[1] = dimy;
        dims[0] = dimx;

        dims[3] = dims[0] * dims[1];
        dims[4] = dims[0] * dims[1] * dims[2];

        this->U_Integral_image->GetSpacing(m_Spacing_mm);

        cout << "UpdateArraysAfterTransform of KSegmentor3D with dims[0,1,2] = "
            << dimx << "," << dimy << "," << dimz << endl;

        // TODO: Ivan, does this need to change and read the input args?
        this->rad = 3.0 / std::max(m_Spacing_mm[0], m_Spacing_mm[1]); // about 3mm in physical units
        this->rad = std::min(7.0, this->rad); // force non-huge radius if the spacing is retarded
        this->rad = std::max(3.0, this->rad); // force non-tiny radius if the spacing is retarded

        this->initializeData( );

        this->CreateLLs(LL3D);

        this->intializeLevelSet3D( );
    }


    void KSegmentor3D::initializeData( )
    {
        imageVol->GetScalarRange(imgRange);
        labelVol->GetScalarRange(labelRange);
        if (abs(labelRange[1]) < 1e-3)
        { // empty label; so set the proper range
            labelRange[1] = getDefaultDrawLabelMaxVal( );
        }

        this->imgRange = imgRange;
        ptrCurrImage = static_cast<unsigned short*>(imageVol->GetScalarPointer( ));
        ptrCurrLabel = static_cast<unsigned short*>(labelVol->GetScalarPointer( ));

        long elemNum = 0;
        for (int k = 0; k <= dimx - 1; k++) {
            for (int j = 0; j <= dimy - 1; j++)  {
                for (int i = 0; i <= dimz - 1; i++, elemNum++) {
                    this->mask[elemNum] = (double)(0 < ptrCurrLabel[elemNum]);
                    this->img[elemNum] = (double)ptrCurrImage[elemNum];
                }
            }
        }
    }

    void KSegmentor3D::OnUserPaintsLabel( ) {
        this->prevSlice = -1;
    }

    namespace {

        std::vector<double> cache_phi(0);

        std::vector<int>    save_count_per_slice(0); // store index for sequential saves of a given slice #

    }

    void KSegmentor3D::VerboseSaveToPng(cv::Mat tmp, cv::Mat tmpI)
    {
      bool bSavePNG = false;
      if (bSavePNG) {
          std::stringstream ss, ssI;
          ss << "label_slice_" << THREE_DIGITS(currSlice) << "_"
              << THREE_DIGITS(save_count_per_slice[currSlice]) << "_"
              << FIVE_DIGITS(num_actuated_voxels) << ".png";
          ssI << "image_slice_" << THREE_DIGITS(currSlice) << ".png";
          static std::vector<cv::Mat> chans(3); static cv::Mat tmpPng;
          cv::flip(tmp.clone( ), tmp, 0); double dmin, dmax; cv::minMaxLoc(tmp, &dmin, &dmax);
          cout << "min_label=" << dmin << ", max_label=" << dmax << ";  ";
          for (int c = 0; c < 3; c++){ tmp.convertTo(chans[c], CV_8UC1); }
          cv::merge(chans, tmpPng);
          cout << "write to " << ss.str( ) << " result: " << cv::imwrite(ss.str( ), tmpPng) << endl;
          cv::flip(tmpI.clone( ), tmpI, 0); cv::minMaxLoc(tmpI, &dmin, &dmax);
          cout << "min_image=" << dmin << ", max_image=" << dmax << ";  ";
          for (int c = 0; c < 3; c++){ tmpI.convertTo(chans[c], CV_8UC1); }
          cv::merge(chans, tmpPng);
          cout << "write to " << ssI.str( ) << " result: " << cv::imwrite(ssI.str( ), tmpPng) << endl;

          save_count_per_slice[currSlice] += 1;
          //saveMatToPNG( tmp, ss.str() );
      }
    }

    void KSegmentor3D::Update2D( )
    {
        this->integrateUserInput( );
        this->CreateLLs(LL2D);

        ptrCurrImage = static_cast<unsigned short*>(imageVol->GetScalarPointer( ));
        ptrCurrLabel = static_cast<unsigned short*>(labelVol->GetScalarPointer( ));
        ptrIntegral_Image = static_cast<double*>(this->U_Integral_image->GetScalarPointer( ));

        size_t sz = mdims[0] * mdims[1];
        if (cache_phi.size( ) != (size_t)sz)
            cache_phi.resize(sz);

        if (save_count_per_slice.size( ) != mdims[2]) {
            save_count_per_slice = std::vector<int>(mdims[2], 0);
        }

        double* phiSlice = new double[mdims[0] * mdims[1]];
        double* imgSlice = new double[mdims[0] * mdims[1]];
        double* maskSlice = new double[mdims[0] * mdims[1]];
        double* U_I_slice = new double[mdims[0] * mdims[1]];
        double* labelSlice = new double[mdims[0] * mdims[1]];

        unsigned int element3D;
        long elemNum = 0;
        long maskThresh = 0.5*labelRange[1];
        double maxU = -1e99;
        double minU = 1e99;

        double imgMinValue = this->m_SatRange[0];
        double imgMaxValue = this->m_SatRange[1];
        PRINT_AND_EVAL( imgMinValue << imgMaxValue );

        for (int j = 0; j <= mdims[1] - 1; j++)  {
            for (int i = 0; i <= mdims[0] - 1; i++) {
                // indexing definition:  ptr[k*mdims[1]*mdims[0] +j*mdims[0]+i];
                element3D = this->currSlice*mdims[1] * mdims[0] + j*mdims[0] + i;
                imgSlice[elemNum] = (double)ptrCurrImage[element3D];
                bool labelOK       = (maskThresh  < ptrCurrLabel[element3D]);
                bool imageOK       = (imgMinValue < ptrCurrImage[element3D]);
                maskSlice[elemNum] = (double) (labelOK && imageOK);

                U_I_slice[elemNum] = (double)ptrIntegral_Image[element3D];
                maxU = std::max( maxU, U_I_slice[elemNum]);
                minU = std::min( minU, U_I_slice[elemNum]);

                elemNum++;
            }
        }
        cout << "minU = " << maxU << ", " << "maxU = " << maxU << endl;

        std::vector<long> dimsSlice(5);
        dimsSlice[0] = mdims[0];
        dimsSlice[1] = mdims[1];
        dimsSlice[2] = 1;
        dimsSlice[3] = dimsSlice[0] * dimsSlice[1];
        dimsSlice[4] = dimsSlice[0] * dimsSlice[1] * dimsSlice[2];

        ls_mask2phi3c(maskSlice, phiSlice, labelSlice, &(dimsSlice[0]),
            LL2D.Lz, LL2D.Ln1, LL2D.Ln2, LL2D.Lp1, LL2D.Lp2);

        cout << "prevslice=" << prevSlice << ", " << "currslice= " << currSlice << endl;
        if ((prevSlice == this->currSlice)) {
            //    cout <<  "\033[01;32m\033]"
            //         << "using cached phi " << "\033[00m\033]" << endl;
            std::memcpy(&(phiSlice[0]), &(cache_phi[0]), sizeof(double)*mdims[0] * mdims[1]);
        }
        else {
            //    cout <<  "\033[01;42m\033]"
            //         << "first time on slice! " << "\033[00m\033]" << endl;
        }
        prevSlice = currSlice;


        interactive_rbchanvese(imgSlice, phiSlice, U_I_slice, labelSlice, &(dimsSlice[0]),
            LL2D.Lz, LL2D.Ln1, LL2D.Lp1, LL2D.Ln2, LL2D.Lp2, LL2D.Lin2out, LL2D.Lout2in,
            iter, rad, lambda*0.5, display);

        //threshold phi to find segmentation label, assign it to appropriate range of label!
        elemNum = 0;
        cv::Mat tmp(mdims[1], mdims[0], CV_64FC1);
        cv::Mat tmpI(mdims[1], mdims[0], CV_64FC1);
        for (int j = 0; j <= mdims[1] - 1; j++)  {
            for (int i = 0; i <= mdims[0] - 1; i++) {
                double phi_val = phiSlice[elemNum];
                double phi_out = (-phi_val + 3.0) / 6.0;/*6*/; // shift and scale from [-3,3] to [0,1]

                element3D = this->currSlice*mdims[1] * mdims[0] + j*mdims[0] + i;

                unsigned short value_IK = ((unsigned short)0 >= phi_val)*labelRange[1];   // IK: if phi=0, make label 0, else it's 1
                unsigned short value_PK = ((unsigned short)(((phi_out > 0.95)
                    + (phi_out > 0.8) + (phi_out > 0.65)
                    + (phi_out > 0.5)) * labelRange[1] / 4.0));

                bool imageOK = (imgMinValue < ptrCurrImage[element3D]);
                value_IK *= imageOK;
                value_PK *= imageOK;

                ptrCurrLabel[element3D] = value_IK;//value_PK;
                tmp.at<double>(j, i) = (phi_val <= 0.0)*255.0f;
                tmpI.at<double>(j, i) = (imgSlice[elemNum]);
                labelSlice[elemNum] = (double)value_IK;
                if ((value_PK == 0) && !(0 == value_IK)) {
                    if (value_IK != value_PK){
                        cout << "IK!=PK;  IK = " << value_IK << ", PK = " << value_PK << endl;
                    }
                }
                elemNum++;
            }
        }

        VerboseSaveToPng(tmp, tmpI);

        cout << string("update2D in ") + string(__FILE__) + ":   Lz size: " << LL2D.Lz->length << endl;

        // store cached \phi level set func
        std::memcpy(&(cache_phi[0]), &(phiSlice[0]), sizeof(double)*sz);

        delete[] imgSlice;
        delete[] labelSlice;
        delete[] maskSlice;
        delete[] phiSlice;
        delete[] U_I_slice;
        m_UpdateVector.clear( );
        m_CoordinatesVector.clear( );
    }

    void KSegmentor3D::Update3D( )
    {
        bool bSmoothU = false;
        if (bSmoothU) {
            cout << "smoothing integral image 3D " << endl;
            vtkSmartPointer<vtkImageGaussianSmooth> imageSmoother = vtkSmartPointer<vtkImageGaussianSmooth>::New( );
            imageSmoother->SetDimensionality(3);
            imageSmoother->SetStandardDeviations(1.0, 1.0, 1.0);
            imageSmoother->SetInput(this->U_Integral_image);
            imageSmoother->Update( );
            this->U_Integral_image->DeepCopy(imageSmoother->GetOutput( ));
        }

        /** PK:
                        Large commented blocks around Lchanged, lsmask2phi3c_ext:
                        april 2012 we wrote these codes that mostly work, except that the lists are really *sets* and will accumulate
                        repeat entries as written. Ran out of time and commented out; not clear if this is a bottle-neck or not. (10 jan 2013)
                        */

        cout << "integrating mask 3D " << endl;
        this->integrateUserInput( );

        cout << "updating mask 3D " << endl;
        this->UpdateMask(true);

        this->CreateLLs(LL3D); // TODO: fix the caching so that only uniques are kept

        ll_init(LL3D.Lchanged);
        //
        // TODO: need this?
        //        std::set<unsigned int> idx_used;
        //        int Nelements=this->m_UpdateVector.size();
        //        for (int element=0;element<Nelements;element++)
        //        {
        //            unsigned int idx=this->m_UpdateVector[element];
        //            int found       = std::count( idx_used.begin(), idx_used.end(), idx );
        //            if( 0 < found ) {
        //                ll_pushnew(Lchanged,m_CoordinatesVector[element][0],m_CoordinatesVector[element][1],
        //                                                           m_CoordinatesVector[element][2],idx);
        //                idx_used.insert(idx);
        //            }
        //        }
        cout << "m_UpdateVector Size BEGIN: " << m_UpdateVector.size( )
            << ", Lchanged size BEGIN: " << LL3D.Lchanged->length
            << ", Lz size BEGIN: " << LL3D.Lz->length << endl;

        ptrCurrImage = static_cast<unsigned short*>(imageVol->GetScalarPointer( ));
        ptrCurrLabel = static_cast<unsigned short*>(labelVol->GetScalarPointer( ));
        ptrIntegral_Image = static_cast<double*>(this->U_Integral_image->GetScalarPointer( ));
        double u_in, u_out;
        // Crap, the linked lists will accumulate zillions of repeating entries ...
        // Must flush them and keep unique entries, or something (?)
        //if(this->m_UpdateVector.size()!=0)
        //        ls_mask2phi3c_update(this->m_UpdateVector,
        //                            this->m_CoordinatesVector,mask,phi,label,dims,
        //                            LL3D.Lz,LL3D.Ln1,LL3D.Ln2,LL3D.Lp1,LL3D.Lp2,LL3D.Lchanged);

        ls_mask2phi3c(mask, phi, label, dims, LL3D.Lz, LL3D.Ln1, LL3D.Ln2, LL3D.Lp1, LL3D.Lp2);
        //   ls_mask2phi3c_ext(mask,phi,label,dims,LL3D.Lz,LL3D.Ln1,LL3D.Ln2,LL3D.Lp1,LL3D.Lp2,LL3D.Lchanged);
        cout << "m_UpdateVector Size: " << m_UpdateVector.size( )
            << " m_CoordinatesVector Size: " << this->m_CoordinatesVector.size( )
            << ", Lchanged size: " << LL3D.Lchanged->length
            << ", Lz size: " << LL3D.Lz->length << endl;

        assert(m_PlaneNormalVector && m_PlaneCenter);

        if (0 == m_EnergyName.compare("ChanVese")) {
            interactive_chanvese_ext(img, phi, ptrIntegral_Image, label, dims,
                LL3D.Lz, LL3D.Ln1, LL3D.Lp1, LL3D.Ln2, LL3D.Lp2, LL3D.Lin2out, LL3D.Lout2in, LL3D.Lchanged,
                iter, 0.5*lambda, display, this->m_PlaneNormalVector,
                this->m_PlaneCenter, this->m_DistWeight);
            bool bDisplayChanVeseCost = false;
            if (bDisplayChanVeseCost) {
                double cv_cost = this->evalChanVeseCost(u_in, u_out);
                cout << "chan vese cost = " << cv_cost << endl;
            }
        }
        else if (0 == m_EnergyName.compare("LocalCV"))
            interactive_rbchanvese_ext(img, phi, ptrIntegral_Image, label, dims,
            LL3D.Lz, LL3D.Ln1, LL3D.Lp1, LL3D.Ln2, LL3D.Lp2, LL3D.Lin2out, LL3D.Lout2in, LL3D.Lchanged,
            iter, rad, lambda, display, this->m_PlaneNormalVector,
            this->m_PlaneCenter, this->m_DistWeight);
        else
            cout << "Error, unsupported energy name! " << m_EnergyName << endl;


        this->evalChanVeseCost(u_in, u_out);
        double cutoff_thresh = (u_in - u_out)*this->m_ThreshWeight;
        cout << "uin, uout, cutoff=" << u_in << ", " << u_out << ", " << cutoff_thresh << endl;

        //double mult=labelRange[1] / 4.0;

        double phi_val = 0;
        double phi_out = 0;

        // Caution: Lchanged can contain duplicate entries !!!
        ll_init(LL3D.Lchanged);

        double changeInLabel = 0;
        int Nelements = this->dimx * this->dimy * this->dimz;
        for (int idx = 0; idx < Nelements; idx++) {
            phi_val = phi[idx];
            if ((u_out < u_in) && (img[idx] < (u_out + 2 * cutoff_thresh)))
            { // force it to be out
                phi_val = 3.0;
            }
            else if ((u_out < u_in) && (img[idx] < (u_out + cutoff_thresh)))
            { // force it to be out
                phi_val = std::min(phi_val, 0.0);
            }
            phi_out = (-phi_val + 3.0) / 6.0;
            unsigned short value_IK = ((unsigned short)0 >= phi_val)*labelRange[1];   // IK: if phi=0, make label 0, else it's 1
            unsigned short value_PK = ((unsigned short)(((phi_out > 0.95)
                + (phi_out > 0.8) + (phi_out > 0.65)
                + (phi_out > 0.5)) * labelRange[1] / 4.0));
            unsigned short outputVal = 1*value_IK + 0 * value_PK;
            // Argh, fine use the "bug fix/workaround", but nice to make
            // value_PK work to enable (1) accurate volume computation and  (2) better mesh model generation
            //       i.e. keep a range of values for phi instead of truncating like this
            changeInLabel += (fabs((double)((outputVal - ptrCurrLabel[idx]))) > 1e-3);  // fixed?? TODO: Isn't this a math typo ??
            ptrCurrLabel[idx] = outputVal;
            mask[idx] = (double)(0 < outputVal); // d'oh, *update the mask!*
        }
        cout << "m_UpdateVector Size: " << m_UpdateVector.size( )
            << ", Lchanged size: " << LL3D.Lchanged->length
            << ", |change| in label: " << changeInLabel << endl;

        double spc[3];
        this->U_Integral_image->GetSpacing(spc);

        m_UpdateVector.clear( );
        m_CoordinatesVector.clear( );
    }


    KSegmentor3D::~KSegmentor3D( ){

        //delete [] this->mdims;//Causes trouble! Haveto find out why!!
        delete[] this->img;
        delete[] this->mask;
        delete[] this->imgRange;
        delete[] this->labelRange;
        delete[] this->phi;
        delete[] this->label;

        LL *Lz, *Ln1, *Ln2, *Lp1, *Lp2;
        LL *Lin2out, *Lout2in, *Lchanged;


        Lz = LL3D.Lz;
        Ln1 = LL3D.Ln1;
        Ln2 = LL3D.Ln2;
        Lp1 = LL3D.Lp1;
        Lp2 = LL3D.Lp2;
        Lin2out = LL3D.Lin2out;
        Lout2in = LL3D.Lout2in;
        Lchanged = LL3D.Lchanged;

        ll_destroy(Lz);
        ll_destroy(Ln1);
        ll_destroy(Ln2);
        ll_destroy(Lp1);
        ll_destroy(Lp2);
        ll_destroy(Lin2out);
        ll_destroy(Lout2in);
        ll_destroy(Lchanged);

        Lz = LL2D.Lz;
        Ln1 = LL2D.Ln1;
        Ln2 = LL2D.Ln2;
        Lp1 = LL2D.Lp1;
        Lp2 = LL2D.Lp2;
        Lin2out = LL2D.Lin2out;
        Lout2in = LL2D.Lout2in;
        Lchanged = LL2D.Lchanged;

        ll_destroy(Lz);
        ll_destroy(Ln1);
        ll_destroy(Ln2);
        ll_destroy(Lp1);
        ll_destroy(Lp2);
        ll_destroy(Lin2out);
        ll_destroy(Lout2in);
        ll_destroy(Lchanged);
    }

}





//////////////////////////////////////////////////////////// Gulag

#if 0
if (m_CustomSpeedImgPointer!=NULL)
{
    /* interactive_customspeed(this->m_CustomSpeedImgPointer,img,phi,ptrIntegral_Image,label,dims,
                              Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in,Lchanged,
                              iter,rad,lambda,display,this->m_PlaneNormalVector,this->m_PlaneCenter,this->m_DistWeight);*/
}
else if( m_bUseEdgeBased ) {
    interactive_edgebased_ext(img,phi,ptrIntegral_Image,label,dims,
        Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in,Lchanged,
        iter,rad,0.5*lambda,display,m_SatRange[0],m_SatRange[1],
        this->m_PlaneNormalVector,this->m_PlaneCenter,this->m_DistWeight);
}

/*vtkMetaImageWriter* labelWriter=  vtkMetaImageWriter::New();
labelWriter->SetInput(createVTKImageFromPointer<double>(this->ptrIntegral_Image,this->U_Integral_image->GetDimensions(), spc) );
labelWriter->SetFileName( "0-Integral.mhd");
labelWriter->Write();

labelWriter->SetInput(this->U_Integral_image );
labelWriter->SetFileName( "0-IntegralImage.mhd");
labelWriter->Write();

labelWriter->SetInput(createVTKImageFromPointer<double>(this->ptrU_t_Image,this->U_Integral_image->GetDimensions(), spc) );
labelWriter->SetFileName( "0-U_t.mhd");
labelWriter->Write();


labelWriter->SetInput(this->U_t_image);
labelWriter->SetFileName( "0-U_t-image.mhd");
labelWriter->Write();*/


if ( writePhi ){
    std::ofstream phiRec("phiRec.txt",std::ios_base::out);
    for(int i=0;i< elemNum; i++){
        phiRec<<ptrCurrLabel[element3D=this->currSlice*mdims[1]*mdims[0] + i]<<' ';
    }
    phiRec.close();
}

bool bSavePNG = false;
if( bSavePNG ) {
    std::stringstream ss;
    ss << "U_integral_ " << std::setw(3) << std::setfill('0') << currSlice << ".png";
    saveMatToPNG( U_I_slice, ss.str() );
}
#endif
