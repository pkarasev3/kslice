#ifndef VRCL_PATHFINDER_H
#define VRCL_PATHFINDER_H

#include "vtkPolyData.h"
#include "vtkFloatArray.h"
#include <vector>
#include <list>

namespace vrcl {

/** \brief compute distance map to each vertex at most N steps away from a single vertex
  */
void compute_distance_map( vtkPolyData* polydata,
                          std::vector<double>& Lstar, int idx );

/** \brief compute distance maps to each vertex from a list of indices
  * Updates the distanceMap, does NOT change polydata. The caller of
  * this function should give distanceMaps a string name and attach it to polydata,
  * if desired.
  */
void compute_distance_map( /* const */ vtkPolyData* polydata, /* const */  vtkFloatArray* distanceMaps,
      const std::vector<int>& seedIdx );


/** \brief compute least-distance path from some query vertex to a seed once
  * we have the distance maps pre-computed. Creates 'index_list', in-order list of indices
  * starting at the query point and working back to seed_index.
  */
void compute_backwards_optimal_path( /* const */ vtkPolyData* polydata, /* const */ vtkFloatArray* distanceMaps,
    const std::vector<int>& seed_pts, const int& seed_list_idx,
    const int& from_query_idx , std::list<int>& path_query2seed  );

/** get a list of indices using the vtkDijkstraPath class. Suboptimal in length but tends to get
  * smoother path than above. Doesn't need the distance maps. Fills in path_query2seed list.
  */
void compute_backwards_optimal_path( /* const */ vtkPolyData* polydata,
    const int& to_destination_idx, const int& from_query_idx , std::list<int>& path_query2seed  );


/**  If the seeds are given in order, form the L0 in the traversal order.
  */
void initialize_closed_path( /* const */ vtkPolyData* mesh, const std::vector<int> & seeds, std::list<int>& L0 );




}



#endif
