
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
#include "vtkMath.h"
#include "vtkImageGradientMagnitude.h"
#include <cmath>
#include <vector>
#include "KSegmentorBase.h"
#include "KSandbox.h"
#include "boost/foreach.hpp"
#include "boost/program_options.hpp"

#define SP( X )  vtkSmartPointer<X>
using namespace std;
using namespace cv;
using namespace vrcl;
namespace po = boost::program_options;

struct DumpHistogramOptions
{

  std::vector<std::string> LabelArrayFilenames;
  std::string ImageArrayFilename;
  std::vector<SP(vtkImageData)>  label_volumes;
  vtkSmartPointer<vtkImageData>  image_volume;
  vtkSmartPointer<vtkImageData>  imgGradMag_volume;

  int iVerbosity;

  /** send args from main() to me to set params
    */
  void setFromArgs(int argc, char **argv) {

    po::options_description commands("Allowed options");
    commands.add_options()
        ("Labels,L",po::value< std::vector<std::string> >(&LabelArrayFilenames)->multitoken(),
         "which labels to pre-load, e.g. --Labels=a.mha b.mha c.mha")
        ("Image,I",po::value<std::string>(&ImageArrayFilename)->default_value(""),"image volume file")
        ("Verbosity,v",po::value<int>(&iVerbosity)->default_value(0),"verbosity; 0 to print final values only, v >= 1 for more info")
        ("help","print help")
        ;

    po::variables_map vm;
    po::store( parse_command_line(argc, argv, commands, po::command_line_style::unix_style ), vm);

    po::notify(vm);

    if (vm.count("help")) {
      cout << commands << "\n";
      exit(1);
    }

    SP(vtkMetaImageReader) imgReader =  SP(vtkMetaImageReader)::New();
    SP(vtkMetaImageReader) lblReader =  SP(vtkMetaImageReader)::New();

    image_volume = SP(vtkImageData)::New();
    label_volumes.resize(LabelArrayFilenames.size());

    if( ! LabelArrayFilenames.empty() ) {
      if( iVerbosity > 0 )  {  cout << "attempting to load image: " << ImageArrayFilename << endl;  }

      imgReader->SetFileName(ImageArrayFilename.c_str());
      imgReader->SetDataScalarTypeToUnsignedShort();
      imgReader->Update();

      if( imgReader->GetDataSpacing()[2] < (imgReader->GetDataSpacing()[0] + imgReader->GetDataSpacing()[1] ) ) {
        if( iVerbosity > 0 ) {
          cout << "WARNING: this volume does not appear to be oriented as W x H x N with (W,H) much greater than N" << endl;
        }
      }
      image_volume = image2ushort(imgReader->GetOutput());

      imgGradMag_volume = SP(vtkImageData)::New();
      SP(vtkImageGradientMagnitude) grad_mag_getter = SP(vtkImageGradientMagnitude)::New();
      grad_mag_getter->SetInput(image_volume);
      grad_mag_getter->SetDimensionality(2);
      grad_mag_getter->SetNumberOfThreads(4);
      grad_mag_getter->SetHandleBoundaries(1);
      grad_mag_getter->Update();
      imgGradMag_volume = grad_mag_getter->GetOutput();

      if( iVerbosity > 0 ) { cout << "attempting to load labels: " << endl; }
      for( int k = 0; k < (int) LabelArrayFilenames.size(); k++ ) {
        label_volumes[k] = SP(vtkImageData)::New();
        lblReader->SetFileName(LabelArrayFilenames[k].c_str());
        lblReader->Update();
        label_volumes[k] = image2ushort(lblReader->GetOutput());
        int npts = label_volumes[k]->GetNumberOfPoints();
        if( iVerbosity > 0 ) { cout << "loading label file " << LabelArrayFilenames[k] << ", #points = " << npts << endl; }
        assert( npts == image_volume->GetNumberOfPoints() ); // same size !?
      }
    } else {
      exit(2);
    }

  }


};


void print_per_slice_mean_and_stddev( const string& name, const vector<double>& nnz,
                                      const vector<double>& nzmean,  const vector<double>& nzstddev,
                                      const vector<double>& Gnzmean, const vector<double>& Gnzstddev )

