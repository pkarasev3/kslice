#include "KSegmentor3D.h"
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


#include <ctime>

using std::string;
using cv::Mat;

//these global variables are no good, need to fix later
extern double ain, aout, auser; // means
extern double *pdfin, *pdfout, *pdfuser;
extern long numdims;
extern double engEval;
extern bool UseInitContour;
extern double *Ain, *Aout, *Sin, *Sout; //local means


KSegmentor3D::KSegmentor3D(vtkImageData* image, vtkImageData* label, KViewerOptions* ksliceOptions){

    bool contInit = ksliceOptions->contInit;

    m_EnergyName = GetSupportedEnergyNames()[1];
    this->InitializeVariables(image,label, ksliceOptions);

    if(contInit)
    {
        std::cout<<"Initializing user input using label data"<<std::endl;
        this->initializeUserInputImageWithContour();
    }



    this->initializeData();
    this->CreateLLs(this->LL3D);

    LL* Lztmp = this->LL3D.Lz;

    this->intializeLevelSet3D();

}



void KSegmentor3D::accumulateUserInputInUserInputImages( double value,const unsigned int element)
{
    double Umax            = 1.0;   // It is bizarre that having this at 10.0 works,
    // technically it shouldn't because we're using inside the  tanh() function
    // comparing it with \phi() which is between -3 and +3 . If we can't get
    // values between -3 and +3 the smoothness breaks down.
    double user_input      = -Umax * ( value > 0.5 ) + Umax * ( value <= 0.5 );

    //Changed accumulation! (+=) instead of (=)
    //this->ptrU_t_Image[element]+=user_input; // I think this leads to crazily high/disparate values ...
    // at least, smoothness needs to be enforced somewhere else ...
    this->ptrU_t_Image[element] = user_input;

}

void KSegmentor3D::integrateUserInputInUserInputImage()
{
    ptrIntegral_Image = static_cast<double*>(this->U_Integral_image->GetScalarPointer());
    ptrU_t_Image      = static_cast<double*>(this->U_t_image->GetScalarPointer());

    int pos=0;
    int Nelements=this->m_UpdateVector.size(); // compiler may not optimize this out, b/c technically m_UpdateVector could change size in the loop
    for (int element=0;element<Nelements;element++)
    {
        pos=this->m_UpdateVector[element];

        this->ptrIntegral_Image[pos] += this->ptrU_t_Image[pos];
        this->ptrU_t_Image[pos]= 0; // this->ptrU_t_Image[pos]*0.5;
    }
}


void KSegmentor3D::UpdateArraysAfterTransform()
{
    this->mdims=U_t_image->GetDimensions();

    //Set dimensions
    dimz = (int)mdims[2];
    dimy = (int)mdims[1];
    dimx = (int)mdims[0];

    dims[2] = dimz;
    dims[1] = dimy;
    dims[0] = dimx;

    dims[3] = dims[0]*dims[1];
    dims[4] = dims[0]*dims[1]*dims[2];

    this->U_Integral_image->GetSpacing( m_Spacing_mm );

    cout << "num dims = " << numdims << "; updated KSegmentor3D with dims[0,1,2] = "
            << dimx << "," << dimy << "," << dimz << endl;

    this->rad = 3.0 / std::max( m_Spacing_mm[0],m_Spacing_mm[1] ); // about 3mm in physical units
    this->rad = std::min(7.0,this->rad); // force non-huge radius if the spacing is retarded
    this->rad = std::max(3.0, this->rad); // force non-tiny radius if the spacing is retarded

    this->initializeData();

    this->CreateLLs(LL3D);

    this->intializeLevelSet3D();
}


void KSegmentor3D::initializeData()
{
    imageVol->GetScalarRange( imgRange );
    labelVol->GetScalarRange( labelRange );
    if( abs(labelRange[1]) < 1e-3 )
    { // empty label; so set the proper range
        labelRange[1] = KViewerOptions::getDefaultDrawLabelMaxVal();
    }
    //assert( 0 != imgRange[1] ); // what the, all black ?? impossible !

    this->imgRange=imgRange;
    ptrCurrImage = static_cast<unsigned short*>(imageVol->GetScalarPointer());
    ptrCurrLabel = static_cast<unsigned short*>(labelVol->GetScalarPointer());


    long elemNum=0;
    for (int k=0; k<=dimx-1; k++) {
        for (int j=0; j<=dimy-1; j++)  {
            for (int i=0; i<=dimz-1; i++,elemNum++) {
                this->mask[elemNum]=(double) ( 0 < ptrCurrLabel[elemNum] );
                this->img[elemNum] = (double) ptrCurrImage[elemNum];
            }
        }
    }
}



