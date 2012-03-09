
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
#include "vtkSmartPointer.h"
#include "vtkMetaImageReader.h"
#include "vtkImageResample.h"
#include "opencv2/core/core.hpp"
#include <vtkMath.h>
#include <cmath>
#include <vector>
#include "KSegmentorBase.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "vtkMetaImageWriter.h"

#define SP( X )  vtkSmartPointer<X>
using namespace std;
using namespace cv;

int main( int argc, char **argv)
{

  SP(vtkMetaImageReader) reader = SP(vtkMetaImageReader)::New();
  std::string filename = argv[1];
  cout << "attempting to read " << filename << endl;
  reader->SetFileName(filename.c_str());
  reader->Update();
  SP(vtkImageData) img = SP(vtkImageData)::New();
  img = reader->GetOutput();

  SP(vtkMetaImageWriter) labelWriter=   SP(vtkMetaImageWriter)::New();
  labelWriter->SetInput( img );
  labelWriter->SetFileName( (filename+"_decompressed.mha").c_str() );
  labelWriter->SetCompression( false );
  labelWriter->Write();
  cout << "Wrote decompressed file: " << labelWriter->GetFileName() << endl;


  return 0;
}
