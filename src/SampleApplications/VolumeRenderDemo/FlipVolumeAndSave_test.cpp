
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
#include "vtkImageMedian3D.h"
#include "vtkImageContinuousDilate3D.h"
#include "vtkImageContinuousErode3D.h"
#include "vtkImageGaussianSmooth.h"
#include "vtkMetaImageWriter.h"
#include "opencv2/core/core.hpp"
#include <vtkMath.h>
#include <cmath>
#include <vector>
#include "KSegmentorBase.h"
#include "KSandbox.h"
#include <omp.h>

#define SP( X )  vtkSmartPointer<X>
using namespace std;
using namespace cv;
using namespace vrcl;

struct LabelVolumeMetaInfo {
  string filename;
  string volume_string;
  string extents_string;
};


vtkSmartPointer<vtkImageData> flip_image2ushort( vtkImageData* imageData )
{
  int dims[3];
  imageData->GetDimensions( dims );
  double img_range[2];
  imageData->GetScalarRange(img_range);
  double spc[3];
  imageData->GetSpacing( spc );

  vtkSmartPointer<vtkImageData> imgvol = vtkSmartPointer<vtkImageData>::New( );
  imgvol->SetDimensions( dims[0],dims[2],dims[1] );
  imgvol->SetNumberOfScalarComponents(1);
  imgvol->SetSpacing( spc[0], spc[2], spc[1] );
  imgvol->SetOrigin( 0,0,0 );
  imgvol->SetScalarTypeToUnsignedShort( );
  imgvol->AllocateScalars( );

  // Values stored 'linearly'
  // TODO: sort this out properly
  unsigned short*  outputPtr = (unsigned short *) imgvol->GetScalarPointer();
  short *inputPtr = static_cast<short*>( imageData->GetScalarPointer() );

  int i,j,k;

  const int K         = dims[2];
  const int J         = dims[1];
  const int I         = dims[0];
  const double imgMin = img_range[0];
  for( k = 0; k < K; k++) {
    for( i=0; i < I; i++) {
      for (j=0; j<J; j++) {
        long idx0                 =  k * I * J + j * I + i;
        long idx1                 =  j * J * K + k * J + i;
        double invalue            =  (double) inputPtr[idx0] - imgMin;
        outputPtr[idx1]           = (unsigned short) invalue;
      }
    }
  }


  return imgvol;
}

vtkSmartPointer<vtkImageData> unflip_ushort( vtkImageData* imageData )
{
  int dims[3];
  imageData->GetDimensions( dims );
  double img_range[2];
  imageData->GetScalarRange(img_range);
  cout << "flipped range: " << img_range[0] << ", " << img_range[1] << endl;
  double spc[3];
  imageData->GetSpacing( spc );

  vtkSmartPointer<vtkImageData> imgvol = vtkSmartPointer<vtkImageData>::New( );
  imgvol->SetDimensions( dims[0],dims[2],dims[1] );
  imgvol->SetNumberOfScalarComponents(1);
  imgvol->SetSpacing( spc[0], spc[2], spc[1] );
  imgvol->SetOrigin( 0,0,0 );
  imgvol->SetScalarTypeToUnsignedShort( );
  imgvol->AllocateScalars( );

  // Values stored 'linearly'
  // TODO: sort this out properly
  unsigned short*  outputPtr = static_cast<unsigned short*>( imgvol->GetScalarPointer());
  unsigned short*  inputPtr  = static_cast<unsigned short*>( imageData->GetScalarPointer() );

  int i,j,k;

  const int K         = dims[1];
  const int J         = dims[2];
  const int I         = dims[0];

  for (j=0; j<J; j++) {
    for( i=0; i < I; i++) {
      for( k = 0; k < K; k++) {
        long idx0                 =  k * I * J + j * I + i;
        long idx1                 =  j * J * K + k * J + i;

        outputPtr[idx0]           =  inputPtr[idx1];
      }
    }
  }
  imgvol->Update();

  imgvol->GetScalarRange(img_range);
  cout << "unflip range: " << img_range[0] << ", " << img_range[1] << endl;

  return imgvol;
}



int main( int argc, char **argv)
{
  SP(vtkMetaImageReader) imgReader =  SP(vtkMetaImageReader)::New();
  SP(vtkMetaImageReader) lblReader =  SP(vtkMetaImageReader)::New();


  SP( vtkImageMedian3D ) median_filter = SP(vtkImageMedian3D)::New();
  SP( vtkImageGaussianSmooth ) gauss_filter = SP(vtkImageGaussianSmooth)::New();
  SP( vtkImageContinuousErode3D ) erode_filter = SP(vtkImageContinuousErode3D)::New();
  SP( vtkImageContinuousDilate3D ) dilate_filter = SP(vtkImageContinuousDilate3D)::New();

  std::vector<LabelVolumeMetaInfo> volume_info( argc-1 );




  if( argc >= 2 ) {

    int kstart = 1;
    bool bUnFlip = false;
    if( std::string(argv[1]).compare( "u" ) == 0 ) {
      cout << "doing UN-flip!" << endl;
      bUnFlip = true;
      kstart = 2;
    }


    for( int k = kstart; k < argc; k++ )
    {
      imgReader->SetFileName( argv[k] );
      imgReader->Update();
      volume_info[k-1].filename = argv[k];
      SP(vtkImageData) imageDataTmp   =  SP(vtkImageData)::New();
      imageDataTmp = imgReader->GetOutput();


      // =============================== part 1: filter for the volume measurements

      // make it a ushort and bag it
      SP(vtkImageData) img      =  SP(vtkImageData)::New();
      if( !bUnFlip )
        img    = flip_image2ushort( imageDataTmp );
      else
        img    = unflip_ushort( imageDataTmp );
      if( img != NULL ) {
        SP(vtkMetaImageWriter) writer = SP(vtkMetaImageWriter)::New();
        std::string filename = "flip_" + string(argv[k]) ;
        writer->SetFileName(filename.c_str());
        writer->SetInput(img);
        writer->Write();
        cout << "wrote to " << filename << endl;
      }

    }

    return 0;
  } else {
    cout << "usage: " << argv[0] << " a.mha b.mha c.mha ... " << endl;
    return -1;
  }
}
