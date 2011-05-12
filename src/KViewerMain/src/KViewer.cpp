#include <QMenu>
#include <iostream>
#include <QtGui>

#include "vtkCommand.h"
#include "vtkEventQtSlotConnect.h"
#include "vtkDebugLeaks.h"
#include "vtkConeSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCamera.h"
#include "vtkMetaImageReader.h"
#include "vtkIndent.h"
#include "vtkImageData.h"
#include "vtkDataSetMapper.h"
#include "vtkProperty.h"
#include "vtkPolyDataNormals.h"
#include "vtkMarchingCubes.h"
#include "vtkImagePlaneWidget.h"
#include "vtkImageMapper.h"
#include "vtkImageMapToColors.h"
#include "vtkLookupTable.h"
#include "vtkSmartPointer.h"
#include "vtkInteractorStyleImage.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkDataSetMapper.h"
#include "vtkImageData.h"
#include "vtkRendererCollection.h"
#include "vtkKWImage.h"
#include "vtkImageCast.h"
#include "vtkIndent.h"
#include "vtkImageThreshold.h"
#include "vtkImageSource.h"

#include "KInteractiveLabelMap.h"
#include "KViewer.h"
#include "Logger.h"
#include "KvtkImageInteractionCallback.h"
#include "KSandbox.h"
#include "KDataWarehouse.h"

#define SP( X )  vtkSmartPointer<X>

using namespace cv;
using namespace vrcl;
using std::vector;


KViewer::KViewer( const KViewerOptions& kv_opts_in ) {

  kv_opts           =  Ptr<KViewerOptions>( new KViewerOptions( kv_opts_in ) );
  kv_data           =  Ptr<KDataWarehouse>( new KDataWarehouse( kv_opts ) );

  this->setupQVTKandData( );
  this->updatePaintBrushStatus(NULL);
}


KViewer::~KViewer() {
  Connections->Delete();
}


void KViewer::updateCoords(vtkObject* obj) {
  this->kwidget_2d_left->CallbackMousePosition( obj );
  std::stringstream ss;
  ss << kwidget_2d_left->mouse_position_string ;
  coord->setText(QString( ss.str().c_str() ) );
  qVTK1->setFocus( );
}

void KViewer::updatePaintBrushStatus(vtkObject*) {
  std::stringstream ss;
  std::stringstream toggle;
  ss << "Brush Size: " << this->kv_opts->GetBrushSize();

  if( ! image_callback->Erase() ) {
    toggle << "Mode: Draw";
  } else {
    toggle << "Mode: Erase";
  }
  if( this->image_callback->ButtonDown() ) {
    toggle << "\t(On)";
  } else {
    toggle << "\t(Off)";
  }
  ss << "       " << toggle.str();
  QString str = QString(ss.str().c_str());
  paintBrushStatus->setText(str);
  qVTK1->setFocus( );

}

void KViewer::SliceSelect(int SliderVal) {
  double currSliceOrigin=kv_opts->sliderMin +kv_opts->sliceZSpace*SliderVal;
  kwidget_2d_left->CallbackSliceSlider( SliderVal, currSliceOrigin );
  qVTK1->update();
}

void KViewer::UpdateVolumeStatus()
{   // performance note: iterates over the label image, not instantaneous like other labels
  kv_data->labelDataArray->Update();
  std::string volumeDisplay;
  getVolumeAsString( kv_opts->imageSpacing,
                     kv_data->labelDataArray, volumeDisplay );
  QString str = QString( volumeDisplay.c_str() );
  volumeOfLabelmap->setText(str);
  updatePaintBrushStatus(NULL);

  UpdateModel3D();
  qVTK1->setFocus( );
}

void KViewer::ToggleFillEraseMode() {

  updatePaintBrushStatus(NULL);
  qVTK1->setFocus( );
}

void KViewer::UpdateModel3D() {
  kwidget_3d_right->UpdateVolumeRenderer( kv_data->imageVolumeRaw, kv_data->labelDataArray );
  qVTK1->update();
  qVTK2->update();
}


