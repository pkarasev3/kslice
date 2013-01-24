#include "KVolumeRenderView.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToColors.h"
#include "vtkImageShiftScale.h"
#include "KViewerOptions.h"
#include "vtkImageData.h"
#include "vtkInteractorStyleImage.h"
#include "vtkRenderWindowInteractor.h"
#include "Logger.h"
#include "vtkMarchingCubes.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkImageResample.h"
#include "vtkPolyDataNormals.h"
#include "vtkImageGaussianSmooth.h"
#include "vtkPolyData.h"
#include "vtkDataSetMapper.h"
#include "vtkProperty.h"
#include "vtkLODActor.h"
#include "QVTKWidget.h"
#include "KDataWarehouse.h"
#include "vtkReverseSense.h"
#include "vtkSurfaceReconstructionFilter.h"
#include "vtkContourFilter.h"
#include "vtkPiecewiseFunction.h"
#include "vtkVolumeRayCastCompositeFunction.h"
#include "vtkVolumeMapper.h"
#include "vtkXMLPolyDataWriter.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"
#include "vtkVolumeRayCastMapper.h"
#include "KSandbox.h"

using namespace vrcl;

void VolumeRenderInternal::reset()
{
  opacityFunc = SP(vtkPiecewiseFunction)::New();
  colorTransferFunction = SP(vtkColorTransferFunction)::New();
  volumeProperty = SP(vtkVolumeProperty)::New();
  compositeFunction=SP(vtkVolumeRayCastCompositeFunction)::New();
  volumeMapper = SP(vtkVolumeRayCastMapper)::New();
  data =  SP(vtkImageData)::New();
  volume =  SP(vtkVolume)::New();
  data->SetScalarTypeToUnsignedShort();
  focusPoints.clear();
}

void UpdateInternal( vtkImageData* img, VolumeRenderInternal&  vol)
{
 
  
  if( img != vol.data ) {
    vol.data->ShallowCopy( img );
  }

  double imgRange[2];
  img->GetScalarRange( imgRange );
    
  std::vector<double> v(5);
  for( int k = 0; k < (int) v.size(); k++ ) {
    v[k] =  (k/double(v.size())) * imgRange[1] * 0.8 + (1.0 - k/double(v.size())) * (imgRange[0]+0.1 * imgRange[1] );
    cout << "added colorpoint ... " << v[k] << endl;
  }
  
  //
  vol.colorTransferFunction->RemoveAllPoints();
  vol.colorTransferFunction->AddRGBPoint(   v[0], 1.0,1.0,1.0);
  vol.colorTransferFunction->AddRGBPoint(   v[1], 0.0, 0.5, 1.0);
  vol.colorTransferFunction->AddRGBPoint(   v[2], 0.0, 1.0, 0.5);
  vol.colorTransferFunction->AddRGBPoint(   v[3], 1.0, 0.5, 0.0);
  vol.colorTransferFunction->AddRGBPoint(   v[4], 1.0,1.0,1.0);

  
  
  //
  vol.volumeProperty->SetGradientOpacity( vol.opacityFunc );
  vol.volumeProperty->SetColor(vol.colorTransferFunction);
  vol.volumeProperty->ShadeOn();
  vol.volumeProperty->SetInterpolationTypeToLinear();

  //
  vol.volumeMapper->SetVolumeRayCastFunction(vol.compositeFunction);
  vol.volumeMapper->SetInput( vol.data );
  vol.volumeMapper->Update( );

  //  
  vol.volume->SetMapper(vol.volumeMapper);
  vol.volume->SetProperty(vol.volumeProperty);
  vol.volume->Update();
}

void KVolumeRenderView::AddFocusPoint( int x, int y, int z ) 
{
  if( NULL == vriImage.data )
    return;
  if( NULL == vriLabel.data )
    return;
    
  double ival = this->vriImage.data->GetScalarComponentAsDouble( x, y, z, 0 );
  
  // TODO: display places where they're clicking ...
  
  
}

void KVolumeRenderView::UpdateDisplay( vtkImageData* img, vtkImageData* lbl ) 
  {
    if( NULL == vriImage.data )
      this->vriImage.reset();
    if( NULL == vriLabel.data )
      this->vriLabel.reset();
    
    vriImage.opacityFunc->RemoveAllPoints();
    vriLabel.opacityFunc->RemoveAllPoints();
    
    double ival1 = img->GetScalarRange()[0];
    double ival2 = img->GetScalarRange()[1];
    double lval1 = lbl->GetScalarRange()[0];
    double lval2 = lbl->GetScalarRange()[1];

    vriImage.opacityFunc->AddPoint( ival1, 0.01   );
    vriImage.opacityFunc->AddPoint( ival2, 0.2    );
    vriImage.opacityFunc->AddPoint( ival2*0.9, 0.0    );
    vriImage.opacityFunc->AddPoint( ival2*0.5,  0.2 );
    vriImage.opacityFunc->AddPoint( ival2*0.4,  0.05 );
    
    for( int k = 0; k < (int) vriImage.focusPoints.size(); k++ ) {
      vriImage.opacityFunc->AddPoint( vriImage.focusPoints[k], 0.1 );
    }
    
    vriLabel.opacityFunc->AddPoint( lval1, 0.0   );
    vriLabel.opacityFunc->AddPoint( lval2, 0.9    );
    vriLabel.opacityFunc->AddPoint( lval2/2, .7 );
    
    UpdateInternal( img, vriImage );
    UpdateInternal( lbl, vriLabel );
    
    this->volumeImage->ShallowCopy(vriImage.volume);
    this->volumeLabel->ShallowCopy(vriLabel.volume);
    //volumeImage->Update();
    //volumeLabel->Update();
    
  }
