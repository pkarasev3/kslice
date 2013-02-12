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

#include "polydata_helper.h"
#include "ModuleEntry.h"
#include "Utils.h"
#include "getopt_pp_standalone.h"
#include "Logger.h"
#include "meshcurvolver_test_common.h"

using namespace vrcl;
using std::valarray;
using std::vector;
using std::string;
using std::list;

// variables that get used in callbacks
vtkRenderWindowInteractor*                  iren ;
vtkSmartPointer<vtkCellPicker>*             picker_ptr;
vtkSmartPointer<vtkRenderer>*               ren_ptr;
vtkSmartPointer<vtkPolyDataMapper>*		      map_ptr;
vtkSmartPointer<vtkFloatArray>*             colormap_ptr;
vtkSmartPointer<vtkIntArray>*               colormap_int_ptr;



vtkSmartPointer<vtkPolyData> createSyntheticSurface()
{

    vtkPoints* points_ = createSyntheticBumps();
    vtkSmartPointer<vtkPoints> points = points_;
    points_->Delete();

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

void displaySyntheticSurface(vtkSmartPointer<vtkPolyData> outputMesh)
{
    // display names of arrays that got attached
    PrintPolydataArrayInfo( outputMesh );
    int num_verts = outputMesh->GetNumberOfPoints( );

    string array_name     = MeshContourEvolver::getDistanceMapArrayName( );
    vtkFloatArray* distanceMaps =
            vtkFloatArray::SafeDownCast( outputMesh->GetPointData()->GetArray( array_name.c_str() ) );
    vtkSmartPointer<vtkFloatArray> colormap = vtkSmartPointer<vtkFloatArray>::New();
    colormap->SetNumberOfComponents(1);
    colormap->SetNumberOfTuples(num_verts);

    for ( int k = 0; k < num_verts; k++ ) {
        double minVal = 1e6;
        int nC      = distanceMaps->GetNumberOfComponents();
        for( int m = 0; m < nC; m++ ) {
            double val = distanceMaps->GetComponent( k, m );
            if( val < minVal ) {
                minVal  = val;
                colormap->SetValue(k,val);
            }
        }
    }

    double ptx[2];
    colormap->GetRange( ptx, 0 );

    double dminx = ptx[0];
    double dmaxx = ptx[1];

    cout << "grabbed min/max distance map values: " << dminx << ", " << dmaxx << endl;

    /* Step 1 */
    outputMesh->GetPointData()->SetScalars(colormap); // attach colormap to mesh

    /* Step 2 */
    vtkSmartPointer<vtkPolyDataMapper> map = vtkSmartPointer<vtkPolyDataMapper>::New();

    /* Step 3 */
    map->SetInput( outputMesh );
    map->ScalarVisibilityOn( );

    /* Step 4 */
    map->SetScalarRange( ptx[0], ptx[1] );

    /* Step 5 */
    vtkSmartPointer<vtkActor> surfaceActor = vtkSmartPointer<vtkActor>::New();
    surfaceActor->SetMapper(map);
    surfaceActor->GetProperty()->SetDiffuseColor(0.0000, 0.6882, 0.4784);
    surfaceActor->GetProperty()->SetSpecularColor(1, 1, 1);
    surfaceActor->GetProperty()->SetSpecular(.4);
    surfaceActor->GetProperty()->SetSpecularPower(50);

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
    ren->SetActiveCamera(camera);
    ren->SetBackground(1,1,1);
    ren->ResetCamera();
    renWin->SetSize(640,480);

    renWin->Render();
    iren->Initialize();
    iren->Start();


    iren->Delete();

}

/** Careful: being on an edge can make the distance-function computation go astray
  */
void getSynthetic3DPoints( vector< vector<float> >&  pts3D ) {

    pts3D.clear();

    vector<float> p1(3); 	vector<float> p2(3);
    vector<float> p3(3);	vector<float> p4(3);
    p1[0] = 1;  p1[1] = 1;  p1[2] = 0;
    p2[0] = 1;  p2[1] = 20; p2[2] = 0;
    p3[0] = 20; p3[1] = 1;  p3[2] = 0;
    p4[0] = 20; p4[1] = 20; p4[2] = 0;
    pts3D.push_back( p2 );
    pts3D.push_back( p1 );
    pts3D.push_back( p3 );
    pts3D.push_back( p4 );
}

void LaunchNukes()
{
    // allocate polydata objects
    vtkSmartPointer<vtkPolyData> outputMesh = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPolyData> inputMesh  = createSyntheticSurface();

    // enter module: compute curvature
    MeshGeometryCompute::curvature(inputMesh,outputMesh);

    // get some 3D seed points
    std::vector< std::vector<float> >  init_points_3d;
    getSynthetic3DPoints( init_points_3d );

    // outputMesh has curvature too
    inputMesh->DeepCopy( outputMesh );

    // initial levelset and distance maps
    MeshContourEvolver::entry_main( inputMesh, init_points_3d, outputMesh );



    displaySyntheticSurface(outputMesh);

    cout <<"End of Test3"<<endl;
}

int main()
{
    LOUT "Test 5: distance functions from marker points. " LENDL

    LaunchNukes();

}

