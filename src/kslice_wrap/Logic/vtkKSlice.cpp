#include <iostream>
//#include <boost/python.hpp>

#include "vtkKSlice.h"

#include "KSandbox.h"
#include "KDataWarehouse.h"
#include "KSegmentorBase.h"

#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"


vtkCxxRevisionMacro(vtkKSlice, "$Revision$"); //necessary?
vtkStandardNewMacro(vtkKSlice); //for the new() macro

//----------------------------------------------------------------------------


vtkKSlice::vtkKSlice( ) {
    dataWarehouse= new KDataWarehouse();

    BrushRad=7;
    NumIts=50;
    CurrSlice=50; //make this so segmentation is non-trivial (no points initialized on level set)
    DistWeight=.3;
    m_bUseEdgeBased=0;
    contInit=0;
    initCorrectFlag=0; //should not run updates before this flag is set to 1
}


vtkKSlice::~vtkKSlice() {
  delete dataWarehouse;
  std::cout<<"KSlice destroyed"<<std::endl;
}

void vtkKSlice::SetOrientation(const std::string& orient) {
 // axial,sagittal,coronal,etc
  std::cout<<"set kslice orientation to " << orient << std::endl;
}

void vtkKSlice::applyUserIncrement(int i, int j, int k, double val) {
  std::cout<<"increment U in kslice c++ by " << val << std::endl;
  double Uinit = this->UIVol->GetScalarComponentAsDouble(i,j,k,0);
  double Umod  = Uinit + val; // TODO: this is unsafe! do it in KSegmentor! 
  this->UIVol->SetScalarComponentFromDouble(i,j,k,0,Umod);
}


void vtkKSlice::PasteSlice(int toSlice){
    ToSlice=toSlice;
    vrcl::copySliceFromTo(LabelVol, FromSlice, ToSlice);
}

void vtkKSlice::Initialize(){
    //set up the segmentor
    dataWarehouse->ksegmentor= new KSegmentor3D(ImageVol, LabelVol, contInit, CurrSlice, NumIts, DistWeight, BrushRad);
    //dataWarehouse->ksegmentor->SetUseEdgeBasedEnergy( m_bUseEdgeBased );
    dataWarehouse->ksegmentor->SetDistanceWeight(DistWeight);
    initCorrectFlag=1; //initialization is complete
}

void vtkKSlice::runUpdate(bool reInitFromMask){
    if(initCorrectFlag==1){ //already initialized
        dataWarehouse->ksegmentor->SetCurrentSlice(CurrSlice);
        dataWarehouse->ksegmentor->Update2D(reInitFromMask);
        std::cout<<"did the update for slice:" <<CurrSlice<<std::endl;
        //LabelVol->Modified(); //why do we need this??
    }
}

void vtkKSlice::PrintSelf(ostream &os, vtkIndent indent)
{
    os << "I've been called!" << std::endl;
}

void vtkKSlice::PrintEmpty()
{
    std::cout<<"This empty print works" <<std::endl;
}
//void vtkKSlice::CollectRevisions(std::ostream&){
//}

//void vtkKSlice::PrintImage(ostream &os, vtkIndent indent)
//{
//    this->ImageVol->PrintSelf(os, indent);
//}

void vtkKSlice::PrintImage()
{
    double imgRange[2];
    std::cout<<this->ImageVol->GetScalarTypeMax()<<std::endl;
    this->ImageVol->GetScalarRange(imgRange);
    std::cout<<imgRange[0]<<"-"<<imgRange[1]<<std::endl;
}
