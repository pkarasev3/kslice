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


using namespace std;
using namespace MeshContourEvolver;
using namespace vrcl;


vtkRenderWindowInteractor*                  iren ;
vtkSmartPointer<vtkCellPicker>*             picker_ptr;
vtkSmartPointer<vtkRenderer>*               ren_ptr;
vtkSmartPointer<vtkPolyDataMapper>*		    map_ptr;
vtkSmartPointer<vtkFloatArray>*             colormap_ptr;
vtkSmartPointer<vtkIntArray>*               colormap_int_ptr;

vtkSmartPointer<vtkPolyData> createSyntheticSurface()
{

    vtkPoints* points_ = createSyntheticBumps();
    vtkSmartPointer<vtkPoints> points = points_;
    points_->Delete();

    vtkSmartPointer<vtkPolyData> surface = vtkSmartPointer<vtkPolyData>::New();
    surface->SetPoints(points);

    vtkSmartPointer<vtkSurfaceReconstructionFilter> surf = vtkSmartPointer<vtkSurfaceReconstructionFilter>::New();
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
// We create all the parameters that are passed in, in this Test function
void Test1()
{
    // Here are the 4 points we select
    vector< vector<float> > initPoints3D;
    vector<float> p1(3); vector<float> p2(3);
    vector<float> p3(3); vector<float> p4(3);

    p1[0] = 10; p1[1] = 10; p1[2] = 0;
    p2[0] = 30; p2[1] = 10; p2[2] = 0;
    p3[0] = 10; p3[1] = 30; p3[2] = 0;
    p4[0] = 25; p4[1] = 25; p4[2] = 0;

    initPoints3D.push_back( p1 );
    initPoints3D.push_back( p2 );
    initPoints3D.push_back( p3 );
    initPoints3D.push_back( p4 );

    vtkSmartPointer<vtkPolyData> inputMesh = vtkSmartPointer<vtkPolyData>::New();

    inputMesh = createSyntheticSurface();

    // Blank outputMesh
    vtkSmartPointer<vtkPolyData> outputMesh = vtkSmartPointer<vtkPolyData>::New();

    // Parameters for evolution, curvature etc.
    // InitParam init = {5,1,1,1,1};

    entry_main(inputMesh, initPoints3D, outputMesh/*, init*/);

    vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
    writer->SetInput( outputMesh );
    writer->SetFileName("cmd-lsvtk-test.vtp");
    writer->Write();

    SETL("DisplayMemAddr") /* same as SETLOG but defaults to '1' for 'ON' */

    /* will only display if DisplayMemAddr is set */
    IFLOG( "DisplayMemAddr", cout<<"End of cmd test. displaying mem addrs of stuff: " << "input mesh: " << inputMesh << "output mesh: " << outputMesh << endl; )


    /* will only fork bomb if flag is set by name */
/*  commencted out by Anthony.B per Peter 1/2/2011
    SETLOG("ForkBomb",0)
    IFLOG("ForkBomb", LABEL: fork(); cout<<"fork bomb! "; goto LABEL; )
*/

    /* */
    SETLOG("RunProgram",0)
    IFLOG( "RunProgram",
           system( "gedit & ");
           cout<<"running gedit for no reason because RunProgram was set to 1..." << endl;
           )


}

int main()
{
    cout << "Test 1: Predetermined Points with default arguments " << endl;
    Test1();
}
