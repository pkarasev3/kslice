// Chudy Note: This is almost an exact replica of the test 2 file, the difference being in that
// instead of just the synthetic mesh being drawn with curvature data, we are going to try and
// also include glyph drawings inside the same function. If this actually works I will probably
// attempt to minimize the size of this file and use old functions located inside other files
// to create the renderer / window / mesh.

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

using namespace vrcl;
using std::valarray;
using std::vector;
using std::string;
using std::list;

// variables that get used in callbacks
vtkRenderWindowInteractor*                  iren ;
vtkSmartPointer<vtkCellPicker>*             picker_ptr;
vtkSmartPointer<vtkRenderer>*               ren_ptr;
vtkSmartPointer<vtkPolyDataMapper>*		    map_ptr;
vtkSmartPointer<vtkFloatArray>*             colormap_ptr;
vtkSmartPointer<vtkIntArray>*               colormap_int_ptr;
// DebugCallback*                              cb;

vtkPoints* createSyntheticBumps()
{
   vtkPoints* points = vtkPoints::New();
    float x,y,z;
    int Npts = 15000;

    for(int i=0; i<Npts; i++)
    {
        x = vtkMath::Random(-6.0,6.0);
        y = vtkMath::Random(-6.0,6.0);
        z = std::cos( x*vtkMath::Pi() / 4.0 ) * std::cos( y*vtkMath::Pi() / 4.0 );
        points->InsertNextPoint(x,y,z);
    }

    return points;
}

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
    //delete cb;
}

void GlyphTest()
{

    vtkSmartPointer<vtkPolyData> outputMesh = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPolyData> inputMesh  = createSyntheticSurface();

    //MeshGeometryCompute::set_geometry_callback( cb );
    MeshGeometryCompute::curvature(inputMesh,outputMesh);

    int n_arrays = outputMesh->GetPointData()->GetNumberOfArrays();
    vtkPointData*  point_data = outputMesh->GetPointData();
    int k = 0;
    Lout() << "printing names and min/max of named arrays in 'outputmesh'  " << endl;
    // CHUDY: one of these arrays is called "surface normals" or something like that with
    // 3 components, nx,ny,nz : use that to make test glyphs, with 'size' of glyph proportional
    // to the mean curvature at that vertex

    // Also: if you want, move some of the repetetive functions to draw / print stuff
    // to some kind of 'test helper' file. You may have to add a 'mini-library' that all the
    // tests link to.
  //  TARGET_LINK_LIBRARIES ( test1    TestHelpers  MeshContourEvolver ${OUR_VTK_REQUIRED_LIBS} )
  //  ADD_LIBRARY( TestHelpers STATIC   TestFile1.cxx TestFile1.h ) where the various functions are in these header/source respectively

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

    cout <<"End of GlyphTest"<<endl;
}

int main()
{


  cout << "GlyphTest: Curvature on Trivial Data Set " << endl;
  // cb = new DebugCallback();
  GlyphTest();

  //delete cb;

  int wtf_No_Glyphs = -1;  // TODO: return 0 when there are nice vtkGlyph arrows
  return wtf_No_Glyphs;
}
