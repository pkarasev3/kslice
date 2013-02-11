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
double KSegmentorBase::defaultKappaParam = 0.35;


void KSegmentorBase::InitializeVariables(KSegmentorBase* segPointer,vtkImageData *image,
                                         vtkImageData *label, bool contInit)
{
    segPointer->m_CustomSpeedImgPointer=NULL;
    segPointer->imageVol=image;
    segPointer->labelVol=label;

    segPointer->numberdims=3;

    segPointer->m_bUseEdgeBased = false;
    segPointer->penaltyAlpha=0;
    segPointer->seed=0;
    segPointer->useContInit=contInit;
    segPointer->display=0;

    //Should we keep these abolute values in here
    segPointer->iter=500;
    segPointer->m_DistWeight=0;
    segPointer->m_ThreshWeight=0;
    segPointer->lambda=defaultKappaParam; // this could/should be user togglable!
    segPointer->mdims = new int[3];
    image->GetDimensions( segPointer->mdims );

    segPointer->LL3D.init();
    segPointer->LL2D.init();
    segPointer->Sz=NULL ;
    segPointer->Sn1=NULL ;
    segPointer->Sn2=NULL ;
    segPointer->Sp1=NULL ;
    segPointer->Sp2=NULL ;

    segPointer->img=NULL;
    segPointer->mask=NULL;

    image->GetSpacing( segPointer->m_Spacing_mm );

    // want rad to be '10' for 512 . A 512x512 mri with xy spacing 0.3mm is 153.6000 across
    // "10" pixels is 3mm in this context.
    segPointer->rad = 3.0 / std::max( segPointer->m_Spacing_mm[0],segPointer->m_Spacing_mm[1] ); // about 3mm in physical units
    segPointer->rad = std::min(7.0,segPointer->rad); // force non-huge radius if the spacing is retarded
    segPointer->rad = std::max(3.0, segPointer->rad); // force non-tiny radius if the spacing is retarded
    // what the fuck? not in IK's version? // segPointer->rad=7; //IKChange
    cout << "segmentor using ROI size: " << segPointer->rad << endl;

    segPointer->U_Integral_image = vtkSmartPointer<vtkImageData>::New();
    segPointer->U_t_image= vtkSmartPointer<vtkImageData>::New();

    segPointer->U_Integral_image->SetExtent(image->GetExtent());
    segPointer->U_Integral_image->SetScalarTypeToDouble();
    segPointer->U_Integral_image->SetSpacing(image->GetSpacing());
    segPointer->U_Integral_image->AllocateScalars();
    segPointer->ptrIntegral_Image = static_cast<double*>(segPointer->U_Integral_image->GetScalarPointer());

    segPointer->U_t_image->SetExtent(image->GetExtent());
    segPointer->U_t_image->SetScalarTypeToDouble();
    segPointer->U_t_image->SetSpacing(image->GetSpacing());
    segPointer->U_t_image->AllocateScalars();
    segPointer->ptrU_t_Image = static_cast<double*>(segPointer->U_t_image->GetScalarPointer());

    segPointer->m_Reslicer = vtkSmartPointer<vtkImageReslice>::New();

    segPointer->imgRange   = new double[2];
    segPointer->labelRange = new double[2];

    cout << "I think the # of rows is: " << segPointer->mdims[1] << ", # of cols is: " << segPointer->mdims[0] << endl;

    segPointer->iList=NULL;
    segPointer->jList=NULL;

    //Set dimensions
    segPointer->dimz = (int)segPointer->mdims[2];
    segPointer->dimy = (int)segPointer->mdims[1];
    segPointer->dimx = (int)segPointer->mdims[0];

    try {
      segPointer->phi        = new double[segPointer->dimx*segPointer->dimy*segPointer->dimz];
      segPointer->label      = new double[segPointer->dimx*segPointer->dimy*segPointer->dimz];
      segPointer->mask       = new double[segPointer->dimx*segPointer->dimy*segPointer->dimz];
      segPointer->img        = new double[segPointer->dimx*segPointer->dimy*segPointer->dimz];
    } catch ( const std::bad_alloc& e) {
      std::cout << "Failed to allocate KSegmentorBase pointers! ";
      std::cout << "Perhaps it is a big data-set on 32-bit OS? " << std::endl;
      exit(-1);
    }

    segPointer->dims[2] = segPointer->dimz;
    segPointer->dims[1] = segPointer->dimy;
    segPointer->dims[0] = segPointer->dimx;

    segPointer->dims[3] = segPointer->dims[0]*segPointer->dims[1];
    segPointer->dims[4] = segPointer->dims[0]*segPointer->dims[1]*segPointer->dims[2];

    this->m_Umax = 1.0;

    cout << "num dims = " << numdims << "; initialized KSegmentor3D with dims[0,1,2] = "
         << segPointer->dims[0] << "," << segPointer->dims[1] << "," << segPointer->dims[2] << endl;

}