void KViewer::SaveSegmentation( ) {
  kwidget_2d_left->SaveCurrentLabelMap( );
}

void KViewer::SaveAsSegmentation() {
  kwidget_2d_left->SaveAsCurrentLabelMap( this->saveAsLineEdit->text().toStdString() );
}

void KViewer::LoadImage() {
  this->kv_opts->LoadImage( );
  this->setupQVTKandData( );
}

void KViewer::LoadLabelMap(){
  QString path;
  path = QFileDialog::getOpenFileName(    this,    "Choose a file to open",    "../data/",   "*.mha" );
  this->kv_opts->LoadLabel(path.toStdString());
  this->setupQVTKandData( );
}

void KViewer::About() {
  QMessageBox::about(this, "KSlice Version 0.6", "Created by Peter Karasev, Ivan Koleskov and Karol Chudy from the Tannenbaum Lab");
}

int KViewer::round(double a) {
  return int(a + 0.5);
}

void KViewer::executeSingleSliceSegmentation( )
{
  handleGenericEvent( NULL, (unsigned long) 's' );
}

void KViewer::copyFromCurrentSlice( )
{
  queueCopyFromCurrentSlice( kwidget_2d_left->currentSliceIndex );
}

void KViewer::pasteToCurrentSlice( )
{
  handleGenericEvent( NULL, (unsigned long) 'v' );
}

void KViewer::queueCopyFromCurrentSlice(int idxFrom)
{   
  cout << "... queued copy from slice " << idxFrom << endl;
  kwidget_2d_left->cacheSliceIndex = idxFrom; // bag it for pasting later
}

void KViewer::MoveSlider( int shiftNumberOfSlices )
{
  int currentSlice  = this->kwidget_2d_left->currentSliceIndex;
  int newSliceIndex = currentSlice + shiftNumberOfSlices;
  if( newSliceIndex < this->kv_opts->numSlices   &&  newSliceIndex >= 0 ) {
    this->SliceSelect( newSliceIndex );
    this->Slider->setValue( newSliceIndex );
  }
}

void KViewer::AddNewLabelMap( )
{
  // create a new label map
  kwidget_2d_left->AddNewLabelMap( );

  qVTK1->update();

  cout << "added new label map. total # = " << kwidget_2d_left->multiLabelMaps.size() << endl;
}

void KViewer::handleGenericEvent( vtkObject* obj, unsigned long event )
{   

  int cache_idx1 = kwidget_2d_left->cacheSliceIndex;
  if ( cache_idx1 >= 0 ) // if a copy-from is stored
  {
    char keyPressed = ' ';
    if( NULL != obj ) { // not null <=> we got here from a vtkCallBack, not a button press
      vtkRenderWindowInteractor* imgWindowInteractor = vtkRenderWindowInteractor::SafeDownCast(obj);
      keyPressed       = *imgWindowInteractor->GetKeySym();
    } else if (event == (unsigned long) 's' ) {
      keyPressed = 's';
    } else if (event == (unsigned long) 'v' ){
      keyPressed = 'v'; // pressing 'paste button' in QT took us here
    }
    int slice_idx                 = kwidget_2d_left->currentSliceIndex;
    int label_idx                 = kwidget_2d_left->activeLabelMapIndex;
    Ptr<KSegmentor> kseg          = kwidget_2d_left->multiLabelMaps[label_idx]->ksegmentor;
    SP(vtkImageData) active_label = SP(vtkImageData)::New();
    active_label                  = kwidget_2d_left->GetActiveLabelMap();
    bool needToPropagateData = false;
    switch ( keyPressed ) {
    case '1':
      kwidget_2d_left->SelectActiveLabelMap( label_idx - 1 );
      break;
    case '2':
      kwidget_2d_left->SelectActiveLabelMap( label_idx + 1 );
      break;
    case 'b': // update 3D
      UpdateVolumeStatus();
      break;
    case 'v': // Paste!
      kwidget_2d_left->CopyLabelsFromTo( cache_idx1, slice_idx );
      break;
    case 's': // run "KSegmentor"
      // TODO: make this work inside of kwidget_2d_left, like CopyLabelsFromTo does!
      // then get rid of 'propagate data' crap, use 
      // the private update function in widget 2d!
      kv_data->labelDataArray_new           = SP(vtkImageData)::New();
      kv_data->labelDataArray_new->ShallowCopy( kv_data->labelDataArray );
      kseg->setCurrLabelArray(kv_data->labelDataArray_new);
      kseg->setRadius( this->kv_opts->paintBrushRad / 2 );
      kseg->setCurrIndex( slice_idx );
      kseg->setNumIterations( 30 );
      kseg->initializeData();
      kseg->intializeLevelSet();
      kseg->Update();

      needToPropagateData = true;
      break;
    default:
      break;
    }

    if( needToPropagateData ) {
      needToPropagateData = false;
      // propagate the new "input" into the display objects
      kwidget_2d_left->multiLabelMaps[label_idx]->label2D_shifter_scaler->SetInput( kv_data->labelDataArray_new );
      kwidget_2d_left->multiLabelMaps[label_idx]->label2D_shifter_scaler->Update();
      kv_data->labelDataArray = kv_data->labelDataArray_new;
      qVTK1->update();
    }
  }


}

