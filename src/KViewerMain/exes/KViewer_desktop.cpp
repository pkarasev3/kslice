#include "KSlice.h"


/**
 * this function is only used to debug the KSlice module
 */
int main(int argc, char** argv)
{
    //inputs
    std::string imgVolName = "../../data/bbTest/imgVol.mha";
    std::string labVolName = "../../data/bbTest/labVol.mha";
    std::string uiVolName  = "../../data/bbTest/uiVol.mha";

    //create two readers one for the image and one for the labels
    vtkMetaImageReader labReader       = vtkMetaImageReader::New();
    vtkMetaImageReader imgReader       = vtkMetaImageReader::New();
    vtkMetaImageReader uiReader        = vtkMetaImageReader::New();

    //one instance of a call to KSlice
    vtkImageData* labVol;
    vtkImageData* imgVol;
    vtkImageData* uiVol;
    int numIts;
    int rad;
    float distWeight=.3;
    int currSlice=0;


    //test if we can read each file
    int canReadImg = imgReader->CanReadFile(imgVolName);
    int canReadLab = 0;               // try to read multiple labels later

    if ( canReadLab == 0 )
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
  //evolve
  bbKSlice->Update();



  return 0;
}

