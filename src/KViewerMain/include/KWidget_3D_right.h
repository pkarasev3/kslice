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
#include "opencv2/core/core.hpp"


using cv::Ptr;
class QVTKWidget;
struct KViewerOptions;
struct KDataWarehouse;


// The mouse motion callback, to pick the image and recover pixel values
class KWidget_3D_right
{
public:
  KWidget_3D_right( QVTKWidget* qvtk_handle );
  ~KWidget_3D_right( ) { }

  /*  Member Pointers  */

  /* Assume for now that these are grabbed by KViewerMain and/or the right
             widge! (e.g. loose ownership of the render window, actors, etc )    */

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
  cv::Ptr<KViewerOptions>                  kv_opts;

  // handles to data objects and file IO
  cv::Ptr<KDataWarehouse>                  kv_data;
  
  void AddFocusPoint( int x, int y, int z );
  
  cv::Ptr<KVolumeRenderView>               volRenView;

  // written to during mouse callbacks, 
  //     can be grabbed from QT side and put into QString.   (???)
  std::string mouse_position_string;
  
  /** set and re-update the sub volume extractor source data 
    */
  void UpdateSubVolumeExtractor(vtkImageData* new_subvolume_source );
                             
  void UpdateVolumeRenderer( vtkImageData* image, vtkImageData* label );

  /* Initialization */

  static void Initialize( Ptr<KWidget_3D_right> kwidget_3d_right,
                          Ptr<KViewerOptions> kv_opts,
                          Ptr<KDataWarehouse> kv_data );

  /* Debug/Help */

  void PrintHelp () { }

private:
  KWidget_3D_right( );
  KWidget_3D_right( const KWidget_3D_right& );
  KWidget_3D_right & operator=(const KWidget_3D_right &rhs);
};


#endif // KWIDGET_3D_RIGHT_H
