
#include "KWidget_2D_left.h"
#include "vtkSmartPointer.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToColors.h"
#include "vtkImageShiftScale.h"
#include "KViewerOptions.h"
#include "vtkImageData.h"
#include "vtkInteractorStyleImage.h"
#include "vtkRenderWindowInteractor.h"
#include "Logger.h"
#include "QVTKWidget.h"
#include "KSandbox.h"
#include "KSegmentor.h"
#include "KDataWarehouse.h"
#include "vtkImageContinuousDilate3D.h"
#include "vtkMetaImageReader.h"
#include "vtkMetaImageWriter.h"
#include "vtkImageReslice.h"
#include "KInteractiveLabelMap.h"
#include "KWidget_2D_left.h"

#include <stdio.h>
#include <time.h>

#define SP( X )  vtkSmartPointer<X>
using cv::Ptr;
using namespace vrcl;
using std::string;
using std::stringstream;

const std::string KWidget_2D_left::VERBOSE = "VerboseWidget2D_IO";

namespace {

struct BogusSingletonException : public std::exception
{
  virtual const char* what() const throw ()
  {
    return "kv_data and/or kv_opts are bogus!";
  }
};

void SetupSaturationLUT( vtkLookupTable* satLUT, Ptr<KViewerOptions> kv_opts, Ptr<KDataWarehouse> kv_data )
{
  /** Sets up the mapping of intensity values to displayed colors.
    * Needs to be adjusted if all you see is black & white.
    */
  vtkImageData* image = kv_data->imageVolumeRaw;

  if( (kv_opts->minIntensity < 0) || (kv_opts->maxIntensity < 0) ) {
    cout << "no min,max passed; setting default window: min,max of image." << endl;
    double minMaxImage[2];
    image->GetScalarRange( minMaxImage );
    kv_opts->minIntensity = minMaxImage[0];
    kv_opts->maxIntensity = minMaxImage[1];
  }

  cout << "attempting to use Image Range: "
       << kv_opts->minIntensity << ", " << kv_opts->maxIntensity << endl;
  satLUT->SetTableRange (kv_opts->minIntensity * 0.9, kv_opts->maxIntensity * 1.1);

  satLUT->SetSaturationRange (0.0, 0.1);
  satLUT->SetHueRange (  0.0, 1);
  satLUT->SetValueRange (0.0, 1);
  satLUT->ForceBuild();
}

#if WIN32
/**
   * \warning method added for Windows OS. Might conflict with definitions in non windows environment
   */
int round(double d)
{
  return d+0.5;
}
#endif

}

void KWidget_2D_left:: SetupRenderWindow() {

  kvImageRenderer = vtkSmartPointer<vtkRenderer>::New();
  // This is where multiple actors are added / multiple label maps here
  kvImageRenderer->AddActor( imageActor );
  kvImageRenderer->AddActor( multiLabelMaps[0]->labelActor2D );
  kvImageRenderer->SetBackground(0.5,0.5,0.5);

  renderWindowLeft = vtkSmartPointer<vtkRenderWindow>::New();
  renderWindowLeft->AddRenderer( kvImageRenderer );

  sliceViewPicker = vtkSmartPointer<vtkPropPicker>::New();
  sliceViewPicker->PickFromListOn();
  sliceViewPicker->AddPickList( imageActor );
  image_interactor_style = vtkSmartPointer<vtkInteractorStyleImage>::New();

  vtkRenderWindow* window_alias = renderWindowLeft;
  QVTKWidget* qvtk = qVTK_widget_left;
  qvtk->GetRenderWindow()->GetInteractor( )->SetInteractorStyle( image_interactor_style );
  qvtk->SetRenderWindow( window_alias );
  window_alias->GetInteractor( )->SetInteractorStyle( image_interactor_style );

}



void KWidget_2D_left::SetupImageDisplay() {

  vtkSmartPointer<vtkLookupTable> satLUT = vtkSmartPointer<vtkLookupTable>::New();
  SetupSaturationLUT( satLUT, kv_opts, kv_data );

  vtkSmartPointer<vtkImageShiftScale> intensShift=vtkSmartPointer<vtkImageShiftScale>::New();
  intensShift->SetInput( kv_data->imageVolumeRaw );

  imageReslicer=vtkSmartPointer<vtkImageReslice>::New();
  imageReslicer->SetInput(intensShift->GetOutput());
  imageReslicer->SetOutputDimensionality(2);  //drawing just a single slice
  imageReslicer->SetResliceAxesDirectionCosines(1,0,0,    0,1,0,     0,0,1);
  imageReslicer->SetResliceAxesOrigin(0,0,currentSliceIndex );

  // assign color mapping via lookup table
  vtkSmartPointer<vtkImageMapToColors> colorMap = vtkSmartPointer<vtkImageMapToColors>::New();
  colorMap->SetLookupTable(satLUT);
  colorMap->SetInput( imageReslicer->GetOutput() );
  colorMap->Update();

  imageActor = vtkSmartPointer<vtkImageActor>::New( );
  imageActor->SetInput( colorMap->GetOutput() );
  imageActor->SetInterpolate( kv_opts->labelInterpolate );

  // keep a handle on it so that user can change contrast later from KViewer
  color_HSV_LookupTable = satLUT;

}

