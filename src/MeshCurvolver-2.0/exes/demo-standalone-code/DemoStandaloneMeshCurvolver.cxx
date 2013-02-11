#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyDataReader.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkDebugLeaks.h"
#include "vtkTestUtilities.h"
#include "vtkFieldData.h"
#include "vtkCellPicker.h"
#include "vtkCallbackCommand.h"
#include "vtkPolyDataWriter.h"
#include "vtkXMLPolyDataWriter.h"
#include "vtkSurfaceReconstructionFilter.h"
#include "vtkProgrammableSource.h"
#include "vtkContourFilter.h"
#include "vtkReverseSense.h"
#include "vtkProperty.h"
#include "vtkSmartPointer.h"
#include <vtkMath.h>
#include "vtkXMLPolyDataReader.h"
#include <cmath>

#include "ModuleEntry.h"
#include "Utils.h"
#include "getopt_pp_standalone.h"
#include "polydata_helper.h"

using namespace std;
using namespace MeshContourEvolver;
using namespace vrcl;

// TODO: this stuff seems to cause problems, encapsulate it better!
// variables that get used in callbacks
vtkRenderWindowInteractor*                  iren ;
vtkSmartPointer<vtkPolyData>*               inputMesh_ptr;
vtkSmartPointer<vtkCellPicker>*             picker_ptr;
vtkSmartPointer<vtkRenderer>*               ren_ptr;
vtkSmartPointer<vtkPolyDataMapper>*		      map_ptr;
vtkSmartPointer<vtkFloatArray>*             colormap_ptr;
vtkSmartPointer<vtkIntArray>*               colormap_int_ptr;

int done_init = false;
std::vector<int> init_verts;
vector< vector<float> > init_points_f;

void TestModuleInteraction1()
{
    std::cout << "running demo and will display curvature..." << std::endl;
    vtkPolyData* inputMesh = *inputMesh_ptr;

    vtkSmartPointer<vtkIntArray> initPoints = vtkSmartPointer<vtkIntArray>::New();
      vtkPolyData* outputMesh;
    //InitParam init = {5,1,1,1,1};
    path_finder( inputMesh, initPoints, outputMesh  /*, init*/);

    inputMesh->DeepCopy( outputMesh );
    outputMesh->Delete();


  PrintPolydataArrayInfo( inputMesh ); // display the names of arrays in this polydata...
                                        // should have the various geometric values stored
  vtkDataArray* surfaceCurvature = inputMesh->GetPointData()->GetArray("SurfaceMeanCurvature");
  if( NULL != surfaceCurvature ) {
    vtkFloatArray* colormap = *colormap_ptr;
    vtkPolyDataMapper* map = *map_ptr;
    colormap = vtkFloatArray::SafeDownCast( surfaceCurvature );
    double dminmax[2];
    colormap->GetRange( dminmax );
      double dmax = dminmax[1]; double dmin = dminmax[0];
    map->SetScalarRange( dmin, dmax );
    inputMesh->GetPointData()->SetScalars(colormap);
    map->Update();
    done_init = false;
  }
}

void TestModuleInteraction2(bool bUseClickedInit = false)
{
  vector< vector<float> > initPoints3D;
  if( !bUseClickedInit ) {
      vector<float> p1(3); 	vector<float> p2(3);
      vector<float> p3(3);	vector<float> p4(3);
      p1[0] = 10; p1[1] = 10; p1[2] = 0;
      p2[0] = 30; p2[1] = 10; p2[2] = 0;
      p3[0] = 10; p3[1] = 30; p3[2] = 0;
      p4[0] = 25; p4[1] = 25; p4[2] = 0;

      initPoints3D.push_back( p1 );
      initPoints3D.push_back( p2 );
      initPoints3D.push_back( p3 );
      initPoints3D.push_back( p4 );
  }
  else
    initPoints3D = init_points_f;

    vtkPolyData* inputMesh = *inputMesh_ptr;
    vtkSmartPointer<vtkPolyData> outputMesh = vtkSmartPointer<vtkPolyData>::New();
  //InitParam init = {5,1,1,1,1};
  entry_main( inputMesh, initPoints3D, outputMesh/*, init*/);

    inputMesh->DeepCopy( outputMesh );

  PrintPolydataArrayInfo( outputMesh ); // display the names of arrays in this polydata...
                                        // should have the various geometric values stored

  vtkDataArray* surfaceCurvature = inputMesh->GetPointData()->GetArray("SurfaceMeanCurvature");
  if( NULL != surfaceCurvature ) {
    vtkFloatArray* colormap = *colormap_ptr;
    vtkPolyDataMapper* map = *map_ptr;
    colormap = vtkFloatArray::SafeDownCast( surfaceCurvature );
    double dminmax[2];
    colormap->GetRange( dminmax );
      double dmax = dminmax[1]; double dmin = dminmax[0];
    map->SetScalarRange( dmin, dmax );
    inputMesh->GetPointData()->SetScalars(colormap);
    map->Update();
    done_init = false;
  }

}

