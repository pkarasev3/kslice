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
#include "vtkCleanPolyData.h"
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
vtkSmartPointer<vtkPolyDataMapper>* 		map_ptr;
vtkSmartPointer<vtkFloatArray>*             colormap_ptr;
vtkSmartPointer<vtkIntArray>*               colormap_int_ptr;

vtkPoints* createSyntheticPoints()
{
   vtkPoints* points = vtkPoints::New();
    float x,y,z;
    int Npts = 5000;

    for(int i=0; i<Npts; i++)
    {
        x = vtkMath::Random(-4.0,4.0);
        y = vtkMath::Random(-4.0,4.0);
        z = std::cos( x*vtkMath::Pi() / 4.0 ) * std::cos( y*vtkMath::Pi() / 4.0 );
        points->InsertNextPoint(x,y,z);
    }

    return points;
}

void SavePolyData( vtkSmartPointer<vtkPolyData> inputMesh, const string& outName )
{
    std::cout<<"Saving vtkPolyData to disk..." << endl;
    vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
    writer->SetInput( inputMesh );
    std::string str_savefile = outName;
    writer->SetFileName( str_savefile.c_str() );
    writer->Write();
    cout << "wrote polydata file with float array attached: " << str_savefile << endl;
}

vtkSmartPointer<vtkPolyData> createSyntheticSurface()
{

    vtkPoints* points_ = createSyntheticPoints();
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

void ComputeCurvatureAndSavePolydataFile(const string& inName, const string& outName )
{

    vtkSmartPointer<vtkPolyData> outputMesh = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPolyData> inputMesh  = vtkSmartPointer<vtkPolyData>::New();
    if( inName.empty() ) {
        inputMesh  = createSyntheticSurface();
    } else {
        vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
        reader->SetFileName( inName.c_str() );
        reader->Update();
        inputMesh = reader->GetOutput( );
    }
    vtkSmartPointer<vtkCleanPolyData> cleaner = vtkSmartPointer<vtkCleanPolyData>::New();
    cleaner->SetInput( inputMesh );
    cleaner->Update();
    inputMesh = cleaner->GetOutput();

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

    SavePolyData( outputMesh, outName );

    cout <<"... done "<<endl;
}

using namespace GetOpt;
int main(int argc, char** argv)
{
  cout << "usage:    ./" << argv[0]
                         << " -i  InputName.vtp (defaults to synthetic) "
                         << " -o   surface_curvature_out.vtp  " << endl;
  cout << "Computing Surface Curvature, no display... " << endl;

  string vtpStrInputFileName = "";
  string vtpStrOutputFileName = "surface_curvature_out.vtp";

  GetOpt_pp ops(argc, argv);

  ops >> Option('o', "output file name (vtp)", vtpStrOutputFileName);
  ops >> Option('i', "input file name (vtp)", vtpStrInputFileName);

  if (!(ops >> Option('i', "input", vtpStrInputFileName))) {
      cerr << "Did not see valid arg 'i', generating synthetic surface... " << endl;
  }
  if (!(ops >> Option('o', "output file name (vtp)", vtpStrOutputFileName))) {
      cerr << "not using output file..." << endl;
  }

  ComputeCurvatureAndSavePolydataFile( vtpStrInputFileName, vtpStrOutputFileName );
}