namespace {

const double epsH = sqrt(2.0);   // epsilon   = sqrt(2);

inline double Heavi( double z )
{

//    Heavi     = @(z)  1 * (z >= epsilon) + (abs(z) < epsilon).*(1+z/epsilon+1/pi * sin(pi*z/epsilon))/2.0;
    return ( 1.0*(z>=epsH) + (abs(z)<epsH)*(1+z/epsH+(1/CV_PI)*sin(CV_PI*z/epsH) )/2.0  );
}
inline double Delta( double z )
{
//    delta     = @(z)  1 * (z == 0) + (abs(z) < epsilon).*(1 + cos(pi*z/epsilon))/(epsilon*2.0);
    return ( 1.0*(z==0) + (abs(z)<epsH) * (1 + cos(CV_PI * z /epsH) )/(2 * epsH) );
}

}

double KSegmentorBase::evalChanVeseCost( double& mu_i, double& mu_o ) const
{
    double E      = 0.0;
    int Nelements = dimx * dimy * dimz;
    double integral_zero_plus_H = 0; // $ \int \Heavi{ }       $
    double integral_one_minus_H = 0; // $ \int (1-\Heavi{ })   $
    double integral_I_zpH       = 0; // $ \int I*\Heavi{ }     $
    double integral_I_omH       = 0; // $ \int I*(1-\Heavi{ }) $
    for (int voxel_idx = 0; voxel_idx < Nelements; voxel_idx++ )
    {
        integral_zero_plus_H += Heavi(phi[voxel_idx]) ;
        integral_one_minus_H += 1.0 - Heavi(phi[voxel_idx]) ;
        integral_I_zpH       += (img[voxel_idx]) * Heavi(phi[voxel_idx]) ;
        integral_I_omH       += (img[voxel_idx]) * (1.0 - Heavi(phi[voxel_idx]));
    }
    mu_i = integral_I_omH / (integral_one_minus_H +1e-12);
    mu_o = integral_I_zpH / (integral_zero_plus_H +1e-12);

//    cout << "mu_i = " << mu_i << ", mu_o = " << mu_o << endl;
//    for (int voxel_idx = 0; voxel_idx < Nelements; voxel_idx++ )
//    {
//        E +=  pow( (img[voxel_idx]-mu_i),2.0 ) + pow( (img[voxel_idx]-mu_o),2.0 );
//    }

    return (E/2.0);
}


void KSegmentorBase::UpdateMask(bool bForceUpdateAll)
{
    if( bForceUpdateAll )
    {
        int Nelements = this->dimx * this->dimy * this->dimz;
        for (int element=0;element<Nelements;element++)
        {
            this->mask[element]=(double) ( 0 < ptrCurrLabel[element] );
        }
    }
    else
    {
        int Nelements=this->m_UpdateVector.size(); // compiler may not optimize this out, b/c technically m_UpdateVector could change size in the loop
        for (int element=0;element<Nelements;element++)
        {
            unsigned int el=this->m_UpdateVector[element];
            this->mask[el]=(double) ( 0 < ptrCurrLabel[el] );
        }
    }
}

void KSegmentorBase::saveMatToPNG( double* data, const std::string& fileName )
{
    std::stringstream  ss;
    ss << fileName;
    bool name_is_png=(0==std::string(".png").compare(fileName.substr(fileName.size()-4,4)) );
    if( !name_is_png ) {
      ss << ".png" ;
    }
    string png_name = ss.str();
    cout << " reference png file: " << png_name << endl;
    cv::Mat source(mdims[1],mdims[0],CV_64F);
    memcpy(source.ptr<double>(0), data, mdims[0]*mdims[1]);
    cv::flip( -1.0 * source.clone(), source, 1 /* flipVert */ );
    double dmin, dmax;
    cv::minMaxLoc( source, &dmin, &dmax );
    cv::Mat saveImg = (255.0 / (dmax - dmin )) * (source - dmin);
    cv::imwrite(png_name, saveImg );
    cout<<"wrote to " << png_name << endl;
}