void SavePolyData()
{
    std::cout<<"Saving vtkPolyData to disk...\n";
    vtkPolyData* inputMesh = *inputMesh_ptr;
    vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
    writer->SetInput( inputMesh );
    writer->SetFileName("LSVTKOut.vtk");
    writer->Write();
}

// Select the mesh cell over which the mouse is located
// for initialization of contours.
void SelectCell( vtkRenderer* ren, vtkCellPicker* pick, int X, int Y )
{
    vtkPolyData* inputMesh  = *inputMesh_ptr;
    vtkPolyDataMapper* map  = *map_ptr;
    vtkFloatArray* colormap = *colormap_ptr;

    vtkIdType numpts;
    vtkIdType* ptsIds;
    int CellId;
    pick->Pick( (double)X, (double)Y, (double)0, ren );
    CellId = pick->GetCellId();
    if( CellId >= 0 ) {
        inputMesh->GetCellPoints( CellId, numpts, ptsIds );
        int i0 = ptsIds[0];
        std::cout<<"You have picked vertex number: "<<i0<<"\n";
        init_verts.push_back(i0);
        double xyzpt[3];
        inputMesh->GetPoint( i0, xyzpt );
        vector<float> xyzptf(3);
        for( int k=0;k<3;k++)
          xyzptf[k] = xyzpt[k];
        init_points_f.push_back( xyzptf );
        /* The following dmin code forces an update on the selected points as opposed to
        a static double */
        double dmin = -1.0 + (rand()%101) / 101.0;
        double dmax = 1.0;
        map->SetScalarRange( dmin, dmax );
        colormap->SetTuple1(i0,dmax);
        inputMesh->GetPointData()->SetScalars(colormap);
        map->Update();

        if( done_init ) {
          //TestModuleInteraction1( );  // display curvature
          TestModuleInteraction2( 0 );  // display contour idx
        }
    }
    else
        cout<<"Bogus CellId "<<CellId<<"\n";
    return;
}

// redraw the scene when something has changed
void redraw_callback( vtkObject* caller, unsigned long eventID, void* ClientData, void* data )
{
    switch (eventID) {
        case vtkCommand::MouseWheelForwardEvent:
        case vtkCommand::MouseWheelBackwardEvent:
            break;
        default:
            break;
    }
}

// Callback to handle keyboard press
void userio_callback( vtkObject* caller, unsigned long eventID, void* ClientData, void* data )
{
    char keypressed = '.';
    switch (eventID ) {
        case vtkCommand::TimerEvent:
            cout<<"TimerEvent! \n";
            break;
        case vtkCommand::KeyPressEvent:
            int X, Y;
            keypressed = *(iren -> GetKeySym());
            iren->GetEventPosition( X, Y );
            switch( keypressed )
            {
                case 'y':
                    SelectCell( *ren_ptr, *picker_ptr, X, Y );
                    break;
                case 'Y':
                    done_init = true;
                    SelectCell( *ren_ptr, *picker_ptr, X, Y );
                    break;
                case 's':
                    SavePolyData();
                    break;
            }
            break;
      // Test the three different entry points for module code!
        case vtkCommand::RightButtonPressEvent:
            cout<<"Right Mouse - Running Tests! \n";
            //TestModuleInteraction();
               TestModuleInteraction2();
            break;
        default:
            cout<<"Bogus Event! \n";
            break;
    }
}

