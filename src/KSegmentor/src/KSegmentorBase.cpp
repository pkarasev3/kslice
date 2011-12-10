#include "KSegmentorBase.h"
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

void test_OpenMP()
{
  int nthreads, tid;
  omp_set_num_threads(8);
  nthreads = omp_get_num_threads();
  #pragma omp parallel shared(nthreads, tid)
  { // fork some threads, each one does one expensive operation
    tid = omp_get_thread_num();
    if( tid == 0 )      { }
    else if( tid == 1 ) { }
    else if( tid == 2 ) { }

  }
}

/** default curvature penalty term. can be set externally when a KSegmentorBase is made. */
double KSegmentorBase::defaultKappaParam = 0.5;


void KSegmentorBase::InitializeVariables(KSegmentorBase* segPointer,vtkImageData *image, vtkImageData *label, bool contInit)
{
    segPointer->imageVol=image;
    segPointer->labelVol=label;

    segPointer->numberdims=3;

    segPointer->m_bUseEdgeBased = false;
    //this->mdims[2] = 1;
    segPointer->penaltyAlpha=0;
    segPointer->seed=0;
    segPointer->useContInit=contInit;
    segPointer->display=0;
    //Should we keep these abolute values in here
    segPointer->dthresh=500;
    segPointer->iter=500;
    segPointer->lambda=defaultKappaParam; // this could/should be user togglable!
    segPointer->mdims = new int[3];
    image->GetDimensions( segPointer->mdims );

    segPointer->Lz=NULL;
    segPointer->Ln1= NULL;
    segPointer->Ln2= NULL;
    segPointer->Lp1=NULL ;
    segPointer->Lp2=NULL ;
    segPointer->Sz=NULL ;
    segPointer->Sn1=NULL ;
    segPointer->Sn2=NULL ;
    segPointer->Sp1=NULL ;
    segPointer->Sp2=NULL ;
    segPointer->Lin2out=NULL ;
    segPointer->Lout2in=NULL ;
    segPointer->seg=NULL;
    segPointer->img=NULL;
    segPointer->mask=NULL;

    image->GetSpacing( segPointer->m_Spacing_mm );

    // want rad to be '10' for 512 . A 512x512 mri with xy spacing 0.3mm is 153.6000 across
    // "10" pixels is 3mm in this context.
    segPointer->rad = 3.0 / std::max( segPointer->m_Spacing_mm[0],segPointer->m_Spacing_mm[1] ); // about 3mm in physical units
    segPointer->rad = std::min(18.0,segPointer->rad); // force non-huge radius if the spacing is retarded
    segPointer->rad = std::max(3.0, segPointer->rad); // force non-tiny radius if the spacing is retarded
    cout << "segmentor using ROI size: " << segPointer->rad << endl;

    segPointer->U_Integral_image = vtkSmartPointer<vtkImageData>::New();
    segPointer->U_t_image= vtkSmartPointer<vtkImageData>::New();

    segPointer->U_Integral_image->SetExtent(image->GetExtent());
    segPointer->U_Integral_image->SetScalarTypeToDouble();
    segPointer->U_Integral_image->SetSpacing(image->GetSpacing());
    segPointer->ptrIntegral_Image = static_cast<double*>(segPointer->U_Integral_image->GetScalarPointer());

    segPointer->U_t_image->SetExtent(image->GetExtent());
    segPointer->U_t_image->SetScalarTypeToDouble();
    segPointer->U_t_image->SetSpacing(image->GetSpacing());
    segPointer->ptrU_t_Image = static_cast<double*>(segPointer->U_t_image->GetScalarPointer());

    segPointer->m_Reslicer = vtkSmartPointer<vtkImageReslice>::New();

    segPointer->imgRange   = new double[2];
    segPointer->labelRange = new double[2];

    segPointer->phi        = new double[segPointer->mdims[0]*segPointer->mdims[1]*segPointer->mdims[2]];
    segPointer->label      = new double[segPointer->mdims[0]*segPointer->mdims[1]*segPointer->mdims[2]];
    segPointer->mask     = new double[segPointer->mdims[0]*segPointer->mdims[1]*segPointer->mdims[2]];
    segPointer->img     = new double[segPointer->mdims[0]*segPointer->mdims[1]*segPointer->mdims[2]];


    cout << "I think the # of rows is: " << segPointer->mdims[0] << ", # of cols is: " << segPointer->mdims[1] << endl;

    segPointer->iList=NULL;
    segPointer->jList=NULL;

    if(segPointer->useContInit)
    {
       std::cout<<"Initializing user input using label data"<<std::endl;
        segPointer->initializeUserInputImageWithContour();
    }

    //Set dimensions
    segPointer->dimz = (int)segPointer->mdims[2];
    segPointer->dimx = (int)segPointer->mdims[1];
    segPointer->dimy = (int)segPointer->mdims[0];

    segPointer->dims[2] = segPointer->dimz;
    segPointer->dims[1] = segPointer->dimx;
    segPointer->dims[0] = segPointer->dimy;

    segPointer->dims[3] = segPointer->dims[0]*segPointer->dims[1];
    segPointer->dims[4] = segPointer->dims[0]*segPointer->dims[1]*segPointer->dims[2];

    cout << "num dims = " << numdims << "; initialized KSegmentor3D with dims[0,1,2] = "
         << segPointer->dims[0] << "," << segPointer->dims[1] << "," << segPointer->dims[2] << endl;

    segPointer->seg = new  unsigned short[segPointer->dims[0]*segPointer->dims[1]*segPointer->dims[2]];
    //segPointer->initializeData();
    //segPointer->intializeLevelSet();
}

