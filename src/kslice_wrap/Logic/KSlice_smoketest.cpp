//  KSlice_smoketest.cpp


#include "vtkKSlice.h"
#include <vtkSmartPointer.h>
#include "vtkMetaImageReader.h"
#include "vtkMetaImageWriter.h"
#include "vtkImageData.h"

int main(int argc, char** argv) {

        vtkKSlice* bbKSlice = vtkKSlice::New(); 
//        bbKSlice->SetImageVol(imgVol);
//        bbKSlice->SetLabelVol(labVol);
//        bbKSlice->SetUIVol(uiVol);
        bbKSlice->SetNumIts(5);
        bbKSlice->SetBrushRad(3.0);
    //    bbKSlice->SetSpacing(1.0);
        bbKSlice->SetCurrSlice(1);
        bbKSlice->SetDistWeight(0.01);
//        bbKSlice->SetOrientation("JK");
        bbKSlice->Delete();
        return 0;
}



