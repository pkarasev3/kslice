/*=========================================================================

  Copyright 2004 Sandia Corporation.
  Under the terms of Contract DE-AC04-94AL85000, there is a non-exclusive
  license for use of this work by or on behalf of the
  U.S. Government. Redistribution and use in source and binary forms, with
  or without modification, are permitted provided that this Notice and any
  statement of authorship are reproduced on all copies.

=========================================================================*/

/*========================================================================
 For general information about using VTK and Qt, see:
 http://www.trolltech.com/products/3rdparty/vtksupport.html
=========================================================================*/

/*========================================================================
 !!! WARNING for those who want to contribute code to this file.
 !!! If you use a commercial edition of Qt, you can modify this code.
 !!! If you use an open source version of Qt, you are free to modify
 !!! and use this code within the guidelines of the GPL license.
 !!! Unfortunately, you cannot contribute the changes back into this
 !!! file.  Doing so creates a conflict between the GPL and BSD-like VTK
 !!! license.
=========================================================================*/

#ifndef KVIEWER_GUI_H
#define KVIEWER_GUI_H

#include  <QMainWindow>
#include  <vtkSmartPointer.h>
#include  "GUISetup.h"
#include  "KWidget_2D_left.h"
#include  "KWidget_3D_right.h"
#include "vtkRegularPolygonSource.h"
#include "vtkProperty.h"

class vtkImageThreshold;
class vtkRenderer;
class vtkEventQtSlotConnect;
class vtkObject;
class vtkCommand;
class KvtkImageInteractionCallback;

using cv::Ptr;

class KViewer : public QMainWindow , public Ui::GUI
{
  Q_OBJECT

    vtkSmartPointer<vtkPolyDataMapper> m_CircleMapper;
    vtkSmartPointer<vtkActor> m_CircleActor;
    vtkSmartPointer<vtkRegularPolygonSource> m_Circle;

public:
  KViewer( ) { }
  KViewer( const KViewerOptions& input_options);
  virtual ~KViewer();
  int round(double a);

public slots:

  /**  \brief display the mouse coords in main QT window
    */
  void updateCoords(vtkObject*);

  /**  \brief Handle slider update and show new slice in QVTK window
    */
  void SliceSelect(int SliderVal);
  void SliderCB( int sliceNum );

  /**  \brief Callback for 'click on toggle draw/erase button'
    */
  void ToggleFillEraseMode();

  /**  \brief Callback for 'click on update model button'
    */
  void UpdateModel3D();
  

  /** \brief Record to file the current label map segmentation result
    */
  void SaveSegmentation( );

  /** Saves as user chosen filename
    */
  void SaveAsSegmentation();

  /**  \brief handle click to paint event in draw window
    */
  void mousePaintEvent(vtkObject* obj);

  /**  \brief File -> Load new image file (.mha)
    */
  void LoadImage();

  /**  \brief File -> Load new label file (.mha)
    */
  void LoadLabelMap();

  /** \brief Help -> About
  */
  void About();

  /** \brief update the labels showing brush size and on/off
    */
  void updatePaintBrushStatus(vtkObject*);

  /**  \brief update the volume count and display it in qlabel
  */
  void UpdateVolumeStatus();

  /**  \brief refresh display of all labels, e.g. if opacity or color are changed via gui
  */
  void UpdateMultiLabelDisplay();

  /**  \brief update the volume count and display it in qlabel
   */
  void AddNewLabelMap( );


  /** return the index, stored in left widget
    */
  int  getCurrentSliceIndex() {
    return kwidget_2d_left->currentSliceIndex;
  }
  
  /** don't try to do it yet, this is called from a pipelined function */
  void queueCopyFromCurrentSlice( int idxFrom = -1 );
  
  /** load the single-slice sfls contour segmentation, updates kvdata in place */
  void executeSingleSliceSegmentation( );
  
  /** now do the copy operation, stuff should be in-sync now */
  void handleGenericEvent( vtkObject* obj, unsigned long event);

  /** Button-Press version of queueCopyFromCurrentSlice */
  void copyFromCurrentSlice( );
  
  /** Button-Press version of handleGenericEvent( ... ) */
  void pasteToCurrentSlice( );

  /** move the slider and update everything by +/- amount */
  void MoveSlider( int shiftNumberOfSlices = 0 );

  void SetCircleCursorSize(unsigned int size){
      this->m_Circle->SetRadius(size);
  }

  void SetCircleCursorOpacity(float opacity)
  {
      this->m_CircleActor->GetProperty()->SetOpacity(opacity);
  }

  Ptr<KWidget_3D_right> Get3DWidget()
  {
      return kwidget_3d_right;
  }

  void ResetRotation(bool rotX,bool rotY, bool rotZ);

protected:
  vtkSmartPointer<vtkEventQtSlotConnect>  Connections;
  vtkSmartPointer<KvtkImageInteractionCallback> image_callback;
  // Stores some input and state parameters that don't fit neatly elsewhere.
  Ptr<KViewerOptions>  kv_opts;
  Ptr<KDataWarehouse>  kv_data;

  // top-level state containers and interaction handlers
  Ptr<KWidget_2D_left>   kwidget_2d_left;
  Ptr<KWidget_3D_right>  kwidget_3d_right;


  clock_t t1, t2;

  /** \brief Creates circle cursor
    */
  void InitializeCircleCursor();


  /** \brief Setup slot/signal connections for GUI to fire the member functions
    */
  void ConnectQTSlots( );

  /** \brief load the data files and setup QVTK widgets, setup dimensions for
    *  LUT, slider based on the data file read. Called on startup and file loading.
    */
  void setupQVTKandData( );

  /** \brief Update image information after transformation
    */
  void UpdateImageInformation(vtkImageData* image);

  bool m_RotX,m_RotY,m_RotZ,m_FlipZ;

};

#endif // KVIEWER_GUI_h


// Getting it to build in Linux or Mac: you must follow guidelines noted here:
// http://gcc.gnu.org/faq.html#vtables
// Namely, since you inherit from QMainWindow and Ui::GUI, you have to provide
// bodies for all pure virtual functions declared in those base classes.
// Microsoft C++ compiler circumvents this in a weird way, but it is the kosher c++ spec.
// so, if there's a method "virtual void foo()" in QMainWindow, you could write
// void KViewer::foo() { /* null */ }
// to handle it.


#if 0   // gulag

  /** \brief Create thresholding operator during startup, connect it to UI interactors
    */
  void CreateThresholdFilter( );

  /**  \brief Callback for update min threshold slider
    */
  void minThreshSliderChanged(int lowerLimit);

  /**  \brief Callback for update max threshold slider
    */
  void maxThreshSliderChanged(int upperLimit);

  /////////////////////////// Move to KSegmentor, or a new KWidget?
      vtkSmartPointer<vtkImageActor>          threshActor2D;
      vtkSmartPointer<vtkImageThreshold>      thresholdFilt;
      vtkSmartPointer<vtkImageReslice>        threshReslicer;
 ////////////////////////



#endif
