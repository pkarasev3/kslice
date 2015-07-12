#include "vtkProgrammableSource.h"
#include "vtkContourFilter.h"
#include "vtkReverseSense.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkPolyData.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkCellArray.h"
#include "vtkMath.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkImageContinuousDilate3D.h"
#include "vtkImageContinuousErode3D.h"
#include "vtkImageGaussianSmooth.h"
#include "vtkImageContinuousErode3D.h"
#include "vtkImageContinuousDilate3D.h"
#include "vtkSmartPointer.h"
#include "KSandbox.h"
#include "KSegmentorBase.h"

#include "Logger.h"
#include "algorithm"
#include "vector"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//#include "LevelSetCurve.h"
#include <string>
#include <thread>

using std::vector;
using std::string;
using std::cout;
using std::endl;
using namespace vrcl;
using namespace cv;

#define SP( X )  vtkSmartPointer<X>

namespace {
  const static string PrintVerbose = "VerboseKSegUtils";
}

namespace  vrcl
{

  void waterMark(const std::string& text, Mat & img)
  {
    int baseline = 0;
    Size textSize = getTextSize(text, cv::FONT_HERSHEY_PLAIN, 3, 2, &baseline);
    Point textOrg((img.cols - textSize.width) / 2, (img.rows + textSize.height) / 2);
    putText(img, text, textOrg, cv::FONT_HERSHEY_PLAIN, 3, Scalar::all(1), 2, 8);

  }




void fillSliceFromTo(vtkImageData* label_map, int idxFrom, int idxTo,
  vtkImageData* image, double imgMIN_in, bool bPasteAsMax)
{
  int step = (idxTo > idxFrom) * 2 - 1;
  int k = idxFrom + step;
  
  std::list<std::thread> jobs;
  while (k != idxFrom)
  {
    jobs.push_back( std::thread(copySliceFromTo,label_map, idxFrom, k, image, imgMIN_in, bPasteAsMax) );
    if (k == idxTo)
      break;
    k += step;    
  }

   while(!jobs.empty()) 
   {
     jobs.front().join();
     jobs.pop_front();
   }

}


void copySliceFromTo(vtkImageData* label_map,int idxFrom,int idxTo,
                     vtkImageData* image,double imgMIN_in,bool bPasteAsMax)
{
    double imgMIN   = -std::numeric_limits<double>::max();
    short* ptrImage = nullptr;
    if( image ) 
    {
        imgMIN = imgMIN_in;
        ptrImage = static_cast<short*>(image->GetScalarPointer());
    }
    short *ptrLabel = static_cast<short*>(label_map->GetScalarPointer());
    
    int dims[3];
    label_map->GetDimensions( dims );
    assert( idxFrom >= 0 && idxTo >= 0 && idxFrom < dims[2] && idxTo < dims[2] );
    int num_changed = 0;
    for (int i = 0; i < dims[0]; i++)      {
      for (int j = 0; j < dims[1]; j++)    {
            long elemNumFrom = idxFrom * dims[0] * dims[1] + j * dims[0] + i;
            long elemNumTo   = idxTo   * dims[0] * dims[1] + j * dims[0] + i;
            short prevVal    = ptrLabel[elemNumTo];
            short newVal     = ptrLabel[elemNumFrom];
            newVal           = ( bPasteAsMax ) ? std::max(newVal,prevVal) : newVal;
            if(ptrImage)
                if(ptrImage[elemNumTo] < imgMIN_in)
                    newVal = 0;

            ptrLabel[elemNumTo] = newVal;
            num_changed  +=  ( newVal != prevVal );
      }
    }
    cout << "copy/paste from " << idxFrom << " to "
         << idxTo << " changed # pixels: " << num_changed << endl;
}


SP(vtkImageData) removeImageOstrava( vtkImageData* img_dirty,
                                               int erode_sz, int dilate_sz )
{
  int dims[3];
  img_dirty->GetDimensions( dims );
  SP(vtkImageData) img_clean = SP(vtkImageData)::New();
  img_clean->DeepCopy(img_dirty);

  SP(vtkImageData) imageDataTmp   =  SP(vtkImageData)::New();
  SP( vtkImageContinuousErode3D ) erode_filter = SP(vtkImageContinuousErode3D)::New();
  SP( vtkImageContinuousDilate3D ) dilate_filter = SP(vtkImageContinuousDilate3D)::New();

  // erode: the islands should be deleted
  erode_filter->SetInput( img_clean );
  erode_filter->SetKernelSize(erode_sz,erode_sz,erode_sz);
  erode_filter->Update();

  // dilate: non-islands should expand and fill in well. dilate more than erode
  // to ensure the erode does not delete any borders
  dilate_filter->SetInput(erode_filter->GetOutput());
  dilate_filter->SetKernelSize(dilate_sz,dilate_sz,dilate_sz);
  dilate_filter->Update();
  imageDataTmp = dilate_filter->GetOutput();

  // do an "AND" of dilate_filter output and original data.
  unsigned short *ptrDilate   = static_cast<unsigned short*>(imageDataTmp->GetScalarPointer());
  unsigned short *ptrOriginal = static_cast<unsigned short*>(img_dirty->GetScalarPointer());
  unsigned short *ptrClean    = static_cast<unsigned short*>(img_clean->GetScalarPointer());
  int numel                   = dims[0]*dims[1]*dims[2];
  for( int i=0; i<numel; i++ )
  {
    unsigned short dilateVal       =  ptrDilate[i];
    unsigned short originalVal     =  ptrOriginal[i];
    unsigned short newVal          =  (dilateVal > 0 ) * originalVal;
    ptrClean[i]                    = newVal;
  }

  img_clean->Update();
  return img_clean;
}


void getVolumeAsString( const vector<double>& imageSpacing,
                                 vtkImageData* label_map,
                                 string & volumeString, bool numberOnly,
                                 const std::string& right_left_both, double center_line)
{

    short *ptrLabel = static_cast<short*>(label_map->GetScalarPointer());
    int dims[3];
    label_map->GetDimensions( dims );
    double area_sum = 0.0;
    double label_range[2];
    label_map->GetScalarRange( label_range );

    int imin = 0;
    int imax = dims[0];

    if( center_line < 0 ) {
      center_line = dims[0]/2;
    }

    { // only do lateral or medial half, optionally.
      if( 0 == right_left_both.compare("right") ) {
        imax = dims[0];
        imin = center_line;
      } else if( 0 == right_left_both.compare("left") ) {
        imax = center_line;
        imin = 0;
      }
    }

    for (int k = 0; k < dims[2]; k++)    {
      for (int i = imin; i < imax; i++)      {
        for (int j = 0; j < dims[1]; j++)    {
                long elemNum = k * dims[0] * dims[1] + j * dims[0] + i;
                // area_sum    += 1.0*(1e-6 < double( ptrLabel[elemNum] ) ) ;
                area_sum    += double( ptrLabel[elemNum] ) ;
        }
      }
    }
    area_sum -= label_range[0];
    area_sum /= (1e-3 + label_range[1] - label_range[0]);
    area_sum /= pow(10.0, 3.0); // 1mL = 1cm^3 = 1000mm^3
    area_sum *= (imageSpacing[0] * imageSpacing[1] * imageSpacing[2]);
    std::stringstream ss;
    if( numberOnly ) {
      ss << area_sum ;
    } else {
      ss << "  Volume: " << area_sum << " mL ";
    }
    volumeString = ss.str();
    IFLOG( PrintVerbose, cout << "label range: "<<label_range[0]
                              << ","<<label_range[1]<< endl )
    IFLOG( PrintVerbose, cout << "image spacing: " << cv::Mat( imageSpacing ) )
    IFLOG( PrintVerbose, cout << volumeString )
}

std::string getVolumeAsString_ret(vector<double> imageSpacing, vtkImageData* label_map)
{
  std::string tmp;
  getVolumeAsString( imageSpacing, label_map, tmp);
  return tmp;
}

void getXYZExtents( const std::vector<double>& imageSpacing,
                                 vtkImageData* label_map,
                                 std::vector<double>& xyzMin, std::vector<double>& xyzMax)
{
  short *ptrLabel = static_cast<short*>(label_map->GetScalarPointer());
  int dims[3];
  label_map->GetDimensions( dims );

  double label_range[2];
  label_map->GetScalarRange( label_range );
  double Imin = (label_range[1] + label_range[0])/2.0;
  double xmin = 1e99; double xmax = -1e99;
  double ymin = 1e99; double ymax = -1e99;
  double zmin = 1e99; double zmax = -1e99;

  xyzMin = std::vector<double>(3);
  xyzMax = std::vector<double>(3);

  for (int k = 0; k < dims[2]; k++)    {
    for (int i = 0; i < dims[0]; i++)      {
      for (int j = 0; j < dims[1]; j++)    {
              long elemNum = k * dims[0] * dims[1] + j * dims[0] + i;
              double I_of_ijk = double( ptrLabel[elemNum] ) ;
              if( I_of_ijk > Imin ) {
                if( i < xmin )
                  xmin = (double) i;
                if( i > xmax )
                  xmax = (double) i;
                if( j < ymin )
                  ymin = (double) j;
                if( j > ymax )
                  ymax = (double) j;
                if( k < zmin )
                  zmin = (double) k;
                if( k > zmax )
                  zmax = (double) k;
              }
      }
    }
  }

 // set to zero extent if there is nothing here
  if( xmax < 0   ) xmax = 0;
  if( ymax < 0   ) ymax = 0;
  if( zmax < 0   ) zmax = 0;
  if( xmin > 1e3 ) xmin = xmax;
  if( ymin > 1e3 ) ymin = xmax;
  if( zmin > 1e3 ) zmin = xmax;

  xyzMin[0] = xmin;
  xyzMin[1] = ymin;
  xyzMin[2] = zmin;
  xyzMax[0] = xmax;
  xyzMax[1] = ymax;
  xyzMax[2] = zmax;

}

void getXYZExtentsAsString( const vector<double>& imageSpacing,
                                 vtkImageData* label_map,
                                 string & volumeString, std::vector<double>& xyzMin,
                                 std::vector<double>& xyzMax,
                                 bool numberOnly )
{
    short *ptrLabel = static_cast<short*>(label_map->GetScalarPointer());
    int dims[3];
    label_map->GetDimensions( dims );

    double label_range[2];
    label_map->GetScalarRange( label_range );
    double Imin = (label_range[1] + label_range[0])/2.0;
    double xmin = 1e99; double xmax = -1e99;
    double ymin = 1e99; double ymax = -1e99;
    double zmin = 1e99; double zmax = -1e99;

    for (int k = 0; k < dims[2]; k++)    {
      for (int i = 0; i < dims[0]; i++)      {
        for (int j = 0; j < dims[1]; j++)    {
                long elemNum = k * dims[0] * dims[1] + j * dims[0] + i;
                double I_of_ijk = double( ptrLabel[elemNum] ) ;
                if( I_of_ijk > Imin ) {
                  if( i < xmin )
                    xmin = (double) i;
                  if( i > xmax )
                    xmax = (double) i;
                  if( j < ymin )
                    ymin = (double) j;
                  if( j > ymax )
                    ymax = (double) j;
                  if( k < zmin )
                    zmin = (double) k;
                  if( k > zmax )
                    zmax = (double) k;
                }
        }
      }
    }

   // set to zero extent if there is nothing here
    if( xmax < 0   ) xmax = 0;
    if( ymax < 0   ) ymax = 0;
    if( zmax < 0   ) zmax = 0;
    if( xmin > 1e3 ) xmin = xmax;
    if( ymin > 1e3 ) ymin = xmax;
    if( zmin > 1e3 ) zmin = xmax;

    double x_size = (xmax - xmin) * imageSpacing[0];
    double y_size = (ymax - ymin) * imageSpacing[1];
    double z_size = (zmax - zmin) * imageSpacing[2];

    std::stringstream ss;
    if( numberOnly ) {
      ss << x_size << "," << y_size << "," << z_size ;
    } else {
      ss << "X size = " << x_size << ", Y size = " << y_size << ", Z size " << z_size << "  [mm] ";
    }
    volumeString = ss.str();

    xyzMin[0] = xmin;
    xyzMin[1] = ymin;
    xyzMin[2] = zmin;
    xyzMax[0] = xmax;
    xyzMax[1] = ymax;
    xyzMax[2] = zmax;
}


} // end namespace




