#include <iostream>
#include "KSlice.h"

#include "vtkCommand.h"
#include "vtkEventQtSlotConnect.h"
#include "vtkDebugLeaks.h"
#include "vtkConeSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCamera.h"
#include "vtkMetaImageReader.h"
#include "vtkIndent.h"
#include "vtkImageData.h"
#include "vtkDataSetMapper.h"
#include "vtkProperty.h"
#include "vtkPolyDataNormals.h"
#include "vtkMarchingCubes.h"
#include "vtkImagePlaneWidget.h"
#include "vtkImageMapper.h"
#include "vtkImageMapToColors.h"
#include "vtkLookupTable.h"
#include "vtkSmartPointer.h"
#include "vtkInteractorStyleImage.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkDataSetMapper.h"
#include "vtkImageData.h"
#include "vtkRendererCollection.h"
#include "vtkImageCast.h"
#include "vtkIndent.h"
#include "vtkImageThreshold.h"
#include "vtkImageSource.h"

#include "Logger.h"
#include "KvtkImageInteractionCallback.h"
#include "KSandbox.h"
#include "KDataWarehouse.h"



KSlice::KSlice( ) {
    ksliceOptions= new KViewerOptions();
    dataWarehouse= new KDataWarehouse();
    ksliceOptions->initCorrectFlag=0; //should not run updates before this flag is set to 1
    ksliceOptions->brushRad=7;
    ksliceOptions->numIters=50;
    ksliceOptions->sliceNum=50; //make this so segmentation is non-trivial (no points initialized on level set)
    ksliceOptions->m_bUseEdgeBased=0;
    ksliceOptions->distWeight=.3;
    ksliceOptions->contInit=0;
}


KSlice::~KSlice() {
  delete dataWarehouse;
  delete ksliceOptions;
}


void KSlice::SetImage(vtkImageData* image){
    dataWarehouse->imgVol = image;
}

void KSlice::SetLabel(vtkImageData* label){
    dataWarehouse->labVol = label;
}

void KSlice::SetUI(vtkImageData* uiVol){
    dataWarehouse->uiVol= uiVol;
}

void KSlice::SetNumIters(int numIts){
    ksliceOptions->numIters = numIts;
}

void KSlice::SetBrushRad(int rad){
    ksliceOptions->brushRad = rad;
}

void KSlice::SetCurrSlice(int currSlice){
    ksliceOptions->sliceNum = currSlice;
}

void KSlice::SetDistWeight(float distWeight){
    ksliceOptions->distWeight=distWeight;
}

void KSlice::Initialize(){
    dataWarehouse->ksegmentor= new KSegmentor3D(dataWarehouse->imgVol, dataWarehouse->labVol, ksliceOptions);
    dataWarehouse->ksegmentor->SetUseEdgeBasedEnergy( ksliceOptions->m_bUseEdgeBased );
    dataWarehouse->ksegmentor->SetDistanceWeight(ksliceOptions->distWeight);
    ksliceOptions->initCorrectFlag=1; //initialization is complete

}

void KSlice::runUpdate(){
    if(ksliceOptions->initCorrectFlag==1){ //already initialized
        dataWarehouse->ksegmentor->SetCurrentSlice(ksliceOptions->sliceNum);
        dataWarehouse->ksegmentor->Update2D();
    }
}