void KSegmentor3D::Update2D()
{
    this->integrateUserInputInUserInputImage();
    this->CreateLLs(LL2D);

    ptrCurrImage        = static_cast<unsigned short*>(imageVol->GetScalarPointer());
    ptrCurrLabel        = static_cast<unsigned short*>(labelVol->GetScalarPointer());
    ptrIntegral_Image = static_cast<double*>(this->U_Integral_image->GetScalarPointer());

    double* imgSlice          = new double[  mdims[0]*mdims[1] ];
    double* maskSlice       = new double[ mdims[0]*mdims[1] ];
    double* U_I_slice        = new double[ mdims[0]*mdims[1] ];
    double* phiSlice          = new double[ mdims[0]*mdims[1] ];
    double* labelSlice       = new double[ mdims[0]*mdims[1] ];

    unsigned int element3D;
    long elemNum=0;
    for (int j=0; j<=mdims[1]-1; j++)  {
        for (int i=0; i<=mdims[0]-1; i++) {
            // indexing definition:  ptr[k*mdims[1]*mdims[0] +j*mdims[0]+i];
            element3D=this->currSlice*mdims[1]*mdims[0] +j*mdims[0]+i;
            imgSlice[elemNum]        = (double) ptrCurrImage[element3D];
            maskSlice[elemNum]       = (double) ( 0 < ptrCurrLabel[element3D] );

            U_I_slice[elemNum] =this->ptrIntegral_Image[element3D];
            elemNum++;
        }
    }

    std::vector<long> dimsSlice(5);
    dimsSlice[0] = mdims[0];
    dimsSlice[1] = mdims[1];
    dimsSlice[2] = 1;
    dimsSlice[3] = dimsSlice[0]*dimsSlice[1];
    dimsSlice[4] = dimsSlice[0]*dimsSlice[1]*dimsSlice[2];


    ls_mask2phi3c(maskSlice,phiSlice,labelSlice,&(dimsSlice[0]),
                  LL2D.Lz,LL2D.Ln1,LL2D.Ln2,LL2D.Lp1,LL2D.Lp2);

    interactive_rbchanvese(imgSlice,phiSlice,U_I_slice,labelSlice,&(dimsSlice[0]),
                           LL2D.Lz,LL2D.Ln1,LL2D.Lp1,LL2D.Ln2,LL2D.Lp2,LL2D.Lin2out,LL2D.Lout2in,
                           iter,rad,lambda*0.5,display);


    //threshold phi to find segmentation label, assign it to appropriate range of label!
    elemNum=0;
    for (int j=0; j<=mdims[1]-1; j++)  {
        for (int i=0; i<=mdims[0]-1; i++) {
            double phi_val = phiSlice[elemNum];
            double phi_out = (-phi_val + 3.0) / 6.0; // shift and scale from [-3,3] to [0,1]

            element3D=this->currSlice*mdims[1]*mdims[0] +j*mdims[0]+i;
//            ptrCurrLabel[element3D]= (unsigned short) ( ( (phi_out > 0.95)
//                                                          + (phi_out > 0.8) + (phi_out > 0.65)
//                                                          + (phi_out > 0.5) ) * labelRange[1] / 4.0 );

            ptrCurrLabel[element3D]=   ( (unsigned short) 0 >= phi_val )*labelRange[1];

            elemNum++;
        }
    }

    cout <<  "Lz size: "       << LL2D.Lz->length << endl;

    bool bSavePNG = false;
    if( bSavePNG ) {
        std::stringstream ss;
        ss << "U_integral_ " << std::setw(3) << std::setfill('0') << currSlice << ".png";
        saveMatToPNG( imgSlice, ss.str() ); //saveMatToPNG( U_I_slice, ss.str() );
    }
    delete imgSlice;
    delete labelSlice;
    delete maskSlice;
    delete phiSlice;
    delete U_I_slice;


    m_UpdateVector.clear();
    m_CoordinatesVector.clear();


}


