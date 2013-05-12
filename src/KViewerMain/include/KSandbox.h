#ifndef KSANDBOX_H_
#define KSANDBOX_H_


#include "vtkSmartPointer.h"
#include "KViewerOptions.h"
//#include "KDataWarehouse.h"
//#include "Logger.h"
#include <assert.h>
#include "vtkImageData.h"
#include "vtkLookupTable.h"
//class vtkLookupTable;
//class vtkImageData;

//using cv::Ptr;

/**   KSandbox: a place to temporarily place 'compact  stand alone functions' while
  *   developing. Inside of the vrcl namespace so that you must conciously
  *   'using namespace vrcl' or  vrcl::Function() to get here, don't want someone
  *   to think the scrap functions here are
  *   part of VTK / ITK and trust them more than they should.
  */



namespace vrcl {

  enum Orient {SLICE_IJ=0, SLICE_JK=1, SLICE_IK=2};
  std::vector<double> get_good_color_0to7( int idx );

  vtkSmartPointer<vtkImageData> image2ushort( vtkImageData* imageData );

  void multiplyImageByLabels( vtkImageData* imgData, vtkImageData* lblData );

  //void setup_file_reader( Ptr<KViewerOptions> kv_opts, Ptr<KDataWarehouse> kv_data );

  //vtkSmartPointer<vtkImageData> run_KViewer_Gao3D_interface( Ptr<KViewerOptions> kv_opts,
   //                                                          Ptr<KDataWarehouse> kv_data );

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


  template<typename T> void convertSliceToDouble(T* array, double *destination, int dim0, int dim1, int dim2, int currSlice, Orient sliceView)
  {
      long elemNum=0;
      unsigned int element3D;
      int k=currSlice;

      //copy elements into the slices
      for (int j=0; j<dim1; j++)
      {
          for (int i=0; i<dim0; i++)
          {
              switch(sliceView)
              {
                  case SLICE_IJ:
                      element3D    =  k*dim1*dim0 +j*dim0+i;// wrong for non-IJ orientations!
                      break;
                  case SLICE_JK:
                      element3D    =  j*dim0*dim2 + i*dim2+k;//
                      break;
                  case SLICE_IK:
                      element3D    =  j*dim0*dim2 + k*dim0+i;//
                      break;
                  default:
                      assert(0);
                      break;
              }
              destination[elemNum]        = (double) array[element3D];
              elemNum++;
          }
      }
  }

  template<typename T> void convertSliceToDouble(int labType, T* array, double *destination,
                                                 int dim0, int dim1, int dim2, int currSlice, Orient sliceView)
  {
      switch(labType)
      {
      case 0:     //#define VTK_VOID            0
          assert(0);
          break;
      case 1:    //#define VTK_BIT             1
          vrcl::convertSliceToDouble( (bool *)array, destination  , dim0, dim1, dim2, currSlice, sliceView );
          break;
      case 2:    //#define VTK_CHAR            2
          vrcl::convertSliceToDouble( (char *)array, destination  , dim0, dim1, dim2, currSlice, sliceView );
          break;
      case 3:    //#define VTK_UNSIGNED_CHAR   3
          vrcl::convertSliceToDouble( (unsigned char *)array, destination  , dim0, dim1, dim2, currSlice, sliceView );
          break;
      case 4:    //#define VTK_SHORT           4
          vrcl::convertSliceToDouble( (short *)array, destination  , dim0, dim1, dim2, currSlice, sliceView );
          break;
      case 5:    //#define VTK_UNSIGNED_SHORT  5
          vrcl::convertSliceToDouble( (unsigned short *)array, destination  , dim0, dim1, dim2, currSlice, sliceView );
          break;
      case 6:    //#define VTK_INT             6
          vrcl::convertSliceToDouble( (int *)array, destination  , dim0, dim1, dim2, currSlice, sliceView );
          break;
      case 7:    //#define VTK_UNSIGNED_INT    7
          vrcl::convertSliceToDouble( (unsigned int *)array, destination  , dim0, dim1, dim2, currSlice, sliceView );
          break;
      case 8:    //#define VTK_LONG            8
          vrcl::convertSliceToDouble( (long *)array, destination  , dim0, dim1, dim2, currSlice, sliceView );
          break;
      case 9:    //#define VTK_UNSIGNED_LONG   9
          vrcl::convertSliceToDouble( (unsigned long *)array, destination  , dim0, dim1, dim2, currSlice, sliceView );
          break;
      case 10:    //#define VTK_FLOAT          10
          vrcl::convertSliceToDouble( (float *)array, destination , dim0, dim1, dim2, currSlice, sliceView );
          break;
      case 11:    //#define VTK_DOUBLE         11
          vrcl::convertSliceToDouble( (double *)array, destination , dim0, dim1, dim2, currSlice, sliceView );
          break;
      }
  }








