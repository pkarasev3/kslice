
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


SP(vtkImageData) makeVtkImage( const std::vector<cv::Mat> & image_glob )
{
  SP(vtkImageData) imgvol   =  SP(vtkImageData)::New();

  cv::Mat img1 = image_glob[0];

  imgvol->SetDimensions( img1.cols,img1.rows,image_glob.size() );
  imgvol->SetNumberOfScalarComponents(1);
  imgvol->SetSpacing(1.0,1.0,1.0);
  imgvol->SetOrigin( 0,0,0);
  imgvol->SetScalarTypeToUnsignedShort( );
  imgvol->AllocateScalars( );


  int nImgs = image_glob.size();
  double labelRange[2];
  unsigned short*  img_out  = (unsigned short *) imgvol->GetScalarPointer();
  int numel = img1.cols * img1.rows;
  for( int k = 0; k < nImgs; k++ ) {

    unsigned short*  img_k_in = (unsigned short *) image_glob[k].ptr<unsigned short>(0); // ->GetScalarPointer();
    for( int i=0; i<numel; i++ )
    {
      unsigned short ival =  img_k_in[i];
      img_out[i + k * numel] = ival;
    }
  }
  imgvol->GetScalarRange( labelRange );
  cout << "min,max: " << labelRange[0] << "," << labelRange[1] << endl;
  return imgvol;
}

int main( int argc, char **argv)
{

  std::vector< cv::Mat > image_glob(argc-1);

  if( argc >= 2 ) {
    for( int k = 1; k < argc; k++ ) {
      double dmin, dmax;
      cout << "attempting to load " << argv[k] << endl;
      cv::Mat img = imread(argv[k]); // read it from disk, fails if file name is wacked
      cv::flip(img.clone(),img,0);
      img.clone().convertTo(img,CV_32FC3);

      cv::cvtColor(img.clone(),img,CV_RGB2GRAY);
      cv::minMaxLoc(img,&dmin,&dmax);
      cout << "initial min,max = " << dmin << "," << dmax << " ... ";
      img = (img - dmin) / (dmax - dmin) * (pow(2.0,12)-1.0);
      cv::minMaxLoc(img,&dmin,&dmax);
      cout << "final min,max = " << dmin << "," << dmax << endl;
      img.convertTo(image_glob[k-1],CV_16UC1);
    }
  } else {
    cout << "bogus args! usage: " << argv[0] << " odin.jpg dva.png tri.bmp ... blyat.pgm " << endl;
    exit(1);
  }

  SP(vtkImageData) imageData = makeVtkImage( image_glob );

  SP(vtkMetaImageWriter) writer = SP(vtkMetaImageWriter)::New();
  writer->SetInput(imageData);
  writer->SetFileName("image_glob.mha");
  writer->Write();

  cout << "writer claims to have written to " << writer->GetFileName() << endl;

  return 0;
}
