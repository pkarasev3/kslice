
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
#include "KSandbox.h"

#define SP( X )  vtkSmartPointer<X>
using namespace std;
using namespace cv;
using namespace vrcl;

struct LabelVolumeMetaInfo {
  string filename;
  string volume_string;
  string volume_Lstring;
  string volume_Rstring;
  string extents_string;
};




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
    cout << "% filename,volume_in_mL,[left],[right],extent_x,extent_y,extent_z" << endl;

    int kstart = 1;
    bool bWriteMHA = false;
    bool bLeftRightSplit = false;
    if( argv[1][0] == 'w' ) { // e.g. if first arg is "write"
      kstart = 2;
      bWriteMHA = true;
      cout << "write mha is on! " << endl;
    } else if( argv[1][0] == 's' ) { // e.g. if first arg is "split"
      kstart = 2;
      bLeftRightSplit = true;
      cout << "split left/right volume is on! " << endl;
    }

    for( int k = kstart; k < argc; k++ )
    {
      string input_filename = argv[k];

      imgReader->SetFileName( input_filename.c_str() );
      int canReadFile = imgReader->CanReadFile(input_filename.c_str());
      if( !canReadFile ) {
        cout << "cannot read file! skipping " << canReadFile << endl;
        continue;
      }

      imgReader->Update();
      volume_info[k-1].filename = input_filename;
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

      // make it a ushort and bag it
      SP(vtkImageData) img      =  SP(vtkImageData)::New();
      img    = image2ushort( imageDataTmp );

      // compute measurements!
      double spc[3];
      img->GetSpacing(spc);
      vector<double> spacing(spc,spc+3);
      double z_factor = std::max(1.0, spc[2] / ( (spc[0] + spc[1]) * 0.5 ) );
      //cout << "resampling Z by factor " << z_factor << endl;
      if( z_factor > 1.0 ) {

        SP(vtkImageResample) resampler = SP(vtkImageResample)::New();
        resampler->SetInput( img );
        resampler->SetAxisMagnificationFactor(0,1.0);
        resampler->SetAxisMagnificationFactor(1,1.0);
        resampler->SetAxisMagnificationFactor(2,z_factor);
        resampler->SetInterpolationModeToCubic();
        resampler->Update();
        imageDataTmp = resampler->GetOutput();

        bool bUseGaussSmooth = true;
        if( bUseGaussSmooth ) {
          gauss_filter->SetInput( imageDataTmp );
          gauss_filter->SetStandardDeviations(1.0,1.0,1.0);
          gauss_filter->Update();
          imageDataTmp = gauss_filter->GetOutput();
        }

        imageDataTmp->GetSpacing(spc);
        vector<double> spacing_new(spc,spc+3);
        vrcl::getVolumeAsString(spacing_new,imageDataTmp, volume_info[k-1].volume_string,true /* number only!*/ );
        if( bLeftRightSplit ) {
          vrcl::getVolumeAsString(spacing_new,imageDataTmp, volume_info[k-1].volume_Lstring,true /* number only!*/,"left");
          vrcl::getVolumeAsString(spacing_new,imageDataTmp, volume_info[k-1].volume_Rstring,true /* number only!*/,"right");
        }

      } else {
        vrcl::getVolumeAsString(spacing,img, volume_info[k-1].volume_string,true /* number only!*/ );
        if( bLeftRightSplit ) {
          vrcl::getVolumeAsString(spacing,img, volume_info[k-1].volume_Lstring,true /* number only!*/,"left");
          vrcl::getVolumeAsString(spacing,img, volume_info[k-1].volume_Rstring,true /* number only!*/,"right");
        }
      }


      ////////////////////////////////////////////////
      // =============================== part 2: filter for the extent measurements

      imageDataTmp = vrcl::removeImageOstrava(imgReader->GetOutput(),3,5);
      img          = image2ushort(imageDataTmp);

      // now get the min/max extents of nonzero portions after median filtering
      vrcl::getXYZExtentsAsString(spacing,img,volume_info[k-1].extents_string,true /* number only!*/ );

      if( !bLeftRightSplit ) {
        cout << volume_info[k-1].filename << ","
             << volume_info[k-1].volume_string << ","
             << volume_info[k-1].extents_string << endl;
      } else {
        cout << volume_info[k-1].filename << ","
             << volume_info[k-1].volume_string << ","
             << volume_info[k-1].volume_Lstring << ","
             << volume_info[k-1].volume_Rstring << ","
             << volume_info[k-1].extents_string << endl;
      }
    }
  } else {
    cout << "bogus args! usage: " << argv[0] << " odin.mha dva.mha tri.mha ... blyat.mha " << endl;
    exit(1);
  }





  return 0;
}
