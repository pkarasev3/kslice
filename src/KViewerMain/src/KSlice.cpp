#include <iostream>
#include "KSlice.h"

#include "KSandbox.h"
#include "KDataWarehouse.h"
#include "KSegmentorBase.h"

#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"


//vtkCxxRevisionMacro(KSlice, "$Revision$"); //necessary?
vtkStandardNewMacro(KSlice); //for the new() macro

//----------------------------------------------------------------------------


KSlice::KSlice( ) {
    ksliceOptions= new KViewerOptions();
    dataWarehouse= new KDataWarehouse();

    ksliceOptions->BrushRad=7;
    ksliceOptions->NumIts=50;
    ksliceOptions->CurrSlice=50; //make this so segmentation is non-trivial (no points initialized on level set)
    ksliceOptions->DistWeight=.3;
    ksliceOptions->m_bUseEdgeBased=0;
    ksliceOptions->contInit=0;
    ksliceOptions->initCorrectFlag=0; //should not run updates before this flag is set to 1
}


KSlice::~KSlice() {
  delete dataWarehouse;
  delete ksliceOptions;
}



void KSlice::PasteSlice(int toSlice){
    ksliceOptions->ToSlice=toSlice;
    vrcl::copySliceFromTo( dataWarehouse->LabelVol, ksliceOptions->FromSlice, ksliceOptions->ToSlice);
}

void KSlice::Initialize(){

    //organize this into containers (perhaps this should be removed, but vtk structure seems to enforce this)
    //these values all got set with vtkSetMacros
    dataWarehouse->ImageVol=ImageVol;
    dataWarehouse->LabelVol=LabelVol;
    dataWarehouse->UIVol=UIVol;
    ksliceOptions->NumIts=NumIts;
    ksliceOptions->BrushRad=BrushRad;
    ksliceOptions->CurrSlice=CurrSlice;
    ksliceOptions->DistWeight=DistWeight;
    ksliceOptions->FromSlice=FromSlice;

    //set up the segmentor
    dataWarehouse->ksegmentor= new KSegmentor3D(dataWarehouse->ImageVol, dataWarehouse->LabelVol, ksliceOptions);
    dataWarehouse->ksegmentor->SetUseEdgeBasedEnergy( ksliceOptions->m_bUseEdgeBased );
    dataWarehouse->ksegmentor->SetDistanceWeight(ksliceOptions->DistWeight);
    ksliceOptions->initCorrectFlag=1; //initialization is complete

}

void KSlice::runUpdate(){
    if(ksliceOptions->initCorrectFlag==1){ //already initialized
        dataWarehouse->ksegmentor->SetCurrentSlice(ksliceOptions->CurrSlice);
        dataWarehouse->ksegmentor->Update2D();
    }
}

void KSlice::PrintSelf(ostream &os, vtkIndent indent)
{
    os << "Fill in the print function!" << std::endl;
}


