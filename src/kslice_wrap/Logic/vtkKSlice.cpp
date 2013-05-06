#include <iostream>
//#include <boost/python.hpp>

#include "vtkKSlice.h"

#include "KSandbox.h"
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
    CurrSlice=1;
    DistWeight=.2;
    m_bUseEdgeBased=0;
    contInit=0;        //is doing init from contour
    initCorrectFlag=0; //should not run updates before this flag is set to 1
    CurrLabel=1;
    Orientation="IJ";

}


vtkKSlice::~vtkKSlice() {
  delete this->ksegmentor;
  std::cout<<"KSlice destroyed"<<std::endl;
}

void vtkKSlice::SetOrientation(const std::string& orient) {
 // ijk <=> axial,sagittal,coronal,etc
  std::cout<<"set kslice orientation to " << orient << std::endl;
  this->ksegmentor->SetSliceOrientationIJK(orient);
  this->Orientation=orient;
}

void vtkKSlice::applyUserIncrement(int i, int j, int k, double val)
{
  std::cout << "\033[94m  vtkKSlice::applyUserIncrement" << val << " at i,j,k =  " << i << "," <<j << ", " << k << "\033[0m" << std::endl;
    this->ksegmentor->accumulateUserInput(val,i,j,k);
  assert( UIVol == this->ksegmentor->U_Integral_image );
  UIVol->Update();
}

void vtkKSlice::PasteSlice(int toSlice){
    bool toSliceCheck   = checkSliceValid(this->Orientation, toSlice);
    bool fromSliceCheck = checkSliceValid(this->Orientation, FromSlice);

    if (toSliceCheck==1 && fromSliceCheck==1) //both checks passed
    {
        ToSlice=toSlice;
        vrcl::copySliceFromTo(LabelVol, FromSlice, ToSlice, Orientation);
    }else
    {
        std::cout<<"Execution stopped bc slice checks not passed"<<std::endl;
        std::cout<<"Attempted to access slice "<< FromSlice<< " and "<< toSlice<<std::endl;
    }
}

void vtkKSlice::Initialize(){  // Called on "start bot" button
    this->mdims=ImageVol->GetDimensions();

    //set up the segmentor
    this->ksegmentor= new KSegmentor3D(ImageVol, LabelVol, UIVol,
                                       contInit, CurrSlice, NumIts, DistWeight, BrushRad, CurrLabel);
    this->ksegmentor->SetDistanceWeight(DistWeight);
    initCorrectFlag=1; //initialization is complete
}

void vtkKSlice::runUpdate2D(bool reInitFromMask){      // E key now
    bool currSliceCheck   = checkSliceValid(this->Orientation, CurrSlice);
    if(initCorrectFlag==1 && currSliceCheck==1){ //already initialized and slice is in bounds
        this->ksegmentor->SetCurrentSlice(CurrSlice);
        this->ksegmentor->Update2D(reInitFromMask);
        std::cout<<"did the update for slice:" <<CurrSlice<<std::endl;
        //LabelVol->Modified(); // don't need this but keep in mind that this call will
                                // trigger python function for vtkObservers
    }else{
        std::cout<<"Execution stopped bc slice, initialization checks not passed"<<std::endl;
        std::cout<<"Attempted to access slice "<< CurrSlice<<std::endl;
    }
}

void vtkKSlice::runUpdate2p5D(bool reInitFromMask){      // U key now
    bool currSliceCheck   = checkSliceValid(this->Orientation, CurrSlice);
    if(initCorrectFlag==1 && currSliceCheck==1){ //already initialized
        //this->ksegmentor->SetEnergyChanVese();
        this->ksegmentor->SetEnergyLocalCVLimited();
        this->ksegmentor->SetDistanceWeight(DistWeight);
        this->ksegmentor->SetCurrentSlice(CurrSlice);
        this->ksegmentor->Update3D(reInitFromMask);
        std::cout<<"did the update for 2p5d" <<std::endl;
    }else
    {
        std::cout<<"Execution stopped bc slice, initialization checks not passed"<<std::endl;
        std::cout<<"Attempted to access slice "<< CurrSlice<<std::endl;
    }
}


void vtkKSlice::runUpdate3D(bool reInitFromMask){      // T key now
    bool currSliceCheck   = checkSliceValid(this->Orientation, CurrSlice);
    if(initCorrectFlag==1 && currSliceCheck==1){ //already initialized
        this->ksegmentor->SetEnergyLocalCV();
        this->ksegmentor->SetCurrentSlice(CurrSlice);
        this->ksegmentor->Update3D(reInitFromMask);
        std::cout<<"did the update for 3d" <<std::endl;
    }else
    {
        std::cout<<"Execution stopped bc slice, initialization checks not passed"<<std::endl;
        std::cout<<"Attempted to access slice "<< CurrSlice<<std::endl;
    }
}

void vtkKSlice::PrintSelf(ostream &os, vtkIndent indent)
{
    os << "I've been called!" << std::endl;
}

void vtkKSlice::PrintEmpty()
{
    std::cout<<"vtkKSlice::PrintEmpty() called" <<std::endl;
}


void vtkKSlice::PrintImage()
{
    double imgRange[2];
    std::cout<<this->ImageVol->GetScalarTypeMax()<<std::endl;
    this->ImageVol->GetScalarRange(imgRange);
    std::cout<<imgRange[0]<<"-"<<imgRange[1]<<std::endl;
}

bool vtkKSlice::checkSliceValid(std::string Orientation, int sliceNum){
    //make sure we're in bounds of allowed slices
    bool checkPassed=false;

    if( Orientation == "IJ" ) {
        if ( ( 0<=sliceNum) & (sliceNum<mdims[2]) )
        {
            checkPassed=true;
        }
        else
        {
            checkPassed=false;
        }
    }else if( Orientation == "JK" ) {
        if ( ( 0<=sliceNum) & (sliceNum<mdims[0]) )
        {
            checkPassed=true;
        }
        else
        {
            checkPassed=false;
        }
    }else if( Orientation == "IK" ) {
        if ( ( 0<=sliceNum) & (sliceNum<mdims[1]) )
        {
            checkPassed=true;
        }
        else
        {
            checkPassed=false;
        }
    }

    return checkPassed;
}

// shtraf-bat
/**
HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[101m'
    ENDC = '\033[0m'

  cout << "before,after accumulate:  " << Uinit << ", " << Uend << std::endl;
//void vtkKSlice::CollectRevisions(std::ostream&){
//}
  double Uend = this->UIVol->GetScalarComponentAsDouble(i,j,k,0);
  double Uinit = this->UIVol->GetScalarComponentAsDouble(i,j,k,0);
//void vtkKSlice::PrintImage(ostream &os, vtkIndent indent)
//{
//    this->ImageVol->PrintSelf(os, indent);
//}
  */
