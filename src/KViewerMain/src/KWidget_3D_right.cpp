
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
#include "vtkPolyDataWriter.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"
#include "vtkVolumeRayCastMapper.h"
#include "KVolumeRenderView.h"  
#include "vtkTransform.h"
#include "KSandbox.h"
#include "vtkLight.h"

#define SP( X )  vtkSmartPointer<X> 

using cv::Ptr;
using namespace vrcl;

namespace {



void SetupSubVolumeExtractor( Ptr<KWidget_3D_right> kwidget_3d_right ) {

    int size=0;
    kwidget_3d_right->multiLabelMaps3D.push_back(std::pair< vtkLODActor*, vtkExtractVOI* >(vtkLODActor::New(),vtkExtractVOI::New() ));
    size=kwidget_3d_right->multiLabelMaps3D.size();
    kwidget_3d_right->SetCurrentNumberOfLabels(size);
}



void SetupLabelActor3D( Ptr<KWidget_3D_right> kwidget_3d_right,std::vector<double> color )
{

    /** TODO: Don't use marching cubes to make surface! Utilize the levelset layers that we have,
    * to directly create the surface normals and mesh it up. Avoid aliasing issues this way,
    * we know more about the label than just "binary values"!
    */

    const unsigned int labnum=kwidget_3d_right->GetNumberOfLabels()-1;

    vtkExtractVOI* currentExtVOI=kwidget_3d_right->multiLabelMaps3D[labnum].second;
    currentExtVOI->SetInput( kwidget_3d_right->kv_data->labelDataArray );
    currentExtVOI->Update();

    vtkSmartPointer<vtkMarchingCubes> cube_marcher= vtkSmartPointer<vtkMarchingCubes>::New();
    cube_marcher->SetInputConnection( kwidget_3d_right->multiLabelMaps3D[labnum].second->GetOutputPort() );
    cube_marcher->SetValue(0,1);

    vtkSmartPointer<vtkPolyDataNormals> labSurfNormals = vtkSmartPointer<vtkPolyDataNormals>::New();
    labSurfNormals->FlipNormalsOff();

    bool bFancySurface = true; // TODO: set me from args or gui
    if( bFancySurface ) {
        cube_marcher->ComputeGradientsOn();
        cube_marcher->ComputeNormalsOn();
        vtkSmartPointer<vtkSmoothPolyDataFilter> smoother = vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
        smoother->SetInputConnection( cube_marcher->GetOutputPort() );
        smoother->SetFeatureEdgeSmoothing( true );
        smoother->SetBoundarySmoothing( true );
        smoother->SetNumberOfIterations(20);
        labSurfNormals->SetInputConnection(smoother->GetOutputPort());

    } else
    {
        labSurfNormals->SetInputConnection(  cube_marcher->GetOutputPort() );
    }

    vtkSmartPointer<vtkDataSetMapper> labelMapper = vtkSmartPointer<vtkDataSetMapper>::New();
    labelMapper->SetInputConnection( labSurfNormals->GetOutputPort() );


    // TODO: display info on the surface mesh!
    labelMapper->SetImmediateModeRendering(1);
    labelMapper->Update();

    kwidget_3d_right->multiLabelMaps3D[ labnum].first->SetMapper( labelMapper );

    vtkSmartPointer<vtkProperty> propSetter=vtkSmartPointer<vtkProperty>::New();
    propSetter = kwidget_3d_right->multiLabelMaps3D[labnum].first->GetProperty();
    propSetter->SetOpacity(kwidget_3d_right->kv_opts->modelOpacity3D);
    propSetter->SetAmbient(1);
    propSetter->SetAmbientColor(color[0],color[1],color[2]);
    propSetter->SetDiffuse(1);
    propSetter->SetDiffuseColor(color[0],color[1],color[2]);
    propSetter->SetRepresentationToWireframe();



    if( bFancySurface )
    {
        propSetter->SetLighting(true);
        //propSetter->SetBackfaceCulling(true);
        propSetter->SetBackfaceCulling(true);
        SP(vtkLight) L1 = SP(vtkLight)::New();
        SP(vtkLight) L2 = SP(vtkLight)::New();
        L1->SetLightTypeToSceneLight();
        L1->SetPositional(true);
        L1->SetAmbientColor(1.0,1.0,1.0);
        L1->SetSpecularColor(1.0,0.5,1.0);
        L1->SetIntensity(1.0);
        L1->SetConeAngle(90.0);
        L1->SetAttenuationValues(0.0,0.1,0.0);
        L1->SetPosition(0.0,2.0,0.0);
        L1->SetFocalPoint(0.0,0.0,0.0);
        L2->DeepCopy(L1);
        L2->SetPosition(-2.0,0.0,2.0);
        L2->SetSpecularColor(0.0,0.5,1.0);
        L2->SetIntensity(0.5);
        kwidget_3d_right->kv3DModelRenderer->AddLight(L1);
        kwidget_3d_right->kv3DModelRenderer->AddLight(L2);

        propSetter->SetSpecular(1);
        propSetter->SetSpecularPower(1);
        propSetter->SetSpecularColor(color[0],color[1],color[2]);
    } else
    {
        propSetter->SetSpecular(0);
    }
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
    qvtk->GetRenderWindow()->GetInteractor( )->SetDesiredUpdateRate(2);

}

}  // end anonymous namespace ... todo: move to structs if we feel like it.
// if we don't feel like it, then these should be member functions!