KWidget_2D_left::KWidget_2D_left( QVTKWidget* qvtk_handle ) {
  // must explicity call Initialize()!
  qVTK_widget_left  = qvtk_handle;
  currentSliceIndex = 0;
}


void KWidget_2D_left::Initialize( Ptr<KViewerOptions> kv_opts_input,
                                  Ptr<KDataWarehouse> kv_data_input ) {
  // set Logger flags for test/debug
  SETLOG(KWidget_2D_left::VERBOSE,1);

  // grab options and state variables from KViewer main app.
  kv_opts = kv_opts_input;
  kv_data = kv_data_input;
  currentSliceIndex = 0;
  cacheSliceIndex   = 0;
  if( !kv_opts || !kv_data ) {
    throw BogusSingletonException();
  }

  bool bNoInputLabelFiles = true;
  if( ! kv_opts_input->LabelArrayFilenames.empty() )
  { // were there any label file names?
    if( ! kv_opts_input->LabelArrayFilenames[0].empty() )
    { // and its not a leading "" ?
      bNoInputLabelFiles = false; // then we DO have input label files
    }
  }

  this->AddNewLabelMap();  // create a new, empty, all-zeros labelmap

  kv_data->labelDataArray = this->GetActiveLabelMap( );

  SetupImageDisplay( ); // must come before renderwindow setup!

  SetupRenderWindow( ); // must come after image display setup!

  if( !bNoInputLabelFiles )
  { // must happen after renderers are set up so we can insert actors right away
    this->LoadMultiLabels( kv_opts->LabelArrayFilenames );
  }

}

//  ************************************************************************   //
//  ******************   END PRIVATE INITIALIZATION STUFF  *****************   //








//  *********************************************************   //
//  ******************   PUBLIC INTERFACE   *****************   //

void KWidget_2D_left::CallbackMousePosition(vtkObject* obj)
{
  vtkRenderWindowInteractor* imgWindowInteractor = vtkRenderWindowInteractor::SafeDownCast(obj);
  double event_pos[3];
  this->sliceViewPicker->Pick( imgWindowInteractor->GetEventPosition()[0],
                               imgWindowInteractor->GetEventPosition()[1],
                               0.0, this->kvImageRenderer );
  this->sliceViewPicker->GetPickPosition( event_pos );
  std::stringstream ss;
  ss << "x = \t " << event_pos[0] << "\t , y = \t " << event_pos[1] << "\t , z = \t " << currentSliceIndex ;
  mouse_position_string = ss.str();
}

void KWidget_2D_left::CallbackSliceSlider( int currSlice, double currSliceOrigin ) {
  currentSliceIndex  = currSlice;
  imageReslicer->SetResliceAxesOrigin(  0,  0,  currSliceOrigin );

  for( int k = 0; k < (int) multiLabelMaps.size(); k++ ) {
    multiLabelMaps[k]->labelReslicer->SetResliceAxesOrigin(  0,  0,  currSliceOrigin );
    multiLabelMaps[k]->ksegmentor->setCurrIndex( currentSliceIndex );
  }

}

void KWidget_2D_left::SaveLabelsInternal( const std::stringstream& ss )
{
  string labelmap_name_base = ss.str(); // base: might have ".mha" at the end

  int label_idx = 0;
  for( ; label_idx < (int) multiLabelMaps.size(); label_idx++)
  {
    IFLOG(VERBOSE, cout << "... attempting to write label map to file ...  " )
        stringstream label_index_ss;
    label_index_ss << "_" << std::setw(4) << std::setfill('0') << label_idx;
    string labelmap_filename = labelmap_name_base + label_index_ss.str() + ".mha";

    SP(vtkMetaImageWriter) labelWriter=   SP(vtkMetaImageWriter)::New();
    labelWriter->SetInput( multiLabelMaps[label_idx]->labelDataArray );
    labelWriter->SetFileName( labelmap_filename.c_str() );
    labelWriter->SetCompression( kv_opts->writeCompressed );
    labelWriter->Write();
    IFLOG(VERBOSE, cout << "Wrote label map to file: " << labelmap_filename << endl )

        // wtf @ png libs ...
        // multiLabelMaps[label_idx]->ksegmentor->saveCurrentSliceToPNG( labelmap_filename );
  }

}

