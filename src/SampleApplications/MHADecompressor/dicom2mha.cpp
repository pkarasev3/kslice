#include "vtkImageViewer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkStructuredPointsReader.h"
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkVolumeRayCastMapper.h"
#include "vtkVolumeRayCastCompositeFunction.h"
#include "vtkVolume.h"
#include "vtkRenderer.h"
#include "vtkPoints.h"
#include "vtkImageData.h"
#include "vtkDICOMImageReader.h"
#include "vtkSmartPointer.h"
#include  "vtkMetaImageWriter.h"
#include <string>

using namespace std;

int main( int argc, char **argv)
{
  if( argc < 3 ) {
    cout << "usage: " << argv[0] << "  ../path/fullOfDicomSeries   ../output/foo.mha    little/big" << endl;
    exit(1);
  }
  
  vtkSmartPointer<vtkDICOMImageReader>  dicomReader = vtkSmartPointer<vtkDICOMImageReader>::New();
  
  dicomReader->SetDirectoryName(argv[1]);
  dicomReader->SetDataByteOrderToBigEndian(); // typical computers
  if( argc == 3 )  { 
    if( 0 == std::string(argv[3]).compare("little") )  {
     dicomReader->SetDataByteOrderToLittleEndian();  // on 'exotic' hardware
    }
  }
  
  dicomReader->SetDebug(true);
  dicomReader->Update();
  
  vtkSmartPointer<vtkImageData> image = vtkSmartPointer<vtkImageData>::New();
  image = dicomReader->GetOutput();
  //vtkIndent indentVTK = vtkIndent::vtkIndent(0);
  //image->PrintSelf( cout, indentVTK );
  
  vtkSmartPointer<vtkMetaImageWriter>  writer = vtkSmartPointer<vtkMetaImageWriter>::New();
  writer->SetInput( image );
  writer->SetCompression( false );
  writer->SetFileName( argv[2] );
  writer->Write();
  
  cout << "finished write attempt to " << argv[2] << endl;
  
}
