#include <iostream>
//#include <boost/python.hpp>

#include "vtkKSlice.h"

#include "KSandbox.h"
//#include "KDataWarehouse.h"
#include "KSegmentorBase.h"

#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"


vtkCxxRevisionMacro(vtkKSlice, "$Revision$"); //necessary?
vtkStandardNewMacro(vtkKSlice); //for the new() macro

//----------------------------------------------------------------------------


vtkKSlice::vtkKSlice( ) {
    ImageVol   = NULL;
    LabelVol   = NULL;
    UIVol      = NULL;
    ksegmentor = NULL;

    BrushRad=7;
    NumIts=50;
    CurrSlice=50; //make this so segmentation is non-trivial (no points initialized on level set)
    DistWeight=.3;
    m_bUseEdgeBased=0;
    contInit=0;
    initCorrectFlag=0; //should not run updates before this flag is set to 1
    CurrLabel=1;

}


vtkKSlice::~vtkKSlice() {
  delete this->ksegmentor;
  std::cout<<"KSlice destroyed"<<std::endl;
}

void vtkKSlice::SetOrientation(const std::string& orient) {
 // axial,sagittal,coronal,etc
  std::cout<<"set kslice orientation to " << orient << std::endl;
  this->ksegmentor->SetSliceOrientationIJK(orient);
  this->Orientation=orient;
}

void vtkKSlice::applyUserIncrement(int i, int j, int k, double val) {

  std::cout << "vtkKSlice::applyUserIncrement" << val << " at i,j,k =  "
            << i << "," <<j << ", " << k << std::endl; // UIVol->Print(std::cout);
  double Uinit = this->UIVol->GetScalarComponentAsDouble(i,j,k,0);
  this->ksegmentor->accumulateUserInput(val,i,j,k);
  //UIVol->DeepCopy(ksegmentor->U_Integral_image);
  cout << "same pointer? " << UIVol << ", " << this->ksegmentor->U_Integral_image << std::endl;
  double Uend = this->UIVol->GetScalarComponentAsDouble(i,j,k,0);
  cout << "before,after accumulate:  " << Uinit << ", " << Uend << std::endl; // UIVol->Print(std::cout);
  UIVol->Update();
}

void vtkKSlice::PasteSlice(int toSlice){
    ToSlice=toSlice;
    vrcl::copySliceFromTo(LabelVol, FromSlice, ToSlice, Orientation);
}

void vtkKSlice::Initialize(){  // Called on "start bot" button
    //set up the segmentor
    this->ksegmentor= new KSegmentor3D(ImageVol, LabelVol, UIVol,
                                                contInit, CurrSlice, NumIts, DistWeight, BrushRad, CurrLabel);
    this->ksegmentor->SetDistanceWeight(DistWeight);
    initCorrectFlag=1; //initialization is complete
}

void vtkKSlice::runUpdate2D(bool reInitFromMask){      // E key now
    if(initCorrectFlag==1){ //already initialized
        this->ksegmentor->SetCurrentSlice(CurrSlice);
        this->ksegmentor->Update2D(reInitFromMask);
        std::cout<<"did the update for slice:" <<CurrSlice<<std::endl;
        //LabelVol->Modified(); //why do we need this??
    }
}

void vtkKSlice::runUpdate3D(bool reInitFromMask){      // T key now
    if(initCorrectFlag==1){ //already initialized
        this->ksegmentor->SetCurrentSlice(CurrSlice);
        this->ksegmentor->Update3D(reInitFromMask);
        std::cout<<"did the update for 3d" <<std::endl;
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