void KViewer::mousePaintEvent(vtkObject* obj) {

  if ( this->image_callback->ButtonDown() )
  {
    int slice_idx          = kwidget_2d_left->currentSliceIndex;
    int label_idx          = kwidget_2d_left->activeLabelMapIndex;
    Ptr<KSegmentor> kseg   = kwidget_2d_left->multiLabelMaps[label_idx]->ksegmentor;
    vtkRenderWindowInteractor* imgWindowInteractor = vtkRenderWindowInteractor::SafeDownCast(obj);
    double event_pos[3];

    // TODO: must handle in left 2D widget!
    kwidget_2d_left->sliceViewPicker->Pick( imgWindowInteractor->GetEventPosition()[0],
                                            imgWindowInteractor->GetEventPosition()[1],
                                            0.0, kwidget_2d_left->kvImageRenderer );

    kwidget_2d_left->sliceViewPicker->GetPickPosition( event_pos );  // Get the world coordinates of the pick
    vector<double>  imageSpacing = kv_opts->imageSpacing;
    int event_PixCoord[3]; 						       				//the X,Y,Z voxel coordinates
    event_PixCoord[0]=  round(event_pos[0]/imageSpacing[0]);
    event_PixCoord[1]=  round(event_pos[1]/imageSpacing[1]);
    event_PixCoord[2]=  slice_idx;

    if (0 != event_PixCoord[0] && 0 != event_PixCoord[1] )       {

      int imax = std::min(event_PixCoord[0]+kv_opts->paintBrushRad,(kv_opts->imH-1));
      int jmax = std::min(event_PixCoord[1]+kv_opts->paintBrushRad,(kv_opts->imV-1));
      int jmin = std::max(event_PixCoord[1]-kv_opts->paintBrushRad,0);
      int imin = std::max(event_PixCoord[0]-kv_opts->paintBrushRad,0);
      int k = event_PixCoord[2];

      double image_range[2];
      kv_data->imageVolumeRaw->GetScalarRange( image_range );
      double paintSimilarityMinimum = (image_range[1] - image_range[0]) * kv_opts->paintBrushThreshold;
      double imgValAtClickPoint = kv_data->imageVolumeRaw->GetScalarComponentAsDouble(
          event_PixCoord[0], event_PixCoord[1], event_PixCoord[2], 0 );

      double label_range[2];
      kv_data->labelDataArray->GetScalarRange( label_range );
      if( label_range[1] == 0 ) // if this was an empty labelmap to begin with!
        label_range[1] = 1000;
      float Label_Fill_Value = label_range[1] * (! image_callback->Erase() );

      // semi-hack that forces visualization update without copying entire volume
      kv_data->labelDataArray_new           = SP(vtkImageData)::New();
      kv_data->labelDataArray_new->ShallowCopy( kv_data->labelDataArray );

      short *ptrLabel=static_cast<short*>(kv_data->labelDataArray_new->GetScalarPointer());
      short *ptrImage=static_cast<short*>(kv_data->imageVolumeRaw->GetScalarPointer());

      for (int i=imin; i<=imax; i++)  {
        for (int j=jmin; j<=jmax; j++) {
          float distance = pow( (i-event_PixCoord[0])*(i-event_PixCoord[0])*1.0 +
                                (j-event_PixCoord[1])*(j-event_PixCoord[1])*1.0 , 0.5 ) + 1e-3;
          if( distance < kv_opts->paintBrushRad ) {
            float dRatio = pow( ( kv_opts->paintBrushRad / distance), 2.0 );
            short imgMax = imgValAtClickPoint + paintSimilarityMinimum * dRatio;
            short imgMin = imgValAtClickPoint - paintSimilarityMinimum * dRatio;

            // Need to revisit this... user (Grant) didn't like PK attempt at Z-fill
            int kmin = k; // - floor( sqrt(kv_opts->paintBrushRad - distance) );
            int kmax = k; // + floor( sqrt(kv_opts->paintBrushRad - distance) );
            kmin     = (kmin >= 0 ) ? kmin : 0;
            kmax     = (kmax < kv_opts->numSlices ) ? kmax : kv_opts->numSlices;
            for( int kk = kmin; kk <= kmax; kk++) {
              long elemNum = kk * kv_opts->imV * kv_opts->imH + j * kv_opts->imH + i;
              if( ptrImage[elemNum] < imgMax && ptrImage[elemNum] > imgMin ) {
                ptrLabel[elemNum] = Label_Fill_Value;
                kseg->accumulateUserInput( Label_Fill_Value, i, j, kk );
              }
            }
          }
        }
      }
      // perform update on the labelmap being edited
      kv_data->labelDataArray_new->Update();
      kwidget_2d_left->multiLabelMaps[label_idx]->label2D_shifter_scaler->SetInput( kv_data->labelDataArray_new );
      kwidget_2d_left->multiLabelMaps[label_idx]->label2D_shifter_scaler->Update();
      kv_data->labelDataArray = kv_data->labelDataArray_new;
      qVTK1->update();
    }
  }
}



