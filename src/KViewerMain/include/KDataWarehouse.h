#ifndef KDATAWAREHOUSE_H_
#define KDATAWAREHOUSE_H_


#include "vtkSmartPointer.h"
#include "vtkMetaImageReader.h"
#include "vtkImageData.h"
#include "opencv2/core/core.hpp"

#include <list>
#include <map>
#include <set>


//#include "KSegmentorBase.h"
#include "KViewerOptions.h"

using cv::Ptr;

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
  KDataWarehouse( cv::Ptr<KViewerOptions> kv_opts_in ) ;

  // smart pointers know to kill themselves
  ~KDataWarehouse( ) { }

  vtkSmartPointer<vtkImageData>       imageVolumeRaw;
  vtkSmartPointer<vtkImageData>       imageVolumeRawTrans;

  // the 'saved' or 'initial' label array
  vtkSmartPointer<vtkImageData>       labelDataArray;

  // modified label array
  vtkSmartPointer<vtkImageData>       labelDataArray_new;

  Ptr<KViewerOptions> kv_opts;
  
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



#endif
