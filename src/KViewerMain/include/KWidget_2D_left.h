#ifndef KWIDGET_2D_LEFT_H
#define KWIDGET_2D_LEFT_H

#include "vtkSmartPointer.h"
#include "vtkImageActor.h"
#include "vtkImageReslice.h"
#include "vtkImageShiftScale.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkPropPicker.h"
#include "KViewerOptions.h"
#include "KDataWarehouse.h"
#include "vtkInteractorStyleImage.h"
#include "KSegmentorBase.h"
#include "KInteractiveLabelMap.h"
#include "vtkTransform.h"

using  cv::Ptr;
class  QVTKWidget;
class  vtkLookupTable;


class KWidget_2D_left
{

public:
  KWidget_2D_left( QVTKWidget *qVTK_widget_left );
  virtual ~KWidget_2D_left( ) { }

  vtkSmartPointer<vtkRenderer>            kvImageRenderer;
  vtkSmartPointer<vtkPropPicker>          sliceViewPicker;
  vtkSmartPointer<vtkImageActor>          imageActor;
  vtkSmartPointer<vtkImageActor>          labelActor2D;


  std::vector< Ptr<KInteractiveLabelMap> > multiLabelMaps;
  int activeLabelMapIndex; // which label is being functed

  int currentSliceIndex;   // which slice we're viewing now
  int m_CurrentSliceOrigin; // currentSliceIndex in "world coordinates"
  int cacheSliceIndex;     // stored for copy/paste operations on 'c' key

  // handle on the QT object, primarily to do update() after some computations
  QVTKWidget *qVTK_widget_left;

  // contains the 'saturation lookup table', sets windowing range
  vtkSmartPointer<vtkImageMapToColors> colorMap;

  // update the scaling/display of image slice data in 2D
  vtkSmartPointer<vtkImageReslice>        imageReslicer;

  // TODO: probably should be encapsulated lower problem is the 'thresholdReslicer' wants it
  vtkSmartPointer<vtkLookupTable>         labelLUT;

  // map data values to colors in display (shift and scale them)
  vtkSmartPointer<vtkImageShiftScale>     label2D_shifter_scaler;

  // stores event handling behvaior of 2D display
  vtkSmartPointer<vtkInteractorStyleImage> image_interactor_style;

  /** where we render to
    */
  vtkSmartPointer<vtkRenderWindow>         renderWindowLeft;

  /** maps volume data intensity to display color
    */
  vtkSmartPointer<vtkLookupTable>          color_HSV_LookupTable;

  // options and parameter settings
  cv::Ptr<KViewerOptions>                  kv_opts;

  // handles to data objects and file IO
  cv::Ptr<KDataWarehouse>                  kv_data;

  // written to during ^ouse callbacks, QT side can put in a QString.
  std::string mouse_position_string;

  /** Save the current label map to a unique (time-stamped) file name, .mha format */
  void SaveCurrentLabelMap( );

  /** Save current label map to a specific filename */
  void SaveAsCurrentLabelMap( const std::string &fileName );


  // Initialization

  /** Create the necessary framework for displaying label and image in a render window,
    * connected to a QTVTK widget and interactor
    */
  void Initialize( Ptr<KViewerOptions> kv_opts, Ptr<KDataWarehouse> kv_data );
  void InitializeTransform(char trans=' ',float angle=90);


  // Callbacks

  void CallbackMousePosition( vtkObject* obj );

  void CallbackSliceSlider( int currSlice, double currSliceOrigin );

  void PrintHelp( ) {
    cout << "Setting the interactation style for IMAGE. \nMouse wheel zoom, ";
    cout << " click-select, cntrl-click-drag to rotate "
        << "about normal axis, alt-click-drag to pan around. " << endl;
  }

  /*********************************************************************
Multiple Label Maps
*********************************************************************/

  /** allocate a new label map, push it to the list, register the kv_opts
    and image volume data. \warning: user might run out of memory very fast
    if they click the new label button many times */
  void AddNewLabelMap( );

  void LoadMultiLabels( const std::vector<std::string>& label_files );

  /** toggle which map in the list is 'active' for UI edits */
  void SelectActiveLabelMap(int labelMapIndex);

  /** free all the labelmaps, wipe the list, make a fresh list with one labelmap */
  void ResetLabelMapList( );
  
  /** for every labelmap, copy & paste from index A to index B */ 
  void CopyLabelsFromTo( int iFrom, int iTo, bool bPasteAll = false );

  void RunSegmentor( int slice_index = -1, bool bAllLabels = false, bool use2D=true );
 
  /** called internally when a display update is needed,
      such as when a new labelmap is created */
  void UpdateMultiLabelMapDisplay( bool UpdateTransform=false );

  void UpdateTransform();

  /** get the one that's being "edited" now */
  vtkSmartPointer<vtkImageData> GetActiveLabelMap( );

  static const std::string VERBOSE;

  std::vector<double> GetOutputSpacing()
  {
    if( m_OutputSpacing.size() != 3 ) {
      m_OutputSpacing = std::vector<double>(3,1.0);
    }
    for( int k = 0; k < 3; k++ ) {
      m_OutputSpacing[k] = kv_opts->imageSpacing[k];
    }
    std::cout << "widget2D output spacing = " << cv::Mat( m_OutputSpacing ) << std::endl;
    return m_OutputSpacing;
  }

 ///TODO: make private
vtkTransform* m_SliderTrans;
private:

  /**  no copying!  */
  KWidget_2D_left( const KWidget_2D_left& );
  KWidget_2D_left & operator=(const KWidget_2D_left &rhs);
  KWidget_2D_left( );

  vtkSmartPointer<vtkLookupTable> satLUT;
  vtkSmartPointer<vtkImageShiftScale> intensShift;

  bool bNoInputLabelFiles;
  std::vector<double> m_OutputSpacing;




  /** internal common code for saving label files */
  void SaveLabelsInternal( const std::stringstream& ss );

  void SetupRenderWindow();
  void SetupImageDisplay(bool transformUpdate=false);
  void SetupLabelDisplay();

};



#endif // KWIDGET_2D_LEFT_H
