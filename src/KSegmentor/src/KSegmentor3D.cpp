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

namespace vrcl
{

    KSegmentor3D* KSegmentor3D::CreateSegmentor(vtkImageData *image, vtkImageData *label, bool contInit)
    {
        KSegmentor3D* seg3DPointer = new KSegmentor3D;
        seg3DPointer->InitializeVariables(seg3DPointer,image,label, contInit);
        seg3DPointer->ptrCurrImage = static_cast<unsigned short*>(
            seg3DPointer->imageVol->GetScalarPointer());
        seg3DPointer->ptrCurrLabel = static_cast<unsigned short*>(
            seg3DPointer->labelVol->GetScalarPointer());
        if(contInit)
        {
           std::cout<<"Initializing user input using label data"<<std::endl;
            seg3DPointer->initializeUserInputImageWithContour();
        }
        seg3DPointer->initializeData();
        seg3DPointer->intializeLevelSet();
        return seg3DPointer;
    }

    void KSegmentor3D::accumulateUserInputInUserInputImages( double value,const unsigned int element)
    {
        double user_input      = -1.0 * ( value > 0.5 ) + 1.0 * ( value <= 0.5 );
        //Changed accumulation! (+=) instead of (+)
        this->ptrU_t_Image[element]+=user_input;
    }


    void KSegmentor3D::integrateUserInputInUserInputImage()
    {
        int pos=0;
        for (int element=0;element<this->m_UpdateVector.size();element++)
        {
            pos=this->m_UpdateVector[element];
            this->ptrIntegral_Image[pos]+=this->ptrU_t_Image[pos];
            this->ptrU_t_Image[pos]=this->ptrU_t_Image[pos]*0.5;
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
        this->rad = std::min(18.0,this->rad); // force non-huge radius if the spacing is retarded
        this->rad = std::max(3.0, this->rad); // force non-tiny radius if the spacing is retarded

        this->initializeData();
        //this->initializeUserInputImageWithContour(false);
        this->intializeLevelSet();

        double spc[3];
        this->U_Integral_image->GetSpacing(spc);

    }


    void KSegmentor3D::initializeData()
    {
        imageVol->GetScalarRange( imgRange );
        labelVol->GetScalarRange( labelRange );
        if( abs(labelRange[1]) < 1e-3 )
        { // empty label; so set the proper range
          labelRange[1] = KViewerOptions::getDefaultDrawLabelMaxVal();
        }
        assert( 0 != imgRange[1] ); // what the, all black ?? impossible !

        this->imgRange=imgRange;

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

    void KSegmentor3D::Update()
    {
        this->integrateUserInputInUserInputImage();
        this->UpdateMask();

        double spc[3];
        this->U_Integral_image->GetSpacing(spc);

        if(this->m_UpdateVector.size()!=0)
            ls_mask2phi3c_update(this->m_UpdateVector,this->m_CoordinatesVector,mask,phi,label,dims,Lz,Ln1,Ln2,Lp1,Lp2);


        if( !m_bUseEdgeBased ) {
            /*short plhs[1];
            chanvese(img,phi,label,dims,
                     Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in,
                     iter,lambda,plhs,display);*/
            interactive_chanvese(img,phi,ptrIntegral_Image,label,dims,
                                 Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in,
                                 iter,rad,lambda,display);
        } else {
            interactive_edgebased(img,phi,ptrIntegral_Image,label,dims,
                                  Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in,
                                  iter,rad,0.5*lambda,display,m_SatRange[0],m_SatRange[1]);
        }

        if(iList!=NULL){
            delete[] iList;
        }
        if(jList!=NULL){
            delete[] jList;
        }

        //get number and coordinates of point (row, col) on the zero level set
        prep_C_output(Lz,dims,phi, &iList, &jList, lengthZLS);

        //threshold phi to find segmentation label,
        // assign it to appropriate range of label!
        // shift and scale from [-3,3] to [0,1] x max_label
        long elemNum=0;
        double mult=labelRange[1] / 4.0;
        for (int k=0; k<dimz; k++) {
            for (int j=0; j<dimy; j++)  {
                for (int i=0; i<dimx; i++) {
                    double phi_val = phi[elemNum];

                    double phi_out = (-phi_val + 3.0) / 6.0;

                    double outputVal=  (unsigned short) ( ( (phi_out > 0.95) +
                                                            (phi_out > 0.8) +
                                                            (phi_out > 0.65) +
                                                            (phi_out > 0.5) )
                                                          * mult );
                    ptrCurrLabel[elemNum] =outputVal;
                    elemNum++;
                }
            }
        }

        //Still needed? For copy/paste?
        //prevSlice = currSlice;
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

            ll_destroy(Lz);
            ll_destroy(Ln1);
            ll_destroy(Ln2);
            ll_destroy(Lp1);
            ll_destroy(Lp2);
            ll_destroy(Lin2out);
            ll_destroy(Lout2in);
    }

}