void KSegmentorBase::saveMatToPNG( const cv::Mat& sliceImg, const std::string& fileName )
{
    std::stringstream  ss;
    ss << fileName;
    bool name_is_png=(0==std::string(".png").compare(fileName.substr(fileName.size()-4,4)) );
    if( !name_is_png ) {
      ss << ".png" ;
    }
    string png_name = ss.str();
    cout << " reference png file: " << png_name << endl;
    cv::Mat source;
    cv::flip( -1.0 * sliceImg, source, 0 /* flipVert */ );
    double dmin, dmax;
    cv::minMaxLoc( source, &dmin, &dmax );
    cv::Mat saveImg = (255.0 / (dmax - dmin )) * (source - dmin);
    cv::imwrite(png_name, saveImg );
}

void KSegmentorBase::initializeUserInputImageWithContour(){
    int element=0;
    unsigned short* label3DPointer = static_cast<unsigned short*>(labelVol->GetScalarPointer());
    for (int i=0; i<=imageVol->GetExtent()[1]-1; i++)  {
        for (int j=0; j<=imageVol->GetExtent()[3]-1; j++) {
            for(int k=0;k<=imageVol->GetExtent()[5]-1;k++){
                this->accumulateUserInputInUserInputImages((double)label3DPointer[element],element);
                element++;
        }
     }
  }
  for(int k=0;k<=imageVol->GetExtent()[5]-1;k++)
    this->integrateUserInputInUserInputImage(k);
}


void KSegmentorBase::integrateUserInputInUserInputImage( int k )
{
    unsigned int element3D=0;
    for (int j=0; j<=mdims[1]-1; j++)  {
        for (int i=0; i<=mdims[0]-1; i++) {
            element3D=k*mdims[1]*mdims[0] +j*mdims[0]+i;
            this->ptrIntegral_Image[element3D]+=this->ptrU_t_Image[element3D];
            this->ptrU_t_Image[element3D]=this->ptrU_t_Image[element3D]*0.5;
        }
    }
}


void KSegmentorBase::setNumIterations(int itersToRun){
    this->iter=itersToRun;
}


void KSegmentorBase::TransformUserInputImages(vtkTransform* transform, bool invert)
{
    double spacing_mm[3];
    this->U_Integral_image->GetSpacing( spacing_mm );
    this->m_Reslicer->SetInput(this->U_Integral_image);
    this->m_Reslicer->SetResliceAxesDirectionCosines(1,0,0,    0,1,0,     0,0,1);
    this->m_Reslicer->AutoCropOutputOn();
    this->m_Reslicer->SetOutputOrigin(0,0,0);
    this->m_Reslicer->SetOutputSpacing(m_Spacing_mm);
    this->m_Reslicer->SetOutputDimensionality(3);
    if (invert)
       this->m_Reslicer->SetResliceTransform(transform->GetInverse());
    else
      this->m_Reslicer->SetResliceTransform(transform);

    this->m_Reslicer->UpdateWholeExtent();
    this->m_Reslicer->Update();
    this->m_Reslicer->UpdateInformation();
    this->m_Reslicer->GetOutput()->UpdateInformation();

    this->U_Integral_image->DeepCopy(m_Reslicer->GetOutput());
    this->ptrIntegral_Image = static_cast<double*>(this->U_Integral_image->GetScalarPointer());

     this->U_Integral_image->GetSpacing( spacing_mm );

    this->m_Reslicer->SetInput(this->U_t_image);
    this->m_Reslicer->SetResliceAxesDirectionCosines(1,0,0,    0,1,0,     0,0,1);
    this->m_Reslicer->Modified();
    this->m_Reslicer->UpdateWholeExtent();
    this->m_Reslicer->UpdateInformation();
    this->m_Reslicer->GetOutput()->UpdateInformation();

    this->U_t_image->DeepCopy(m_Reslicer->GetOutput());
    this->ptrU_t_Image = static_cast<double*>(this->U_t_image->GetScalarPointer());

    this->UpdateArraysAfterTransform();

}


void KSegmentorBase::setCurrLabelArray(vtkImageData *label){
    this->labelVol=label;
}

void KSegmentorBase::intializeLevelSet(){

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

}

void KSegmentorBase::copyIntegralDuringPaste(int kFrom, int kTo)
{
  for (int i=0;i<=mdims[0]-1; i++)  {
      for (int j=0; j<=mdims[1]-1; j++) {
          double val=0.9*this->U_Integral_image->GetScalarComponentAsDouble(i,j,kFrom,0);
          this->U_Integral_image->SetScalarComponentFromDouble(i,j,kTo,0,val);
      }
  }
}

KSegmentorBase::~KSegmentorBase()
{
        //delete [] this->mdims;//Causes trouble! Have to find out why!!
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
