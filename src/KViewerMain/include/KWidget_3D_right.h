#ifndef KWIDGET_3D_RIGHT_H
#define KWIDGET_3D_RIGHT_H

#include "vtkSmartPointer.h"
#include "vtkImageActor.h"
#include "vtkImageReslice.h"
#include "vtkImageShiftScale.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkPropPicker.h"
#include "vtkInteractorStyleImage.h"
#include "vtkLODActor.h"
#include "vtkExtractVOI.h"
#include "KViewerOptions.h"
#include "KVolumeRenderView.h"
//#include "opencv2/core/core.hpp"
#include "vtkImagePlaneWidget.h"
#include <memory>

class QVTKWidget;
struct KViewerOptions;
struct KDataWarehouse;


// The mouse motion callback, to pick the image and recover pixel values
class KWidget_3D_right
{

public:


  KWidget_3D_right( QVTKWidget* qvtk_handle );
  ~KWidget_3D_right( ) { }

  typedef std::vector< std::pair< vtkLODActor*, vtkExtractVOI* > > LabelActorMap3DType;

  // whatever is shown in 3D, passes thru this renderer
  vtkSmartPointer<vtkRenderer>            kv3DModelRenderer;
  vtkSmartPointer<vtkLODActor>            labelActor3D;
  QVTKWidget*                             qVTK_widget_right;

  //  sends 3D data map to renderable surface
  vtkSmartPointer<vtkExtractVOI> labelSubVolumeExtractor;

  // stores event handling behvaior for 3D view
  vtkSmartPointer<vtkInteractorStyleTrackballCamera> interactor_style_3DTrackball;

  vtkSmartPointer<vtkRenderWindow>         renderWindowRight;

  // options and parameter settings
  std::shared_ptr<KViewerOptions>                  kv_opts;

  // handles to data objects and file IO
  std::shared_ptr<KDataWarehouse>                  kv_data;

  //void AddFocusPoint( int x, int y, int z );

  std::shared_ptr<KVolumeRenderView>               volRenView;

  // written to during mouse callbacks,
  //     can be grabbed from QT side and put into QString.   (???)
  std::string mouse_position_string;

  /** set and re-update the sub volume extractor source data
    */
  void UpdateSubVolumeExtractor(vtkImageData* new_subvolume_source, unsigned int labNumber=0 );

  void UpdateVolumeRenderer( vtkImageData* image, vtkImageData* label );

  LabelActorMap3DType Get3DLabelMaps()
  {
      return multiLabelMaps3D;
  }

  /* Initialization */

  static void Initialize( std::shared_ptr<KWidget_3D_right> kwidget_3d_right,
                          std::shared_ptr<KViewerOptions> kv_opts,
                          std::shared_ptr<KDataWarehouse> kv_data );

  unsigned int GetNumberOfLabels()
  {
      return multiLabelMaps3D.size();
  }

  unsigned int SetCurrentNumberOfLabels(unsigned int num)
  { // TODO: this seems like an invitation to getting out of sync...
      m_CurrentNumberOfLabels=num;
      return 0;
  }

  void MoveSlice(int increment);

  void MoveSliceTo(int index);


  static void AddNewLabel(std::shared_ptr<KWidget_3D_right> kwidget_3d_right,std::vector<double> color);

  vtkImagePlaneWidget* GetImagePlane(){
      return m_PlaneWidgetZ;
  }

  LabelActorMap3DType   multiLabelMaps3D;
private:
  KWidget_3D_right( );
  KWidget_3D_right( const KWidget_3D_right& );
  KWidget_3D_right & operator=(const KWidget_3D_right &rhs);

  vtkImagePlaneWidget* m_PlaneWidgetZ;

  unsigned int m_CurrentNumberOfLabels;
  unsigned int m_SliceIndex;


};


#endif // KWIDGET_3D_RIGHT_H
