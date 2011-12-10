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
        return seg3DPointer;
    }

    void KSegmentor3D::accumulateUserInputInUserInputImages( double value,const unsigned int element)
    {
        double user_input      = -1.0 * ( value > 0.5 ) + 1.0 * ( value <= 0.5 );
        //Changed accumulation! (+=) instead of (+)
        this->ptrU_t_Image[element]+=user_input;
    }


    void KSegmentor3D::integrateUserInputInUserInputImage( int k )
    {
        int element3D=0;
        for (int i=0; i<=imageVol->GetExtent()[1]-1; i++)  {
            for (int j=0; j<=imageVol->GetExtent()[3]-1; j++) {
                for(int k=0;k<=imageVol->GetExtent()[5]-1;k++,element3D++){
                    this->ptrIntegral_Image[element3D]+=this->ptrU_t_Image[element3D];
                    this->ptrU_t_Image[element3D]=this->ptrU_t_Image[element3D]*0.5;
                }
            }
        }
    }


    void KSegmentor3D::UpdateArraysAfterTransform()
    {
        this->mdims=U_t_image->GetDimensions();

        //Set dimensions
        dimz = (int)mdims[2];
        dimx = (int)mdims[1];
        dimy = (int)mdims[0];

        dims[2] = dimz;
        dims[1] = dimx;
        dims[0] = dimy;

        dims[3] = dims[0]*dims[1];
        dims[4] = dims[0]*dims[1]*dims[2];

        delete [] seg;
        delete [] img;
        delete [] mask;
        delete [] phi;
        delete [] label;

        this->seg = new  unsigned short[dims[0]*dims[1]*dims[2]];

        this->phi        = new double[mdims[0]*mdims[1]*mdims[2]];
        this->label      = new double[mdims[0]*mdims[1]*mdims[2]];
        this->mask     = new double[mdims[0]*mdims[1]*mdims[2]];
        this->img     = new double[mdims[0]*mdims[1]*mdims[2]];

        this->U_Integral_image->GetSpacing( m_Spacing_mm );

        this->rad = 3.0 / std::max( m_Spacing_mm[0],m_Spacing_mm[1] ); // about 3mm in physical units
        this->rad = std::min(18.0,this->rad); // force non-huge radius if the spacing is retarded
        this->rad = std::max(3.0, this->rad); // force non-tiny radius if the spacing is retarded
    }




    void KSegmentor3D::initializeData()
    {
        this->ptrCurrImage = static_cast<unsigned short*>(
            imageVol->GetScalarPointer());
        this->ptrCurrLabel = static_cast<unsigned short*>(
            labelVol->GetScalarPointer());

        imageVol->GetScalarRange( imgRange );
        labelVol->GetScalarRange( labelRange );
        if( abs(labelRange[1]) < 1e-3 )
        { // empty label; so set the proper range
          labelRange[1] = KViewerOptions::getDefaultDrawLabelMaxVal();
        }
        assert( 0 != imgRange[1] ); // what the, all black ?? impossible !

        this->imgRange=imgRange;
        this->integrateUserInputInUserInputImage(currSlice);

        long elemNum=0;
        for (int k=0; k<=mdims[2]-1; k++) {
            for (int j=0; j<=mdims[1]-1; j++)  {
                for (int i=0; i<=mdims[0]-1; i++,elemNum++) {
                    this->mask[elemNum]=(double) ( 0 < ptrCurrLabel[elemNum] );
                    this->img[elemNum] = (double) ptrCurrImage[elemNum];
                }
            }
        }
    }


    void KSegmentor3D::intializeLevelSet(){

        if (Lz!=NULL){
            //destroy linked lists
            ll_destroy(Lz);
            ll_destroy(Ln1);
            ll_destroy(Ln2);
            ll_destroy(Lp1);
            ll_destroy(Lp2);
            ll_destroy(Lin2out);
            ll_destroy(Lout2in);
        }


        //create linked lists
        Lz  = ll_create();
        Ln1 = ll_create();
        Ln2 = ll_create();
        Lp1 = ll_create();
        Lp2 = ll_create();
        Lin2out = ll_create();
        Lout2in = ll_create();

        //initialize lists, phi, and labels
        ls_mask2phi3c(mask,phi,label,dims,Lz,Ln1,Ln2,Lp1,Lp2);

        if( currSlice == prevSlice ) {

        }

    }

    void KSegmentor3D::Update()
    {
        //Do we need to initialize again after initializing in CreatSegmentor method
        this->initializeData();
        this->intializeLevelSet();
        if( !m_bUseEdgeBased ) {
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
      for (int k=0; k<=mdims[2]-1; k++) {
          for (int j=0; j<=mdims[1]-1; j++)  {
              for (int i=0; i<=mdims[0]-1; i++) {
                  double phi_val = phi[elemNum];
                  double phi_out = (-phi_val + 3.0) / 6.0;
                  ptrCurrLabel[elemNum] =
                          (unsigned short) ( ( (phi_out > 0.95) +
                                               (phi_out > 0.8) +
                                               (phi_out > 0.65) +
                                               (phi_out > 0.5) )
                                             * labelRange[1] / 4.0 );
                  elemNum++;
              }
          }
      }

      prevSlice = currSlice;
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