void KViewer::setupQVTKandData( )
{ /** \warning LEAKS MEMORY (on multiple file->load) */
  if(kv_opts->loadImageTrigger==1)
  {
    this->kv_opts->loadImageTrigger=0;
    this->LoadImage();
  }
  // load the data according to what's in kv_opts now
  setup_file_reader( kv_opts, kv_data );

  // create QT GUI base class with slider info
  setupGUI(this,0,kv_opts->numSlices-1,1);

  // Setup the 3D Widget: volume, label map(s), some user interaction objects
  kwidget_3d_right = cv::Ptr<KWidget_3D_right>( new KWidget_3D_right( qVTK2 ) );
  KWidget_3D_right::Initialize( kwidget_3d_right, kv_opts, kv_data );
  kwidget_3d_right->PrintHelp( );

  // Setup the 2D Widget: image, label map(s), some user interaction objects
  kwidget_2d_left = cv::Ptr<KWidget_2D_left>( new KWidget_2D_left( qVTK1 ) );
  kwidget_2d_left->Initialize( kv_opts, kv_data);
  kwidget_2d_left->PrintHelp( );

  // CreateThresholdFilter( );
  ConnectQTSlots( );

  image_callback = SP(KvtkImageInteractionCallback)::New();
  image_callback->SetKViewerHandle( this );
  image_callback->SetOptions(               kv_opts);
  image_callback->SetRenderWindow(          qVTK1->GetRenderWindow() );
  image_callback->SetSaturationLookupTable( kwidget_2d_left->color_HSV_LookupTable );

  vtkRenderWindowInteractor* interactor = qVTK1->GetRenderWindow()->GetInteractor();
  interactor->AddObserver(vtkCommand::LeftButtonPressEvent, image_callback);
  interactor->AddObserver(vtkCommand::LeftButtonReleaseEvent, image_callback);
  interactor->AddObserver(vtkCommand::KeyPressEvent, image_callback);

  if( ! kv_opts->LabelArrayFilename.empty() ) {
    assert( NULL != saveAsLineEdit ); // it must be created first!
    saveAsLineEdit->setText( QString( kv_opts->LabelArrayFilename.c_str() ) );
  }

  qVTK1->setUpdatesEnabled(true);
  qVTK2->setUpdatesEnabled(true);
  qVTK1->update();
  qVTK2->update();
  qVTK1->setFocus( );
}

