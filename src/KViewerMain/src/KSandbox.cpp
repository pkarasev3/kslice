#include "KSandbox.h"
#include "vtkImageCast.h"
#include "vtkImageData.h"
#include "vtkImageResample.h"
#include "vtkLookupTable.h"
#include "KDataWarehouse.h"
#include <vector>
#include "vtkDICOMImageReader.h"
#include "vtkImageAccumulate.h"
#include "vtkImageGaussianSmooth.h"
#include "opencv2/imgproc/imgproc.hpp"

#define SP( X )  vtkSmartPointer<X>

#define BE_EVIL  0
#if BE_EVIL
#include "itkImage.h"
#include "itkImageToVTKImageFilter.h"
#include "itkVTKImageToImageFilter.h"
#include "itkImageDuplicator.h"
#include "itkImageFileWriter.h"
#endif

using std::vector;
using namespace cv;

namespace vrcl  {

vector<double> get_good_color_0to7( int idx )
{
  double rgb[3];
  switch( idx )
  {
  case 0:
    rgb = {0,117,220}; // blue
    break;
  case 1:
    rgb = {255,0,16}; // red
    break;
  case 2:
    rgb = {43,206,72}; // green
    break;
  case 3:
    rgb = {224,255,102}; // uranium
    break;
  case 4:
    rgb = {194,0,136}; // mallow
    break;
  case 5:
    rgb = {255,80,5}; // zinnia
    break;
  case 6:
    rgb = {220, 163, 255}; // amethyst
    break;
  case 7:
    rgb = {0,153,143}; // turquoise
    break;
  default:
    rgb = {200,200,200};
    break;
  }
  rgb[0] /= 255.0;
  rgb[1] /= 255.0;
  rgb[2] /= 255.0;
  return std::vector<double>(rgb,rgb+3);
}

vtkSmartPointer<vtkImageData> image2ushort( vtkImageData* imageData )
{
  int dims[3];
  imageData->GetDimensions( dims );
  double img_range[2];
  imageData->GetScalarRange(img_range);
  double spc[3];
  imageData->GetSpacing( spc );

  vtkSmartPointer<vtkImageData> imgvol = vtkSmartPointer<vtkImageData>::New( );
  imgvol->SetDimensions( dims[0],dims[1],dims[2] );
  imgvol->SetNumberOfScalarComponents(1);
  imgvol->SetSpacing( spc );
  imgvol->SetOrigin( 0,0,0 );
  imgvol->SetScalarTypeToUnsignedShort( );
  imgvol->AllocateScalars( );

  // Values stored 'linearly'
  unsigned short*  outputPtr = (unsigned short *) imgvol->GetScalarPointer();
  short *inputPtr = static_cast<short*>( imageData->GetScalarPointer() );
  int numel                 = dims[0]*dims[1]*dims[2];
  for( int i=0; i<numel; i++ )
  { // intentionally unwrapped for debugging; some files have weird data ...
    double invalue            =  (double) inputPtr[i] - img_range[0];
    unsigned short nextvalue = (unsigned short ) invalue ;
    outputPtr[i]= nextvalue;
  }

  return imgvol;
}

void multiplyImageByLabels( vtkImageData* imgData, vtkImageData* lblData )
{
  int dims[3];
  imgData->GetDimensions( dims );
  int numel                 = dims[0]*dims[1]*dims[2];
  unsigned short*  img = (unsigned short *) imgData->GetScalarPointer();
  unsigned short*  lbl = (unsigned short *) lblData->GetScalarPointer();
  for( int i=0; i<numel; i++ )
  {
    unsigned short ival =  img[i];
    unsigned short lval =  lbl[i];
    img[i] = ( lval > 0 ) * ival;
  }
}

void compute_intensity_modes( vtkImageData* image, std::vector<double>& intensityModes )
{
  double range[2];
  image->GetScalarRange( range );
  vtkSmartPointer<vtkImageAccumulate> histogram =  vtkSmartPointer<vtkImageAccumulate>::New();
  histogram->SetInput( image );
  histogram->SetComponentExtent( 0, static_cast<int>(range[1])-static_cast<int>(range[0])-1,0,0,0,0 );
  histogram->SetComponentOrigin( 0,0,0 );
  histogram->SetComponentSpacing( 1,0,0 );
  histogram->SetIgnoreZero( 1 );
  histogram->Update();
  vtkSmartPointer<vtkImageData>  imhist = histogram->GetOutput( );

  int numPts = imhist->GetNumberOfPoints();
  cout << "num histogram points: " << numPts << endl;
  int numKeep = 5; // how many modes to compute

  unsigned short*  imhist_ptr = (unsigned short *) imhist->GetScalarPointer();
  intensityModes = std::vector<double>(numKeep);
  int min_k   = (int) range[1] * 0.1; // ignore the "almost zero" crap

  for( int i = 0; i < numKeep; i++ )
  { // brute force way to find the modes...
    int max_k = -1;
    unsigned short maxVal = 0;
    for( int k = min_k; k < numPts; k++ ) {
      if( imhist_ptr[k] > maxVal ) {
        maxVal = imhist_ptr[k];
        max_k  = k;
        imhist_ptr[k] = 0;
      }
    }
    if( max_k == -1 ) {
      std::cout << "Warning, mode-finding failed. Image seems badly un-equalized or skewed!" << std::endl;
      max_k = 0;
    }
    intensityModes[i] = max_k;
    std::cout << "intensity mode at " << intensityModes[i]
              << " of " << numPts << std::endl;
  }



}


void print_dereferenced_vtkSmartPointer_pair( SP(vtkObject) obj1, SP(vtkObject) obj2) {
  // e.g. make sure they're not the same memory address...
  vtkObject*  obj1_ptr            = &(*obj1);
  vtkObject*  obj2_ptr            = &(*obj2);

  bool ptr_equal = (obj1_ptr == obj2_ptr );
  std::string  print_output = ptr_equal ? "EQUAL " : "NOT EQUAL " ;
  cout << print_output << ", Obj1: " << obj1_ptr << ", Obj2: " << obj2_ptr << endl;

}

void check_extents( vtkImageData* input ) {
  int label_extents[6];
  input->GetExtent( label_extents );
  assert( label_extents[1] > 0 );
}

SP(vtkLookupTable)  create_default_labelLUT( double maxVal, const std::vector<double>& rgb_primary )
{
  /**  Create the color lookup table for labels. Color is given as input, or a default used if it's empty.
    *  Set opacity to be higher at the edge of label to show contour. "maxVal" should be on the order of
    *  the image data being shown in conjunction with the label, probably from one to several thousand.
    */
  double pR,pG,pB;
  if( rgb_primary.empty() ) {
    pR = 1.0; pG = 0.0; pG = 0.5;
  } else {
    pR = rgb_primary[0];
    pG = rgb_primary[1];
    pB = rgb_primary[2];
  }
  SP(vtkLookupTable) labelLUT = SP(vtkLookupTable)::New();
  double mid[4]  = {pR,pG,pB,0.4};
  double far[4]  = {pR,pG,pB,0.5};
  double near[4] = {pR,pG,pB,0.3};
  double transparent[4] = {0,0,0,0};
  double main[4]         = {pR,pG,pB,0.3};

  labelLUT->SetNumberOfTableValues( maxVal );
  labelLUT->SetTableRange(0,maxVal);
  labelLUT->SetTableValue(0,   transparent);
  labelLUT->SetTableValue(maxVal/4,     far);
  labelLUT->SetTableValue(maxVal/2,     mid);
  labelLUT->SetTableValue(3*maxVal/4,   near);
  labelLUT->SetTableValue(maxVal-1,     main);
  labelLUT->SetRampToLinear();
  labelLUT->Build();

  IFLOG("create_default_labelLUT_verbose", cout << " Created default Label LUT! " << endl; )
      return labelLUT;
}

void setup_file_reader(Ptr<KViewerOptions> kv_opts, Ptr<KDataWarehouse> kv_data) {

  //create two readers one for the image and one for the labels
  SP(vtkMetaImageReader) labelFileReader = SP(vtkMetaImageReader)::New();
  SP(vtkMetaImageReader) imgReader       = SP(vtkMetaImageReader)::New();
  //Why does name contain "2D" when it ist storing 3D information??
  // Gute Frage!
  SP(vtkImageData)       label2D   ;//      = SP(vtkImageData)::New();
  SP(vtkImageData)       image2D;//         = SP(vtkImageData)::New();

  //test if we can read each file
  int canReadLab = 0;               // try to read multiple labels later
  int canReadImg = imgReader->CanReadFile(kv_opts->ImageArrayFilename.c_str());
  if ( canReadLab == 0 )
  {		// can't read label file, try to create a blank one
    if( canReadImg==0)
    {
      cout<<"Could not read file "<<kv_opts->ImageArrayFilename<<" \n";
      // TODO: return "fail" and have them re-select a file...
      exit(-1);
    }
    imgReader->SetFileName(kv_opts->ImageArrayFilename.c_str());
    imgReader->SetDataScalarTypeToUnsignedShort();
    imgReader->Update();

    std::string byteOrderName( imgReader->GetDataByteOrderAsString() );
    std::cout << "byte orderness of " << kv_opts->ImageArrayFilename << ": " << byteOrderName << endl;

    //?????
    // read the image file into the label array to force correct type & meta-data
    std::string fakeLabelFileName = kv_opts->ImageArrayFilename;
    labelFileReader->SetFileName( fakeLabelFileName.c_str() );
    labelFileReader->Update();

    label2D = labelFileReader->GetOutput();
    image2D = imgReader->GetOutput();
    int* imgDim = imgReader->GetDataExtent();
    int imin=imgDim[0];             int imax=imgDim[1];            int jmin=imgDim[2];
    int jmax=imgDim[3];             int kmin=imgDim[4];            int kmax=imgDim[5];

    short *ptrLabel=static_cast<short*>(label2D->GetScalarPointer());
    long elemNum=0;
    for (int k=kmin; k<=kmax; k++) {
      for (int i=imin; i<=imax; i++)  {
        for (int j=jmin; j<=jmax; j++) {
          ptrLabel[elemNum] = 0;
          elemNum++;
        }
      }
    }

    label2D->Update();
    image2D->Update();


  } else if(canReadImg==0){
    cout<<"Could not read file"<<kv_opts->ImageArrayFilename<<"\n";
    exit(-1);
  }

  // we null out the origin information, so image and world coords correspond
  // easier for drawing with mouse
  double ox, oy, oz;
  image2D->GetOrigin( ox, oy, oz);
  cout << "original input image origin: " << ox << ", " << oy << ", " << oz << endl;
  ox = 0;
  oy = 0;
  oz = 0;
  image2D->SetOrigin( ox, oy, oz );
  label2D->SetOrigin( ox, oy, oz );


  //kv_opts stores parameters from data for persistent scope
  std::vector<double> dataSpacing(3);
  dataSpacing[0]= imgReader->GetDataSpacing()[0];
  dataSpacing[1]= imgReader->GetDataSpacing()[1];
  dataSpacing[2]= imgReader->GetDataSpacing()[2];

  ///Difference between imageExtent and dataExtent variables???
  int* dataExtent         = imgReader->GetDataExtent();
  kv_opts->imageSpacing   = dataSpacing;
  kv_opts->numSlices      = dataExtent[5]-dataExtent[4]+1;
  kv_opts->sliceZSpace    = dataSpacing[2];
  kv_opts->sliderMin      = double(image2D->GetOrigin()[2]);
  kv_opts->sliderMax      = double(kv_opts->numSlices)*(kv_opts->imageSpacing)[2]
      + kv_opts->sliderMin;
  kv_opts->imgHeight            = dataExtent[3]-dataExtent[2]+1;
  kv_opts->imgWidth            = dataExtent[1]-dataExtent[0]+1;

  kv_opts->minIntensity        = image2D->GetScalarRange()[0]*1.1;
  kv_opts->maxIntensity        = image2D->GetScalarRange()[1];

  std::vector<double> imgOrigin(3);
  memcpy( &(imgOrigin[0]), image2D->GetOrigin(), 3 * sizeof(double) );
  kv_opts->imageOrigin    = imgOrigin;
  std::vector<int> imgExtent(6);
  memcpy( &(imgExtent[0]), image2D->GetExtent(), 6 * sizeof(int) );
  kv_opts->imageExtent    = imgExtent;
  cout << "image origin and extent: " << Mat( imgOrigin )
       << " ... " << Mat( imgExtent ) << endl;

  //Should not be here!
  kv_data->imageVolumeRaw = vtkSmartPointer<vtkImageData>::New();
  kv_data->labelDataArray = vtkSmartPointer<vtkImageData>::New();

  bool forceToUShort      = true;
  if( !forceToUShort ) {
    kv_data->UpdateRawImage(image2D) ;
    kv_data->UpdateLabelDataArray( label2D) ;
  } else {
    kv_data->UpdateRawImage( image2ushort( image2D ));
    kv_data->UpdateLabelDataArray( image2ushort( label2D ));
  }
  kv_opts->m_Center[0]= (kv_opts->imageExtent[1]*kv_opts->imageSpacing[0]-kv_opts->imageExtent[0]*kv_opts->imageSpacing[0])*0.5;
  kv_opts->m_Center[1]= (kv_opts->imageExtent[3]*kv_opts->imageSpacing[1]-kv_opts->imageExtent[2]*kv_opts->imageSpacing[1])*0.5;
  kv_opts->m_Center[2]= (kv_opts->imageExtent[5]*kv_opts->imageSpacing[2]-kv_opts->imageExtent[4]*kv_opts->imageSpacing[2])*0.5;


  cout<<"ImageArrayFilename: "<<kv_opts->ImageArrayFilename<<"\n";

  //        SP(vtkIndent) indentVTK= SP(vtkIndent)::New();
  //        vtkIndent* p_indentVTK = indentVTK;
  //        std::ofstream  LogFile("kviewer_desktop.log");
  //        kv_data->imageVolumeRaw->PrintSelf(LogFile, *p_indentVTK);
  //        kv_data->labelDataArray->PrintSelf(LogFile, *indentVTK);
  //        LogFile.close();
}

}