  template<typename T> void convertSliceToShort(T* array, short *destination,
                                                 int dim0, int dim1, int dim2, int currSlice, Orient sliceView)
  {
      long elemNum=0;
      unsigned int element3D;
      int k=currSlice;

      //copy elements into the slices
      for (int j=0; j<dim1; j++)
      {
          for (int i=0; i<dim0; i++)
          {
              switch(sliceView)
              {
                  case SLICE_IJ:
                      element3D    =  k*dim1*dim0 +j*dim0+i;// wrong for non-IJ orientations!
                      break;
                  case SLICE_JK:
                      element3D    =  j*dim0*dim2 + i*dim2+k;//
                      break;
                  case SLICE_IK:
                      element3D    =  j*dim0*dim2 + k*dim0+i;//
                      break;
                  default:
                      assert(0);
                      break;
              }
              destination[elemNum]        = (short) array[element3D];
              elemNum++;
          }
      }
  }


  template<typename T> void convertSliceToShort(int labType, T* array, short *destination,
                                                 int dim0, int dim1, int dim2, int currSlice, Orient sliceView)
  {
      switch(labType)
      {
      case 0:     //#define VTK_VOID            0
          assert(0);
          break;
      case 1:    //#define VTK_BIT             1
          vrcl::convertSliceToShort( (bool *)array, destination  , dim0, dim1, dim2, currSlice, sliceView );
          break;
      case 2:    //#define VTK_CHAR            2
          vrcl::convertSliceToShort( (char *)array, destination  , dim0, dim1, dim2, currSlice, sliceView );
          break;
      case 3:    //#define VTK_UNSIGNED_CHAR   3
          vrcl::convertSliceToShort( (unsigned char *)array, destination  , dim0, dim1, dim2, currSlice, sliceView );
          break;
      case 4:    //#define VTK_SHORT           4
          vrcl::convertSliceToShort( (short *)array, destination  , dim0, dim1, dim2, currSlice, sliceView );
          break;
      case 5:    //#define VTK_UNSIGNED_SHORT  5
          vrcl::convertSliceToShort( (unsigned short *)array, destination  , dim0, dim1, dim2, currSlice, sliceView );
          break;
      case 6:    //#define VTK_INT             6
          vrcl::convertSliceToShort( (int *)array, destination  , dim0, dim1, dim2, currSlice, sliceView );
          break;
      case 7:    //#define VTK_UNSIGNED_INT    7
          vrcl::convertSliceToShort( (unsigned int *)array, destination  , dim0, dim1, dim2, currSlice, sliceView );
          break;
      case 8:    //#define VTK_LONG            8
          vrcl::convertSliceToShort( (long *)array, destination  , dim0, dim1, dim2, currSlice, sliceView );
          break;
      case 9:    //#define VTK_UNSIGNED_LONG   9
          vrcl::convertSliceToShort( (unsigned long *)array, destination  , dim0, dim1, dim2, currSlice, sliceView );
          break;
      case 10:    //#define VTK_FLOAT          10
          vrcl::convertSliceToShort( (float *)array, destination , dim0, dim1, dim2, currSlice, sliceView );
          break;
      case 11:    //#define VTK_DOUBLE         11
          vrcl::convertSliceToShort( (double *)array, destination , dim0, dim1, dim2, currSlice, sliceView );
          break;
      }
  }



  template<typename T> void convertFloatToSlice(T *array, float *source,
                                                 int dim0, int dim1, int dim2, int currSlice, Orient sliceView, int currLabel)
  {
      long elemNum=0;
      unsigned int element3D;
      int k=currSlice;

      float phi_val = 0;
      elemNum=0;
      for (int j=0; j<dim1; j++)
      {
          for (int i=0; i<dim0; i++)
          {
              phi_val= source[elemNum];
              switch(sliceView)
              {
                  case SLICE_IJ:
                      element3D    =  k*dim1*dim0 +j*dim0+i;// wrong for non-IJ orientations!
                      break;
                  case SLICE_JK:
                      element3D    =  j*dim0*dim2 + i*dim2+k;//
                      break;
                  case SLICE_IK:
                      element3D    =  j*dim0*dim2 + k*dim0+i;//
                      break;
                  default:
                      assert(0);
                      break;
              }
              array[element3D]=   ( (T) 0 >= phi_val )*currLabel;
              elemNum++;
          }
      }
  }



