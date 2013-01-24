#include "KSlice.h"


/**
 * this function is only used to debug the KSlice module
 */
int main(int argc, char** argv)
{
    //inputs
    char imgVolName[] = "../../data/bbTest/imgVol.mha";
    char labVolName[] = "../../data/bbTest/labVol.mha";
    char uiVolName[]  = "../../data/bbTest/uiVol.mha";

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

    if ( canReadLab !=0  )
    {		// can't read label file, try to create a blank one
      if( canReadImg==0)
      {
        cout<<"Could not read file "<<imgVolName<<" \n";
        exit(-1);
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
      exit(-1);
    }


   //debug to make sure we've loaded things correctly
   double range[2];
   imgVol->GetScalarRange(range);
   labVol->GetScalarRange(range);

  //set up the black box
  KSlice* bbKSlice=new KSlice(); //created the data, options structures empty for now
  bbKSlice->SetImage(imgVol);
  bbKSlice->SetLabel(labVol);
  bbKSlice->SetUI(uiVol);
  bbKSlice->SetNumIters(numIts);
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
}

