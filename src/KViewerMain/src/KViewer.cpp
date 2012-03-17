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

//TESTING
#include "vtkMetaImageWriter.h"
#include "vtkRegularPolygonSource.h"


// EVIL:  #include "vtkKWImage.h"

#define SP( X )  vtkSmartPointer<X>

using namespace cv;
using namespace vrcl;
using std::vector;


KViewer::KViewer( const KViewerOptions& kv_opts_in ) {

  kv_opts           =  Ptr<KViewerOptions>( new KViewerOptions( kv_opts_in ) );
  kv_data           =  Ptr<KDataWarehouse>( new KDataWarehouse( kv_opts ) );

  kv_opts->InitializeTransform();

  this->setupQVTKandData( );
  this->updatePaintBrushStatus(NULL);
  //initializing "times" for seg interval clock
  t1=clock();
  t2=t1;
  this-> InitializeCircleCursor();

  m_RotX=true;
  m_RotY=false;
  m_RotZ=false;
  if(kwidget_2d_left->multiLabelMaps.size()>1)
  {
      for(int labnum=0;labnum<kwidget_2d_left->multiLabelMaps.size();labnum++)
      {
         kwidget_2d_left->SelectActiveLabelMap( labnum );
         kwidget_3d_right->UpdateSubVolumeExtractor(kv_data->labelDataArray,labnum);
       }
  }

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


/** \brief Creates circle cursor .Should this be moved to KWidget_2D_left?
  */
void KViewer::InitializeCircleCursor()
{

    m_Circle= vtkSmartPointer<vtkRegularPolygonSource>::New();
    m_Circle->SetNumberOfSides(30);
    m_Circle->SetRadius(kv_opts->GetBrushSize()*kv_opts->imageSpacing[0]);
    m_Circle->SetCenter(0,0,0);

    m_CircleMapper= vtkSmartPointer<vtkPolyDataMapper>::New();
    m_CircleMapper->SetInputConnection(m_Circle->GetOutputPort());

    m_CircleActor = vtkSmartPointer<vtkActor>::New();
    m_CircleActor->SetMapper(m_CircleMapper);
    m_CircleActor->GetProperty()->EdgeVisibilityOn();
    m_CircleActor->GetProperty()->SetRepresentationToWireframe();
    m_CircleActor->GetProperty()->SetColor(0,1,0);
    m_CircleActor->GetProperty()->SetOpacity(0);
    kwidget_2d_left->kvImageRenderer->AddActor(m_CircleActor);
}

void KViewer::updatePaintBrushStatus(vtkObject*) {
  std::stringstream ss;
  std::stringstream toggle;
  ss << "Brush Sizetest: " << this->kv_opts->GetBrushSize();

  if( ! image_callback->Erase() ) {
    toggle << "Mode: Draw";
    if(this->m_CircleActor)
    {
     this->m_CircleActor->GetProperty()->SetColor(0,1,0);
     this->m_CircleActor->GetProperty()->SetRepresentationToWireframe();
    }
  } else {
    toggle << "Mode: Erase";
    if(this->m_CircleActor)
    {
     this->m_CircleActor->GetProperty()->SetColor(1,0,0);
     this->m_CircleActor->GetProperty()->SetRepresentationToWireframe();
    }
  }
  if( this->image_callback->ButtonDown() ) {
    toggle << "\t(On)";
    if( image_callback->Erase() ) {
      qVTK1->GetRenderWindow()->SetCurrentCursor(VTK_CURSOR_SIZESE);
    } else {
      qVTK1->GetRenderWindow()->SetCurrentCursor(VTK_CURSOR_HAND);
    }
  } else {
    toggle << "\t(Off)";
    if( image_callback->Erase() ) {
      qVTK1->GetRenderWindow()->SetCurrentCursor(VTK_CURSOR_SIZENW);
    } else {
      qVTK1->GetRenderWindow()->SetCurrentCursor(VTK_CURSOR_DEFAULT);
    }

  }
  ss << "       " << toggle.str();
  QString str = QString(ss.str().c_str());
  paintBrushStatus->setText(str);
  qVTK1->setFocus( );


}

void KViewer::SliceSelect(int SliderVal) {
  double currSliceOrigin=kv_opts->sliderMin +kv_opts->sliceZSpace*SliderVal;
  kwidget_2d_left->CallbackSliceSlider( SliderVal, currSliceOrigin );
  for (int i=0;i<3;i++)
      this->kv_opts->m_PlaneCenter[i]=this->Get3DWidget()->GetImagePlane()->GetCenter()[i]/(double)this->kv_opts->imageSpacing[i];
  this->kv_opts->m_PlaneNormalVector=this->Get3DWidget()->GetImagePlane()->GetNormal();

}

void KViewer::UpdateVolumeStatus()
{   // performance note: iterates over the label image, not instantaneous like other labels
  kv_data->labelDataArray=kwidget_2d_left->GetActiveLabelMap();

  std::string volumeDisplay;
  getVolumeAsString( kv_opts->imageSpacing,
                     kv_data->labelDataArray, volumeDisplay );
  QString str = QString( volumeDisplay.c_str() );
  volumeOfLabelmap->setText(str);
  updatePaintBrushStatus(NULL);

  UpdateModel3D();
  qVTK2->setFocus( );
  qVTK1->update();
  qVTK2->update();
  qVTK1->setFocus( );
}

void KViewer::UpdateMultiLabelDisplay()
{
  kwidget_2d_left->UpdateMultiLabelMapDisplay();
}

void KViewer::ToggleFillEraseMode() {

  updatePaintBrushStatus(NULL);
  qVTK1->setFocus( );
}

void KViewer::UpdateModel3D() {
  //kwidget_3d_right->UpdateVolumeRenderer( kv_data->imageVolumeRaw, kv_data->labelDataArray );
  unsigned int activeLabel=kwidget_2d_left->activeLabelMapIndex;
  kwidget_3d_right->UpdateSubVolumeExtractor(kwidget_2d_left->GetActiveLabelMap(),activeLabel);
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
  if(this->kv_opts->loadImageTrigger==0)
    this->setupQVTKandData( );
}

void KViewer::LoadLabelMap(){
  QString path;
  path = QFileDialog::getOpenFileName(    this,    "Choose a file to open",    "../data/",   "*.mha" );
  string name = kv_opts->LabelArrayFilenames[0].c_str() ;
  this->kv_opts->LoadLabel(path.toStdString());
  this->kwidget_2d_left->LoadMultiLabels( kv_opts->LabelArrayFilenames );
  this->kwidget_2d_left->kv_data->UpdateLabelDataArray( this->kwidget_2d_left->GetActiveLabelMap( ));
  this->kwidget_2d_left->multiLabelMaps[this->kwidget_2d_left->activeLabelMapIndex]->ksegmentor = KSegmentor3D::CreateSegmentor(kv_data->imageVolumeRaw,kv_data->labelDataArray, true);

  saveAsLineEdit->setText( QString( name.c_str() ) );
}

void KViewer::About() {
  QMessageBox::about(this, "KSlice3D Version 1.0", "Created by Peter Karasev, Karl Fritscher, Ivan Kolesov and Karol Chudy from the Tannenbaum Lab");
}

int KViewer::round(double a) {
  return int(a + 0.5);
}

void KViewer::executeSingleSliceSegmentation( )
{
  handleGenericEvent( NULL, (unsigned long) 'a' );
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


void KViewer::SliderCB( int sliceNum )
{
    int sliceDiff=sliceNum-this->kwidget_2d_left->currentSliceIndex;
    MoveSlider(sliceDiff);
}

void KViewer::MoveSlider( int shiftNumberOfSlices )
{

  int currentSlice  = this->kwidget_2d_left->currentSliceIndex;
  int newSliceIndex = currentSlice + shiftNumberOfSlices;
  if( newSliceIndex < this->kv_opts->numSlices   &&  newSliceIndex >= 0 ) {
    this->kwidget_2d_left->currentSliceIndex=newSliceIndex;
    this->SliceSelect( newSliceIndex );
    this->Get3DWidget()->MoveSliceTo(newSliceIndex);
    this->Slider->setValue( newSliceIndex );
  }
  this->qVTK2->setFocus( );
  this->qVTK1->update();
  this->qVTK2->update();
  this->qVTK1->setFocus( );

}

void KViewer::AddNewLabelMap( )
{
  // create a new label map
  kwidget_2d_left->AddNewLabelMap( );
  unsigned int labidx=kwidget_2d_left->activeLabelMapIndex;
  KWidget_3D_right::AddNewLabel(kwidget_3d_right,vrcl::get_good_color_0to7(labidx));
  this->UpdateVolumeStatus();
  qVTK1->update();
  qVTK2->update();
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
    } else if (event == (unsigned long) '0' ){
    keyPressed = '0';
    } else if (event == (unsigned long) '9' ){
     keyPressed = '9';
    }else if (event == (unsigned long) 'm' ){
        keyPressed = 'm';
    }
    int slice_idx                 = kwidget_2d_left->currentSliceIndex;
    int label_idx                 = kwidget_2d_left->activeLabelMapIndex;

    switch ( keyPressed ) {
    case '1':
      kwidget_2d_left->SelectActiveLabelMap( label_idx - 1 );
      this->UpdateVolumeStatus();
      break;
    case '2':
      kwidget_2d_left->SelectActiveLabelMap( label_idx + 1 );
       this->UpdateVolumeStatus();
      break;
    case 'b': // update 3D
       cout << "b key pressed: update 3D" << endl;
       UpdateVolumeStatus();
      break;
    case 'v': // Paste!
      kwidget_2d_left->CopyLabelsFromTo( cache_idx1, slice_idx, kv_opts->multilabel_paste_mode );
      break;
    case 's': // run "KSegmentor"
      cout << "s key pressed: this-slice 2D segmentation " << endl;
      kwidget_2d_left->RunSegmentor(slice_idx,kv_opts->multilabel_sgmnt_mode,true);
      break;
    case 'a': // run "KSegmentor" // This can't be "S", because "shift" tends to generate the same 8bit code
                                  // and we need capital letters for toggling the contrast levels
      cout << "a key pressed: 3D segmentation " << endl;
      kwidget_2d_left->RunSegmentor(slice_idx,kv_opts->multilabel_sgmnt_mode,false);
      break;
    case '0':
       //kv_opts->seg_time_interval+=0.05;
        kv_opts->distWeight+=0.05;
        distweight->setText("dist. weight: "+QString::number(kv_opts->distWeight));
        for (int i=0;i<kwidget_2d_left->multiLabelMaps.size();i++)
        {
            this->kwidget_2d_left->multiLabelMaps[i]->ksegmentor->SetDistanceWeight(kv_opts->distWeight);
        }
        cout << "0 key, updated distWeight: " << kv_opts->distWeight << endl;
      break;
    case '9':
       //kv_opts->seg_time_interval-=0.05;
        if((kv_opts->distWeight-0.05)<0)
            kv_opts->distWeight=0;
        else
            kv_opts->distWeight-=0.05;
        distweight->setText("dist. weight: "+QString::number(kv_opts->distWeight));
        for (int i=0;i<kwidget_2d_left->multiLabelMaps.size();i++)
        {
            this->kwidget_2d_left->multiLabelMaps[i]->ksegmentor->SetDistanceWeight(kv_opts->distWeight);
        }
        cout << "9 key, updated distWeight: " << kv_opts->distWeight << endl;
      break;
    case 'm':
        cout << "m key pressed: toggling time-triggered updates " << endl;
        kv_opts->time_triggered_seg_update=!kv_opts->time_triggered_seg_update;
        break;
    case 'e':
        cout << "e key pressed: X-rotate 90 degrees " << endl;
        if(this->m_RotX)
         kwidget_2d_left->InitializeTransform('x');
        else
          kwidget_2d_left->InitializeTransform('x',-90);
        this->m_RotX=!this->m_RotX;
        break;
    case 't':
        cout << "t key pressed: Y-rotate 90 degrees " << endl;
        if(this->m_RotY)
         kwidget_2d_left->InitializeTransform('y');
        else
          kwidget_2d_left->InitializeTransform('y',-90);
        this->m_RotY=!this->m_RotY;
        break;
    case 'z':    
        cout << "z key pressed: Z-rotate 90 degrees " << endl;
        if(this->m_RotZ)
         kwidget_2d_left->InitializeTransform('z');
        else
          kwidget_2d_left->InitializeTransform('z',-90);
        this->m_RotZ=!this->m_RotZ;
        break;
    case 'u':
         cout << "u key pressed: updating volume status and 3D view " << endl;
         this->UpdateVolumeStatus();
         break;
    default:
      break;
    }
    if(keyPressed=='e' ||keyPressed=='t'||keyPressed=='z')
    {
        kwidget_2d_left->UpdateTransform();
        this->UpdateImageInformation(kv_data->imageVolumeRaw);
        int id=  kwidget_2d_left->activeLabelMapIndex;
        for (int k=0;k<kwidget_2d_left-> multiLabelMaps.size(); k++ )
        {
            kwidget_2d_left->SelectActiveLabelMap( k);
            this->UpdateVolumeStatus();
        }
        kwidget_2d_left->SelectActiveLabelMap(id);
    }
  }
}

