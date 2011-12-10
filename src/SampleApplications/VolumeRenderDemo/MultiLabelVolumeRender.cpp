
#include "vtkImageViewer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkStructuredPointsReader.h"
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkVolumeRayCastMapper.h"
#include "vtkVolumeRayCastCompositeFunction.h"
#include "vtkVolume.h"
#include "vtkRenderer.h"
#include "vtkPoints.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkMetaImageReader.h"
#include "vtkImageResample.h"
#include "vtkImageMedian3D.h"
#include "vtkImageContinuousErode3D.h"
#include "vtkImageContinuousDilate3D.h"
#include "opencv2/core/core.hpp"
#include <vtkMath.h>
#include <cmath>
#include <vector>
#include "KSegmentorBase.h"

#define SP( X )  vtkSmartPointer<X> 
using namespace std;
using namespace cv;

void get_good_color( int idx, vector<int>& rgb_out )
{
  int rgb[3]; 
  switch( idx ) {
   
  case 0:
    rgb = {255, 146, 5}; break;
    //Dk. Gray
  case 1:
    rgb = {87, 87, 87}; break; 
    //Red
  case 2:
    rgb = {173, 35, 35}; break;
    //Blue
  case 3: 
    rgb = {42, 75, 215}; break;
    //Green
  case 4: 
    rgb = {29, 105, 20}; break;
    //Brown
  case 5: 
    rgb = {129, 74, 25}; break;
    //Purple
  case 6:
    rgb = {129, 38, 192}; break;
    //Lt. Gray
    
  case 7:
    rgb = {160, 160, 160}; break;
    //Lt. Green
    
  case 8:
    rgb = {129, 197, 122}; break;
    //Lt. Blue
    
  case 9:
    rgb = {157, 175, 255}; break;
    //Cyan
    
  case 10: 
    rgb = {41, 208, 208}; break;
    //Orange
  case 11:
    rgb = { 255, 146, 51}; break;
    //Yellow
  case 12: 
    rgb = {255, 238, 51}; break;
    //Tan
  case 13:
    rgb = {233, 222, 187}; break;
    //Pink
  case 14: 
    rgb = {255, 205, 243}; break;
    //White
  case 15: 
    rgb = {255, 255, 255}; break;
  }
  
  rgb_out = std::vector<int>(rgb,rgb+3);
  
}

struct LabelVolumeMetaInfo {
  string filename;
  string volume_string;
};


SP(vtkImageData) image2ushort( vtkImageData* imageData )
{
  SP(vtkImageData) imgvol = SP(vtkImageData)::New( );
  int dims[3];
  imageData->GetDimensions( dims );
  double spacing_in[3];
  imageData->GetSpacing(spacing_in);
  cout << "spacing:" << Mat( vector<double>(spacing_in,spacing_in+3) ) << endl;
  
  imgvol->SetDimensions( dims[0],dims[1],dims[2] );
  imgvol->SetNumberOfScalarComponents(1);
  imgvol->SetSpacing( spacing_in );
  imgvol->SetOrigin( 0,0,0 );
  imgvol->SetScalarTypeToUnsignedShort( );
  imgvol->AllocateScalars( );
  
  // Values stored 'linearly', slightly unsure about the orientation though.
  unsigned short*  outputPtr = (unsigned short *) imgvol->GetScalarPointer();
  short *inputPtr = static_cast<short*>( imageData->GetScalarPointer() );
  int numel                 = dims[0]*dims[1]*dims[2];
  for( int i=0; i<numel; i++ )
  {
    short invalue            =  inputPtr[i];
    unsigned short nextvalue = (unsigned short ) invalue ;
    *outputPtr= nextvalue;
    *outputPtr++;
  }
  
  return imgvol;
}

SP(vtkImageData) mergeLabelMaps( const std::vector<SP(vtkImageData)> & multiLabels,
                                 std::vector<LabelVolumeMetaInfo> & volume_info )
{
  SP(vtkImageData) img   =  SP(vtkImageData)::New();
  img->DeepCopy( multiLabels[0] );
  int nLabels = multiLabels.size();
  int dims[3];
  double labelRange[2];
  for( int k = 0; k < nLabels; k++ ) {
    vtkImageData* imgData = multiLabels[k];
    
    // display the volume of each label for quick sanity checking of usefulness of label maps
    double spc[3];
    imgData->GetSpacing(spc);
    vector<double> spacing(spc,spc+3);
    string strVolume;
    vrcl::getVolumeAsString(spacing,imgData,strVolume,true);
    volume_info[k].volume_string = strVolume;

    imgData->GetDimensions( dims );
    unsigned short*  img_k_in = (unsigned short *) imgData->GetScalarPointer();
    unsigned short*  img_out  = (unsigned short *) img->GetScalarPointer();
    int numel                 = dims[0]*dims[1]*dims[2];
    for( int i=0; i<numel; i++ )   {
      unsigned short ival =  img_k_in[i];
      if( img_out[i] > 0 ) {
        img_out[i] /= 2;
        ival       /= 2;
      }
      img_out[i] += (ival > 0 ) ? ( 4*(k+1) ) : 0;
    }
  }
  img->GetScalarRange( labelRange );
  return img;
}