void KSegmentor3D::Update3D()
{
    bool bSmoothU = false;
    if( bSmoothU ) {
        cout << "smoothing integral image 3D " << endl;
        vtkSmartPointer<vtkImageGaussianSmooth> imageSmoother = vtkSmartPointer<vtkImageGaussianSmooth>::New();
        imageSmoother->SetDimensionality(3);
        imageSmoother->SetStandardDeviations(1.0,1.0,1.0);
        imageSmoother->SetInput(this->U_Integral_image);
        imageSmoother->Update();
        this->U_Integral_image->DeepCopy(imageSmoother->GetOutput());
    }


    cout << "integrating mask 3D " << endl;
    this->integrateUserInputInUserInputImage();

    // The bug is as follows: the level-set evolution modifies Lchanged indices,
    // but afterwards they are not appearing in the list of modified coordinates!
    // Thus, the next time around the mask doesn't get to update them!

    cout << "updating mask 3D " << endl;
    this->UpdateMask(true);

    this->CreateLLs(LL3D); // TODO: fix the caching so that only uniques are kept

    // The bug is as follows: the level-set evolution modifies Lchanged indices,
    // but afterwards they are not appearing in the list of modified coordinates!
    // Thus, the next time around the mask doesn't get to update them!

    ll_init(LL3D.Lchanged);

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
    cout << "m_UpdateVector Size BEGIN: " << m_UpdateVector.size()
            <<  ", Lchanged size BEGIN: " << LL3D.Lchanged->length
            <<  ", Lz size BEGIN: "       << LL3D.Lz->length << endl;

    ptrCurrImage = static_cast<unsigned short*>(imageVol->GetScalarPointer());
    ptrCurrLabel = static_cast<unsigned short*>(labelVol->GetScalarPointer());
    ptrIntegral_Image = static_cast<double*>(this->U_Integral_image->GetScalarPointer());
    double u_in, u_out;
    // Crap, the linked lists will accumulate zillions of repeating entries ...
    // Must flush them and keep unique entries, or something (?)
    //if(this->m_UpdateVector.size()!=0)
    //        ls_mask2phi3c_update(this->m_UpdateVector,
    //                            this->m_CoordinatesVector,mask,phi,label,dims,
    //                            LL3D.Lz,LL3D.Ln1,LL3D.Ln2,LL3D.Lp1,LL3D.Lp2,LL3D.Lchanged);

    ls_mask2phi3c(mask,phi,label,dims,LL3D.Lz,LL3D.Ln1,LL3D.Ln2,LL3D.Lp1,LL3D.Lp2);
    //   ls_mask2phi3c_ext(mask,phi,label,dims,LL3D.Lz,LL3D.Ln1,LL3D.Ln2,LL3D.Lp1,LL3D.Lp2,LL3D.Lchanged);
    cout << "m_UpdateVector Size: " << m_UpdateVector.size()
            <<" m_CoordinatesVector Size: "<<this->m_CoordinatesVector.size()
            <<  ", Lchanged size: " << LL3D.Lchanged->length
            <<  ", Lz size: "       << LL3D.Lz->length << endl;

    if( 0 == m_EnergyName.compare("ChanVese") ) {
        interactive_chanvese_ext(img,phi,ptrIntegral_Image,label,dims,
                                 LL3D.Lz,LL3D.Ln1,LL3D.Lp1,LL3D.Ln2,LL3D.Lp2,LL3D.Lin2out,LL3D.Lout2in,LL3D.Lchanged,
                                 iter,0.5*lambda,display,this->m_PlaneNormalVector,
                                 this->m_PlaneCenter,this->m_DistWeight);
        bool bDisplayChanVeseCost = false;
        if( bDisplayChanVeseCost ) {
            double cv_cost = this->evalChanVeseCost(u_in,u_out);
            cout << "chan vese cost = " << cv_cost << endl;
        }
    }
    else if( 0 == m_EnergyName.compare("LocalCV") )
        interactive_rbchanvese_ext(img,phi,ptrIntegral_Image,label,dims,
                                   LL3D.Lz,LL3D.Ln1,LL3D.Lp1,LL3D.Ln2,LL3D.Lp2,LL3D.Lin2out,LL3D.Lout2in,LL3D.Lchanged,
                                   iter,rad,lambda,display,this->m_PlaneNormalVector,
                                   this->m_PlaneCenter,this->m_DistWeight);
    else
        cout << "Error, unsupported energy name! " << m_EnergyName << endl;


    this->evalChanVeseCost(u_in,u_out);
    double cutoff_thresh = (u_in - u_out)*this->m_ThreshWeight;
    cout << "uin, uout, cutoff=" << u_in << ", " << u_out << ", " << cutoff_thresh << endl;

    long elemNum=0;
    double mult=labelRange[1] / 4.0;
    elemNum=0;
    int x,y,z,x0,y0,z0,idx;

    double phi_val = 0;
    double phi_out = 0;
    double outputVal=0;

    // Caution: Lchanged can contain duplicate entries !!!
    ll_init(LL3D.Lchanged);
    // while(Lchanged->curr != NULL)
    double changeInLabel = 0;
    int Nelements = this->dimx * this->dimy * this->dimz;
    for (int idx=0;idx<Nelements;idx++)
    {

        // traverse through all modified Lz *and their neighbors!*
        // neighbors are where phi_out is something between 0 and 1
        //          x0 = Lchanged->curr->x;
        //          y0 = Lchanged->curr->y;
        //          z0 = Lchanged->curr->z;
        //          for (int dx = -2; dx < 2; dx ++ ) {
        //              x = x0 + dx;
        //              for (int dy = -2; dy < 2; dy ++ ) {
        //                  y = y0 + dy;
        //                  for (int dz = -2; dz < 2; dz ++ ) {
        //                      z = z0 + dz;
        //                      if( 0 < ( (x<0)+(y<0)+(z<0)+(x>=mdims[0])+(y>=mdims[1])+(z>=mdims[2]) ) )
        //                      { // the neighbor is out of bounds !
        //                          continue;
        //                      }
        //    idx = (z)*mdims[1]*mdims[0] +(y)*mdims[0]+(x);
        phi_val = phi[idx];
        if( (u_out < u_in) && ( img[idx] < (u_out+2*cutoff_thresh) ) )
        { // force it to be out
            phi_val = 3.0;
        }
        else if( (u_out < u_in) && ( img[idx] < (u_out+cutoff_thresh) ) )
        { // force it to be out
            phi_val = std::min( phi_val, 0.0 );
        }
        phi_out = (-phi_val + 3.0) / 6.0;
        outputVal=  (unsigned short) ( ( (phi_out > 0.95) +
                                         (phi_out > 0.8) +
                                         (phi_out > 0.65) +
                                         (phi_out > 0.5) )
                                       *mult);
        changeInLabel += fabs( (outputVal - ptrCurrLabel[idx]) > 1e-3 );
        ptrCurrLabel[idx] =outputVal;
        mask[idx]         =(double) ( 0 < ptrCurrLabel[idx] ); // d'oh, *update the mask!*

        //                  }
        //              }
        //          }
        //          ll_step(Lchanged);
    }
    cout << "m_UpdateVector Size: " << m_UpdateVector.size()
            <<  ", Lchanged size: " << LL3D.Lchanged->length
            <<  ", |change| in label: " << changeInLabel << endl;

    double spc[3];
    this->U_Integral_image->GetSpacing(spc);

    m_UpdateVector.clear();
    m_CoordinatesVector.clear();
}


