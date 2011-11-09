#include "KSegmentor.h"
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

/** default curvature penalty term. can be set externally when a KSegmentor is made. */
double KSegmentor::defaultKappaParam = 0.2;

void KSegmentor::saveMatToPNG( const cv::Mat& sliceImg, const std::string& fileName )
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

void KSegmentor::saveCurrentSliceToPNG(const std::string &fileName) {
    cout << "saving png file, currSlice = " << currSlice << " ... ";
    saveMatToPNG(U_integral[currSlice], fileName );
}


KSegmentor::KSegmentor(vtkImageData *image, vtkImageData *label, int sliceIndex, bool contInit)
{
        int num_slices = image->GetDimensions()[2];
        this->imageVol=image;
        this->labelVol=label;
        this->currSlice=sliceIndex;
        this->mdims = new int[3];
        image->GetDimensions( mdims );
        this->numberdims=2;

        this->mdims[2] = 1;
        this->penaltyAlpha=0;
        this->seed=0;
        this->useContInit=contInit;
        this->display=0;
        this->dthresh=500;
        this->iter=200;
        this->lambda=defaultKappaParam; // this could/should be user togglable!

        Lz=NULL;
        Ln1= NULL;
        Ln2= NULL;
        Lp1=NULL ;
        Lp2=NULL ;
        Sz=NULL ;
        Sn1=NULL ;
        Sn2=NULL ;
        Sp1=NULL ;
        Sp2=NULL ;
        Lin2out=NULL ;
        Lout2in=NULL ;
        this->seg=NULL;
        this->img=NULL;
        this->mask=NULL;

        double spacing_mm[3];
        image->GetSpacing( spacing_mm );

        // want rad to be '10' for 512 . A 512x512 mri with xy spacing 0.3mm is 153.6000 across
        // "10" pixels is 3mm in this context.
        this->rad = 3.0 / std::max( spacing_mm[0],spacing_mm[1] ); // about 3mm in physical units
        this->rad = std::min(18.0,this->rad); // force non-huge radius if the spacing is retarded
        this->rad = std::max(3.0, this->rad); // force non-tiny radius if the spacing is retarded
        cout << "segmentor using ROI size: " << rad << endl;

        this->img        = new double[ mdims[0]*mdims[1] ];
        this->mask       = new double[ mdims[0]*mdims[1] ];
        this->U_I_slice  = new double[ mdims[0]*mdims[1] ];
        this->U_t_slice  = new double[ mdims[0]*mdims[1] ];

        this->U_Integral_image = vtkSmartPointer<vtkImageData>::New();
        this->U_t_image= vtkSmartPointer<vtkImageData>::New();


        this->U_Integral_image->SetExtent(image->GetExtent());
        this->U_Integral_image->SetScalarTypeToDouble();
        this->ptrIntegral_Image = static_cast<double*>(this->U_Integral_image->GetScalarPointer());

        this->U_t_image->SetExtent(image->GetExtent());
        this->U_t_image->SetScalarTypeToDouble();
        this->ptrU_t_Image = static_cast<double*>(this->U_t_image->GetScalarPointer());


        this->m_Reslicer = vtkSmartPointer<vtkImageReslice>::New();


        this->imgRange   = new double[2];
        this->labelRange = new double[2];

        this->phi        = new double[mdims[0]*mdims[1]*mdims[2]];
        this->label      = new double[mdims[0]*mdims[1]*mdims[2]];

        U_integral = std::vector< cv::Mat >( num_slices );
        U_t        = std::vector< cv::Mat >( num_slices );
        cout << "I think the # of rows is: " << mdims[0] << ", # of cols is: " << mdims[1] << endl;
        for( int k = 0; k < num_slices; k++ )
        { /** per-slice user updates  */
          U_integral[k] = cv::Mat::zeros( mdims[1], mdims[0], CV_64F );
          U_t[k]        = cv::Mat::zeros( mdims[1], mdims[0], CV_64F );
        }

        this->iList=NULL;
        this->jList=NULL;

        numdims=numberdims;

        if(this->useContInit)
        {
            this->initializeUserInputImageWithContour();
            std::cout<<"Initializing user input using label data"<<std::endl;
        }

        dims[2] = 1;
        dims[1] = 1;
        switch(numdims){
        case 3:
          dimz = (int)mdims[2];
          dims[2] = dimz;
        case 2:
          dimx = (int)mdims[1];
          dims[1] = dimx;
        case 1:
          dimy = (int)mdims[0];
          dims[0] = dimy;
        }
        dims[3] = dims[0]*dims[1];
        dims[4] = dims[0]*dims[1]*dims[2];

        this->seg = new  unsigned short[dims[0]*dims[1]*dims[2]];
        initializeData();
       intializeLevelSet();
}