vtkPoints* createSyntheticPoints()
{
    vtkPoints* points = vtkPoints::New();

    float x, y, z,u,v;
    // generate random points on bumpy surface
  int ii,jj;
  ii = 100;
  jj = 100;
    int Npts = 15000;

  bool bHarderExample = false;
  if( bHarderExample) {
    for(int i=0; i<ii; i++) {
      for( int j = 0; j < jj; j++ ) {
            u     = vtkMath::Random(-5.0,5.0);
            v     = vtkMath::Random(-5.0,5.0);
            z     = std::sin(x) * std::sin(y);
        x     = u;
        y     = v;
            points->InsertNextPoint(x, y, z);
      }
      }
  }
  else {
   for(int i=0; i<Npts; i++) {
        x = vtkMath::Random(-10.0,10.0);
        y = vtkMath::Random(-10.0,10.0);
        z = std::sin(x) * std::sin(y);
        points->InsertNextPoint(x, y, z);
   }
  }
    return points;
}

void displaySyntheticSurface()
{
    // variables that get used in callbacks
    vtkSmartPointer<vtkCellPicker>				picker;
    vtkSmartPointer<vtkPolyData>				inputMesh;

    picker = vtkSmartPointer<vtkCellPicker>::New();
    picker_ptr = &picker;
    vtkSmartPointer<vtkCallbackCommand> userio = vtkSmartPointer<vtkCallbackCommand>::New();
    vtkSmartPointer<vtkCallbackCommand> idle   = vtkSmartPointer<vtkCallbackCommand>::New();
    userio->SetCallback( userio_callback );
    idle->SetCallback( redraw_callback );

    vtkPoints* points_ = createSyntheticPoints();
    vtkSmartPointer<vtkPoints> points = points_;
    points_->Delete();
    vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(points);

    // Construct the surface and create isosurface.
    vtkSmartPointer<vtkSurfaceReconstructionFilter> surf = vtkSmartPointer<vtkSurfaceReconstructionFilter>::New();
    surf->SetInput(polydata);

    vtkSmartPointer<vtkContourFilter> cf = vtkSmartPointer<vtkContourFilter>::New();
    cf->SetInputConnection(surf->GetOutputPort());
    cf->SetValue(0, 0.0);

    // Sometimes the contouring algorithm can create a volume whose gradient
    // vector and ordering of polygon (using the right hand rule) are
    // inconsistent. vtkReverseSense cures this problem.
    vtkSmartPointer<vtkReverseSense> reverse = vtkSmartPointer<vtkReverseSense>::New();
    reverse->SetInputConnection(cf->GetOutputPort());
    reverse->ReverseCellsOn();
    reverse->ReverseNormalsOn();
    reverse->Update();
    inputMesh			= reverse->GetOutput();
    inputMesh_ptr		= &inputMesh;

    // create and initialize to zero the colormap for display
    vtkSmartPointer<vtkFloatArray> colormap = vtkSmartPointer<vtkFloatArray>::New();
    colormap_ptr = &colormap;
    vtkSmartPointer<vtkIntArray> colormap_int = vtkSmartPointer<vtkIntArray>::New();
    colormap_int_ptr = &colormap_int;

    int iNumDispComp = 1; // number of display components
    int iNumVerts = inputMesh->GetNumberOfPoints( );
    colormap->SetNumberOfComponents( iNumDispComp );
    colormap->SetNumberOfTuples( iNumVerts );
    colormap_int->SetNumberOfComponents( iNumDispComp );
    colormap_int->SetNumberOfTuples( iNumVerts );

    double ptx[3];
    for(int i = 0; i < iNumVerts; i++) {
        inputMesh->GetPoint( i , ptx );
        colormap->SetTuple1(i, ptx[0]);
        colormap_int->SetValue( i, 0 );
    }
    colormap->GetRange( ptx );
    double dminx = ptx[0]; double dmaxx = ptx[1];

    double pty[3];
    for(int i = 0; i < iNumVerts; i++) {
        inputMesh->GetPoint( i , pty );
        colormap->SetTuple1(i, pty[1]);
        colormap_int->SetValue( i, 0 );
    }
    colormap->GetRange( pty );

    /* Step 1 */
    inputMesh->GetPointData()->SetScalars(colormap); // attach colormap to mesh

    /* Step 2 */
    vtkSmartPointer<vtkPolyDataMapper> map = vtkSmartPointer<vtkPolyDataMapper>::New();
    map_ptr = &map;

    /* Step 3 */
    map->SetInput( inputMesh );
    map->ScalarVisibilityOn( );

    /* Step 4 */
    map->SetScalarRange( dminx, dmaxx );

    /* Step 5 */
    vtkSmartPointer<vtkActor> surfaceActor = vtkSmartPointer<vtkActor>::New();
    surfaceActor->SetMapper(map);
    surfaceActor->GetProperty()->SetDiffuseColor(0.0000, 0.6882, 0.4784);
    surfaceActor->GetProperty()->SetSpecularColor(1, 1, 1);
    surfaceActor->GetProperty()->SetSpecular(.4);
    surfaceActor->GetProperty()->SetSpecularPower(50);

    // Create the RenderWindow, Renderer and both Actors
    vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();
    ren_ptr = &ren;

    vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
    renWin->AddRenderer(ren);
    iren = vtkRenderWindowInteractor::New();
    iren->SetRenderWindow(renWin);

    vtkSmartPointer<vtkCamera> camera = vtkCamera::New();
    camera->SetPosition(100,100,100);
    camera->SetFocalPoint(99,99,99);

    /* Add the surfaceActor to the renderer, set the background and size */
    ren->AddActor(surfaceActor);
    ren->SetActiveCamera(camera);
    ren->SetBackground(1,1,1);
    ren->ResetCamera();
    renWin->SetSize(640,480);

    iren->AddObserver( vtkCommand::KeyPressEvent, userio );
    iren->AddObserver( vtkCommand::RightButtonPressEvent, userio );
    iren->AddObserver( vtkCommand::MouseWheelBackwardEvent, idle );
    iren->AddObserver( vtkCommand::MouseWheelForwardEvent, idle );

    renWin->Render();
    iren->Initialize();

    iren->Start();

    // smart pointers don't need Delete()
    // delete the raw pointers on the heap
    iren->Delete();
}

