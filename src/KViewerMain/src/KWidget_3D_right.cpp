
#include "vtkSmartPointer.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToColors.h"
#include "vtkImageShiftScale.h"
#include "KViewerOptions.h"
#include "vtkImageData.h"
#include "vtkInteractorStyleImage.h"
#include "vtkRenderWindowInteractor.h"
#include "Logger.h"
#include "vtkMarchingCubes.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkImageResample.h"
#include "vtkPolyDataNormals.h"
#include "vtkImageGaussianSmooth.h"
#include "vtkPolyData.h"
#include "vtkDataSetMapper.h"
#include "KWidget_3D_right.h"
#include "vtkProperty.h"
#include "vtkLODActor.h"
#include "QVTKWidget.h"
#include "KDataWarehouse.h"
#include "vtkReverseSense.h"
#include "vtkSurfaceReconstructionFilter.h"
#include "vtkContourFilter.h"
#include "vtkPiecewiseFunction.h"
#include "vtkVolumeRayCastCompositeFunction.h"
#include "vtkVolumeMapper.h"
#include "vtkXMLPolyDataWriter.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"
#include "vtkVolumeRayCastMapper.h"
#include "KVolumeRenderView.h"  
#include "vtkTransform.h"
#include "KSandbox.h"

#define SP( X )  vtkSmartPointer<X> 

using cv::Ptr;
using namespace vrcl;

namespace {



void SetupSubVolumeExtractor( Ptr<KWidget_3D_right> kwidget_3d_right ) {
  
  Ptr<KDataWarehouse> kv_data = kwidget_3d_right->kv_data;
  vtkExtractVOI* labelSubVolumeExtractor = vtkExtractVOI::New();

  int size=0;
  kwidget_3d_right->multiLabelMaps3D.push_back(std::pair< vtkLODActor*, vtkExtractVOI* >(vtkLODActor::New(),vtkExtractVOI::New() ));
  size=kwidget_3d_right->multiLabelMaps3D.size();
  kwidget_3d_right->SetCurrentNumberOfLabels(size);
}



void SetupLabelActor3D( Ptr<KWidget_3D_right> kwidget_3d_right,std::vector<double> color ) {

  const unsigned int labnum=kwidget_3d_right->GetNumberOfLabels()-1;
  int size=kwidget_3d_right->multiLabelMaps3D.size();
  vtkExtractVOI* currentExtVOI=kwidget_3d_right->multiLabelMaps3D[labnum].second;
  std::string name=currentExtVOI->GetClassName();
  currentExtVOI->SetInput( kwidget_3d_right->kv_data->labelDataArray );
  currentExtVOI->Update();
  vtkSmartPointer<vtkMarchingCubes> cube_marcher= vtkSmartPointer<vtkMarchingCubes>::New();
  cube_marcher->SetInputConnection( kwidget_3d_right->multiLabelMaps3D[labnum].second->GetOutputPort() );
  cube_marcher->ComputeNormalsOn();
  cube_marcher->SetValue(0,1);
  cube_marcher->ComputeGradientsOn();

  vtkSmartPointer<vtkSmoothPolyDataFilter> smoother = vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
  smoother->SetInputConnection( cube_marcher->GetOutputPort() );
  smoother->SetFeatureEdgeSmoothing( true );
  smoother->SetBoundarySmoothing( true );
  //increased smoothing iterations
  smoother->SetNumberOfIterations(60);

  vtkSmartPointer<vtkPolyDataNormals> labSurfNormals = vtkSmartPointer<vtkPolyDataNormals>::New();
  labSurfNormals->SetInputConnection(  smoother->GetOutputPort() );
  labSurfNormals->SetFeatureAngle(10);
  labSurfNormals->SetSplitting( true );

  vtkSmartPointer<vtkDataSetMapper> labelMapper = vtkSmartPointer<vtkDataSetMapper>::New();
  labelMapper->SetInputConnection( labSurfNormals->GetOutputPort() );
  labelMapper->SetImmediateModeRendering(1);
  //labelMapper->Update();

  vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
  polydata = smoother->GetOutput( );


  kwidget_3d_right->multiLabelMaps3D[ labnum].first->SetMapper( labelMapper );

  vtkSmartPointer<vtkProperty> propSetter=vtkSmartPointer<vtkProperty>::New();
  propSetter = kwidget_3d_right->multiLabelMaps3D[labnum].first->GetProperty();
  propSetter->SetRepresentationToWireframe();
  propSetter->SetOpacity(kwidget_3d_right->kv_opts->modelOpacity3D);
  propSetter->SetAmbient(1);
  propSetter->SetAmbientColor(color[0],color[1],color[2]);
  propSetter->SetDiffuse(1);
  propSetter->SetDiffuseColor(color[0],color[1],color[2]);
  propSetter->SetSpecular(0);
  //propSetter->SetSpecularPower(1);
  //propSetter->SetSpecularColor(.7,.49,.25);
  propSetter->SetInterpolationToPhong(); // more crazy: propSetter1->SetInterpolationToFlat();
  kwidget_3d_right->kv3DModelRenderer->AddActor(kwidget_3d_right->multiLabelMaps3D[labnum].first);
}

void SaveTimestampedPolyData( vtkPolyData* polydata )
{
  time_t seconds;
  seconds = time(NULL);
  std::stringstream  ss;
  ss << "KViewerSaved_polydata_" << seconds << ".vtp" ;
  std::string labelmap_filename = ss.str();
  vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetInput( polydata );
  writer->SetFileName(labelmap_filename.c_str());
  writer->Update();
  writer->Write();
}

void SetupRenderWindow( Ptr<KWidget_3D_right> kwidget_3d_right ) {

  //////////////////// Render Window Right : 3D Display /////////////////////////////
  //vtkImageData* image = kwidget_3d_right->kv_data->imageVolumeRaw;
  //vtkImageData* label = kwidget_3d_right->kv_data->labelDataArray;
  //kwidget_3d_right->volRenView->UpdateDisplay( image, label );
  
  kwidget_3d_right->kv3DModelRenderer = vtkSmartPointer<vtkRenderer>::New();

  vtkVolume* volumeL = kwidget_3d_right->volRenView->volumeLabel;
  kwidget_3d_right->kv3DModelRenderer->AddVolume(volumeL);
  vtkVolume* volumeI = kwidget_3d_right->volRenView->volumeImage;
  kwidget_3d_right->kv3DModelRenderer->AddVolume(volumeI);
  //kwidget_3d_right->kv3DModelRenderer->AddActor(kwidget_3d_right->labelActor3D);
  kwidget_3d_right->kv3DModelRenderer->SetBackground(0,0,0);
  
  
  kwidget_3d_right->renderWindowRight = vtkSmartPointer<vtkRenderWindow>::New();
  kwidget_3d_right->renderWindowRight->AddRenderer( kwidget_3d_right->kv3DModelRenderer );
  
  
  kwidget_3d_right->interactor_style_3DTrackball =
      vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();

  QVTKWidget* qvtk = kwidget_3d_right->qVTK_widget_right;
  qvtk->GetRenderWindow()->GetInteractor( )->SetInteractorStyle( 
                                              kwidget_3d_right->interactor_style_3DTrackball );
  qvtk->SetRenderWindow( kwidget_3d_right->renderWindowRight );

}

}  // end anonymous namespace ... todo: move to structs if we feel like it.
// if we don't feel like it, then these should be member functions!