{

  int numSlices = nzmean.size(); assert( numSlices == (int) nzstddev.size() );

  int numKeep   = 8; // keep this many *adjacent* slices, with highest nnz sum in between
  int max_idx   = 0;
  double max_sum= 0.0;


  for( int m =0 ; m < (numSlices - max_idx); m++ ) {
    double sum_m = 0.0;
    for( int i = 0; i < numKeep; i++ ) {
      sum_m += nnz[m+i];
    }
    if( sum_m > max_sum ) {
      max_idx = m;
      max_sum = sum_m;
    }
  }

  cout << "IMAGE_MEANS,"<< name << "," ;
  for( int k = max_idx; k < (max_idx+numKeep); k++ ) {
    cout << nzmean[k] ;
    if( k < (max_idx+numKeep-1) ) { cout << ","; }
    else { cout << endl; }
  }
  cout << "IMAGE_STD-DEVS," << name << "," ;
  for( int k = max_idx; k < (max_idx+numKeep); k++ ) {
    cout << nzstddev[k];
    if( k < (max_idx+numKeep-1) ) { cout << ","; }
    else { cout << endl; }
  }
  cout << "MAG_GRAD_MEANS," << name << "," ;
  for( int k = max_idx; k < (max_idx+numKeep); k++ ) {
    cout << Gnzmean[k] ;
    if( k < (max_idx+numKeep-1) ) { cout << ","; }
    else { cout << endl; }
  }
  cout << "MAG_GRAD_STD-DEVS," << name << "," ;
  for( int k = max_idx; k < (max_idx+numKeep); k++ ) {
    cout << Gnzstddev[k];
    if( k < (max_idx+numKeep-1) ) { cout << ","; }
    else { cout << endl; }
  }

}


int main( int argc, char **argv)
{
  DumpHistogramOptions opts;
  opts.setFromArgs(argc,argv);

  vtkImageData* image = opts.image_volume; assert(image!=NULL);
  unsigned short *ptrImage=static_cast<unsigned short*>(image->GetScalarPointer());

  vtkImageData* imgGrad = opts.imgGradMag_volume; assert(imgGrad!=NULL);
  unsigned short *ptrIGrad=static_cast<unsigned short*>(imgGrad->GetScalarPointer());

  // assumption: the width and height are high-res, relatively small # of slices

  int imgWidth  = image->GetDimensions()[0];
  int imgHeight = image->GetDimensions()[1];
  int numSlices = image->GetDimensions()[2];
  float sx = image->GetSpacing()[0];  float sy = image->GetSpacing()[1];  float sz = image->GetSpacing()[2];

  cout << opts.ImageArrayFilename << endl;
  printf("width = %d, height = %d, numSlices = %d \n",imgWidth,imgHeight,numSlices );
  printf("x-spacing = %f mm, y-spacing = %f mm, z-spacing = %f mm \n",sx,sy,sz );

  int N = opts.LabelArrayFilenames.size();
  for( int k = 0; k < N; k++ ) { // for every label file loaded
    vtkImageData* label = opts.label_volumes[k]; assert(label!=NULL);

    std::vector<double> nzsum(numSlices,0.0); // sum of image values where label was nonzero
    std::vector<double> nzmean(numSlices,0.0); // mean of image values where label was nonzero
    std::vector<double> nzstddev(numSlices,0.0); // standard deviation of values where label was nonzero
    std::vector<double> nnz(numSlices,1e-9);  // number of nonzero label locations

    std::vector<double> Gnzsum(numSlices,0.0); // sum of grad image values where label was nonzero
    std::vector<double> Gnzmean(numSlices,0.0); // mean of grad image values where label was nonzero
    std::vector<double> Gnzstddev(numSlices,0.0); // standard deviation of grad values where label was nonzero

    unsigned short *ptrLabel=static_cast<unsigned short*>(label->GetScalarPointer());
    long elemNum=0;

    // 1. first extract per-slice means and nonzero counts
    for (int i=0; i<imgWidth; i++)  {
      for (int j=0; j<imgHeight; j++) {
        for( int kk = 0; kk < numSlices; kk++) {
          elemNum = kk * imgHeight * imgWidth + j * imgWidth + i;
          unsigned short labelValue = ptrLabel[elemNum];
          if( labelValue > 0 ) {
            nzsum[kk]  += ptrImage[elemNum];
            Gnzsum[kk] += ptrIGrad[elemNum];
            nnz[kk]    += 1.0;
          }
        }
      }
    }
    for( int kk = 0; kk < numSlices; kk++ ) {
      nzmean[kk]  = nzsum[kk] / nnz[kk]; assert(nnz[kk]>0.0);
      Gnzmean[kk] = Gnzsum[kk] / nnz[kk];
    }

    // 2. next extract per-slice standard deviation
    for (int i=0; i<imgWidth; i++)  {
      for (int j=0; j<imgHeight; j++) {
        for( int kk = 0; kk < numSlices; kk++) {
          elemNum = kk * imgHeight * imgWidth + j * imgWidth + i;
          unsigned short labelValue = ptrLabel[elemNum];
          if( labelValue > 0 ) {
            nzstddev[kk]  += pow( (double)(ptrImage[elemNum])-nzmean[kk],  2.0 );
            Gnzstddev[kk] += pow( (double)(ptrIGrad[elemNum])-Gnzmean[kk], 2.0 );
          }
        }
      }
    }
    for( int kk = 0; kk < numSlices; kk++ ) {
      nzstddev[kk]  = sqrt( nzstddev[kk] /  nnz[kk] );
      Gnzstddev[kk] = sqrt( Gnzstddev[kk] / nnz[kk] );
    }

    print_per_slice_mean_and_stddev( opts.LabelArrayFilenames[k], nnz, nzmean, nzstddev, Gnzsum, Gnzstddev );


  }



  return 0;
}
