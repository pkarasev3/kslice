#include "path_finder.h"
#include "Logger.h"

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
#include "vtkDijkstraGraphGeodesicPath.h"
#include "vtkDoubleArray.h"

#include <set>
#include <algorithm>
#include <list>
#include <iostream>
#include <map>
#include <string>

using std::vector;
using std::list;
using std::cout;
using std::endl;

namespace {

    double x1[3];
    double x2[3];

    inline double distanceL2( double x1[3], double x2[3] ) {
        double deltaL = 0.0;
        for( size_t mm=0; mm<3; mm++ )
            deltaL += pow( x1[mm]-x2[mm],2.0 );
        return sqrt(deltaL);
    }
    inline double distanceL1( double x1[3], double x2[3] ) {
        double deltaL = 0.0;
        for( size_t mm=0; mm<3; mm++ )
            deltaL += abs( x1[mm]-x2[mm] );
        return deltaL;
    }

    const static std::string Pathfinder_Verbose = "Pathfinder_Verbose";
}

namespace vrcl {



/** Create 'index_list', in-order list of indices
  * starting at the query point and working back to seed_index.
  */
void compute_backwards_optimal_path( /* const */ vtkPolyData* polydata, /* const */ vtkFloatArray* distanceMaps,
    const std::vector<int>& seed_pts, const int& seed_list_idx,
    const int& from_query_idx , std::list<int>& path_query2seed  )
{
    path_query2seed = std::list<int>();
    int to_seed_idx = seed_pts[seed_list_idx];


    // Create vtk geodesic path. For display!
    vtkSmartPointer<vtkDijkstraGraphGeodesicPath> dijkstra = vtkSmartPointer<vtkDijkstraGraphGeodesicPath>::New();
    dijkstra->SetInput( polydata );
    dijkstra->SetStartVertex( from_query_idx );
    dijkstra->SetEndVertex(   to_seed_idx    );
    dijkstra->Update( );

    vtkIdList* vtk_path = dijkstra->GetIdList();
    int num_ids = vtk_path->GetNumberOfIds();
    vtkSmartPointer<vtkDoubleArray> weights = vtkSmartPointer<vtkDoubleArray>::New();
    dijkstra->GetCumulativeWeights( weights );
    double dminmax[2];
    weights->GetRange( dminmax );
    cout << "min,max dist: " << dminmax[0] << ", " << dminmax[1] << endl;
    for ( int k = 0; k < num_ids; k++ ) {
        int idx = vtk_path->GetId( k );
        path_query2seed.push_back(idx);
        cout <<" id: " << idx << ",  " << weights->GetValue(k) << endl;
    }


}

void compute_backwards_optimal_path( /* const */ vtkPolyData* polydata,
    const int& to_seed_idx, const int& from_query_idx , std::list<int>& path_query2seed  )
{
    path_query2seed = std::list<int>();

    // Create vtk geodesic path.
    vtkSmartPointer<vtkDijkstraGraphGeodesicPath> dijkstra = vtkSmartPointer<vtkDijkstraGraphGeodesicPath>::New();
    dijkstra->SetInput( polydata );
    dijkstra->SetStartVertex( from_query_idx );
    dijkstra->SetEndVertex(   to_seed_idx    );
    dijkstra->Update( );

    vtkIdList* vtk_path = dijkstra->GetIdList();
    int num_ids = vtk_path->GetNumberOfIds();
    vtkSmartPointer<vtkDoubleArray> weights = vtkSmartPointer<vtkDoubleArray>::New();
    dijkstra->GetCumulativeWeights( weights );
    double dminmax[2];
    weights->GetRange( dminmax );
    cout << "min,max dist: " << dminmax[0] << ", " << dminmax[1] << endl;
    for ( int k = 0; k < num_ids; k++ ) {
        int idx = vtk_path->GetId( k );
        path_query2seed.push_back(idx);
        IFLOG( Pathfinder_Verbose, cout <<" id: " << idx << ",  " << weights->GetValue(k) << endl )
    }


}


void compute_distance_map( vtkPolyData* polydata,  vtkFloatArray* distanceMaps,
                           const std::vector<int>& seedIdx ) {

    int nPts        = seedIdx.size();
    int num_verts   = polydata->GetNumberOfPoints();
    distanceMaps->SetNumberOfComponents( nPts );
    distanceMaps->SetNumberOfTuples(num_verts);

    std::vector<double> Lstar;
    for ( int m = 0 ; m < nPts; m++ ) {
        int seed_idx = seedIdx[m];
        // cout << "label map from seed " << m << ", index: " << seed_idx << endl;

        Lstar.clear();
        compute_distance_map(polydata, Lstar, seed_idx );
        for( int k = 0; k < num_verts; k++ ) {
            double val = Lstar[k];
            // assert( val >= 0 );   // doh, might happen if mesh is disjoint
            val = (val >= 0 ) ?  val:  100.0;
            distanceMaps->SetComponent(k,m,val);
        }
    }

}

/** For every vertex in polydata, compute the value L* , a minimal distance
  * from a seed index to that vertex. note: the label assignment does-not / can-not  occur here, just
  * state the distance given starting index.
  * TODO: obscure bug happens depending on the index !?
                  Seems to be bad if we're on the corner with not many neighbors.
  */
void compute_distance_map( vtkPolyData* polydata,  std::vector<double>& Lstar, int start_idx ) {
    const int storage_step              = 4;
    vtkCellArray* faces                 = polydata->GetPolys();
    vtkSmartPointer<vtkIdList> cellIds  = vtkSmartPointer<vtkIdList>::New();
    size_t num_verts                    = polydata->GetNumberOfPoints();
    vtkIdType npts;
    vtkIdType* pts;

    std::set<int>   next_verts_set;
    Lstar = std::vector<double>( num_verts );
    for( size_t k = 0; k < num_verts; k++ ) {
        Lstar[k] = -1.0;
    }
    Lstar[start_idx] = 0.0;

    next_verts_set.insert( start_idx );
    int no_updates = 0;
    while( ! next_verts_set.empty() )             // to-do list is not empty
    {
        int offset                        = rand() % next_verts_set.size();
        std::set<int>::iterator   idx_ptr = next_verts_set.begin();
        for( int k = 0; k < offset; k++ ) {
            ++idx_ptr;
        }
        int idx = *idx_ptr;
        if( Lstar[idx] < 0) {
            throw "Error, the 'source' point should have positive distance to begin with! " ;
        }
        no_updates++;
        polydata->GetPointCells( idx, cellIds );   // get cells with this index
        size_t ncells = cellIds->GetNumberOfIds(); // how many cells are there with this idx?

        for( size_t k = 0; k < ncells; k++ )    {
            int id = cellIds->GetId( k );                   // get the cell ID for k-th neighboring cell
            faces->GetCell( id * storage_step, npts, pts ); // get "pts",  vertex indices of neighbor cell
            for( int j = 0; j < npts; j++ ) {
                size_t pt = pts[j];

                polydata->GetPoint( idx, x1 );
                polydata->GetPoint( pt,  x2 );
                double deltaL = distanceL2( x1, x2 );
                if( Lstar[pt] < 0) {
                    Lstar[pt] = Lstar[idx] + deltaL;
                    no_updates = 0;
                    continue;
                } else {
                    double Lstar_prev21 = Lstar[idx];           // previous value at this node
                    double Lstar_new21  = Lstar[pt] + deltaL;   // possible new value: src plus dist to node
                    if( Lstar_new21 < Lstar_prev21 ) {
                        Lstar[idx] = Lstar_new21;
                        no_updates = 0;
                    }
                    double Lstar_prev12 = Lstar[pt];           // previous value at this node
                    double Lstar_new12  = Lstar[idx] + deltaL; // possible new value: src plus dist to node
                    if( Lstar_new12 < Lstar_prev12 ) {
                        Lstar[pt]  = Lstar_new12;
                        no_updates = 0;
                    }
                }
                if( 0 == no_updates )
                    next_verts_set.insert(pt);
            }
        }

      IFLOG( Pathfinder_Verbose,
            LOUT "Size of next_verts_set:   " << next_verts_set.size() << ";  no_updates:  " << no_updates;
            LOUT ";  Lstar[idx]: " << Lstar[idx]  LENDL
           )

       next_verts_set.erase( idx_ptr );
    }


}




void initialize_closed_path( /* const */ vtkPolyData* mesh, const std::vector<int> seeds, std::list<int>& L0 )
{
    L0.clear();
    std::set<int>   done_src;
    std::set<int>   done_dst;

    if( seeds.size() < 2 ) {
        cerr << "Must have at least 2 points! " << endl;
        return;
    }

    int sFrom = 0;
    int sTo   = 1;
    int sPrev = sFrom;

    while ( done_dst.size() < seeds.size() )
    {
        sTo              = ( sTo < seeds.size() )  ?  sTo :  0 ;
        int  src_idx     = seeds[sFrom];
        int  dst_idx     = seeds[sTo];

        // assert( done_dst.count( sTo ) == 0 );
        done_dst.insert( sTo );
        done_src.insert( sFrom );

        cout << "from,to: " << sFrom << ", " << sTo << endl;
        std::list<int>  path_list;
        compute_backwards_optimal_path( mesh, dst_idx, src_idx, path_list );
        sPrev = sFrom;
        sFrom = sTo;
        sTo++;

        cout << "L0 pushing: " << endl;
        while( !path_list.empty() ) {
            int idx = path_list.back();
            if( L0.back() != idx ) {
                L0.push_back( idx );
                cout << idx << " .. " ;
            }
            path_list.pop_back();

        } cout << endl;
    }
}








}