void KWidget_2D_left::SaveAsCurrentLabelMap( const std::string &fileName ) {
  std::stringstream  ss;
  ss << fileName;
  unsigned int minLength = 5;
  if( minLength > fileName.size() ) {
    ss.clear();
    ss << "kslice_" << fileName;
  }

  SaveLabelsInternal( ss );
}

void KWidget_2D_left::SaveCurrentLabelMap( ) {
  IFLOG(VERBOSE, cout << "... attempting to write label map to file ... " )
      time_t seconds;
  seconds = time(NULL);
  std::stringstream  ss;
  ss << "KViewerSaved_LabelMap_" << seconds ;
  SaveLabelsInternal( ss );
}







/*********************************************************************
Multiple Label Maps
*********************************************************************/


void KWidget_2D_left::CopyLabelsFromTo( int iFrom, int iTo, bool bPasteAll )
{ /** for every labelmap, copy & paste from index A to index B, then update display */
  if( bPasteAll ) {
    for( int k = 0; k < (int) multiLabelMaps.size(); k++ ) {
      copySliceFromTo( multiLabelMaps[k]->labelDataArray, iFrom, iTo );
      multiLabelMaps[k]->ksegmentor->copyIntegralDuringPaste( iFrom, iTo );
    }
  } else {
    int k = activeLabelMapIndex;
    copySliceFromTo( multiLabelMaps[k]->labelDataArray, iFrom, iTo );
    multiLabelMaps[k]->ksegmentor->copyIntegralDuringPaste( iFrom, iTo );
  }
  UpdateMultiLabelMapDisplay();

}

void KWidget_2D_left::RunSegmentor(int slice_index, bool bAllLabels)
{
  if( slice_index < 0 ) {
    slice_index = currentSliceIndex;
  }

  if( !bAllLabels )
  {                 // update active label only
    int label_idx = activeLabelMapIndex;
    Ptr<KSegmentor> kseg          = multiLabelMaps[label_idx]->ksegmentor;
    kv_data->labelDataArray_new           = SP(vtkImageData)::New();
    kv_data->labelDataArray_new->ShallowCopy( kv_data->labelDataArray );
    kseg->setCurrLabelArray(kv_data->labelDataArray_new);
    kseg->setCurrIndex( slice_index );
    kseg->setNumIterations( kv_opts->segmentor_iters );
    kseg->initializeData();
    kseg->intializeLevelSet();
    kseg->Update();
  } else
  {            // update all labels at once
    for( int label_idx = 0; label_idx < (int) multiLabelMaps.size(); label_idx++ )
    { /** Note: not sure how thread-safe the lowlevel code of Update() is ... */
      Ptr<KSegmentor> kseg          = multiLabelMaps[label_idx]->ksegmentor;
      kseg->setCurrLabelArray(multiLabelMaps[label_idx]->labelDataArray);
      kseg->setNumIterations( kv_opts->segmentor_iters );
      kseg->setCurrIndex( slice_index );
      kseg->initializeData();
      kseg->intializeLevelSet();
      kseg->Update();
    }
  }

  UpdateMultiLabelMapDisplay();

}


void KWidget_2D_left::UpdateMultiLabelMapDisplay( ) {

  // the shallow-copy trick to force instant display update
  // TODO: is this hack necessary / does it work in newest VTK version?

  for( int k = 0; k < (int) multiLabelMaps.size(); k++ ) {
    kv_data->labelDataArray_new           = SP(vtkImageData)::New();
    kv_data->labelDataArray_new->ShallowCopy( multiLabelMaps[k]->labelDataArray );
    double label_opacity = kv_opts->labelOpacity2D;
    if( k != activeLabelMapIndex ) {
      label_opacity *= 0.5;
    }
    multiLabelMaps[k]->labelActor2D->SetOpacity( label_opacity );
    multiLabelMaps[k]->label2D_shifter_scaler->SetInput( kv_data->labelDataArray_new );
    multiLabelMaps[k]->label2D_shifter_scaler->Update();
  }
  kv_data->labelDataArray_new           = SP(vtkImageData)::New();
  kv_data->labelDataArray_new->ShallowCopy( GetActiveLabelMap( ) );
  kv_data->labelDataArray = kv_data->labelDataArray_new;

  // update the QVTK display
  qVTK_widget_left->update( );
}