void KSegmentorBase::initializeUserInputImageWithContour(bool accumulate){
    this->m_UpdateVector.clear();
    this->m_CoordinatesVector.clear();
    unsigned int element=0;
    std::vector<unsigned int> coord;
    ptrCurrImage = static_cast<unsigned short*>(imageVol->GetScalarPointer());
    ptrCurrLabel = static_cast<unsigned short*>(labelVol->GetScalarPointer());
    ptrIntegral_Image = static_cast<double*>(U_Integral_image->GetScalarPointer());
    for (int i=0; i<=this->dimx-1; i++) {
        for (int j=0; j<=this->dimy-1; j++)  {
            for (int k=0; k<=this->dimz-1; k++) {
                element=k*dimx*dimy +j*dimx+i;
                if(accumulate) {
                    this->accumulateUserInputInUserInputImages((double)ptrCurrLabel[element],element);
                    // not sure what the intent of this is
                }
                // force the initial U based on loaded label
                ptrIntegral_Image[element] = (double)( +3.0 * ( ptrCurrLabel[element]<=0 )
                                                       -3.0 * ( ptrCurrLabel[element]>0 ) );
                if(ptrCurrLabel[element]>0)
                {
                    this->AddPointToUpdateVector(element);
                    coord.push_back(i);
                    coord.push_back(j);
                    coord.push_back(k);
                    this->AddPointToCoordinatesVector(coord);
                    coord.clear();
                }
        }
     }
  }
  this->integrateUserInputInUserInputImage();
    double spc[3];
    this->U_Integral_image->GetSpacing(spc);

    /*vtkMetaImageWriter* labelWriter=  vtkMetaImageWriter::New();
    labelWriter->SetInput(createVTKImageFromPointer<double>(this->ptrIntegral_Image,this->U_Integral_image->GetDimensions(), spc) );
    labelWriter->SetFileName( "0-Integral0.mhd");
    labelWriter->Write();

    labelWriter->SetInput(this->U_Integral_image );
    labelWriter->SetFileName( "0-IntegralImage0.mhd");
    labelWriter->Write();*/
  this->m_UpdateVector.clear();
  this->m_CoordinatesVector.clear();
}


void KSegmentorBase::accumulateUserInputInUserInputImages( double value,const unsigned int element){

    double Umax = this->GetUmax();
    //cout << "using UMax = " << Umax << endl;
    double user_input      = -1.0 * ( value > 0.5 ) + 1.0 * ( value <= 0.5 );
    this->ptrU_t_Image[element]=user_input;
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


void KSegmentorBase::CreateLLs(LLset& ll)
{
    if (ll.Lz!=NULL){
        //destroy linked lists
        ll_destroy(ll.Lz);
        ll_destroy(ll.Ln1);
        ll_destroy(ll.Ln2);
        ll_destroy(ll.Lp1);
        ll_destroy(ll.Lp2);
        ll_destroy(ll.Lin2out);
        ll_destroy(ll.Lout2in);
        ll_destroy(ll.Lchanged);
    }

    //create linked lists
    ll.Lz  = ll_create();
    ll.Ln1 = ll_create();
    ll.Ln2 = ll_create();
    ll.Lp1 = ll_create();
    ll.Lp2 = ll_create();
    ll.Lin2out = ll_create();
    ll.Lout2in = ll_create();
    ll.Lchanged =ll_create();
}

void KSegmentorBase::intializeLevelSet3D(){

    //initialize lists, phi, and labels
    ls_mask2phi3c_ext(mask,phi,label,dims,LL3D.Lz,LL3D.Ln1,
                                    LL3D.Ln2,LL3D.Lp1,LL3D.Lp2,LL3D.Lchanged);
    cout << "initialized levelset 3D. len(lz) = " << LL3D.Lz->length
         << ", len(Lchanged) = " << LL3D.Lchanged << endl;
}

void KSegmentorBase::copyIntegralDuringPaste(int kFrom, int kTo)
{
    std::vector <unsigned int> coord(3);
    unsigned int element=0;
    for (int i=0;i<=this->dimx-1; i++)  {
        for (int j=0; j<=this->dimy-1; j++) {
            double val = this->U_Integral_image->GetScalarComponentAsDouble(i,j,kFrom,0);
            this->U_Integral_image->SetScalarComponentFromDouble(i,j,kTo,0,val);
            if(fabs(val)>0)
            {
                element=kTo*dimx*dimy +j*dimx+i;
                this->AddPointToUpdateVector(element);
                coord[0] = (i);
                coord[1] = (j);
                coord[2] = (kTo);
                this->AddPointToCoordinatesVector(coord);
            }
        }
    }
}

KSegmentorBase::~KSegmentorBase()
{
}


}