void KWidget_3D_right::UpdateSubVolumeExtractor( vtkImageData* new_subvolume_source, unsigned int labNumber ) {

    //kwidget_3d_right->multiLabelMaps3D.push_back(std::pair< vtkLODActor*, vtkExtractVOI* >(vtkLODActor::New(),vtkExtractVOI::New() ));
    std::pair<vtkLODActor*,vtkExtractVOI*>  pairElement =   multiLabelMaps3D[labNumber];
    if( pairElement.second == NULL ) {
        std::cout << "warning, null pair of LOD/VOI!" << std::endl;
    }
    
    multiLabelMaps3D[labNumber].second->SetInput( new_subvolume_source );
    multiLabelMaps3D[labNumber].second->Modified();
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


    bool UseVolumeRender =true; // TODO: 3D view needs total rewrite,
    // a) it doesn't support multiple levels at all
    // b) it will be too slow to volume render multiple labels
    // c) better idea: use x,y,z coords for colormap generation
    // in conjunction with polydata actors
    if(UseVolumeRender) {
        kwidget_3d_right->m_SliceIndex=0;
        // Turn off volume view temporarily for speed
        kwidget_3d_right->kv_opts = kv_opts_input; // grab options and state variables from KViewer main app.
        kwidget_3d_right->kv_data = kv_data_input;
        kwidget_3d_right->volRenView = Ptr<KVolumeRenderView>( new KVolumeRenderView );
        if( !kwidget_3d_right->kv_data )
            throw "kv_data does not exist yet!" ;

        kwidget_3d_right->multiLabelMaps3D = std::vector< std::pair< vtkLODActor*, vtkExtractVOI* > >();

        SetupRenderWindow( kwidget_3d_right );
        SetupSubVolumeExtractor( kwidget_3d_right );
        std::vector<double> firstcolor = get_good_color_0to7(0); // Keep it in agreement with the 2D!
        SetupLabelActor3D( kwidget_3d_right,firstcolor );

        kwidget_3d_right->m_PlaneWidgetZ =vtkImagePlaneWidget::New();
        QVTKWidget* qvtk = kwidget_3d_right->qVTK_widget_right;

        kwidget_3d_right->m_PlaneWidgetZ->SetInput(kwidget_3d_right->kv_data->imageVolumeRaw);
        kwidget_3d_right->m_PlaneWidgetZ->SetInteractor( qvtk->GetRenderWindow()->GetInteractor( ) );
        kwidget_3d_right->m_PlaneWidgetZ->SetCurrentRenderer(kwidget_3d_right->kv3DModelRenderer);
        kwidget_3d_right->m_PlaneWidgetZ->SetKeyPressActivationValue('n');

        kwidget_3d_right->m_PlaneWidgetZ->RestrictPlaneToVolumeOn();
        kwidget_3d_right->m_PlaneWidgetZ->GetPlaneProperty()->SetColor(1,0,0);

        kwidget_3d_right->m_PlaneWidgetZ->TextureInterpolateOn();
        kwidget_3d_right->m_PlaneWidgetZ->SetResliceInterpolateToLinear();
        kwidget_3d_right->m_PlaneWidgetZ->SetPlaneOrientationToZAxes();

        kwidget_3d_right->m_PlaneWidgetZ->SetEnabled(1);

        kwidget_3d_right->m_PlaneWidgetZ->GetTexturePlaneProperty()->SetOpacity(1);
        kwidget_3d_right->m_PlaneWidgetZ->DisplayTextOn();
        kwidget_3d_right->m_PlaneWidgetZ->UpdatePlacement();

    }
}

KWidget_3D_right::KWidget_3D_right( QVTKWidget* qvtk_handle ) {  // must explicity call Initialize()!
    this->qVTK_widget_right  = qvtk_handle;
}
