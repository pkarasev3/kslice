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
vtkSmartPointer<vtkPolyDataMapper>* 		map_ptr;
vtkSmartPointer<vtkFloatArray>*             colormap_ptr;
vtkSmartPointer<vtkIntArray>*               colormap_int_ptr;

vtkSmartPointer<vtkPolyData> createSyntheticSurface()
{

    vtkPoints* points_ = createSyntheticBumps();
    vtkSmartPointer<vtkPoints> points = points_;
    points_->Delete();

    vtkSmartPointer<vtkPolyData> surface = vtkSmartPointer<vtkPolyData>::New();
    surface->SetPoints(points);

    vtkSmartPointer<vtkSurfaceReconstructionFilter> surf =
             vtkSmartPointer<vtkSurfaceReconstructionFilter>::New();
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
    vtkFloatArray* colormap =
                  vtkFloatArray::SafeDownCast( mesh->GetPointData()->GetArray("mean_curvature") );

    double ptx[2];

    colormap->GetRange( ptx );
    double dminx = ptx[0]; double dmaxx = ptx[1];

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

void Test2()
{

    vtkSmartPointer<vtkPolyData> outputMesh = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPolyData> inputMesh  = createSyntheticSurface();

    MeshGeometryCompute::curvature(inputMesh,outputMesh);

    int n_arrays = outputMesh->GetPointData()->GetNumberOfArrays();
    vtkPointData*  point_data = outputMesh->GetPointData();
    int k = 0;
    Lout() << "printing names and min/max of named arrays in 'outputmesh'  " << endl;
    while( k < n_arrays ) {
        vtkFloatArray* farray_k =  vtkFloatArray::SafeDownCast( point_data->GetArray( k ) );
        if(  farray_k->GetName() ) {
          string k_name( farray_k->GetName() );
          Lout() << k_name << ", " ;
          double ptx[2];
          farray_k->GetRange(ptx);
          Lout() << ptx[0] << "," << ptx[1] << "," ;
        }
        k++;
    }
    LENDL;
    displaySyntheticSurface(outputMesh);

    cout <<"End of Test2"<<endl;
}

int main()
{
  cout << "Test 2: Curvature on Trivial Data Set " << endl;

  Test2();
}
