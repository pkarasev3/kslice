
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
#include <vtkMath.h>
#include <cmath>
#include <vector>

#define SP( X )  vtkSmartPointer<X> 

SP(vtkImageData) image2ushort( vtkImageData* imageData )
{
  SP(vtkImageData) imgvol = SP(vtkImageData)::New( );
  int dims[3];
  imageData->GetDimensions( dims );
  
  imgvol->SetDimensions( dims[0],dims[1],dims[2] );
  imgvol->SetNumberOfScalarComponents(1);
  imgvol->SetSpacing( 1,1,1 );
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

SP(vtkImageData) mergeLabelMaps( const std::vector<SP(vtkImageData)> & multiLabels )
{
  SP(vtkImageData) img   =  SP(vtkImageData)::New();
  img->DeepCopy( multiLabels[1] );
  int nLabels = multiLabels.size();
  int dims[3];
  double labelRange[2];
  for( int k = 0; k < nLabels; k++ ) {
    vtkImageData* imgData = multiLabels[k];
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
 SP(vtkMetaImageReader) lblReader =  SP(vtkMetaImageReader)::New();
 
 std::vector< SP(vtkImageData) > multiLabels(argc-1);
 
  if( argc >= 2 ) {
    for( int k = 1; k < argc; k++ ) {
      cout << "attempting to load " << argv[k] << endl;
      imgReader->SetFileName( argv[k] );
      imgReader->Update();
      
      // read it from disk, fails if file name is wacked
      SP(vtkImageData) imageDataTmp   =  SP(vtkImageData)::New();
      imageDataTmp = imgReader->GetOutput();
      
      // make it a ushort and bag it
      SP(vtkImageData) img      =  SP(vtkImageData)::New();
      img    = image2ushort( imageDataTmp );
      multiLabels[k-1] = img;
    }
  } else {
    cout << "bogus args! usage: " << argv[0] << " odin.mha dva.mha tri.mha ... blyat.mha " << endl;
    exit(1);
  }
  
  SP(vtkImageData) imageData = mergeLabelMaps( multiLabels );
  SP(vtkImageResample) resampler = SP(vtkImageResample)::New();
  resampler->SetInput( imageData );
  resampler->SetAxisMagnificationFactor(0,1.0);
  resampler->SetAxisMagnificationFactor(1,1.0);
  resampler->SetAxisMagnificationFactor(2,15.0);
  resampler->Update();
  imageData = resampler->GetOutput();
  
//  vtkIndent indentVTK=vtkIndent::vtkIndent(0);
//  std::ofstream  LogFile("kviewer_desktop.log");  
//  imageData->PrintSelf(LogFile, indentVTK);
//  imageData->PrintSelf(std::cout, indentVTK);
  
  double imgRange[2];
  imageData->GetScalarRange( imgRange );
  
  vtkPiecewiseFunction *opacityTranferFunction = vtkPiecewiseFunction::New();

  opacityTranferFunction->AddPoint( imgRange[1],     0.8);
  opacityTranferFunction->AddPoint( 2,0.8);
  opacityTranferFunction->AddPoint( 1,0.0);
  opacityTranferFunction->AddPoint( 0,     0.0);
  SP(vtkColorTransferFunction) colorTransferFunction = SP(vtkColorTransferFunction)::New();
  std::vector<double> v(2*multiLabels.size());
  for( int k = 0; k < (int) v.size(); k++ ) {
    v[k] =  (k/double(v.size())) * imgRange[1] + (1.0 - k/double(v.size())) * imgRange[0];
    cout << "added colorpoint ... " << v[k] << endl;
    colorTransferFunction->AddRGBPoint(   v[k], k/double(v.size()), vtkMath::Random(0,1), vtkMath::Random(0,1) );
  }
  
  //
  SP(vtkVolumeProperty) volumeProperty = SP(vtkVolumeProperty)::New();
  volumeProperty->SetColor(colorTransferFunction);
  //volumeProperty->SetGradientOpacity( opacityTranferFunction );
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