KSegmentor3D::~KSegmentor3D(){

    //delete [] this->mdims;//Causes trouble! Haveto find out why!!
    delete [] this->img;
    delete [] this->mask;
    delete [] this->imgRange;
    delete [] this->labelRange;
    delete [] this->phi;
    delete [] this->label;
    delete [] this->seg;
    delete [] this->iList;
    delete [] this->jList;


    LL *Lz, *Ln1, *Ln2, *Lp1, *Lp2;
    LL *Lin2out, *Lout2in,*Lchanged;


    Lz=LL3D.Lz;
    Ln1=LL3D.Ln1;
    Ln2=LL3D.Ln2;
    Lp1=LL3D.Lp1;
    Lp2=LL3D.Lp2;
    Lin2out=LL3D.Lin2out;
    Lout2in=LL3D.Lout2in;
    Lchanged=LL3D.Lchanged;

    ll_destroy(Lz);
    ll_destroy(Ln1);
    ll_destroy(Ln2);
    ll_destroy(Lp1);
    ll_destroy(Lp2);
    ll_destroy(Lin2out);
    ll_destroy(Lout2in);
    ll_destroy(Lchanged);

    Lz=LL2D.Lz;
    Ln1=LL2D.Ln1;
    Ln2=LL2D.Ln2;
    Lp1=LL2D.Lp1;
    Lp2=LL2D.Lp2;
    Lin2out=LL2D.Lin2out;
    Lout2in=LL2D.Lout2in;
    Lchanged=LL2D.Lchanged;

    ll_destroy(Lz);
    ll_destroy(Ln1);
    ll_destroy(Ln2);
    ll_destroy(Lp1);
    ll_destroy(Lp2);
    ll_destroy(Lin2out);
    ll_destroy(Lout2in);
    ll_destroy(Lchanged);
}