  template<typename T> void convertFloatToSlice( int labType, T* array, float *source,
                             int dim0, int dim1, int dim2,int currSlice, Orient sliceView, int currLabel)
  {
      switch(labType)
      {
      case 0:     //#define VTK_VOID            0
          assert(0);
          break;
      case 1:    //#define VTK_BIT             1
          vrcl::convertFloatToSlice( (bool *)array, source  , dim0, dim1, dim2, currSlice, sliceView, currLabel );
          break;
      case 2:    //#define VTK_CHAR            2
          vrcl::convertFloatToSlice( (char *)array, source  , dim0, dim1, dim2, currSlice, sliceView, currLabel );
          break;
      case 3:    //#define VTK_UNSIGNED_CHAR   3
          vrcl::convertFloatToSlice( (unsigned char *)array, source  , dim0, dim1, dim2, currSlice, sliceView, currLabel );
          break;
      case 4:    //#define VTK_SHORT           4
          vrcl::convertFloatToSlice( (short *)array, source  , dim0, dim1, dim2, currSlice, sliceView, currLabel );
          break;
      case 5:    //#define VTK_UNSIGNED_SHORT  5
          vrcl::convertFloatToSlice( (unsigned short *)array, source  , dim0, dim1, dim2, currSlice, sliceView, currLabel );
          break;
      case 6:    //#define VTK_INT             6
          vrcl::convertFloatToSlice( (int *)array, source  , dim0, dim1, dim2, currSlice, sliceView, currLabel );
          break;
      case 7:    //#define VTK_UNSIGNED_INT    7
          vrcl::convertFloatToSlice( (unsigned int *)array, source  , dim0, dim1, dim2, currSlice, sliceView, currLabel );
          break;
      case 8:    //#define VTK_LONG            8
          vrcl::convertFloatToSlice( (long *)array, source  , dim0, dim1, dim2, currSlice, sliceView, currLabel );
          break;
      case 9:    //#define VTK_UNSIGNED_LONG   9
          vrcl::convertFloatToSlice( (unsigned long *)array, source  , dim0, dim1, dim2, currSlice, sliceView, currLabel );
          break;
      case 10:    //#define VTK_FLOAT          10
          vrcl::convertFloatToSlice( (float *)array, source  , dim0, dim1, dim2, currSlice, sliceView, currLabel );
          break;
      case 11:    //#define VTK_DOUBLE         11
          vrcl::convertFloatToSlice( (double *)array, source  , dim0, dim1, dim2, currSlice, sliceView, currLabel );
          break;
      default:
          std::cout<< "Invalid Type!\n";
          break;
      }
  }

















  template<typename T> void convertImage(T *array, double *destination, int dimx, int dimy, int dimz)
  {
      long elemNum=0;
      for (int k=0; k<=dimx-1; k++)
      {
        for (int j=0; j<=dimy-1; j++)
        {
            for (int i=0; i<=dimz-1; i++,elemNum++)
            {
                destination[elemNum] = (double) array[elemNum];
            }
        }
      }
  }

  template<typename T> void convertImage(int imgType, T *array, double *destination, int dimx, int dimy, int dimz)
  {
      switch(imgType)
      {
      case 0:     //#define VTK_VOID            0
          assert(0);
          break;
      case 1:    //#define VTK_BIT             1
          vrcl::convertImage( (bool *) array, destination, dimx, dimy, dimz);
          break;
      case 2:    //#define VTK_CHAR            2
          vrcl::convertImage( (char *) array, destination, dimx, dimy, dimz);
          break;
      case 3:    //#define VTK_UNSIGNED_CHAR   3
          vrcl::convertImage( (unsigned char *)array, destination, dimx, dimy, dimz);
          break;
      case 4:    //#define VTK_SHORT           4
          vrcl::convertImage( (short *)array, destination, dimx, dimy, dimz);
          break;
      case 5:    //#define VTK_UNSIGNED_SHORT  5
          vrcl::convertImage( (unsigned short *)array, destination, dimx, dimy, dimz);
          break;
      case 6:    //#define VTK_INT             6
          vrcl::convertImage( (int *)array, destination, dimx, dimy, dimz);
          break;
      case 7:    //#define VTK_UNSIGNED_INT    7
          vrcl::convertImage( (unsigned int *)array, destination, dimx, dimy, dimz);
          break;
      case 8:    //#define VTK_LONG            8
          vrcl::convertImage( (long *)array, destination, dimx, dimy, dimz);
          break;
      case 9:    //#define VTK_UNSIGNED_LONG   9
          vrcl::convertImage( (unsigned long *)array, destination, dimx, dimy, dimz);
          break;
      case 10:    //#define VTK_FLOAT          10
          vrcl::convertImage( (float *)array, destination, dimx, dimy, dimz);
          break;
      case 11:    //#define VTK_DOUBLE         11
          vrcl::convertImage( (double *)array, destination, dimx, dimy, dimz);
          break;
      }
  }