void KViewer::ConnectQTSlots( ) {
  //////////////////// Connect the QT Menus and Callbacks /////////////////////////////

  Connections = SP(vtkEventQtSlotConnect)::New();
  Connections->Connect(qVTK1->GetRenderWindow()->GetInteractor(), vtkCommand::MouseMoveEvent, this, SLOT(updateCoords(vtkObject*)));
  Connections->Connect(qVTK2->GetRenderWindow()->GetInteractor(), vtkCommand::MouseMoveEvent, this, SLOT(updateCoords(vtkObject*)));
  Connections->Connect(qVTK1->GetRenderWindow()->GetInteractor(), vtkCommand::MouseMoveEvent, this, SLOT(updatePaintBrushStatus(vtkObject*)));
  Connections->Connect(qVTK1->GetRenderWindow()->GetInteractor(),
                       vtkCommand::MouseMoveEvent, this, SLOT(mousePaintEvent(vtkObject*)));
  Connections->Connect(qVTK1->GetRenderWindow()->GetInteractor(),
                       vtkCommand::KeyPressEvent, this, SLOT(handleGenericEvent(vtkObject*, unsigned long)));

  connect(Slider,SIGNAL(valueChanged(int)), this, SLOT(SliceSelect(int)));

  //  connect(maxThreshSlider,SIGNAL(valueChanged(int)), this, SLOT(maxThreshSliderChanged(int)));
  //  connect(minThreshSlider,SIGNAL(valueChanged(int)), this, SLOT(minThreshSliderChanged(int)));

  connect(actionLoadImage,      SIGNAL(triggered()), this, SLOT(LoadImage()) );
  connect(actionmousePaintEventMap, SIGNAL(triggered()), this, SLOT(LoadLabelMap()) );
  connect(actionAbout,           SIGNAL(triggered()), this, SLOT(About()) );

  Connections->PrintSelf(cout, vtkIndent());
  connect( FillEraseButton,     SIGNAL(clicked()), this, SLOT( ToggleFillEraseMode()) );
  connect( ComputeVolumeButton, SIGNAL(clicked()), this, SLOT( UpdateVolumeStatus())  );
  connect( RunSegmentButton,    SIGNAL(clicked()), this, SLOT( executeSingleSliceSegmentation() ) );
  connect( SaveSegmentButton,   SIGNAL(clicked()), this, SLOT( SaveSegmentation()    ) );
  connect( SaveAsButton,        SIGNAL(clicked()), this, SLOT( SaveAsSegmentation()  ) );
  connect( CopyButton,          SIGNAL(clicked()), this, SLOT( copyFromCurrentSlice()) );
  connect( PasteButton,         SIGNAL(clicked()), this, SLOT( pasteToCurrentSlice() ) );
  connect( NewLabelMapButton,   SIGNAL(clicked()), this, SLOT( AddNewLabelMap()      ) );

}







#if 0          //   koncz-lager'

