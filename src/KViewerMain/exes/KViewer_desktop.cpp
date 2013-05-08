#include "vtkKSlice.h"
#include <vtkSmartPointer.h>
#include "vtkMetaImageReader.h"
#include "vtkMetaImageWriter.h"
#include "vtkImageData.h"

int main(int argc, char** argv) {
    //inputs
//    char imgVolName[] = "../../data/bbTest/imgVol.mha";
//    char labVolName[] = "../../data/bbTest/labVol.mha";
//    char uiVolName[]  = "../../data/bbTest/uiVol.mha";

    char imgVolName[] = "../../data/AN0084/ANON0084.mha";
    char labVolName[] = "../../data/AN0084/ANON0084_label.mha";
    char uiVolName[]  = "../../data/AN0084/ANON0084_ui.mha";


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

    int rad=6;
    double spacing[3]={1, 2, 3};
    float distWeight=.3;
    int currSlice=256;

    //test if we can read each file
    int canReadImg = imgReader->CanReadFile(imgVolName);
    int canReadLab = labReader->CanReadFile(labVolName);;               // try to read multiple labels later

    std::cout << "can read img and label? " << canReadImg * canReadLab << std::endl;
    if ( canReadLab !=0  )
    {		// can't read label file, try to create a blank one
      if( canReadImg==0)
      {
        cout<<"Could not read file "<<imgVolName<<" \n";
        return 0;
      }

      imgReader->SetFileName(imgVolName);
      //imgReader->SetDataScalarTypeToUnsignedShort();
      imgReader->Update();

      labReader->SetFileName(labVolName);
      //labReader->SetDataScalarTypeToUnsignedShort();
      labReader->Update();

      uiReader->SetFileName(uiVolName);
      //uiReader->SetDataScalarTypeToUnsignedShort();
      uiReader->Update();

      labVol = labReader->GetOutput();
      imgVol = imgReader->GetOutput();
      uiVol = uiReader->GetOutput();



      //labVol->Update();
      //imgVol->Update();
      //uiVol->Update();

    } else if(canReadImg==0){
      cout<<"Could not read file"<<labVolName<<"\n";
      return 0;
    }

    //debug to make sure we've loaded things correctly

    int dims[3];
    imgVol->GetDimensions(dims);


    if ( strcmp(imgVol->GetScalarTypeAsString(),"unsigned short")==0){
        std::cout<<"found our type"<<std::endl;
    }
    std::cout<<imgVol->GetScalarTypeAsString()<<std::endl;



    int test=3;
    int numIts=1;
    for(int i=0; i<numIts; i++)
    {
        //set up the black box
        vtkKSlice* bbKSlice = vtkKSlice::New();  //created the data, options structures empty for now
        //KSlice* bbKSlice=new KSlice();
        bbKSlice->SetImageVol(imgVol);
        bbKSlice->SetLabelVol(labVol);
        bbKSlice->SetUIVol(uiVol);
        bbKSlice->SetNumIts(numIts);
        bbKSlice->SetBrushRad(rad);
        bbKSlice->SetSpacing(spacing);
        bbKSlice->SetCurrSlice(currSlice);
        bbKSlice->SetDistWeight(distWeight);
        bbKSlice->Initialize();
        bbKSlice->SetOrientation("JK");

        switch(test)
        {
        case 1:
            //evolve (simulated user)
            bbKSlice->SetCurrSlice(currSlice-2);
            bbKSlice->runUpdate2p5D(1);
            bbKSlice->SetCurrSlice(currSlice-3);
            bbKSlice->runUpdate2p5D(0);
            break;
        case 2:
            bbKSlice->SetCurrSlice(currSlice-2);
            bbKSlice->runUpdate3D(1);
            bbKSlice->SetCurrSlice(currSlice-3);
            bbKSlice->runUpdate3D(0);
            break;
        case 3:
            bbKSlice->runUpdate2D(1);
            bbKSlice->SetCurrSlice(currSlice-1);
            bbKSlice->runUpdate2D(0);
            break;

        }

        //bbKSlice->PrintImage(std::cout, vtkIndent());

        //record the output (FOR TESTING ONLY)
        vtkMetaImageWriter *writer = vtkMetaImageWriter::New();
        writer->SetInput(labVol);
        writer->SetFileName("../../data/bbTest/perturbedLab.mha");
        writer->Write();

        writer->Delete();

        bbKSlice->Delete();
        std::cout<<"iteration number: "<<i<<std::endl;
    }

    labReader->Delete();
    imgReader->Delete();
    uiReader->Delete();

    return 0;
}



