#ifndef KDATAWAREHOUSE_H_
#define KDATAWAREHOUSE_H_


#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include "KSegmentor3D.h"

struct KDataWarehouse
{
public:
    ~KDataWarehouse(){
        delete ksegmentor;
    }


  //vtkImageData* ImageVol; //image volume
  //vtkImageData* LabelVol; //label volume
  //vtkImageData* UIVol; //integral of the user input per voxel
  KSegmentor3D* ksegmentor; //all the active contour vars
};

#endif
