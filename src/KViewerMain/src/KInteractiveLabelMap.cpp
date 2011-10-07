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
  labelOpacity2D          = 0.6f;
  labelInterpolate        = true;
}

void KInteractiveLabelMap::RegisterSourceWidget(KWidget_2D_left *kwidget2D)
{
  sourceWidget = kwidget2D; // get a 'reverse handle' on the widget whose container I'm in
  
  RegisterState(    kwidget2D->kv_opts );
  RegisterNewImage( kwidget2D->kv_data->imageVolumeRaw, kwidget2D->multiLabelMaps.size() );
  
}


void KInteractiveLabelMap::RegisterNewImage( vtkImageData* image, int index )
{
  if( NULL == kv_opts || NULL == image || NULL == sourceWidget) {
    throw BogusDataException();
  }
  // clone to get image meta-data,then make blank label data
  labelDataArray->DeepCopy(image);

  int imax=kv_opts->imgHeight-1;
  int jmax=kv_opts->imgWidth-1;
  int kmax=kv_opts->numSlices-1;

  short *ptrLabel=static_cast<short*>(labelDataArray->GetScalarPointer());
  unsigned int numPts = labelDataArray->GetNumberOfPoints();  assert(0!=ptrLabel);
  unsigned long elemNum = 0;
  while( elemNum < numPts ) { // Zero it out
    ptrLabel[elemNum] = 0;
    elemNum++;
  }

  bool forceInitialFill = true;
  if( forceInitialFill )
  {
    int fill_sz = 1;  // need some non-zero part so 3D display doesn't break
    int kmin = kmax/2 - fill_sz;
    if( kmin < 0 )
      kmin = 0;

    for( int k = kmin ; k < kmax; k++ ) {

      for( int i = imax/2 - fill_sz ; i < imax/2+fill_sz; i++ ) {
        for( int j = jmax/2 - fill_sz ; j < jmax/2+fill_sz; j++ ) {
          unsigned long elemNum = k * imax * jmax + j * imax + i;
          ptrLabel[elemNum] = 1000;
        }
      }
    }
  }
  int slice_idx = sourceWidget->currentSliceIndex;

  // convert it to unsigned short, our desired internal method...
  labelDataArray  = image2ushort( labelDataArray );
  
  // grab a handle on the image ...
  imageVolume     = image;

  ksegmentor = Ptr<KSegmentor>(new KSegmentor(imageVolume,labelDataArray, slice_idx)  );

  // connect the GUI display for this label map.
  // later a renderer needs to add the actor
  double maxVal = labelDataArray->GetScalarRange()[1];
  vector<double> labelRGB(3); // {2,3,5} prime generators =>
                              // 20 labels before we run out of colors
  labelRGB[0] = std::max( 1.0 - (index%5)/4.0,     0.0);
  labelRGB[1] = std::min( (index%7)/5.0, 1.0          );
  labelRGB[2] = std::min( (index%3)/3.0, 1.0          );
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
