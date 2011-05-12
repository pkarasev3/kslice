#ifndef VRCL_POLYDATA_HELPER_H
#define VRCL_POLYDATA_HELPER_H

#include "vtkPolyData.h"
#include "vtkFloatArray.h"
#include "vtkIntArray.h"
#include "vtkSmartPointer.h"
#include <vector>
#include <set>
#include <list>

namespace vrcl
{

 vtkSmartPointer<vtkPolyData> VertexSet2PointCloud( const std::set<int>&  idx, vtkPolyData* polydata_in );

 vtkSmartPointer<vtkPolyData> VertexList2PolyLine( const std::list<int>&  idx, vtkPolyData* polydata_in );

/** \brief  display the names and some info (min,max,etc) about the
  * polydata. used for verification of computations that attach data to the mesh
  * (e.g. curvature).
  */
void PrintPolydataArrayInfo( vtkPolyData* outputMesh );


/** \brief  given list of 3D points, get the indices closest to those points
  * for vertices on the mesh.
  */
void getClosestMeshIndicesToFloatPoints( const std::vector<std::vector<float> >& initPoints3D,
                           vtkPolyData* inputMesh, vtkIntArray* initialPoints );


}


#endif
