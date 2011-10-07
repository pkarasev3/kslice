
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
#include "KSegmentor.h"

#define SP( X )  vtkSmartPointer<X>
using namespace std;
using namespace cv;

struct LabelVolumeMetaInfo {
  string filename;
  string volume_string;
  string extents_string;
};


SP(vtkImageData) image2ushort( vtkImageData* imageData )
{
  SP(vtkImageData) imgvol = SP(vtkImageData)::New( );
  int dims[3];
  imageData->GetDimensions( dims );
  double spacing_in[3];
  imageData->GetSpacing(spacing_in);
  //cout << "spacing:" << Mat( vector<double>(spacing_in,spacing_in+3) ) << endl;

  imgvol->SetDimensions( dims[0],dims[1],dims[2] );
  imgvol->SetNumberOfScalarComponents(1);
  imgvol->SetSpacing( spacing_in );
  imgvol->SetOrigin( 0,0,0 );
  imgvol->SetScalarTypeToUnsignedShort( );
  imgvol->AllocateScalars( );

  // Values stored 'linearly', slightly unsure about the orientation though.
  unsigned short*  outputPtr = (unsigned short *) imgvol->GetScalarPointer();
  short *inputPtr = static_cast<short*>( imageData->GetScalarPointer() );
  int numel                 = dims[0]*dims[1]*dims[2];
  for( int i=0; i<numel; i++ )
  {
    short invalue            =  inputPtr[i];
    unsigned short nextvalue = (unsigned short ) invalue ;
    *outputPtr= nextvalue;
    *outputPtr++;
  }

  return imgvol;
}

SP(vtkImageData) CleanLabelMap( vtkMetaImageReader* reader )
{
  vtkImageData* img_dirty = reader->GetOutput();
  SP(vtkImageData) clean_img = SP(vtkImageData)::New();
  clean_img->DeepCopy(img_dirty);
  clean_img->Update();

  //  int * imgDim = clean_img->GetExtent();
  //  int imin=imgDim[0];             int imax=imgDim[1];            int jmin=imgDim[2];
  //  int jmax=imgDim[3];             int kmin=imgDim[4];            int kmax=imgDim[5];
  int* dataExtent         = reader->GetDataExtent();

  int kmin = 0;
  int imax= reader->GetHeight()-1;
  int jmax= reader->GetWidth()-1;
  int numSlices = dataExtent[5]-dataExtent[4]+1;
  int kmax= numSlices-1;

  short *ptrLabel=static_cast<short*>(clean_img->GetScalarPointer());

  // Doh, undo the effect of "// need some non-zero part so 3D display doesn't break"
  int fill_sz = 3;
  kmin = kmax/2 - fill_sz;
  kmax = kmax/2 + fill_sz;
  if( kmin < 0 )
    kmin = 0;
  if( kmax > dataExtent[5] )
    kmax = dataExtent[5];

  for( int k = kmin ; k < kmax; k++ ) {
    for( int i = imax/2 - fill_sz ; i < imax/2+fill_sz; i++ ) {
      for( int j = jmax/2 - fill_sz ; j < jmax/2+fill_sz; j++ ) {
        unsigned long elemNum = k * imax * jmax + j * imax + i;
        cout << " ptrLabel[elemNum] = " <<  ptrLabel[elemNum] << endl;
        if( ptrLabel[elemNum] > 0 ) {
          int aha = 1; // for breakpoint
        }
        // ptrLabel[elemNum] = 1000;
      }
    }
  }

  return clean_img;

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
    cout << "% processing label files, printing their meta info. Format: " << endl;
    cout << "% filename,volume_in_mL" << endl;

    int kstart = 1;
    bool bWriteMHA = false;
    if( argv[1][0] == 'w' ) { // if first arg is "w"
      kstart = 2;
      bWriteMHA = true;
      cout << "write mha is on! " << endl;
    }

    for( int k = kstart; k < argc; k++ ) {
      imgReader->SetFileName( argv[k] );
      imgReader->Update();
      volume_info[k-1].filename = argv[k];
      SP(vtkImageData) imageDataTmp   =  SP(vtkImageData)::New();
      imageDataTmp = imgReader->GetOutput();

      if( bWriteMHA ) {
        SP(vtkMetaImageWriter) writer = SP(vtkMetaImageWriter)::New();
        writer->SetFileName("tmp.mha");
        writer->SetInput(imageDataTmp);
        writer->Write();
        cout << "wrote to tmp.mha! " << endl;
      }

      // =============================== part 1: filter for the volume measurements

      // read it from disk, fails if file name is wacked
      bool bUseGaussSmooth = false;
      if( bUseGaussSmooth ) {
        gauss_filter->SetInput( imageDataTmp );
        gauss_filter->SetStandardDeviations(1.0,1.0,0.5);
        gauss_filter->Update();
        imageDataTmp = gauss_filter->GetOutput();
      }

      // make it a ushort and bag it
      SP(vtkImageData) img      =  SP(vtkImageData)::New();
      img    = image2ushort( imageDataTmp );

      // compute measurements!
      double spc[3];
      img->GetSpacing(spc);
      vector<double> spacing(spc,spc+3);
      vrcl::getVolumeAsString(spacing,img, volume_info[k-1].volume_string,true /* number only!*/ );



      ////////////////////////////////////////////////
      // =============================== part 2: filter for the extent measurements

      imageDataTmp = vrcl::removeImageOstrava(imgReader->GetOutput(),3,5);
      img          = image2ushort(imageDataTmp);

      // now get the min/max extents of nonzero portions after median filtering
      vrcl::getXYZExtentsAsString(spacing,img,volume_info[k-1].extents_string,true /* number only!*/ );


      cout << volume_info[k-1].filename << ","
           << volume_info[k-1].volume_string << ","
           << volume_info[k-1].extents_string << endl;
    }
  } else {
    cout << "bogus args! usage: " << argv[0] << " odin.mha dva.mha tri.mha ... blyat.mha " << endl;
    exit(1);
  }





  return 0;
}
