#include <iostream>
#include <boost/python.hpp>

#include "KSlice.h"

#include "KSandbox.h"
#include "KDataWarehouse.h"
#include "KSegmentorBase.h"

#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"


//vtkCxxRevisionMacro(KSlice, "$Revision$"); //necessary?
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

}



void vtkKSlice::PasteSlice(int toSlice){
    ToSlice=toSlice;
    vrcl::copySliceFromTo( dataWarehouse->LabelVol, FromSlice, ToSlice);
}

void vtkKSlice::Initialize(){
    //set up the segmentor
    dataWarehouse->ksegmentor= new KSegmentor3D(ImageVol, LabelVol, contInit, CurrSlice, NumIts, DistWeight, BrushRad);
    dataWarehouse->ksegmentor->SetUseEdgeBasedEnergy( m_bUseEdgeBased );
    dataWarehouse->ksegmentor->SetDistanceWeight(DistWeight);
    initCorrectFlag=1; //initialization is complete

}

void vtkKSlice::runUpdate(){
    if(initCorrectFlag==1){ //already initialized
        dataWarehouse->ksegmentor->SetCurrentSlice(CurrSlice);
        dataWarehouse->ksegmentor->Update2D();
    }
}

void vtkKSlice::PrintSelf(ostream &os, vtkIndent indent)
{
    os << "I've been called!" << std::endl;
}

namespace
{

int test_KSlice_impl( )
{
    std::cout << " пизда с ушами !!1 " << std::endl;

    //inputs
    char imgVolName[] = "../../data/bbTest/imgVol.mha";
    char labVolName[] = "../../data/bbTest/labVol.mha";
    char uiVolName[]  = "../../data/bbTest/uiVol.mha";

    std::cout << "looking for img, label, U: " << imgVolName << ", "
              << labVolName << ", " << uiVolName << std::endl;

    //create two readers one for the image and one for the labels
    vtkMetaImageReader* labReader       = vtkMetaImageReader::New();
    vtkMetaImageReader* imgReader       = vtkMetaImageReader::New();
    vtkMetaImageReader* uiReader        = vtkMetaImageReader::New();

    //one instance of a call to KSlice
    vtkImageData* labVol;
    vtkImageData* imgVol;
    vtkImageData* uiVol;
    int numIts=500;
    int rad=15;
    float distWeight=.3;
    int currSlice=50;

    //test if we can read each file
    int canReadImg = imgReader->CanReadFile(imgVolName);
    int canReadLab = labReader->CanReadFile(labVolName);;               // try to read multiple labels later
    std::cout << "can read img and label? " << canReadImg * canReadLab << std::endl;
    if ( canReadLab !=0  )
    {		// can't read label file, try to create a blank one
      if( canReadImg==0)
      {
        cout<<"Could not read file "<<imgVolName<<" \n";
        //exit(-1); // Satanic! kills python process.
        return 0;
      }

      imgReader->SetFileName(imgVolName);
      imgReader->SetDataScalarTypeToUnsignedShort();
      imgReader->Update();

      labReader->SetFileName(labVolName);
      labReader->SetDataScalarTypeToUnsignedShort();
      labReader->Update();

      uiReader->SetFileName(uiVolName);
      uiReader->SetDataScalarTypeToUnsignedShort();
      uiReader->Update();

      labVol = labReader->GetOutput();
      imgVol = imgReader->GetOutput();
      uiVol = uiReader->GetOutput();

      int* imgDim = imgReader->GetDataExtent();
      int imin=imgDim[0];             int imax=imgDim[1];            int jmin=imgDim[2];
      int jmax=imgDim[3];             int kmin=imgDim[4];            int kmax=imgDim[5];

      labVol->Update();
      imgVol->Update();
      uiVol->Update();

    } else if(canReadImg==0){
      cout<<"Could not read file"<<labVol<<"\n";
      //exit(-1); // Satanic! kills python process.
      return 0;
    }

   //debug to make sure we've loaded things correctly
   double range[2];
   imgVol->GetScalarRange(range);
   labVol->GetScalarRange(range);

  //set up the black box
  vtkSmartPointer<vtkKSlice> bbKSlice = vtkSmartPointer<vtkKSlice>::New();  //created the data, options structures empty for now
  //KSlice* bbKSlice=new KSlice();
  bbKSlice->SetImageVol(imgVol);
  bbKSlice->SetLabelVol(labVol);
  bbKSlice->SetUIVol(uiVol);
  bbKSlice->SetNumIts(numIts);
  bbKSlice->SetBrushRad(rad);
  bbKSlice->SetCurrSlice(currSlice);
  bbKSlice->SetDistWeight(distWeight);
  bbKSlice->Initialize();

  //evolve (simulated user)
  bbKSlice->runUpdate();
  bbKSlice->SetCurrSlice(currSlice-1);
  bbKSlice->runUpdate();
  bbKSlice->SetCurrSlice(currSlice-2);
  bbKSlice->runUpdate();
  bbKSlice->SetCurrSlice(currSlice-3);
  bbKSlice->runUpdate();

  //record the output (FOR TESTING ONLY)
  vtkMetaImageWriter *writer = vtkMetaImageWriter::New();
  writer->SetInput(labVol);
  writer->SetFileName("../../data/bbTest/perturbedLab.mha");
  writer->Write();

  return 0;

  // TODO: dude you need to clean up the shit getting New'd, it crashes python on re-entry
}

} // end anonymous namespace

void test_KSlice( )
{
  test_KSlice_impl( );
}

BOOST_PYTHON_MODULE(KSlice) /* must generate KSlice.so and "import KSlice" in python */
{
  /** \note See this page for more details / extensions:
   *     \link http://www.vtk.org/Wiki/Example_from_and_to_python_converters
   */
  boost::python::def("razstrel",test_KSlice);
}