void KViewer::mousePaintEvent(vtkObject* obj) {

  if ( this->image_callback->ButtonDown() )
  {

    int slice_idx          = kwidget_2d_left->currentSliceIndex;
    int label_idx          = kwidget_2d_left->activeLabelMapIndex;
    //Ptr<KSegmentor3D> kseg   = kwidget_2d_left->multiLabelMaps[label_idx]->ksegmentor;
    vtkRenderWindowInteractor* imgWindowInteractor = vtkRenderWindowInteractor::SafeDownCast(obj);
    double event_pos[3];

    // TODO: must handle in left 2D widget!
    kwidget_2d_left->sliceViewPicker->Pick( imgWindowInteractor->GetEventPosition()[0],
                                            imgWindowInteractor->GetEventPosition()[1],
                                            0.0, kwidget_2d_left->kvImageRenderer );

    kwidget_2d_left->sliceViewPicker->GetPickPosition( event_pos );  // Get the world coordinates of the pick

    this->m_Circle->SetCenter(event_pos);

    vector<double>  imageSpacing = kv_opts->imageSpacing;
    int event_PixCoord[3]; 						       				//the X,Y,Z voxel coordinates
    event_PixCoord[0]=  round(event_pos[0]/imageSpacing[0]);
    event_PixCoord[1]=  round(event_pos[1]/imageSpacing[1]);
    event_PixCoord[2]=  slice_idx;

    if (0 != event_PixCoord[0] && 0 != event_PixCoord[1] )       {
      // imgWidth: ACROSS, imgV: DOWN, when viewed from the vtk window
      int xmax = std::min(event_PixCoord[0]+kv_opts->paintBrushRad,(kv_opts->imgWidth-1));
      int ymax = std::min(event_PixCoord[1]+kv_opts->paintBrushRad,(kv_opts->imgHeight-1));
      int ymin = std::max(event_PixCoord[1]-kv_opts->paintBrushRad,0);
      int xmin = std::max(event_PixCoord[0]-kv_opts->paintBrushRad,0);
      int z = event_PixCoord[2];

      double image_range[2];
      kwidget_2d_left->color_HSV_LookupTable->GetTableRange(image_range);
      double paintSimilarityMinimum = (image_range[1] - image_range[0]) * kv_opts->paintBrushThreshold;
      double imgValAtClickPoint = kv_data->imageVolumeRaw->GetScalarComponentAsDouble(
            event_PixCoord[0], event_PixCoord[1], event_PixCoord[2], 0 );

      double label_val_max   = std::max( kv_data->labelDataArray->GetScalarRange()[1],
                                         kv_opts->drawLabelMaxVal);
      float Label_Fill_Value = label_val_max * (! image_callback->Erase() );

      kv_data->labelDataArray=kwidget_2d_left->multiLabelMaps[label_idx]->labelDataArray;
      unsigned short *ptrLabel=static_cast<unsigned short*>(kv_data->labelDataArray->GetScalarPointer());
      unsigned short *ptrImage=static_cast<unsigned short*>(kv_data->imageVolumeRaw->GetScalarPointer());
      std::vector<unsigned int> coord(3);
      for (int i=xmin; i<=xmax; i++)  {
        for (int j=ymin; j<=ymax; j++) {
          float distance = pow( (i-event_PixCoord[0])*(i-event_PixCoord[0])*1.0 +
                                (j-event_PixCoord[1])*(j-event_PixCoord[1])*1.0 , 0.5 ) + 1e-3;
          if( distance < kv_opts->paintBrushRad ) {
            float dRatio = pow( ( kv_opts->paintBrushRad / distance), 2.0 );
            short imgMax = imgValAtClickPoint + paintSimilarityMinimum * dRatio;
            short imgMin = imgValAtClickPoint - paintSimilarityMinimum * dRatio;

            // Need to revisit this... user (Grant) didn't like PK attempt at Z-fill
            // ARGH this definitely needs to be a command line option... 'draw-spread'
            int ds   = kv_opts->m_DrawSpreadOffViewPlane;
            int zmin = z - ds*floor(   sqrt(kv_opts->paintBrushRad - distance) );
            int zmax = z + ds*1*floor( sqrt(kv_opts->paintBrushRad - distance) );
            zmin     = (zmin >= 0 ) ? zmin : 0;
            zmax     = (zmax < kv_opts->numSlices-1 ) ? zmax : kv_opts->numSlices-1;
            for( int kk = zmin; kk <= zmax; kk++) {

              long elemNum = kk * kv_opts->imgHeight * kv_opts->imgWidth + j * kv_opts->imgWidth + i;
              if( (ptrImage[elemNum] > image_range[0]) && (ptrImage[elemNum] < imgMax) && (ptrImage[elemNum] > imgMin) ) {    
                  coord[0]=(i);
                  coord[1]=(j);
                  coord[2]=(kk);
                kwidget_2d_left->multiLabelMaps[label_idx]->ksegmentor->AddPointToUpdateVector(elemNum);
                kwidget_2d_left->multiLabelMaps[label_idx]->ksegmentor->AddPointToCoordinatesVector(coord);

                //if(kk==k) //User integration only in current slice or not?
                         // PK opinion: drawing period really should be in "this" slice only ...
                         // off-view-slice updates make sense for automated part. But it doesn't make sense to
                         // manually draw edits if we're editing things we can't see! Rather, we go to a slice where automated
                         // part is making mistakes and edit it there.
                kwidget_2d_left->multiLabelMaps[label_idx]->ksegmentor->accumulateUserInputInUserInputImages(Label_Fill_Value,elemNum);
                ptrLabel[elemNum] = Label_Fill_Value;
              }
            }
          }
        }
      }

     t2 = clock();
      if((t2-t1)> kv_opts->seg_time_interval * CLOCKS_PER_SEC && kv_opts->time_triggered_seg_update)
      {
          kwidget_2d_left->RunSegmentor(slice_idx,kv_opts->multilabel_sgmnt_mode);
          t1=t2;
      }
       //this->UpdateVolumeStatus();
      kwidget_2d_left->multiLabelMaps[label_idx]->label2D_shifter_scaler->Modified();
      //kwidget_2d_left->multiLabelMaps[label_idx]->label2D_shifter_scaler->Update();
      qVTK1->update();
      qVTK2->update();
    }

  }
}