  template<typename T> void convertLabel(T *array, short *destination, int dimx, int dimy, int dimz)
  {
      long elemNum=0;
      for (int k=0; k<=dimx-1; k++)
      {
        for (int j=0; j<=dimy-1; j++)
        {
            for (int i=0; i<=dimz-1; i++,elemNum++)
            {
                destination[elemNum]= (short) ( 0 < array[elemNum] );
            }
        }
      }
  }

  template<typename T> void convertLabel(int labType, T *array, short *destination,
                                         int dimx, int dimy, int dimz)
  {
      switch(labType)
      {
      case 0:     //#define VTK_VOID            0
          assert(0);
          break;
      case 1:    //#define VTK_BIT             1
          vrcl::convertLabel( (bool *) array, destination, dimx, dimy, dimz);
          break;
      case 2:    //#define VTK_CHAR            2
          vrcl::convertLabel( (char *) array, destination, dimx, dimy, dimz);
          break;
      case 3:    //#define VTK_UNSIGNED_CHAR   3
          vrcl::convertLabel( (unsigned char *)array, destination, dimx, dimy, dimz);
          break;
      case 4:    //#define VTK_SHORT           4
          vrcl::convertLabel( (short *)array, destination, dimx, dimy, dimz);
          break;
      case 5:    //#define VTK_UNSIGNED_SHORT  5
          vrcl::convertLabel( (unsigned short *)array, destination, dimx, dimy, dimz);
          break;
      case 6:    //#define VTK_INT             6
          vrcl::convertLabel( (int *)array, destination, dimx, dimy, dimz);
          break;
      case 7:    //#define VTK_UNSIGNED_INT    7
          vrcl::convertLabel( (unsigned int *)array, destination, dimx, dimy, dimz);
          break;
      case 8:    //#define VTK_LONG            8
          vrcl::convertLabel( (long *)array, destination, dimx, dimy, dimz);
          break;
      case 9:    //#define VTK_UNSIGNED_LONG   9
          vrcl::convertLabel( (unsigned long *)array, destination, dimx, dimy, dimz);
          break;
      case 10:    //#define VTK_FLOAT          10
          vrcl::convertLabel( (float *)array, destination, dimx, dimy, dimz);
          break;
      case 11:    //#define VTK_DOUBLE         11
          vrcl::convertLabel( (double *)array, destination, dimx, dimy, dimz);
          break;
      }
  }



  template<typename T> void setLabel3D(T *array, float *source, int Nelements, int currLabel)
  {
      float phi_val = 0;
      for (int idx=0;idx<Nelements;idx++)
      {
          phi_val = source[idx];
          array[idx] =( (T) 0 >= phi_val )*currLabel;
      }
  }

  template<typename T> void setLabel3D(int labType, T *array, float *source, int Nelements, int currLabel)
  {
      switch(labType)
      {
      case 0:     //#define VTK_VOID            0
          assert(0);
          break;
      case 1:    //#define VTK_BIT             1
          vrcl::setLabel3D((bool *) array,  source, Nelements, currLabel);
          break;
      case 2:    //#define VTK_CHAR            2
          vrcl::setLabel3D((char *) array,  source, Nelements, currLabel);
          break;
      case 3:    //#define VTK_UNSIGNED_CHAR   3
          vrcl::setLabel3D((unsigned char *) array,  source, Nelements, currLabel);
          break;
      case 4:    //#define VTK_SHORT           4
          vrcl::setLabel3D((short *) array,  source, Nelements, currLabel);
          break;
      case 5:    //#define VTK_UNSIGNED_SHORT  5
          vrcl::setLabel3D((unsigned short *) array,  source, Nelements, currLabel);
          break;
      case 6:    //#define VTK_INT             6
          vrcl::setLabel3D((int *) array,  source, Nelements, currLabel);
          break;
      case 7:    //#define VTK_UNSIGNED_INT    7
          vrcl::setLabel3D((unsigned int *) array,  source, Nelements, currLabel);
          break;
      case 8:    //#define VTK_LONG            8
          vrcl::setLabel3D((long *) array,  source, Nelements, currLabel);
          break;
      case 9:    //#define VTK_UNSIGNED_LONG   9
          vrcl::setLabel3D((unsigned long *) array,  source, Nelements, currLabel);
          break;
      case 10:    //#define VTK_FLOAT          10
          vrcl::setLabel3D((float *) array,  source, Nelements, currLabel);
          break;
      case 11:    //#define VTK_DOUBLE         11
          vrcl::setLabel3D((double *) array,  source, Nelements, currLabel);
          break;
      }

}


}



#endif
