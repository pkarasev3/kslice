#ifndef KDATAWAREHOUSE_H_
#define KDATAWAREHOUSE_H_


#include "vtkSmartPointer.h"
#include "vtkMetaImageReader.h"
#include "vtkImageData.h"
#include "opencv2/core/core.hpp"

#include <list>
#include <map>
#include <set>

#include <memory>
//#include "KSegmentorBase.h"
#include "KViewerOptions.h"

//using cv::Ptr;

struct KDataWarehouse
{
private:
  // want to be able to pull out vtkImageData* objects by name, e.g. foo = label_arrays["initial_data"]
  std::map< std::string, vtkImageData* >     label_arrays;
  std::map< std::string, vtkImageData* >     image_arrays;

  // init smart pointers in constructors, no one else should be doing New() calls NEVER USED!!!!
  KDataWarehouse( ) :
                      labelDataArray(     vtkSmartPointer<vtkImageData>::New() ),
                      labelDataArray_new( vtkSmartPointer<vtkImageData>::New() )
  {
      std::cout<<"Datawarehouse initialized"<<std::endl;
  }

public:
  KDataWarehouse( std::shared_ptr<KViewerOptions> kv_opts_in ) ;

  // smart pointers know to kill themselves
  ~KDataWarehouse( ) { }

  vtkSmartPointer<vtkImageData>       imageVolumeRaw;
  vtkSmartPointer<vtkImageData>       imageVolumeRawTrans;

  // the 'saved' or 'initial' label array
  vtkSmartPointer<vtkImageData>       labelDataArray;

  // modified label array
  vtkSmartPointer<vtkImageData>       labelDataArray_new;

  std::shared_ptr<KViewerOptions> kv_opts;

  std::vector<double> intensityModes;

  /** \brief  Record a new label data as the 'candidate new label data'
    */
  void SetLabelDataNew( vtkImageData* new_label_data ) {
    this->labelDataArray_new = new_label_data;
  }

  /** \brief "accept" the updated label arrays from
    * segmentation, make it the new 'for display' label array
    */
  void PushLabelDataNewToCurrent();


  vtkImageData*  GetImageArrayByName( const std::string& image_array_name ) {
    vtkImageData* data = this->image_arrays[image_array_name];
    if( !data ) throw "bogus image array name!";
    return data;
  }
  vtkImageData*  GetLabelArrayByName( const std::string& image_array_name ) {
    vtkImageData* data = this->label_arrays[image_array_name];
    if( !data ) throw "bogus label array name!";
    return data;
  }

  void InsertImageArray( vtkImageData* data, const std::string& image_array_name) {
    image_arrays[image_array_name] = data;
  }

  void InsertLabelArray( vtkImageData* data, const std::string& label_array_name) {
    label_arrays[label_array_name] = data;
  }

  void UpdateRawImage(vtkImageData* raw3DImage)
  {
      this->imageVolumeRaw->DeepCopy(raw3DImage);
      this->imageVolumeRaw->UpdateInformation();
  }

  //Updating label data array acording to current transformation
  void UpdateLabelDataArray(vtkImageData* raw3DLabel)
  {
      this->labelDataArray->DeepCopy(raw3DLabel);
      this->labelDataArray->UpdateInformation();
  }


};


struct UserInputVerboseRecorder
{
  typedef std::map<unsigned int, unsigned int> index_to_tk_mapper;
  vtkSmartPointer<vtkImageData>          timestamp_volume;
  std::map<unsigned int, unsigned int>   inputLocationandTime;
  int startTime;
  void initialize( vtkImageData* reference_volume ) {
      startTime        = (int) time(NULL);
      timestamp_volume = vtkSmartPointer<vtkImageData>::New();
      timestamp_volume->DeepCopy( reference_volume ); // get the field data, scalar alloc, etc
      unsigned short* ptr = static_cast<unsigned short*>( timestamp_volume->GetScalarPointer() );
      long Nelements      = timestamp_volume->GetNumberOfPoints();
      cout<<"created UserInputVerboseRecorder,  t0=" << startTime << ", #pts=" << Nelements <<endl;
      for( int i=0; i < Nelements; i++ ) {
          ptr[i] = 0;
      }
  }

  void process_click( unsigned int element ) {
      unsigned int timeDiff        = (int) time(NULL) - startTime; assert(timeDiff>0);
      inputLocationandTime.insert(index_to_tk_mapper::value_type(element,timeDiff));
      unsigned short* ptr = static_cast<unsigned short*>( timestamp_volume->GetScalarPointer() );
      ptr[element] = (unsigned short) timeDiff; assert(timeDiff < 1e4);
  }

  int get_number_actuated_voxels() const {
      return (int)inputLocationandTime.size();
  }
};

#endif
