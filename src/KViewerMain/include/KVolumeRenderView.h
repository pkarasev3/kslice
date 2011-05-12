#pragma once 

#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include <vector>
class vtkPiecewiseFunction;
class vtkColorTransferFunction;
class vtkVolumeProperty;
class vtkVolume;
class vtkVolumeRayCastMapper;
class vtkVolumeRayCastCompositeFunction;

#define SP( X )  vtkSmartPointer<X> 

struct VolumeRenderInternal
{
public:
  SP(vtkPiecewiseFunction) opacityFunc;
  SP(vtkColorTransferFunction) colorTransferFunction ;
  SP(vtkVolumeProperty) volumeProperty ;
  SP(vtkVolumeRayCastCompositeFunction) compositeFunction;
  SP(vtkVolumeRayCastMapper) volumeMapper;
  SP(vtkVolume) volume ;
  SP(vtkImageData) data ;
  
  std::vector<double> focusPoints;
  
  void reset();
};


struct KVolumeRenderView
{
public:
  KVolumeRenderView()
  {
    volumeImage = SP(vtkVolume)::New();
    volumeLabel = SP(vtkVolume)::New();
  }
  ~KVolumeRenderView() { }
  
  void AddFocusPoint( int x, int y, int z ) ;
  
  void UpdateDisplay( vtkImageData* image, vtkImageData* label) ;
  
  SP(vtkVolume)  volumeImage;
  SP(vtkVolume)  volumeLabel;
  
  VolumeRenderInternal  vriLabel;
  VolumeRenderInternal  vriImage;
  
};
