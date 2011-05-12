// this file defines a function that communicates
// with the application using the module.

// input function requires a vtkPolyData and
// possible also vtkPoints as initializers.

// It then returns a vtkPolyData.

// the application is responsible for reading or creating
// data files to send as vtkPolyData,
// accepting user input for initial points,
// and handling the output polyData (either to display or
// write to file).

#include "vtkComputeLocalGeometry.h"
#include "vtkInitClosedPath.h"
#include "vtkLevelSetMeshEvolver.h"

#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkSmartPointer.h"
#include "vtkFloatArray.h"
#include "vtkTriangleFilter.h"
#include "callback.h"
#include <vector>


/** @brief    define entry points for 'precomputing geometry everywhere', no contours/segmentation
  */
namespace MeshGeometryCompute {

/** @brief assign callback object that can be attached to algorithm  classes
  */
    // void  set_geometry_callback( vrcl::Callback* cb );

/** @brief   compute geometry info given a mesh in 3D (precomputed from volume data)
  *          plus the original volume, to resolve messed up mesh areas and update output mesh
  */
void entry_main( vtkVolume* vtkRefVol3D, vtkPolyData* inputMesh, vtkPolyData* outputMesh );

/** @brief   compute geometry info given a mesh in 3D (precomputed from volume data)
  */
void curvature(vtkPolyData* inputMesh, vtkPolyData* outputMesh);

}


/** /brief    define entry points for contours/segmentation
  *           if geometry is pre-computed, use it, but always dynamically query neighors for SFLS
  */
namespace MeshContourEvolver {

std::string getContourArrayName( );

std::string getDistanceMapArrayName( );

/** @brief assign callback object that can be attached to algorithm  classes
  */
// void  set_evolver_callback( vrcl::Callback* cb );


/** Input: mesh and indices of vertices for initialization
  * vtkPolyData* entry_main( vtkPolyData* inputMesh, vtkIntArray* initVertIdx );
  */
void path_finder( vtkPolyData* inputMesh, vtkIntArray* initVertIdx,
                                     vtkPolyData* outputMesh );

/** Input: mesh with pre-existing active contour vertices data
  * output: mesh with update active contour vertices float array
  */
void contour_evolver( vtkPolyData* inputMesh, vtkPolyData* outputMesh );


/** Input: mesh with pre-existing active contour vertices data
  *
  */
void region_saver( vtkPolyData* inputMesh, vtkPolyData* outputMesh );


/** Input: mesh and 3D points for initialization. This is what you get
  * when inputting 'fiducials' in Slicer GUI. The 3D points
  * are not on the mesh, you need to first find closest points on the mesh.
  */
void entry_main( vtkPolyData* inputMesh,
            std::vector< std::vector<float> >& seed_points_float3D,
            vtkPolyData* outputMesh );


} // end mesh contour seg namespace







// Deprecated
#if 0
struct InitParam {
  int evolve_its;      // iterations to propagate level set
  int mesh_smooth_its; // iterations to pre-smooth the *Mesh*
  int H_smooth_its;    // iterations to neighbor-average computed surface curvature
  int adj_levels;      // how many 'levels' in neighborhood to fit quadratic form
  int rightHandMesh;   // is N = v1 cross v2 in or out with respect to surface ?
};
#endif