int main( int argc, char **argv)
{
  SP(vtkMetaImageReader) imgReader =  SP(vtkMetaImageReader)::New();

  std::vector< SP(vtkImageData) > multiLabels(argc-1);
  std::vector<LabelVolumeMetaInfo>    volume_info(argc-1);

  if( argc >= 2 ) {
    cout << "loading label files. warning: they should be the same size "
         << " e.g. from the same image file! " << endl;
    for( int k = 1; k < argc; k++ ) {
      cout << "attempting to load " << argv[k] << endl;
      imgReader->SetFileName( argv[k] );
      imgReader->Update();
      volume_info[k-1].filename = argv[k];

      // read it from disk, fails if file name is wacked
      SP(vtkImageData) imageDataTmp   =  SP(vtkImageData)::New();
      imageDataTmp = vrcl::removeImageOstrava( imgReader->GetOutput(),3/*erode*/,5 /*dilate*/ );

      // make it a ushort and bag it
      SP(vtkImageData) img      =  SP(vtkImageData)::New();
      img    = image2ushort( imageDataTmp );
      multiLabels[k-1] = img;
    }
  } else {
    cout << "bogus args! usage: " << argv[0] << " odin.mha dva.mha tri.mha ... blyat.mha " << endl;
    exit(1);
  }
  
  SP(vtkImageData) imageData = mergeLabelMaps( multiLabels, volume_info);

  cout << "successfully processed labels. printing their meta info: " << endl;
  for( int k = 0; k < (int) volume_info.size(); k++ ) {
    cout << volume_info[k].filename << ", " << volume_info[k].volume_string << endl;
  }

  SP(vtkImageResample) resampler = SP(vtkImageResample)::New();
  resampler->SetInput( imageData );
  resampler->SetAxisMagnificationFactor(0,1.0);
  resampler->SetAxisMagnificationFactor(1,1.0);
  resampler->SetAxisMagnificationFactor(2,1.0);
  resampler->Update();
  imageData = resampler->GetOutput();
    
  double imgRange[2];
  imageData->GetScalarRange( imgRange );
  
  vtkPiecewiseFunction *opacityTranferFunction = vtkPiecewiseFunction::New();
  
  opacityTranferFunction->AddPoint( imgRange[1],     0.8);
  opacityTranferFunction->AddPoint( 2,0.8);
  opacityTranferFunction->AddPoint( 1,0.0);
  opacityTranferFunction->AddPoint( 0,     0.0);
  SP(vtkColorTransferFunction) colorTransferFunction = SP(vtkColorTransferFunction)::New();
  std::vector<double> v(2*multiLabels.size());
  std::vector<int> rgb(3);
  for( int k = 0; k < (int) v.size(); k++ ) {
    v[k] =  (k/double(v.size())) * imgRange[1] + (1.0 - k/double(v.size())) * imgRange[0];
    cout << "added colorpoint ... " << v[k] << endl;
    get_good_color(k,rgb);
    colorTransferFunction->AddRGBPoint(   v[k], rgb[0]/255.0, rgb[1]/255.0, rgb[2]/255.0 );
  }
  
  //
  SP(vtkVolumeProperty) volumeProperty = SP(vtkVolumeProperty)::New();
  volumeProperty->SetColor(colorTransferFunction);
  volumeProperty->SetScalarOpacity(opacityTranferFunction);
  volumeProperty->ShadeOn();
  volumeProperty->SetInterpolationTypeToLinear();
  
  //
  SP(vtkVolumeRayCastCompositeFunction) compositeFunction = 
      SP( vtkVolumeRayCastCompositeFunction)::New();
  SP(vtkVolumeRayCastMapper) volumeMapper = SP(vtkVolumeRayCastMapper)::New();
  volumeMapper->SetVolumeRayCastFunction(compositeFunction);
  
  volumeMapper->SetInput( imageData );
  
  volumeMapper->Update( );
  
  //
  vtkVolume *volume = vtkVolume::New();
  volume->SetMapper(volumeMapper);
  volume->SetProperty(volumeProperty);
  
  vtkRenderer *ren1 = vtkRenderer::New();
  vtkRenderWindow *renWin = vtkRenderWindow::New();
  ren1->SetBackground(1.0,1.0,1.0);
  renWin->AddRenderer(ren1);
  vtkRenderWindowInteractor *renderWindowInteractor = vtkRenderWindowInteractor ::New();
  renderWindowInteractor->SetRenderWindow(renWin);
  
  ren1->AddVolume(volume);
  ren1->Render();
  renderWindowInteractor->Start();
  
  return 0;
}