void vtpReader(char* argc, const std::string& colormapName )
{
    // variables that get used in callbacks
    vtkSmartPointer<vtkCellPicker>				picker;
    vtkSmartPointer<vtkPolyData>				inputMesh;

    picker = vtkSmartPointer<vtkCellPicker>::New();
    picker_ptr = &picker;
    vtkSmartPointer<vtkCallbackCommand> userio = vtkSmartPointer<vtkCallbackCommand>::New();
    vtkSmartPointer<vtkCallbackCommand> idle   = vtkSmartPointer<vtkCallbackCommand>::New();
    userio->SetCallback( userio_callback );
    idle->SetCallback( redraw_callback );

    vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
    reader->SetFileName( argc );
    reader->Update();
    vtkSmartPointer<vtkPolyData> polydata  = reader->GetOutput();
        cout<<argc;
    cout<<polydata->GetNumberOfPoints();


    inputMesh			= polydata;
    inputMesh_ptr		= &inputMesh;

    // create and initialize to zero the colormap for display
    vtkSmartPointer<vtkFloatArray> colormap = vtkSmartPointer<vtkFloatArray>::New();
    colormap_ptr = &colormap;
    vtkSmartPointer<vtkIntArray> colormap_int = vtkSmartPointer<vtkIntArray>::New();
    colormap_int_ptr = &colormap_int;

    int iNumDispComp = 1; // number of display components
    int iNumVerts = inputMesh->GetNumberOfPoints( );
    colormap->SetNumberOfComponents( iNumDispComp );
    colormap->SetNumberOfTuples( iNumVerts );
    colormap_int->SetNumberOfComponents( iNumDispComp );
    colormap_int->SetNumberOfTuples( iNumVerts );

    double ptx[3];
    for(int i = 0; i < iNumVerts; i++) {
        inputMesh->GetPoint( i , ptx );
        colormap->SetTuple1(i, ptx[0]);
        colormap_int->SetValue( i, 0 );
    }
    colormap->GetRange( ptx );
    double dminx = ptx[0]; double dmaxx = ptx[1];

    double pty[3];
    for(int i = 0; i < iNumVerts; i++) {
        inputMesh->GetPoint( i , pty );
        colormap->SetTuple1(i, pty[1]);
        colormap_int->SetValue( i, 0 );
    }
    colormap->GetRange( pty );

    /* Step 1 */
    inputMesh->GetPointData()->SetScalars(colormap); // attach colormap to mesh

    /* Step 2 */
    vtkSmartPointer<vtkPolyDataMapper> map = vtkSmartPointer<vtkPolyDataMapper>::New();
    map_ptr = &map;

    /* Step 3 */
    map->SetInput( inputMesh );
    map->ScalarVisibilityOn( );

    /* Step 4 */
    map->SetScalarRange( dminx, dmaxx );

    if( !colormapName.empty() ) {
        vtkDataArray* surfaceCurvature = inputMesh->GetPointData()->GetArray(colormapName.c_str());
        if( NULL != surfaceCurvature ) {
          vtkFloatArray* colormap = *colormap_ptr;
          vtkPolyDataMapper* map = *map_ptr;
          colormap = vtkFloatArray::SafeDownCast( surfaceCurvature );
          double dminmax[2];
          colormap->GetRange( dminmax );
            double dmax = dminmax[1]; double dmin = dminmax[0];
          map->SetScalarRange( dmin, dmax );
          inputMesh->GetPointData()->SetScalars(colormap);
          map->Update();
          done_init = false;
        }
    } else {
        cout << "invalid input colormap name, not found in the mesh data: " << colormapName << endl;
    }


    /* Step 5 */
    vtkSmartPointer<vtkActor> surfaceActor = vtkSmartPointer<vtkActor>::New();
    surfaceActor->SetMapper(map);
    surfaceActor->GetProperty()->SetDiffuseColor(0.0000, 0.6882, 0.4784);
    surfaceActor->GetProperty()->SetSpecularColor(1, 1, 1);
    surfaceActor->GetProperty()->SetSpecular(.4);
    surfaceActor->GetProperty()->SetSpecularPower(50);

    // Create the RenderWindow, Renderer and both Actors
    vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();
    ren_ptr = &ren;

    vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
    renWin->AddRenderer(ren);
    iren = vtkRenderWindowInteractor::New();
    iren->SetRenderWindow(renWin);

    vtkSmartPointer<vtkCamera> camera = vtkCamera::New();
    camera->SetPosition(100,100,100);
    camera->SetFocalPoint(99,99,99);

    /* Add the surfaceActor to the renderer, set the background and size */
    ren->AddActor(surfaceActor);
    ren->SetActiveCamera(camera);
    ren->SetBackground(1,1,1);
    ren->ResetCamera();
    renWin->SetSize(640,480);

    iren->AddObserver( vtkCommand::KeyPressEvent, userio );
    iren->AddObserver( vtkCommand::RightButtonPressEvent, userio );
    iren->AddObserver( vtkCommand::MouseWheelBackwardEvent, idle );
    iren->AddObserver( vtkCommand::MouseWheelForwardEvent, idle );

    renWin->Render();
    iren->Initialize();

    iren->Start();

    // smart pointers don't need Delete()
    // delete the raw pointers on the heap
    iren->Delete();
}