void KViewer::maxThreshSliderChanged(int upperLimit)
{ // TODO: display in GUI
  this->thresholdFilt->ThresholdBetween(this->thresholdFilt->GetLowerThreshold(),upperLimit);
  // cout<<"The new upper limit is " <<upperLimit ; // <<endl;
}

void KViewer::minThreshSliderChanged(int lowerLimit)
{ // TODO: display in GUI
  this->thresholdFilt->ThresholdBetween(lowerLimit, this->thresholdFilt->GetUpperThreshold());
}
void KViewer::CreateThresholdFilter( ) {

  thresholdFilt = SP(vtkImageThreshold)::New();
  thresholdFilt->SetInput( kv_data->imageVolumeRaw );
  thresholdFilt->ThresholdBetween( kv_opts->minIntensity, kv_opts->maxIntensity );
  thresholdFilt->SetOutValue(0);
  thresholdFilt->SetInValue(1);
  thresholdFilt->SetOutputScalarTypeToUnsignedChar();

  threshReslicer=SP(vtkImageReslice)::New();
  threshReslicer->SetInput(thresholdFilt->GetOutput());
  threshReslicer->SetOutputDimensionality(2);  //drawing just a single slice
  threshReslicer->SetResliceAxesDirectionCosines(1,0,0,    0,1,0,     0,0,1);
  threshReslicer->SetResliceAxesOrigin(0,0,kwidget_2d_left->currentSliceIndex );

  SP(vtkImageMapToColors) threshColorMapLabSlice = SP(vtkImageMapToColors)::New();
  SP(vtkLookupTable) labelLUT = create_default_labelLUT( kv_opts->maxIntensity );

  threshColorMapLabSlice->SetLookupTable( labelLUT );  //define how to map the colors
  threshColorMapLabSlice->SetInput(threshReslicer->GetOutput());
  threshColorMapLabSlice->Update();

  threshActor2D = SP(vtkImageActor)::New( );
  threshActor2D->SetInput( threshColorMapLabSlice->GetOutput() );
  threshActor2D->SetOpacity(0.4);
  threshActor2D->SetInterpolate(0);

}

void KViewer::popup(vtkObject * obj, unsigned long, void * client_data, void *, vtkCommand * command) {
  vtkRenderWindowInteractor* imgWindowInteractor =
      vtkRenderWindowInteractor::SafeDownCast(obj);
  // consume event so the interactor style doesn't get it
  command->AbortFlagOn();
  QMenu* popupMenu = static_cast<QMenu*>(client_data);   		  		// init popup menu
  int* sz = imgWindowInteractor->GetSize();
  int* position = imgWindowInteractor->GetEventPosition();  			// get event location
  QPoint pt = QPoint(position[0], sz[1]-position[1]);  						// remember to flip y
  QPoint global_pt = popupMenu->parentWidget()->mapToGlobal(pt);  // map to global
  popupMenu->popup(global_pt); 												            // show at global point

}

void KViewer::color1(QAction* color) {
  if(color->text() == "Background White")
    kwidget_2d_left->kvImageRenderer->SetBackground(1,1,1);
  else if(color->text() == "Background Black")
    kwidget_2d_left->kvImageRenderer->SetBackground(0,0,0);
  else if(color->text() == "Stereo Rendering")  {
    throw "stereo rendering is not implemented!" ;
  }
  qVTK1->update();
}

void KViewer::color2(QAction* color) {
  if(color->text() == "Background White")
    kwidget_3d_right->kv3DModelRenderer->SetBackground(1,1,1);
  else if(color->text() == "Background Black")
    kwidget_3d_right->kv3DModelRenderer->SetBackground(0,0,0);
  else if(color->text() == "Stereo Rendering")    {
    kwidget_3d_right->kv3DModelRenderer->GetRenderWindow()->SetStereoRender(
        !kwidget_3d_right->kv3DModelRenderer->GetRenderWindow()->GetStereoRender());
  }
  qVTK2->update();
}

#endif
