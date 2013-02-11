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
#include <cmath>
#include <set>

#include "ModuleEntry.h"
#include "Utils.h"
#include "getopt_pp_standalone.h"
#include "Logger.h"
#include "polydata_helper.h"
#include "meshcurvolver_test_common.h"
#include "path_finder.h"
#include "vtkDoubleArray.h"

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
vtkSmartPointer<vtkPolyDataMapper>*		    map_ptr;
vtkSmartPointer<vtkFloatArray>*             colormap_ptr;
vtkSmartPointer<vtkIntArray>*               colormap_int_ptr;

vtkSmartPointer<vtkPolyData> createSyntheticSurface()
{

    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    points = createSyntheticDonut( 1500 );
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
    reverse->Update();

    surface = reverse->GetOutput();

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

    std::list<int> path_list;
    compute_backwards_optimal_path( mesh, distanceMaps, seeds, 0, seeds[1], path_list );

    // for every seed, find the closest seed.
    std::vector<size_t> closest_seed1( seeds.size() );
    std::vector<size_t> closest_seed2( seeds.size() );
    std::vector<double> minD_seed1( seeds.size() );
    std::vector<double> minD_seed2( seeds.size() );
    for( size_t sFrom = 0; sFrom < seeds.size(); sFrom++ ) {
        minD_seed1[sFrom] = 1e6;
        for( size_t sTo = 0; sTo < seeds.size(); sTo++ ) {
            if( sTo == sFrom )
                continue;
            double dval = distanceMaps->GetComponent( seeds[sFrom], sTo );
            if( dval < minD_seed1[sFrom] ) {
                minD_seed1[sFrom] = dval;
                closest_seed1[sFrom] = sTo;
            }
        }
        minD_seed2[sFrom] = 1e6;
        for( size_t sTo = 0; sTo < seeds.size(); sTo++ ) {
            if( sTo == sFrom || sTo == closest_seed1[sFrom] )
                continue;
            double dval = distanceMaps->GetComponent( seeds[sFrom], sTo );
            if( dval < minD_seed2[sFrom] ) {
                minD_seed2[sFrom] = dval;
                closest_seed2[sFrom] = sTo;
            }
        }
        cout << "(A,B) for i = " << sFrom << ":  " << closest_seed1[sFrom] << ", " << closest_seed2[sFrom] << endl;
    }

    // Now: if all of the assignments "jive", can just draw L0 now.
    // If they don't, need to 'steal' the closest neighbors so everyone is happy.

    std::list<int> unpaired_seeds(0); // seeds that don't have 2 neighbors in agreement.
    std::list<int> unpaired_froms(0); // where the seeds want to connect
    for( size_t sFrom = 0; sFrom < seeds.size(); sFrom++ ) {
        int  n1 =  closest_seed1[sFrom];
        int  n2 =  closest_seed2[sFrom];
        if( sFrom != closest_seed1[n1] && sFrom != closest_seed2[n1] ) {
            unpaired_seeds.push_back( sFrom );
            unpaired_froms.push_back(n1);
        }
        if( sFrom != closest_seed1[n2] && sFrom != closest_seed2[n2] ) {
            unpaired_seeds.push_back( sFrom );
            unpaired_froms.push_back(n2);
        }
    }

    assert( unpaired_seeds.empty() ); // good to go!
    std::list<int>  L0;
    std::set<int>   done_src;
    std::set<int>   done_dst;

    int sFrom = 0;
    int sPrev = sFrom;
    while ( done_dst.size() < seeds.size() )
    {
        int  n1          =  closest_seed1[sFrom];
        int  n1_is_used  =  done_dst.count( n1 ) + (sPrev == n1 );
        if( n1_is_used )
            n1           =  closest_seed2[sFrom];
        assert( done_dst.count( n1 ) == 0 );
        done_dst.insert( n1 );
        done_src.insert( sFrom );

        cout << "from,to: " << sFrom << ", " << n1 << endl;

        std::list<int>  path_list;
        compute_backwards_optimal_path( mesh, distanceMaps, seeds, n1, seeds[sFrom], path_list );
        sPrev = sFrom;
        sFrom = n1;


        while( !path_list.empty() ) {
            int idx = path_list.back();
            L0.push_back( idx );
            cout << "L0 pushed idx: " << idx << endl;
            path_list.pop_back();
        }


    }

    vtkSmartPointer<vtkPolyData> polydataL0     = VertexList2PolyLine( L0, mesh );
    vtkSmartPointer<vtkPolyDataMapper> L0Mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    L0Mapper->SetInput( polydataL0 );
    vtkSmartPointer<vtkActor> L0Actor = vtkSmartPointer<vtkActor>::New();
    L0Actor->SetMapper(L0Mapper);
    L0Actor->GetProperty()->SetColor(1,0.5,0.5); // reddish
    L0Actor->GetProperty()->SetLineWidth(4);

    // Create vtk geodesic path. For display!
    vtkSmartPointer<vtkDijkstraGraphGeodesicPath> dijkstra = vtkSmartPointer<vtkDijkstraGraphGeodesicPath>::New();
    dijkstra->SetInput( mesh );
    dijkstra->SetStartVertex( seeds[3] );
    dijkstra->SetEndVertex(   seeds[0] );
    dijkstra->Update();
    vtkIdList* vtk_path = dijkstra->GetIdList();
    int num_ids = vtk_path->GetNumberOfIds();

    vtkSmartPointer<vtkDoubleArray> weights = vtkSmartPointer<vtkDoubleArray>::New();
    dijkstra->GetCumulativeWeights( weights );
    for ( int k = 0; k < num_ids; k++ ) {
        cout <<" id: " << vtk_path->GetId( k ) << ",  " << weights->GetValue(k) << " ----  " ;
    }
    cout << endl;

    // Create a mapper and actor for the dijkstra path
    vtkSmartPointer<vtkPolyDataMapper> pathMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    pathMapper->SetInputConnection(dijkstra->GetOutputPort());
    vtkSmartPointer<vtkActor> pathActor = vtkSmartPointer<vtkActor>::New();
    pathActor->SetMapper(pathMapper);
    pathActor->GetProperty()->SetColor(1,1,1); // white
    pathActor->GetProperty()->SetLineWidth(4);

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
    surfaceActor->GetProperty()->SetSpecularPower(50);
    surfaceActor->GetProperty()->SetInterpolationToGouraud();

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
    ren->AddActor(pathActor);
    ren->AddActor(L0Actor);
    ren->SetActiveCamera(camera);
    ren->SetBackground(1,1,1);
    ren->ResetCamera();
    renWin->SetSize(640,480);

    renWin->Render();
    iren->Initialize();
    iren->Start();

    iren->Delete();

}

void Test3()
{
    vtkSmartPointer<vtkPolyData> outputMesh = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPolyData> inputMesh  = createSyntheticSurface();

    MeshGeometryCompute::curvature(inputMesh,outputMesh);
    inputMesh->DeepCopy( outputMesh );

    std::vector< std::vector<float> >  init_points_3d;
    getSyntheticSeedPoints( init_points_3d );

    MeshContourEvolver::entry_main( inputMesh, init_points_3d, outputMesh );

    displaySyntheticSurface(outputMesh);

    cout <<"End of Test3"<<endl;
}

int main()
{
    LOUT "Test 3: Init LevelSet on Synthetic data set. " LENDL

    Test3();

}