void KWidget_2D_left::LoadMultiLabels( const std::vector<std::string>& label_files )
{
  bool bIsInitialization = ( 0 == multiLabelMaps.size() );
  if( bIsInitialization )
    return;
  // We only get here if starting app using --Labels=a.mha b.mha c.mha ..... arguments
  // otherwise labels are sequentially added while segmenting


  multiLabelMaps.clear(); // clean slate
  assert( kv_data->imageVolumeRaw != NULL ); // image better exist or we're screwed
  int nLabels = label_files.size();  assert(nLabels > 0 ); // how many files to load

  for( int k = 0; k < nLabels; k++ ) {
    Ptr<KInteractiveLabelMap>   labelMap = new KInteractiveLabelMap();
    labelMap->RegisterSourceWidget( this ); // give the label map handles to kv_opts, image volume, and this widget
    SP( vtkImageData ) label_from_file = SP( vtkImageData )::New();
    SP( vtkMetaImageReader ) reader    = SP( vtkMetaImageReader )::New();

    if( !reader->CanReadFile(label_files[k].c_str()) ) {
      cout << "quitting due to bogus/unreadable filename " << label_files[k] << endl;
      exit(1);
    }

    { // read the file, convert to uchar, ensure it has same size as image, bag it in labelMap
      reader->SetFileName(label_files[k].c_str());
      reader->Update();
      label_from_file = image2ushort( reader->GetOutput() );
      int npts = label_from_file->GetNumberOfPoints();
      cout << "loading label file " << label_files[k] << ", #points = " << npts << endl;
      assert( npts == kv_data->imageVolumeRaw->GetNumberOfPoints() );
      labelMap->labelDataArray->DeepCopy( label_from_file );
    }
    multiLabelMaps.push_back(labelMap);     // bag it in the array
    activeLabelMapIndex = multiLabelMaps.size()-1; // increment active index
    kvImageRenderer->AddActor( multiLabelMaps[activeLabelMapIndex]->labelActor2D );
  }

  activeLabelMapIndex = 0;
  UpdateMultiLabelMapDisplay( );


}

void KWidget_2D_left::AddNewLabelMap( )
{
  bool bIsInitialization = ( 0 == multiLabelMaps.size() );
  Ptr<KInteractiveLabelMap>   labelMap = new KInteractiveLabelMap();

  // give the label map handles to kv_opts, image volume, and this widget
  labelMap->RegisterSourceWidget( this );
  multiLabelMaps.push_back(labelMap);            // bag it in the array
  activeLabelMapIndex = multiLabelMaps.size()-1; // increment active index

  if( !bIsInitialization ) { // update the display unless this is startup
    kvImageRenderer->AddActor( multiLabelMaps[activeLabelMapIndex]->labelActor2D );
    UpdateMultiLabelMapDisplay( );
  }

  // TODO: fire whatever it is that "slider callback" does to update

}

void KWidget_2D_left::ResetLabelMapList( )
{
  multiLabelMaps.clear(); // clean slate
  assert( kv_data->imageVolumeRaw != NULL ); // it better exist or we're screwed
  AddNewLabelMap( );      // create one empty one
}


void KWidget_2D_left::SelectActiveLabelMap(int labelMapIndex)
{
  if( labelMapIndex < 0 )
    labelMapIndex = 0;
  else if( labelMapIndex >= (int) multiLabelMaps.size() )
    labelMapIndex = multiLabelMaps.size()-1;

  activeLabelMapIndex = labelMapIndex;
  UpdateMultiLabelMapDisplay( );
  cout << "selected active label map index: " << activeLabelMapIndex << endl;
}

SP(vtkImageData) KWidget_2D_left::GetActiveLabelMap( )
{
  SP(vtkImageData) labelMap = (multiLabelMaps[activeLabelMapIndex])->labelDataArray;
  assert( NULL != labelMap );
  return labelMap;
}





#if 0        // Guillotine