void KSegmentor::initializeUserInputImageWithContour(){
    int element=0;
    unsigned short* label3DPointer = static_cast<unsigned short*>(labelVol->GetScalarPointer());
    for (int i=0; i<=imageVol->GetExtent()[1]-1; i++)  {
        for (int j=0; j<=imageVol->GetExtent()[3]-1; j++) {
            for(int k=0;k<=imageVol->GetExtent()[5]-1;k++){
               //this->accumulateAndIntegrateUserInputInUserImages( this->labelVol->GetScalarComponentAsDouble(i,j,k,0),i,j,k);
                this->accumulateUserInputInUserInputImages((double)label3DPointer[element],element);
                element++;
        }
     }
  }
  for(int k=0;k<=imageVol->GetExtent()[5]-1;k++)
    this->integrateUserInputInUserInputImage(k);
}

void KSegmentor::accumulateUserInput( double value, int j, int i, int k )
{ // clicking: set the U_t values, +/- 1.0 ( phi < 0 is inside! )
    assert( j < U_t[k].cols );
    assert( i < U_t[k].rows );
    double user_input      = -1.0 * ( value > 0.5 ) + 1.0 * ( value <= 0.5 );
    U_t[k].at<double>(i,j) = user_input ;

}

void KSegmentor::accumulateUserInputInUserInputImages( double value,const unsigned int element)
{
    double user_input      = -1.0 * ( value > 0.5 ) + 1.0 * ( value <= 0.5 );
    this->ptrU_t_Image[element]=user_input;
    //Integrate user input
    //this->U_t_image->SetScalarComponentFromDouble(i,j,k,0,0.5*this->U_Integral_image->GetScalarComponentAsDouble(i,j,k,0));
    //this->ptrU_t_Image[element]=this->ptrIntegral_Image[element]*0.5;
}

void KSegmentor::integrateUserInput( int k )
{
    U_integral[k]  += U_t[k];
    U_t[k]          = 0.5 * U_t[k];
}

void KSegmentor::integrateUserInputInUserInputImage( int k )
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


void KSegmentor::setNumIterations(int itersToRun){
    this->iter=itersToRun;
}



void KSegmentor::TransformUserInputImages(vtkTransform* transform, bool invert)
{
    this->m_Reslicer->SetInput(this->U_Integral_image);
    this->m_Reslicer->SetResliceAxesDirectionCosines(1,0,0,    0,1,0,     0,0,1);
    this->m_Reslicer->SetOutputDimensionality(3);
    if (invert)
       this->m_Reslicer->SetResliceTransform(transform->GetInverse());
    else
      this->m_Reslicer->SetResliceTransform(transform);
    this->m_Reslicer->UpdateWholeExtent();
    this->U_Integral_image->DeepCopy(m_Reslicer->GetOutput());
    this->ptrIntegral_Image = static_cast<double*>(this->U_Integral_image->GetScalarPointer());

    this->m_Reslicer->SetInput(this->U_t_image);
    this->m_Reslicer->SetResliceAxesDirectionCosines(1,0,0,    0,1,0,     0,0,1);
    this->m_Reslicer->Modified();
    this->m_Reslicer->UpdateWholeExtent();
    this->U_t_image->DeepCopy(m_Reslicer->GetOutput());
    this->ptrU_t_Image = static_cast<double*>(this->U_t_image->GetScalarPointer());

    this->mdims[0]=U_t_image->GetExtent()[1]-U_t_image->GetExtent()[0]+1;
    this->mdims[1]=U_t_image->GetExtent()[3]-U_t_image->GetExtent()[2]+1;
}