void KViewer::UpdateImageInformation(vtkImageData* image)
{
    for(int i=0;i<6;i++) kv_opts->imageExtent[i]=image->GetExtent()[i];

    // for (int i=0;i<3;i++)
   // {
    //Already done in InitializeTransform
        //kv_opts->imageSpacing[i]= image->GetSpacing()[i];
        //According to comments in KSandbox origin should remain 0,0,0
        //kv_opts->imageOrigin[i]=image->GetOrigin()[i];
    //}

    kv_opts->m_Center[0]= (kv_opts->imageExtent[1]*kv_opts->imageSpacing[0]-kv_opts->imageExtent[0]*kv_opts->imageSpacing[0])*0.5;
    kv_opts->m_Center[1]= (kv_opts->imageExtent[3]*kv_opts->imageSpacing[1]-kv_opts->imageExtent[2]*kv_opts->imageSpacing[1])*0.5;
    kv_opts->m_Center[2]= (kv_opts->imageExtent[5]*kv_opts->imageSpacing[2]-kv_opts->imageExtent[4]*kv_opts->imageSpacing[2])*0.5;

    kv_opts->numSlices      = kv_opts->imageExtent[5]-kv_opts->imageExtent[4]+1;
    kv_opts->imgHeight       = kv_opts->imageExtent[3]-kv_opts->imageExtent[2]+1;
    kv_opts->imgWidth        = kv_opts->imageExtent[1]-kv_opts->imageExtent[0]+1;

    kv_opts->sliderMin  =0;
    kv_opts->sliderMax   = kv_opts->numSlices-1;

    this->Slider->setMinimum(kv_opts->sliderMin);
    this->Slider->setMaximum(kv_opts->sliderMax);
    kv_opts->sliceZSpace    = kv_opts->imageSpacing[2];
    this->Slider->setSingleStep(kv_opts->sliceZSpace);
    this->Slider->update();
    for (int i=0;i<3;i++)
        this->kv_opts->m_PlaneCenter[i]=this->Get3DWidget()->GetImagePlane()->GetCenter()[i]/(double)this->kv_opts->imageSpacing[i];
    kv_opts->m_PlaneNormalVector=this->kwidget_3d_right->GetImagePlane()->GetNormal();
    for (int i=0;i<kwidget_2d_left->multiLabelMaps.size();i++)
    {
        this->kwidget_2d_left->multiLabelMaps[i]->ksegmentor->SetPlaneCenter(kv_opts->m_PlaneCenter);
        this->kwidget_2d_left->multiLabelMaps[i]->ksegmentor->SetPlaneNormalVector(kv_opts->m_PlaneNormalVector);
    }

}



