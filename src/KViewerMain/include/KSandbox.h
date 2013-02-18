#ifndef KSANDBOX_H_
#define KSANDBOX_H_


#include "vtkSmartPointer.h"
#include "KViewerOptions.h"
#include "KDataWarehouse.h"
#include "Logger.h"

class vtkLookupTable;
class vtkImageData;
using cv::Ptr;

/**   KSandbox: a place to temporarily place 'compact  stand alone functions' while
  *   developing. Inside of the vrcl namespace so that you must conciously
  *   'using namespace vrcl' or  vrcl::Function() to get here, don't want someone
  *   to think the scrap functions here are
  *   part of VTK / ITK and trust them more than they should.
  */

namespace vrcl {

  std::vector<double> get_good_color_0to7( int idx );

  vtkSmartPointer<vtkImageData> image2ushort( vtkImageData* imageData );
  
  void multiplyImageByLabels( vtkImageData* imgData, vtkImageData* lblData );
  
  void setup_file_reader( Ptr<KViewerOptions> kv_opts, Ptr<KDataWarehouse> kv_data );

  vtkSmartPointer<vtkImageData> run_KViewer_Gao3D_interface( Ptr<KViewerOptions> kv_opts,
                                                             Ptr<KDataWarehouse> kv_data );

  void compute_intensity_modes( vtkImageData* image, std::vector<double>& intensityModes );
  
  /** \brief display their memory addresses for debugging. better to avoid situations that need this.
    */
  void print_dereferenced_vtkSmartPointer_pair(
            vtkSmartPointer<vtkObject> obj1, vtkSmartPointer<vtkObject> obj2 );

  /** \brief verify the imagedata is valid in terms of bounds
    */
  void check_extents( vtkImageData* input );

/** create a 'default lookup table'. optional: rgb triple for the interior color.
  * which is something like magenta by default. something crazy will happen if the
  * arg has length other than 0 or 3. */
  vtkSmartPointer<vtkLookupTable> create_default_labelLUT( double maxVal,
              const std::vector<double>& rgb_primary = std::vector<double>()  );


  template<class InputType>
  vtkSmartPointer<vtkImageData> createVTKImageFromPointer(InputType* imagePointer, int* dims, double spacing[3])
  {
      vtkSmartPointer<vtkImageData> imgvol = vtkSmartPointer<vtkImageData>::New( );
      imgvol->SetDimensions( dims[0],dims[1],dims[2] );
      imgvol->SetNumberOfScalarComponents(1);
      imgvol->SetSpacing( spacing );
      imgvol->SetOrigin( 0,0,0 );
      imgvol->SetScalarTypeToDouble( );
      imgvol->AllocateScalars( );
      double* outputImgPointer=static_cast<double*>(imgvol->GetScalarPointer());
      long element=0;

      for (int k=0; k<dims[0]; k++) {
          for (int j=0; j<dims[1]; j++)  {
              for (int i=0; i<dims[2]; i++, element++) {
                  outputImgPointer[element]=imagePointer[element];
              }
          }
      }
      return imgvol;
  }
}

#endif
