#include "vtkSurfaceReconstructionFilter.h"
#include "vtkProgrammableSource.h"
#include "vtkContourFilter.h"
#include "vtkReverseSense.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkPolyData.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkCellArray.h"
#include <vtkMath.h>
#include <cmath>
#include <valarray>
#include <list>
#include <vector>

using std::valarray;
using std::vector;
using std::string;
using std::list;

vtkPoints* readPoints();
vtkPoints* loadPoints(const string& fileface,
                      const string& filevert);

int main(int argc, char **argv)
{
    vtkPoints*   points;
    vtkPolyData* polydata;
    // Read some points
    if( 1 == argc ) {
        points = readPoints();
    }
    #if 0
    else {
        string fileface = argv[1]; // NOT USED
        string filevert = argv[2];
        cout<<"using vertices "<<filevert<<"\n";
        points = loadPoints( fileface, filevert);
    }
    #endif

    polydata = vtkPolyData::New();
    polydata->SetPoints(points);

    // Construct the surface and create isosurface.
    vtkSurfaceReconstructionFilter* surf = vtkSurfaceReconstructionFilter::New();
    surf->SetInput(polydata);

    vtkContourFilter* cf = vtkContourFilter::New();
    cf->SetInputConnection(surf->GetOutputPort());
    cf->SetValue(0, 0.0);

    // Sometimes the contouring algorithm can create a volume whose gradient
    // vector and ordering of polygon (using the right hand rule) are
    // inconsistent. vtkReverseSense cures this problem.
    vtkReverseSense* reverse = vtkReverseSense::New();
    reverse->SetInputConnection(cf->GetOutputPort());
    reverse->ReverseCellsOn();
    reverse->ReverseNormalsOn();

    vtkPolyDataMapper* map = vtkPolyDataMapper::New();
    map->SetInputConnection(reverse->GetOutputPort());
    map->ScalarVisibilityOff();

    vtkActor* surfaceActor = vtkActor::New();
    surfaceActor->SetMapper(map);
    surfaceActor->GetProperty()->SetDiffuseColor(1.0000, 0.3882, 0.2784);
    surfaceActor->GetProperty()->SetSpecularColor(1, 1, 1);
    surfaceActor->GetProperty()->SetSpecular(.4);
    surfaceActor->GetProperty()->SetSpecularPower(50);

    // Create the RenderWindow, Renderer and both Actors
    vtkRenderer* ren = vtkRenderer::New();
    vtkRenderWindow* renWin = vtkRenderWindow::New();
    renWin->AddRenderer(ren);
    vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
    iren->SetRenderWindow(renWin);

    // Add the actors to the renderer, set the background and size
    ren->AddActor(surfaceActor);
    ren->SetBackground(1, 1, 1);
    renWin->SetSize(400, 400);
    ren->GetActiveCamera()->SetFocalPoint(0, 0, 0);
    ren->GetActiveCamera()->SetPosition(1, 0, 0);
    ren->GetActiveCamera()->SetViewUp(0, 0, 1);
    ren->ResetCamera();
    ren->GetActiveCamera()->Azimuth(20);
    ren->GetActiveCamera()->Elevation(30);
    ren->GetActiveCamera()->Dolly(1.2);
    ren->ResetCameraClippingRange();

    iren->Initialize();
    renWin->Render();
    iren->Start();

    return 0;

}

#if 0
vtkPoints* loadPoints( const string& fileface,
                      const string& filevert)
{
    vtkPoints* points2 = vtkPoints::New();
    valarray<double> VertX;
    valarray<double> VertY;
    valarray<double> VertZ;
    valarray<int> Idx;

    vector<string> files(2);
    files[0] = fileface;
    files[1] = filevert;
    cout<<"...opening files...\n";
    ProcessTriDataFiles( files, Idx, VertX, VertY, VertZ );
    int VertCount = VertX.size();
    for( int i = 0; i < VertCount; i++ ) {
        points2->InsertPoint(i, VertX[i], VertY[i],
            VertZ[i]);
    }
    return points2;
}
#endif

vtkPoints* readPoints()
{
    vtkPoints* points = vtkPoints::New();

    float x, y, z;
    // generate random points on unit sphere
    int Npts = 1000;
    double PI = 3.14159265;
    for(int i=0; i<Npts; i++) {
        double u,v;
        u = vtkMath::Random(0,2*PI);
        v = vtkMath::Random(0,2*PI);

        double R = 2;
        double r = 1;

        x = (R + r*std::cos(v))*std::cos(u);
        y = (R + r*std::cos(v))*std::sin(u);
        z = r * std::sin(v);

        points->InsertNextPoint(x, y, z);
    }
    return points;
}