void KWidget_3D_right::UpdateSubVolumeExtractor( vtkImageData* new_subvolume_source, unsigned int labNumber ) {

    // Don't resample, kind of hopeless: better to segment properly and use thinner images!
    //this->labelSubVolumeExtractor = vtkExtractVOI::New();
  multiLabelMaps3D[labNumber].second->SetInput( new_subvolume_source );
  multiLabelMaps3D[labNumber].second->UpdateWholeExtent( );
 //SetupLabelActor3D(kwidget_3d_right);
  this->qVTK_widget_right->update( );

}

void KWidget_3D_right::AddNewLabel(Ptr<KWidget_3D_right> kwidget_3d_right,std::vector<double> color){
    SetupSubVolumeExtractor( kwidget_3d_right );
    SetupLabelActor3D( kwidget_3d_right,color);
}


void KWidget_3D_right::AddFocusPoint( int x, int y, int z )
{
  
  this->volRenView->AddFocusPoint( x,y,z);
  
}

void KWidget_3D_right::UpdateVolumeRenderer( vtkImageData* image, vtkImageData* label ) {

  this->volRenView->UpdateDisplay( image, label );
  this->qVTK_widget_right->update( );

}

void KWidget_3D_right::Initialize( Ptr<KWidget_3D_right> kwidget_3d_right,
                                   Ptr<KViewerOptions> kv_opts_input,
                                   Ptr<KDataWarehouse> kv_data_input ) {

  // BUG WARNING:

  bool UseVolumeRender =false; // TODO: 3D view needs total rewrite,
                                // a) it doesn't support multiple levels at all
                                // b) it will be too slow to volume render multiple labels
                                // c) better idea: use x,y,z coords for colormap generation
                                // in conjunction with polydata actors
  if(UseVolumeRender) {
    // Turn off volume view temporarily for speed
    kwidget_3d_right->kv_opts = kv_opts_input; // grab options and state variables from KViewer main app.
    kwidget_3d_right->kv_data = kv_data_input;
    kwidget_3d_right->volRenView = Ptr<KVolumeRenderView>( new KVolumeRenderView );
    if( !kwidget_3d_right->kv_data ) 
        throw "kv_data does not exist yet!" ;

    kwidget_3d_right->multiLabelMaps3D = std::vector< std::pair< vtkLODActor*, vtkExtractVOI* > >();
    int numberOfInputLabels = kwidget_3d_right->kv_opts->LabelArrayFilenames.size();
    if( numberOfInputLabels > 1 ) {
      std::cout << "resizing size of multLabelMaps3D" << std::endl;

      // ?? Why do we need to set it separately, instead of using the multiLabelMaps3D.size()?
      //  This seems to be a source of bugs
      kwidget_3d_right->SetCurrentNumberOfLabels(numberOfInputLabels);
      kwidget_3d_right->multiLabelMaps3D.resize(numberOfInputLabels);
    }
    
    SetupRenderWindow( kwidget_3d_right );
    SetupSubVolumeExtractor( kwidget_3d_right );
    std::vector<double> firstcolor= vrcl::get_good_color_0to7(0); //{240/255.0, 163/255.0, 255/255.0};
    SetupLabelActor3D( kwidget_3d_right, firstcolor );
  }
}

KWidget_3D_right::KWidget_3D_right( QVTKWidget* qvtk_handle ) {  // must explicity call Initialize()!
  this->qVTK_widget_right  = qvtk_handle;
}