void KWidget_2D_left::SetupLabelDisplay()  {

  // Begin Code for Transparent 2d ACL Slice
  // Lookup Table for Label Map

  //    kv_data->labelDataArray->Update();
  //    this->labelLUT = create_default_labelLUT( kv_data->labelDataArray->GetScalarRange()[1] );
  //    label2D_shifter_scaler = vtkSmartPointer<vtkImageShiftScale>::New();
  //    label2D_shifter_scaler->SetInput( kv_data->labelDataArray );

  //    labelReslicer=vtkSmartPointer<vtkImageReslice>::New();
  //    labelReslicer->SetInputConnection( label2D_shifter_scaler->GetOutputPort() );
  //    labelReslicer->SetOutputDimensionality(2);  //drawing just a single slice
  //    labelReslicer->SetResliceAxesDirectionCosines(1,0,0,    0,1,0,     0,0,1);
  //    labelReslicer->SetResliceAxesOrigin(0,0,currentSliceIndex );

  //    vtkSmartPointer<vtkImageMapToColors> colorMapLabSlice = vtkSmartPointer<vtkImageMapToColors>::New();
  //    colorMapLabSlice->SetLookupTable(labelLUT);

  //    colorMapLabSlice->SetInputConnection( labelReslicer->GetOutputPort() );
  //    colorMapLabSlice->Update();

  //    labelActor2D = vtkSmartPointer<vtkImageActor>::New( );
  //    labelActor2D->SetInput( colorMapLabSlice->GetOutput() );
  //    labelActor2D->SetOpacity( kv_opts->labelOpacity2D );
  //    labelActor2D->SetInterpolate( kv_opts->labelInterpolate );

}
// TODO: details of label display needs to use stuff in KInteractiveLabelMap
// SetupLabelDisplay( );


// this is bullshit
//void UpdateActiveLabelMap(vtkSmartPointer<vtkImageData> label_image_data);

//void KWidget_2D_left::UpdateActiveLabelMap(vtkSmartPointer<vtkImageData> label_image_data)
//{
//  // Gah, no we need to handle the UI crap here, not just receive data from elsewhere.
//  // See the place where painting happens via mouse callback. That can send us an (x,y,z)
//  // coord and we update the label map.

//  // Need to make sure activeLabelMapIndex is initialized to 0 somewhere.
//  //labelMaps[activeLabelMapIndex].UpdateLabelMap(label_image_data);

//  // currentSliceIndex = labelMaps[activeLabelMapIndex].GetSliceNumber(); bullshit,
//  // I'm the widget, the labelmap doesn't tell me what slice I'm on!

//  qVTK_widget_left->update();
//}


//void KWidget_2D_left::UpdateLabelMap( vtkSmartPointer<vtkImageData>   label_image_data,
//                                      vtkSmartPointer<vtkLookupTable> labelLUT ) {
//  if( !labelLUT ) {
//    labelLUT = create_default_labelLUT( label_image_data->GetScalarRange()[1] );
//  }
//  size_t ptr1 = (size_t) kv_data->labelDataArray_new.GetPointer();
//  size_t ptr2 = (size_t) label_image_data.GetPointer();
//  if( ptr2 != ptr1) {
//    kv_data->labelDataArray_new = label_image_data;
//    check_extents( kv_data->labelDataArray_new );
//  }
//  cout << " copied new to current ... " << endl;
//  kv_data->PushLabelDataNewToCurrent( );
//  check_extents( kv_data->labelDataArray );

//  if( ((void *)label2D_shifter_scaler->GetInput()) != ((void *)kv_data->labelDataArray) ) {
//    label2D_shifter_scaler->SetInput(kv_data->labelDataArray);
//  }

//  labelReslicer->SetInputConnection( label2D_shifter_scaler->GetOutputPort() );

//  int Dimensionality_Single_Slice_2D = 2;
//  labelReslicer->SetOutputDimensionality( Dimensionality_Single_Slice_2D );
//  labelReslicer->SetResliceAxesDirectionCosines(1,0,0,    0,1,0,     0,0,1);
//  int currentSlice = labelActor2D->GetSliceNumber( );
//  currentSliceIndex = currentSlice;
//  labelReslicer->SetResliceAxesOrigin(0,0,currentSliceIndex );

//  vtkSmartPointer<vtkImageMapToColors> colorMapLabSlice =
//                         vtkSmartPointer<vtkImageMapToColors>::New();
//  colorMapLabSlice->SetLookupTable( labelLUT );
//  colorMapLabSlice->SetInputConnection( labelReslicer->GetOutputPort() );
//  colorMapLabSlice->Update();

//  labelActor2D->SetInput( colorMapLabSlice->GetOutput() );
//  labelActor2D->SetOpacity(     kv_opts->labelOpacity2D );
//  labelActor2D->SetInterpolate( kv_opts->labelInterpolate );

//  qVTK_widget_left->update( );
//}

#endif