void KSegmentor::initializeData()
{
    this->ptrCurrImage = static_cast<unsigned short*>(
        imageVol->GetScalarPointer())+(unsigned short)currSlice*mdims[0]*mdims[1];
    this->ptrCurrLabel = static_cast<unsigned short*>(
        labelVol->GetScalarPointer())+(unsigned short)currSlice*mdims[0]*mdims[1];

    imageVol->GetScalarRange( imgRange );
    labelVol->GetScalarRange( labelRange );
    if( abs(labelRange[1]) < 1e-3 )
    { // empty label; so set the proper range
      labelRange[1] = KViewerOptions::getDefaultDrawLabelMaxVal();
    }
    cout << "KSegmentor reads label range: " << labelRange[0] << "," << labelRange[1] << endl;
    cout << "KSegmentor reads image range: " << imgRange[0] << "," << imgRange[1] << endl;
    assert( 0 != imgRange[1] ); // what the, all black ?? impossible !

    this->imgRange=imgRange;
    this->integrateUserInput( currSlice );
    this->integrateUserInputInUserInputImage(currSlice);
    unsigned int element3D;
    long elemNum=0;
    for (int j=0; j<=mdims[1]-1; j++)  {
        for (int i=0; i<=mdims[0]-1; i++) {
          // indexing definition:  ptr[k*mdims[1]*mdims[0] +j*mdims[0]+i];
          this->img[elemNum]        = (double) ptrCurrImage[elemNum];
          this->mask[elemNum]       = (double) ( 0 < ptrCurrLabel[elemNum] );
          //this->U_I_slice[elemNum]  = (U_integral[currSlice].ptr<double>(0))[elemNum];
          //New
          element3D=this->currSlice*mdims[1]*mdims[0] +j*mdims[0]+i;
          this->U_I_slice[elemNum] =this->ptrIntegral_Image[element3D];
          elemNum++;
        }
    }

    bool writeInfoPng = false;
    if( writeInfoPng ) {
      /*std::stringstream ss;
      ss << "U_integral_ " << std::setw(3) << std::setfill('0') << currSlice << ".png";
      saveCurrentSliceToPNG( ss.str() );*/
        vtkSmartPointer<vtkMetaImageWriter> labelWriter=   vtkSmartPointer<vtkMetaImageWriter>::New();
        labelWriter->SetInput( this->U_Integral_image );
        labelWriter->SetFileName( "Integral.mhd");
        labelWriter->Write();
    }
}

void KSegmentor::setCurrLabelArray(vtkImageData *label){
    this->labelVol=label;
}

void KSegmentor::intializeLevelSet(){

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

void KSegmentor::Update()
{
    interactive_chanvese(img,phi,U_I_slice,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in,iter,rad,lambda,display);

  if(iList!=NULL){
      delete[] iList;
  }
  if(jList!=NULL){
      delete[] jList;
  }

  //get number and coordinates of point (row, col) on the zero level set
  prep_C_output(Lz,dims,phi, &iList, &jList, lengthZLS);


  //threshold phi to find segmentation label, assign it to appropriate range of label!
  long elemNum=0;
  for (int j=0; j<=mdims[1]-1; j++)  {
      for (int i=0; i<=mdims[0]-1; i++) {
          double phi_val = phi[elemNum];
          double phi_out = (-phi_val + 3.0) / 6.0; // shift and scale from [-3,3] to [0,1]
          ptrCurrLabel[elemNum]= (unsigned short) ( ( (phi_out > 0.95) + (phi_out > 0.8) + (phi_out > 0.65) + (phi_out > 0.5) ) * labelRange[1] / 4.0 );
          if( phi_val <0){
              seg[elemNum]=labelRange[1];
          }else{
              seg[elemNum]=labelRange[0];
          }
          elemNum++;
      }
  }
  prevSlice = currSlice;
}

void KSegmentor::copyIntegralDuringPaste(int kFrom, int kTo)
{

  Mat tmp =0.9  * U_integral[kFrom];
  tmp.copyTo(U_integral[kTo]);
  for (int i=0;i<=mdims[0]-1; i++)  {
      for (int j=0; j<=mdims[1]-1; j++) {
          double val=0.9*this->U_Integral_image->GetScalarComponentAsDouble(i,j,kFrom,0);
          this->U_Integral_image->SetScalarComponentFromDouble(i,j,kTo,0,val);
      }
  }
}

void KSegmentor::setCurrIndex(int sliceIndex){
    this->currSlice=sliceIndex;
}

KSegmentor::~KSegmentor(){
        delete [] this->mdims;
        delete [] this->img;
        delete [] this->mask;
        delete [] this->imgRange;
        delete [] this->labelRange;
        delete [] this->phi;
        delete [] this->label;
        delete [] this->seg;
        delete [] this->iList;
        delete [] this->jList;
        delete [] this->U_I_slice;
        delete [] this->U_t_slice;

        ll_destroy(Lz);
        ll_destroy(Ln1);
        ll_destroy(Ln2);
        ll_destroy(Lp1);
        ll_destroy(Lp2);
        ll_destroy(Lin2out);
        ll_destroy(Lout2in);
}


KSegmentor::KSegmentor(){
  throw "Oh snap my default constructor is bogus and doesn't allocate my stuff";
}


}
