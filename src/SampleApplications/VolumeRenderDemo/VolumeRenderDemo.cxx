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
#include <vtkMath.h>
#include <cmath>

#define SP( X )  vtkSmartPointer<X> 

vtkImageData* readPoints()
{
  vtkImageData* imgvol = vtkImageData::New( );
  int Dim1 = 20; int Dim2 = 40; int Dim0 = 20;

  imgvol->SetDimensions( Dim1,Dim2,Dim0 );
  imgvol->SetNumberOfScalarComponents(1);
  imgvol->SetSpacing( 1,1,1 );
  imgvol->SetOrigin( 0,0,0 );
  imgvol->SetScalarTypeToUnsignedShort( );
  imgvol->AllocateScalars( );

  // Values stored 'linearly', slightly unsure about the orientation though.
  unsigned short*  VolPtr   = (unsigned short *) imgvol->GetScalarPointer();
  int numel                 = Dim0 * Dim1 * Dim2;
  for( int i=0; i<numel; i++ )
  {
    float fscale = 256.0 ;
    unsigned short nextvalue = (unsigned short ) fscale * ( i / float( numel ) ) ;
    *VolPtr= nextvalue;
    *VolPtr++;
  }

  return imgvol;
}

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

void multiplyImageByLabels( vtkImageData* imgData, vtkImageData* lblData )
{
  int dims[3];
  imgData->GetDimensions( dims );
  int numel                 = dims[0]*dims[1]*dims[2];
  double labelRange[2];
  imgData->GetScalarRange( labelRange );
  unsigned short  lmin = labelRange[0];
  unsigned short*  img = (unsigned short *) imgData->GetScalarPointer();
  unsigned short*  lbl = (unsigned short *) lblData->GetScalarPointer();
  for( int i=0; i<numel; i++ )
  {
    unsigned short ival =  img[i];
    unsigned short lval =  lbl[i];
    img[i] = ( lval > lmin ) * ival;
  }
}

int main( int argc, char **argv)
{
 SP(vtkMetaImageReader) imgReader =  SP(vtkMetaImageReader)::New();
 SP(vtkMetaImageReader) lblReader =  SP(vtkMetaImageReader)::New();
 SP(vtkImageData) imageData      =  SP(vtkImageData)::New();
 SP(vtkImageData) labelData      =  SP(vtkImageData)::New();
  if( argc == 1 ) {
    cout << "No args, creating synthetic points..." << endl;
    imageData = readPoints( );
  } if( argc >= 2 ) {
    cout << "attempting to load " << argv[1] << endl;
    imgReader->SetFileName( argv[1] );
    imgReader->Update();
    
    SP(vtkImageData) imageDataTmp  =  SP(vtkImageData)::New();
    imageDataTmp = imgReader->GetOutput();
    imageData    = image2ushort( imageDataTmp );
     
  } else {
    cout << argv[0] << " given bogus args, need 0 or 1 or 2  (none, image, label)! " << endl;
    exit(1);
  }
  
  if( argc == 3 ) {
      cout << "attempting to load " << argv[2] << endl;
      lblReader->SetFileName( argv[2] );
      lblReader->Update();
      
      SP(vtkImageData) lblDataTmp  =  SP(vtkImageData)::New();
      lblDataTmp   = lblReader->GetOutput();
      labelData    = image2ushort( lblDataTmp );
      
      multiplyImageByLabels( imageData, labelData );
      
  } 
  
  double imgRange[2];
  imageData->GetScalarRange( imgRange );
  
  vtkPiecewiseFunction *opacityTranferFunction = vtkPiecewiseFunction::New();

  opacityTranferFunction->AddPoint( imgRange[1],     0.5);
  opacityTranferFunction->AddPoint( imgRange[1]*0.5, 0.3);
  opacityTranferFunction->AddPoint( imgRange[0],     0.05);

  std::vector<double> v(5);
  for( int k = 0; k < (int) v.size(); k++ ) {
    v[k] =  (k/double(v.size())) * imgRange[1] + (1.0 - k/double(v.size())) * imgRange[0];
    cout << "added colorpoint ... " << v[k] << endl;
  }
  
  //
  SP(vtkColorTransferFunction) colorTransferFunction = SP(vtkColorTransferFunction)::New();
  colorTransferFunction->AddRGBPoint(   v[0], 1.0, 1.0, 1.0);
  colorTransferFunction->AddRGBPoint(   v[1], 0.5, 0.5, 0.5);
  colorTransferFunction->AddRGBPoint(   v[2], 0.5, 0.5, 0.5);
  colorTransferFunction->AddRGBPoint(   v[3], 1.0, 0.0, 0.0);
  colorTransferFunction->AddRGBPoint(   v[4], 1.0, 0.0, 1.0);

  //
  SP(vtkVolumeProperty) volumeProperty = SP(vtkVolumeProperty)::New();
  volumeProperty->SetColor(colorTransferFunction);
  volumeProperty->SetGradientOpacity( opacityTranferFunction );
  //volumeProperty->SetScalarOpacity(opacityTranferFunction);
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