using namespace GetOpt;
int main(int argc, char **argv) {

    string vtpStrInputFileName = "";
    string vtpStrOutputFileName = "";
    string colormapName = "";
    int bVerboseOut = 0;

    GetOpt_pp ops(argc, argv);

    ops >> Option('o', "output file name (vtp)", vtpStrOutputFileName);
    ops >> Option('i', "input file name (vtp)", vtpStrInputFileName);
    ops >> Option('c', "input file colormap to use", colormapName);
    ops >> Option('v', "verbose output", bVerboseOut);

    if (!(ops >> Option('i', "input", vtpStrInputFileName))) {
        cerr << "Did not see valid arg 'i', generating synthetic surface... " << endl;
    }
    if (!(ops >> Option('o', "output file name (vtp)", vtpStrOutputFileName))) {
        cerr << "not using output file..." << endl;
    }
    if ( ops >> Option('v', "verbose", bVerboseOut) ) {
      cout<<"Using verbose output... \n";
    }

    if (vtpStrInputFileName.size() > 1) {
        cout<<"Using VTP file: "<<vtpStrInputFileName<<endl;
        int NBYTES = sizeof(char) * (1+vtpStrInputFileName.size());
        char* str = (char*) malloc( NBYTES );
        memcpy( str, vtpStrInputFileName.c_str(), NBYTES);

        vtpReader( str, colormapName );
        free( str );
    } else {
      displaySyntheticSurface();
    }
    return 0;
}
