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
//extern double ain, aout, auser; // means
//extern double *pdfin, *pdfout, *pdfuser;
//extern long numdims;
//extern double engEval;
//extern bool UseInitContour;
//extern double *Ain, *Aout, *Sin, *Sout; //local means

namespace vrcl
{
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


void KSegmentorBase::InitializeVariables(vtkImageData* image, vtkImageData* label,
                                         vtkImageData* UIVol,
                                        bool contInit, int currSlice, int numIts, float distWeight, int brushRad)
{
    m_CustomSpeedImgPointer=NULL;
    imageVol=image;
    labelVol=label;

    numberdims=3;

    //set some variables, take the defaults outside
    //(into ksliceOptions constructor (for example) if desired)
    this->useContInit=contInit;
    this->currSlice=currSlice;
    this->iter=numIts;
    this->m_DistWeight=distWeight;
    this->rad= brushRad;

    m_bUseEdgeBased = false;
    penaltyAlpha=0;
    seed=0;
    display=0;


    m_ThreshWeight=0;
    lambda=defaultKappaParam; // this could/should be user togglable!
    mdims = new int[3];
    image->GetDimensions( mdims );

    LL3D.init();
    LL2D.init();
    Sz=NULL ;
    Sn1=NULL ;
    Sn2=NULL ;
    Sp1=NULL ;
    Sp2=NULL ;

    img=NULL;
    mask=NULL;

    image->GetSpacing( m_Spacing_mm );


    cout << "segmentor using ROI size: " << rad << endl;

    U_Integral_image = UIVol; //vtkSmartPointer<vtkImageData>::New();
    U_t_image= vtkSmartPointer<vtkImageData>::New();

//    U_Integral_image->SetExtent(image->GetExtent());
//    U_Integral_image->SetScalarTypeToDouble();
//    U_Integral_image->SetSpacing(image->GetSpacing());
//    U_Integral_image->AllocateScalars();
    ptrIntegral_Image = static_cast<double*>(U_Integral_image->GetScalarPointer());

    U_t_image->SetExtent(image->GetExtent());
    U_t_image->SetScalarTypeToDouble();
    U_t_image->SetSpacing(image->GetSpacing());
    U_t_image->AllocateScalars();
    ptrU_t_Image = static_cast<double*>(U_t_image->GetScalarPointer());

    m_Reslicer = vtkSmartPointer<vtkImageReslice>::New();

    imgRange   = new double[2];
    labelRange = new double[2];

    cout << "I think the # of rows is: " << mdims[1] << ", # of cols is: " << mdims[0] << endl;


    //Set dimensions
    dimz = (int) mdims[2];
    dimy = (int) mdims[1];
    dimx = (int) mdims[0];

    try {
      this->phi        = new double[dimx*dimy*dimz];
      this->label      = new double[dimx*dimy*dimz];
      this->mask       = new double[dimx*dimy*dimz];
      this->img        = new double[dimx*dimy*dimz];
    } catch ( const std::bad_alloc& e) {
      std::cout << "Failed to allocate KSegmentorBase pointers! ";
      std::cout << "Perhaps it is a big data-set on 32-bit OS? " << std::endl;
      exit(-1);
    }

    dims[2] = dimz;
    dims[1] = dimy;
    dims[0] = dimx;

    dims[3] = dims[0]*dims[1];
    dims[4] = dims[0]*dims[1]*dims[2];

    cout << "; initialized KSegmentor3D with dims[0,1,2] = "
         << dims[0] << "," << dims[1] << "," << dims[2] << endl;

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
    ptrCurrImage = static_cast< short*>(imageVol->GetScalarPointer());
    ptrCurrLabel = static_cast< short*>(labelVol->GetScalarPointer());

    for (int i=0; i<=this->dimx-1; i++) {
        for (int j=0; j<=this->dimy-1; j++)  {
            for (int k=0; k<=this->dimz-1; k++) {
                element=k*dimx*dimy +j*dimx+i;
                if(accumulate)
                    this->accumulateUserInput((double)ptrCurrLabel[element],element);
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
  this->integrateUserInput();
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


void KSegmentorBase::accumulateUserInput( double value,const unsigned int element){
    double user_input      = -1.0 * ( value > 0.5 ) + 1.0 * ( value <= 0.5 );
    this->ptrU_t_Image[element]=user_input;
}

void KSegmentorBase::accumulateUserInput( double value, int i, int j, int k) {
    double user_input      = -5.0 * ( value > 0.5 ) + 5.0 * ( value <= 0.5 );
    this->U_Integral_image->SetScalarComponentFromDouble(i,j,k,0,user_input);
    std::cout << "KSegmentorBase::accumulateUserInput " << user_input << " at i,j,k =  "
              << i << "," <<j << ", " << k << std::endl;
    this->U_Integral_image->Modified();
}

void KSegmentorBase::setNumIterations(int itersToRun){
    this->iter=itersToRun;
}

void KSegmentorBase::SetCurrentSlice(int currSlice)
{
    this->currSlice=currSlice;
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

//    //IKDebug
//    std::ofstream maskFile("mask.txt",std::ios_base::out);
//    for(int i=0;i< dimx*dimy*dimz; i++){
//        maskFile<<mask[i]<<' ';
//    }
//    maskFile.close();

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
