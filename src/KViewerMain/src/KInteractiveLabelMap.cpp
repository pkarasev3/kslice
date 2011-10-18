#include "KInteractiveLabelMap.h"
#include "KDataWarehouse.h"
#include "KSandbox.h"
#include "vtkImageData.h"
#include "KWidget_2D_left.h"
#include <vector>

using std::vector;
using std::string;
using namespace vrcl;

#define SP( X )  vtkSmartPointer<X> 

namespace {
struct BogusDataException : public std::exception  {
  virtual const char* what() const throw ()    {
    return "kv_opts and/or image data are bogus!";
  }
};
const static string PrintVerbose = "VerboseKInteractiveLabelMap";
}

KInteractiveLabelMap::KInteractiveLabelMap()
{  
  SETL( PrintVerbose );

  // Initializiation! Allocate my stuff
  labelActor2D            = vtkSmartPointer<vtkImageActor>::New();
  labelReslicer           = vtkSmartPointer<vtkImageReslice>::New();
  colorMap                = vtkSmartPointer<vtkImageMapToColors>::New();
  label2D_shifter_scaler  = vtkSmartPointer<vtkImageShiftScale>::New();
  labelLUT                = vtkSmartPointer<vtkLookupTable>::New();
  labelDataArray          = vtkSmartPointer<vtkImageData>::New();
  
  sourceWidget            = NULL;
  labelOpacity2D          = 0.9f;
  labelInterpolate        = true;
}

void KInteractiveLabelMap::RegisterSourceWidget(KWidget_2D_left *kwidget2D)
{
  sourceWidget = kwidget2D; // get a 'reverse handle' on the widget whose container I'm in
  
  RegisterState(    kwidget2D->kv_opts );
  RegisterNewImage( kwidget2D->kv_data->imageVolumeRaw, kwidget2D->multiLabelMaps.size() );
  
}

vector<double> get_good_color_0to7( int idx )
{
  double rgb[3];
  switch( idx )
  {
  case 0:
    rgb = {240, 163, 255}; // amethyst
    break;
  case 1:
    rgb = {255,0,16}; // red
    break;
  case 2:
    rgb = {0,117,220}; // blue
    break;
  case 3:
    rgb = {0,153,143}; // turquoise
    break;
  case 4:
    rgb = {194,0,136}; // mallow
    break;
  case 5:
    rgb = {255,80,5}; // zinnia
    break;
  case 6:
    rgb = {43,206,72}; // green
    break;
  case 7:
    rgb = {224,255,102}; // uranium
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

void KInteractiveLabelMap::RegisterNewImage( vtkImageData* image, int index )
{
  if( NULL == kv_opts || NULL == image || NULL == sourceWidget) {
    throw BogusDataException();
  }
  // clone to get image meta-data,then make blank label data
  labelDataArray->DeepCopy(image);

  short *ptrLabel=static_cast<short*>(labelDataArray->GetScalarPointer());
  unsigned int numPts = labelDataArray->GetNumberOfPoints();  assert(0!=ptrLabel);
  unsigned long elemNum = 0;
  while( elemNum < numPts ) { // Zero it out
    ptrLabel[elemNum] = 0;
    elemNum++;
  }
  labelDataArray->Update();

  int slice_idx = sourceWidget->currentSliceIndex;

  // convert it to unsigned short, our desired internal method...
  labelDataArray  = image2ushort( labelDataArray );
  
  // grab a handle on the image ...
  imageVolume     = image;

  ksegmentor = Ptr<KSegmentor>(new KSegmentor(imageVolume,labelDataArray, slice_idx)  );

  // connect the GUI display for this label map.
  // later a renderer needs to add the actor
  double maxVal = kv_opts->drawLabelMaxVal;
  cout << "new interactive labelmap with actors, max val = " << maxVal << endl;
  vector<double> labelRGB = get_good_color_0to7(index);
  labelLUT = create_default_labelLUT( maxVal, labelRGB );
  label2D_shifter_scaler->SetInput( labelDataArray );
  labelReslicer->SetInputConnection( label2D_shifter_scaler->GetOutputPort() );
  labelReslicer->SetOutputDimensionality(2);  //drawing just a single slice
  labelReslicer->SetResliceAxesDirectionCosines(1,0,0,    0,1,0,     0,0,1);
  labelReslicer->SetResliceAxesOrigin(0,0,slice_idx);
  colorMap->SetLookupTable(labelLUT);
  colorMap->SetInputConnection( labelReslicer->GetOutputPort() );
  colorMap->Update();
  labelActor2D->SetInput( colorMap->GetOutput() );
  labelActor2D->SetOpacity( kv_opts->labelOpacity2D );
  labelActor2D->SetInterpolate( kv_opts->labelInterpolate );
}



#if 0    // Death Row

//imageVolumeRaw = image2ushort( image2D );

//kv_opts stores parameters from data for persistent scope
//  std::vector<double> dataSpacing(3);
//  dataSpacing[0]= imgReader->GetDataSpacing()[0];
//  dataSpacing[1]= imgReader->GetDataSpacing()[1];
//  dataSpacing[2]= imgReader->GetDataSpacing()[2];

//  int* dataExtent         = imgReader->GetDataExtent();
//  kv_opts->imageSpacing   = dataSpacing;
//  kv_opts->numSlices      = dataExtent[5]-dataExtent[4]+1;
//  kv_opts->sliceZSpace    = kv_opts->imageSpacing[2];
//  kv_opts->sliderMin      = double(image2D->GetOrigin()[2]);
//  kv_opts->sliderMax      = double(kv_opts->numSlices)*(kv_opts->imageSpacing)[2] + kv_opts->sliderMin;
//  kv_opts->imgHeight            = imgReader->GetHeight();
//  kv_opts->imgWidth            = imgReader->GetWidth();

//  std::vector<double> imgOrigin(3);
//  memcpy( &(imgOrigin[0]), imgReader->GetDataOrigin(), 3 * sizeof(double) );
//  kv_opts->imageOrigin    = imgOrigin;
//  std::vector<int> imgExtent(6);
//  memcpy( &(imgExtent[0]), imgReader->GetDataExtent(), 6 * sizeof(int) );
//  kv_opts->imageExtent    = imgExtent;
//  cout << "image origin and extent: " << Mat( imgOrigin )
//                                      << " ... " << Mat( imgExtent ) << endl;

//void KInteractiveLabelMap::UpdateLabelMap( 
//    vtkSmartPointer<vtkImageData> label_image_data /* new (?) */ )
//{
//// TODO: connect the label_image_data, 'push to current'
//    labelReslicer->SetInputConnection( label2D_shifter_scaler->GetOutputPort() );
//    labelReslicer->SetOutputDimensionality(2);
//    labelReslicer->SetResliceAxesDirectionCosines(1,0,0, 0,1,0, 0,0,1);
//    labelReslicer->SetResliceAxesOrigin(0,0,labelActor2D->GetSliceNumber());

//    colorMap->SetLookupTable(labelLUT);
//    colorMap->SetInputConnection(labelReslicer->GetOutputPort());
//    colorMap->Update();

//    labelActor2D->SetInput(colorMap->GetOutput());
//    labelActor2D->SetOpacity(labelOpacity2D);
//    labelActor2D->SetInterpolate(labelInterpolate);

//}



#endif