void KViewer::setupQVTKandData( )
{ /** \warning LEAKS MEMORY (on multiple file->load) */
  if(kv_opts->loadImageTrigger==1)
  {
    this->LoadImage();
    this->kv_opts->loadImageTrigger=0;
  }

  // load the data according to what's in kv_opts now
  setup_file_reader( kv_opts, kv_data );

  // create QT GUI base class with slider info
  setupGUI(this,0,kv_opts->numSlices-1,1);


  // Setup the 2D Widget: image, label map(s), some user interaction objects
  kwidget_2d_left = cv::Ptr<KWidget_2D_left>( new KWidget_2D_left( qVTK1 ) );
  kwidget_2d_left->Initialize( kv_opts, kv_data);
  kwidget_2d_left->PrintHelp( );

  // Setup the 3D Widget: volume, label map(s), some user interaction objects
  kwidget_3d_right = cv::Ptr<KWidget_3D_right>( new KWidget_3D_right( qVTK2 ) );
  KWidget_3D_right::Initialize( kwidget_3d_right, kv_opts, kv_data );
  kwidget_3d_right->PrintHelp( );

  //Add additional labels to 3D view  - not very elegant, but it works...
  if(kwidget_2d_left->multiLabelMaps.size()>1)
  {
      for(int labnum=1; (int)labnum<kwidget_2d_left->multiLabelMaps.size();labnum++)
      {
         this->kv_data->UpdateLabelDataArray(kwidget_2d_left->multiLabelMaps[labnum]->labelDataArray);
         KWidget_3D_right::AddNewLabel(kwidget_3d_right,vrcl::get_good_color_0to7(labnum));
         kwidget_3d_right->UpdateSubVolumeExtractor(kv_data->labelDataArray,labnum);
       }
  }

  for (int i=0;i<3;i++)
      this->kv_opts->m_PlaneCenter[i]=this->Get3DWidget()->GetImagePlane()->GetCenter()[i]/(double)this->kv_opts->imageSpacing[i];
  kv_opts->m_PlaneNormalVector=this->kwidget_3d_right->GetImagePlane()->GetNormal();

  for (int i=0;i<kwidget_2d_left->multiLabelMaps.size();i++)
  {
      this->kwidget_2d_left->multiLabelMaps[i]->ksegmentor->SetPlaneCenter(kv_opts->m_PlaneCenter);
      this->kwidget_2d_left->multiLabelMaps[i]->ksegmentor->SetPlaneNormalVector(kv_opts->m_PlaneNormalVector);
  }


  //this->kv_data->UpdateLabelDataArray(kwidget_2d_left->multiLabelMaps[0]->labelDataArray);

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

  if( ! kv_opts->LabelArrayFilenames.size()==0 ) {
    saveAsLineEdit->setText( QString( kv_opts->LabelArrayFilenames[0].c_str() ) );
  }
  else
      saveAsLineEdit->setText( QString(""));

   //initialize display for segmentation interval
  segmentationInterval->setText("time interval for seg. update: "+QString::number(kv_opts->seg_time_interval)+" sec");
  distweight->setText("dist. weight: "+QString::number(kv_opts->distWeight));
  qVTK1->setUpdatesEnabled(true);
  qVTK2->setUpdatesEnabled(true);
  qVTK1->update();
  qVTK2->update();
  qVTK1->setFocus( );

  if(kwidget_2d_left->multiLabelMaps.size()>1)
  {
      for(int labnum=1;labnum<kwidget_2d_left->multiLabelMaps.size();labnum++)
      {
          kwidget_3d_right->UpdateSubVolumeExtractor(kv_data->labelDataArray,labnum);
          qVTK1->update();
          qVTK2->update();
      }
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
  Connections->Connect(qVTK1->GetRenderWindow()->GetInteractor(), vtkCommand::MouseMoveEvent, this, SLOT(updatePaintBrushStatus(vtkObject*)));
  Connections->Connect(qVTK1->GetRenderWindow()->GetInteractor(), vtkCommand::MouseMoveEvent, this, SLOT(mousePaintEvent(vtkObject*)));
  Connections->Connect(qVTK1->GetRenderWindow()->GetInteractor(), vtkCommand::KeyPressEvent, this, SLOT(handleGenericEvent(vtkObject*, unsigned long)));

  Connections->Connect(qVTK2->GetRenderWindow()->GetInteractor(), vtkCommand::KeyPressEvent, this, SLOT(handleGenericEvent(vtkObject*, unsigned long)));
  Connections->Connect(qVTK2->GetRenderWindow()->GetInteractor(), vtkCommand::MouseMoveEvent, this, SLOT(mousePaintEvent(vtkObject*)));
  Connections->Connect(qVTK2->GetRenderWindow()->GetInteractor(), vtkCommand::MouseMoveEvent, this, SLOT(updateCoords(vtkObject*)));

  connect(Slider,SIGNAL(valueChanged(int)), this, SLOT(SliderCB(int)));

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
