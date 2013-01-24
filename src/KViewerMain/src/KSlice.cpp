#include <iostream>
#include "KSlice.h"

#include "KSandbox.h"
#include "KDataWarehouse.h"
#include "KSegmentorBase.h"


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

void KSlice::CopySlice(int fromSlice){
    ksliceOptions->fromSlice=fromSlice;
}

void KSlice::PasteSlice(int toSlice){
    ksliceOptions->toSlice=toSlice;
    vrcl::copySliceFromTo( dataWarehouse->labVol, ksliceOptions->fromSlice, ksliceOptions->toSlice);
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



