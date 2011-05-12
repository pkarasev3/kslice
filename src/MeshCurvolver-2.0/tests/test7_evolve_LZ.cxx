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
#include "vtkDijkstraGraphGeodesicPath.h"
#include "vtkSmoothPolyDataFilter.h"
#include <cmath>
#include <set>

#include "ModuleEntry.h"
#include "Utils.h"
#include "getopt_pp_standalone.h"
#include "Logger.h"
#include "polydata_helper.h"
#include "meshcurvolver_test_common.h"
#include "path_finder.h"
#include "LevelSetInitHelper.h"
#include "vtkDoubleArray.h"
#include "vtkCleanPolyData.h"

using namespace vrcl;
using std::valarray;
using std::vector;
using std::string;
using std::list;
using std::set;

// variables that get used in callbacks
vtkRenderWindowInteractor*                  iren ;
vtkSmartPointer<vtkCellPicker>*             picker_ptr;
vtkSmartPointer<vtkRenderer>*               ren_ptr;
vtkSmartPointer<vtkPolyDataMapper>*		      map_ptr;
vtkSmartPointer<vtkFloatArray>*             colormap_ptr;
vtkSmartPointer<vtkIntArray>*               colormap_int_ptr;

vtkSmartPointer<vtkPolyData> createSyntheticSurface()
{

    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    points = createSyntheticStarBlob( 3000 );
    vtkSmartPointer<vtkPolyData> surface = vtkSmartPointer<vtkPolyData>::New();
    surface->SetPoints(points);

    vtkSmartPointer<vtkSurfaceReconstructionFilter> surf =  vtkSmartPointer<vtkSurfaceReconstructionFilter>::New();
    surf->SetInput(surface);

    vtkSmartPointer<vtkContourFilter> cf = vtkSmartPointer<vtkContourFilter>::New();
    cf->SetInputConnection(surf->GetOutputPort());
    cf->SetValue(0,0.0);

    vtkSmartPointer<vtkReverseSense> reverse = vtkSmartPointer<vtkReverseSense>::New();
    reverse->SetInputConnection(cf->GetOutputPort());
    reverse->ReverseCellsOn();
    reverse->ReverseNormalsOn();

    vtkSmartPointer<vtkSmoothPolyDataFilter> smoother = vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
    smoother->SetInputConnection( reverse->GetOutputPort( ) );
    smoother->SetNumberOfIterations(50);
    smoother->Update();

    surface = smoother->GetOutput();

    return surface;
}

void displaySyntheticSurface(vtkSmartPointer<vtkPolyData> mesh)
{
    // display names of arrays that got attached
    PrintPolydataArrayInfo( mesh );
    int num_verts = mesh->GetNumberOfPoints( );

    string array_name     = MeshContourEvolver::getDistanceMapArrayName( );
    vtkFloatArray* distanceMaps =
            vtkFloatArray::SafeDownCast( mesh->GetPointData()->GetArray( array_name.c_str() ) );
    vtkSmartPointer<vtkFloatArray> colormap = vtkSmartPointer<vtkFloatArray>::New();
    colormap->SetNumberOfComponents(1);
    colormap->SetNumberOfTuples(num_verts);

    for ( int k = 0; k < num_verts; k++ ) {
       double val = distanceMaps->GetComponent( k, 0 );
       colormap->SetValue(k,val);
    }

    std::vector< std::vector<float> >  init_points_3d;
    getSyntheticSeedPoints( init_points_3d );
    vtkSmartPointer<vtkIntArray> vtkSeedIdx = vtkSmartPointer<vtkIntArray>::New();

    getClosestMeshIndicesToFloatPoints( init_points_3d, mesh, vtkSeedIdx );
    std::vector<int> seeds( vtkSeedIdx->GetNumberOfTuples() );
    memcpy( &(seeds[0]), vtkSeedIdx->GetPointer(0), sizeof(int) * seeds.size() );

    list<int> L0; set<int> Lz  ;    set<int> Ln1 ;    set<int> Lp1 ;    set<int> Ln2 ;    set<int> Lp2 ;
    vtkSmartPointer<vtkIntArray>  array = vtkSmartPointer<vtkIntArray>::New();
    initialize_closed_path( mesh, seeds, L0 );
    initLevelSetLists(mesh, L0,Lp1,Ln1,Lp2,Ln2,Lz);

    LevelsetLayerActors  layer_actors;
    layer_actors.createFromIndexSets(mesh,L0,Lp1,Ln1,Lp2,Ln2,Lz);


    double ptx[2];
    colormap->GetRange( ptx );
    double dminx = ptx[0];
    double dmaxx = ptx[1];
    cout << "grabbed min/max distance map values: " << dminx << ", " << dmaxx << endl;

    /* Step 1 */
    mesh->GetPointData()->SetScalars(colormap); // attach colormap to mesh

    /* Step 2 */
    vtkSmartPointer<vtkPolyDataMapper> map = vtkSmartPointer<vtkPolyDataMapper>::New();

    /* Step 3 */
    map->SetInput( mesh );
    map->ScalarVisibilityOn( );

    /* Step 4 */
    map->SetScalarRange( dminx, dmaxx );

    /* Step 5 */
    vtkSmartPointer<vtkActor> surfaceActor = vtkSmartPointer<vtkActor>::New();
    surfaceActor->SetMapper(map);
    surfaceActor->GetProperty()->SetDiffuseColor(0.0000, 0.6882, 0.4784);
    surfaceActor->GetProperty()->SetSpecularColor(1, 1, 1);
    surfaceActor->GetProperty()->SetSpecular(.4);
    surfaceActor->GetProperty()->SetSpecularPower(10);
    surfaceActor->GetProperty()->SetInterpolationToFlat();
    surfaceActor->GetProperty()->SetOpacity(1.0);
    surfaceActor->GetProperty()->SetPointSize(10.0);

    // Create the RenderWindow, Renderer and both Actors
    vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();

    vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
    renWin->AddRenderer(ren);
    iren = vtkRenderWindowInteractor::New();
    iren->SetRenderWindow(renWin);

    vtkSmartPointer<vtkCamera> camera = vtkCamera::New();
    camera->SetPosition(100,100,100);
    camera->SetFocalPoint(99,99,99);

    /* Add the surfaceActor to the renderer, set the background and size */
    ren->AddActor(surfaceActor);
    layer_actors.attachToRenderer( ren );

    ren->SetActiveCamera(camera);
    ren->SetBackground(1,1,1);
    ren->ResetCamera();
    renWin->SetSize(640,480);

    renWin->Render();
    iren->Initialize();
    iren->Start();

    iren->Delete();

}

void TestInitLZ()
{
    vtkSmartPointer<vtkPolyData> outputMesh = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPolyData> inputMesh  = createSyntheticSurface();

    MeshGeometryCompute::curvature(inputMesh,outputMesh);
    inputMesh->DeepCopy( outputMesh );

    std::vector< std::vector<float> >  init_points_3d;
    getSyntheticSeedPoints( init_points_3d );

    // mean_curvature already exists, so we init the curve now.
    MeshContourEvolver::entry_main( inputMesh, init_points_3d, outputMesh );

    displaySyntheticSurface(outputMesh);

    cout <<"End of Test3"<<endl;
}

int main()
{
    LOUT "Test 6: Init LevelSet on Synthetic data set. " LENDL

    TestInitLZ();

}

