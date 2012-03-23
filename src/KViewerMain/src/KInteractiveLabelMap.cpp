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

void KInteractiveLabelMap::RegisterSourceWidget(KWidget_2D_left *kwidget2D, bool regNewImage)
{
  sourceWidget = kwidget2D; // get a 'reverse handle' on the widget whose container I'm in
  
  RegisterState(    sourceWidget->kv_opts );
  if(regNewImage)
    RegisterNewImage( kwidget2D->kv_data->imageVolumeRaw );
  SetupLabelView(sourceWidget->multiLabelMaps.size());

}

void KInteractiveLabelMap::RegisterNewImage( vtkImageData* image)
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
}

void KInteractiveLabelMap::SetupLabelView(int index)
{

  // connect the GUI display for this label map.
  // later a renderer needs to add the actor
  double maxVal = kv_opts->drawLabelMaxVal;
  cout << "new interactive labelmap with actors, max val = " << maxVal << endl;
  vector<double> labelRGB = get_good_color_0to7(index);
  labelLUT = create_default_labelLUT( maxVal, labelRGB );

  labelDataArray=image2ushort( this->labelDataArray);

  label2D_shifter_scaler->SetInput(labelDataArray);

  labelReslicer->SetInputConnection( this->label2D_shifter_scaler->GetOutputPort() );
  labelReslicer->SetOutputDimensionality(2);  //transforming the whole image
  labelReslicer->AutoCropOutputOn();
  labelReslicer->SetOutputExtentToDefault();
  labelReslicer->Modified();
  labelReslicer->UpdateWholeExtent();
  labelReslicer->SetOutputOrigin(0,0,0);
  labelReslicer->Update();

  labelReslicer->SetResliceTransform(sourceWidget->m_SliderTrans);

  // grab a handle on the image ... - what for . not used?
   //imageVolume     = labelDataArray;

  colorMap->SetLookupTable(labelLUT);
  colorMap->SetInputConnection(labelReslicer->GetOutputPort() );
  colorMap->Update();
  labelActor2D->SetInput( colorMap->GetOutput() );
  labelActor2D->SetOpacity( kv_opts->labelOpacity2D );
  labelActor2D->SetInterpolate( kv_opts->labelInterpolate );

}


void KInteractiveLabelMap::UpdateResliceTransform()
{
    std::cout<<"LAbel-01:"<<this->labelDataArray->GetOrigin()[0]<<" "<<this->labelDataArray->GetOrigin()[1]<<" "<<this->labelDataArray->GetOrigin()[2]<<" "<<std::endl;
     std::cout<<"LAbel-E1:"<<this->labelDataArray->GetExtent()[1]<<" "<<this->labelDataArray->GetExtent()[3]<<" "<<this->labelDataArray->GetExtent()[5]<<" "<<std::endl;
    std::cout<<"LAbel-Sp1:"<<this->labelDataArray->GetSpacing()[0]<<" "<<this->labelDataArray->GetSpacing()[1]<<" "<<this->labelDataArray->GetSpacing()[2]<<" "<<std::endl;
       labelReslicer->SetResliceTransform(kv_opts->GetTransform());

    labelReslicer->SetOutputDimensionality(3);
    labelReslicer->AutoCropOutputOn();
    labelReslicer->SetOutputOrigin(0,0,0);
    labelReslicer->SetOutputSpacing(&(sourceWidget->GetOutputSpacing()[0]));

    labelReslicer->Modified();
    labelReslicer->UpdateWholeExtent();
    labelReslicer->Update();
    labelReslicer->UpdateInformation();
    labelReslicer->GetOutput()->UpdateInformation();

    // convert it to unsigned short, our desired internal method...

    this->labelDataArray->DeepCopy( labelReslicer->GetOutput() );

    std::cout<<"Label-02:"<<this->labelDataArray->GetOrigin()[0]<<" "<<this->labelDataArray->GetOrigin()[1]<<" "<<this->labelDataArray->GetOrigin()[2]<<" "<<std::endl;
     std::cout<<"Label-E2:"<<this->labelDataArray->GetExtent()[1]<<" "<<this->labelDataArray->GetExtent()[3]<<" "<<this->labelDataArray->GetExtent()[5]<<" "<<std::endl;
    std::cout<<"Label-Sp2:"<<this->labelDataArray->GetSpacing()[0]<<" "<<this->labelDataArray->GetSpacing()[1]<<" "<<this->labelDataArray->GetSpacing()[2]<<" "<<std::endl;
    //double currSliceOrigin=kv_opts->sliderMin +kv_opts->sliceZSpace*currentSliceIndex;

    labelReslicer->SetResliceTransform(vtkTransform::New());
    labelReslicer->SetOutputDimensionality(2);
    labelReslicer->AutoCropOutputOff();
    labelReslicer->Modified();
    labelReslicer->UpdateWholeExtent();
    labelReslicer->UpdateInformation();
    labelReslicer->GetOutput()->UpdateInformation();
    labelReslicer->Update();

    labelReslicer->SetResliceTransform(sourceWidget->m_SliderTrans);
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
