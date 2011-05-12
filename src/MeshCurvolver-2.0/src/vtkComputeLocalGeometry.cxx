#include "vtkComputeLocalGeometry.h"
#include "MeshOps.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkCellLocator.h"
#include "vtkFloatArray.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkPolygon.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkTriangleFilter.h"
#include "vtkSmartPointer.h"
#include "LevelSetInitHelper.h"
#include "Logger.h"

#include <valarray>
#include <vector>
#include <list>

using namespace vrcl;

namespace { // secret functions

void Debug_Display( vtkIntArray* activeContourVertIdx,
                          const std::vector<int>& Lstar)
{
      int numVerts = Lstar.size();
      for( int i = 0; i < numVerts; i++ ) {
        int val = ( Lstar[i] );
        activeContourVertIdx->SetTupleValue(i, &val);
      }
}

}

vtkCxxRevisionMacro(vtkComputeLocalGeometry, "$Revision: 1.00 $");
vtkStandardNewMacro(vtkComputeLocalGeometry);

vtkComputeLocalGeometry::vtkComputeLocalGeometry() {
    this->H_smooth_its = 50;
    this->adj_levels   = 1;
}

void vtkComputeLocalGeometry::SetSource(vtkPolyData *source) {
  this->SetInput(1, source);
}

vtkPolyData *vtkComputeLocalGeometry::GetSource() {
  if (this->GetNumberOfInputConnections(1) < 1) {
    return NULL;
  }
  return vtkPolyData::SafeDownCast(
    this->GetExecutive()->GetInputData(1, 0));
}

// Where the bulk of work is done!
int vtkComputeLocalGeometry::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector) {

  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // update progress bar at some intervals
  this->UpdateProgress(0.15);

    // get the input and ouptut
  vtkPolyData *input = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  // Check input
  size_t numVerts=input->GetNumberOfPoints();
  size_t numCells=input->GetNumberOfCells();
  if (numVerts < 1 || numCells < 1)
  {
    vtkErrorMacro(<<"No data to smooth!")
    return 1;
  }
  output->CopyStructure(input);
  output->GetPointData()->PassData(input->GetPointData());
  output->GetCellData()->PassData(input->GetCellData());
  vtkDebugMacro(<<"Analyzing ____ ...")

  // compute the mean curvature using K-neighborhood paraboloid-fitting
     myMeshData.polydata    = output;
     myMeshData.smoothH_its = H_smooth_its;
     myMeshData.adj_levels  = adj_levels;
     myMeshData.ComputeCurvatureData( ); // Takes a while to compute neighbor info...

// A: surface H
    vtkSmartPointer<vtkFloatArray> mean_curvature = vtkSmartPointer<vtkFloatArray>::New();
    mean_curvature->SetNumberOfComponents(1);
    mean_curvature->SetNumberOfTuples( numVerts );
    mean_curvature->SetName( "mean_curvature" );
    for( ::size_t i = 0; i < numVerts; i++ ) {
      double val = myMeshData.MeanCurv[i];
      mean_curvature->SetTuple1(i, val );
    }
    output->GetPointData()->AddArray( mean_curvature );

// B: dH/dx_ij
    vtkSmartPointer<vtkFloatArray> mean_curv_gradient = vtkSmartPointer<vtkFloatArray>::New();
    mean_curv_gradient->SetNumberOfComponents(2);
    mean_curv_gradient->SetNumberOfTuples( numVerts );
    mean_curv_gradient->SetName( "mean_curv_gradient" );
    for( ::size_t i = 0; i < numVerts; i++ ) {
      double val1 = myMeshData.dkde1[i];
      double val2 = myMeshData.dkde2[i];
      mean_curv_gradient->SetTuple2(i, val1, val2 );
    }
    output->GetPointData()->AddArray( mean_curv_gradient );

// C: surface normal vector N
    vtkSmartPointer<vtkFloatArray> surface_normal = vtkSmartPointer<vtkFloatArray>::New();
    surface_normal->SetNumberOfComponents(3);
    surface_normal->SetNumberOfTuples( numVerts );
    surface_normal->SetName( "surface_normal" );
    for( ::size_t i = 0; i < numVerts; i++ ) {
      double val1 = myMeshData.nx[i];
      double val2 = myMeshData.ny[i];
      double val3 = myMeshData.nz[i];
      surface_normal->SetTuple3(i, val1, val2, val3 );
    }
    output->GetPointData()->AddArray( surface_normal );

  // update progress bar at some intervals
  this->UpdateProgress(1.0);
  return 1;
 }

int vtkComputeLocalGeometry::FillInputPortInformation(int port,
                                                      vtkInformation *info)
{
  if (!this->Superclass::FillInputPortInformation(port, info))    {
    return 0;
    }

  if (port == 1)    {
    info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
    }
  return 1;
}

void vtkComputeLocalGeometry::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Truth: " << 42 << "\n";
  if ( this->GetSource() )    {
      os << indent << "Source: " << static_cast<void *>(this->GetSource()) << "\n";
    }  else    {
    os << indent << "Source (none)\n";
  }
}
