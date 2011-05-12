#include "ModuleEntry.h"
#include "Logger.h"
#include "polydata_helper.h"
#include <list>

using namespace vrcl;
using std::vector;

namespace MeshGeometryCompute {

   // static Callback* compute_geometry_callback;
    //void set_geometry_callback( Callback* cb ) {
    //    compute_geometry_callback = cb;
  //  }

  //  Callback* getCallback() {
  //      return compute_geometry_callback;
  //  }

} // end namespace

namespace MeshContourEvolver {

   // static Callback* contour_evolver_callback;
  //  void set_evolver_callback( Callback* cb ) {
  //      contour_evolver_callback = cb;
   // }

  // Callback* getCallback() {
  //       return contour_evolver_callback;
  // }
} // end namespace


// Input: mesh and indices of vertices for initialization
void MeshContourEvolver::path_finder( vtkPolyData* inputMesh, vtkIntArray* initVertIdx,
                                     vtkPolyData* outputMesh )
{
  int output_ref_count = outputMesh->GetReferenceCount(); // debug check that something is referring to obj

  // put through triangle filter: ensure that we have a "polys" field
  vtkSmartPointer<vtkTriangleFilter> triangle_filter = vtkSmartPointer<vtkTriangleFilter>::New();
  triangle_filter->SetInput(inputMesh);
  triangle_filter->Update();

  // algorithm #1: initialization of path
  vtkSmartPointer<vtkInitClosedPath> initial_path_finder = vtkSmartPointer<vtkInitClosedPath>::New();
  initial_path_finder->SetInitPointVertexIdx( initVertIdx ); // set the seed point indices
  initial_path_finder->SetInput( triangle_filter->GetOutput() ); // set vtkPolyData input
  initial_path_finder->Update(); // attach vtkIntArray pointers to polydata as 'labels'

  vtkSmartPointer<vtkPolyData> result = initial_path_finder->GetOutput();
  outputMesh->DeepCopy( result );

  return; //  outputMesh;
}


#if 0
  // algorithm #2: computing the geometry (if the input data
  // does NOT already have a scalar data set containing it)
  vtkSmartPointer<vtkComputeLocalGeometry> computeGeometry = vtkSmartPointer<vtkComputeLocalGeometry>::New();

  // Pass in Initialization Parameters
  computeGeometry->Set_evolve_its(init.evolve_its);
  computeGeometry->Set_mesh_smooth_its(init.mesh_smooth_its);
  computeGeometry->Set_H_smooth_its(init.H_smooth_its);
  computeGeometry->Set_adj_levels(init.adj_levels);
  computeGeometry->SetInputConnection( initPath->GetOutputPort() );
  computeGeometry->Update();

  // algorithm #3: run curve evolution and update the scalar dataset for display
  vtkSmartPointer<vtkLevelSetMeshEvolver> evolver = vtkSmartPointer<vtkLevelSetMeshEvolver>::New();
  MeshOps::MeshData* data = computeGeometry->GetMeshData( );
  evolver->SetMeshData( data );
  std::list<int> L_z;
  std::list<int> L_n1;
  std::list<int> L_p1;
  std::list<int> L_n2;
  std::list<int> L_p2;
  std::vector<int> map;

  computeGeometry->GetLists( L_z, L_p1, L_n1, L_p2, L_n2, map );
  evolver->SetLists( L_z, L_p1, L_n1, L_p2, L_n2, map );

  evolver->SetInputConnection( computeGeometry->GetOutputPort() );
  evolver->Update();

  // register and return the result of the pipeline of algorithms
  vtkSmartPointer<vtkPolyData> result = evolver->GetOutput();
  outputMesh->DeepCopy( result );
#endif



std::string MeshContourEvolver::getContourArrayName( ) {

    return vtkInitClosedPath::CONTOUR_ARRAY_NAME;

}

std::string MeshContourEvolver::getDistanceMapArrayName( ) {

    return vtkInitClosedPath::DISTANCEMAP_ARRAY_NAME;

}

namespace {

    void fillBogusFiducialsForDebug( vector< vector<float> >& initPoints3D ) {
        vector<float> X(3); X[0] = 20; X[1] = 0; X[2] = 0;
        vector<float> Y(3); Y[0] = 0; X[1] = 20; Y[2] = 0;
        vector<float> Z(3); Z[0] = 0; Z[1] = 0; Z[2] = 20;
        initPoints3D.push_back( X );
        initPoints3D.push_back( Y );
        initPoints3D.push_back( Z );
    }


}

void MeshContourEvolver::entry_main( vtkPolyData* inputMesh,
              vector< vector<float> >& initPoints3D, vtkPolyData* outputMesh )
{

  vtkSmartPointer<vtkTriangleFilter> triangle_filter = vtkSmartPointer<vtkTriangleFilter>::New();
  triangle_filter->SetInput(inputMesh);
  triangle_filter->Update();
  vtkSmartPointer<vtkPolyData> tri_result = triangle_filter->GetOutput();

  inputMesh->DeepCopy(tri_result);
  vtkSmartPointer<vtkIntArray> initialPoints = vtkSmartPointer<vtkIntArray>::New();

  size_t iNumSeedPoints = initPoints3D.size();
  Lout() <<"appending "<<iNumSeedPoints<<" points to list. "; LENDL
  if( iNumSeedPoints < 2 ) {
    Lout() << "Error, no fiducial points were read...check your input format"; LENDL;
    fillBogusFiducialsForDebug( initPoints3D );
  }

  /* Find closest vertex to each input 3D point */
  getClosestMeshIndicesToFloatPoints( initPoints3D, inputMesh, initialPoints );

  path_finder( inputMesh, initialPoints, outputMesh );

  return;
}


void MeshGeometryCompute::curvature(vtkPolyData* inputMesh, vtkPolyData* outputMesh)
{
    // triangle filter: ensure the mesh has triangles, not some other polygon type
    vtkSmartPointer<vtkTriangleFilter> triangle_filter =
                                             vtkSmartPointer<vtkTriangleFilter>::New();
    triangle_filter->SetInput(inputMesh);
    triangle_filter->Update();

    vtkSmartPointer<vtkComputeLocalGeometry> computeGeometry =
                            vtkSmartPointer<vtkComputeLocalGeometry>::New();

    computeGeometry->SetInput(inputMesh);
    computeGeometry->SetOutput( outputMesh );

    //Callback* cb = getCallback();
//    if( cb ) {
//      computeGeometry->registerCallback( cb );
//    }

    computeGeometry->Update();

}
