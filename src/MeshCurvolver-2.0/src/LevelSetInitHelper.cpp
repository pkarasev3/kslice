/*
 * LevelSetInitHelper.cpp
 *
 *  Created on: Jun 23, 2010
 *      Author: pkarasev3
 */

#include "LevelSetInitHelper.h"
#include "path_finder.h"
#include "vtkProperty.h"
#include "vtkActor.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "polydata_helper.h"

#include <vector>
#include <algorithm>
#include <set>
using std::vector;
using std::count;
using std::list;
using std::set;


namespace {


void   accumulateLayerNeighbors( vtkPolyData* polydata, const std::set<int>&  layer, std::set<int>&  neighbors ) {

    const int storage_step              = 4;
    vtkCellArray* faces                 = polydata->GetPolys();
    vtkSmartPointer<vtkIdList> cellIds  = vtkSmartPointer<vtkIdList>::New();
    vtkIdType npts;
    vtkIdType* pts;

    neighbors.clear();

    for( set<int>::const_iterator  iter  = layer.begin(); iter != layer.end(); ++iter )
    {
        int idx = *iter;
        polydata->GetPointCells( idx, cellIds );   // get cells with this index
        size_t ncells = cellIds->GetNumberOfIds(); // how many cells are there with this idx?

        for( size_t k = 0; k < ncells; k++ )    {
            int id = cellIds->GetId( k );                   // get the cell ID for k-th neighboring cell
            faces->GetCell( id * storage_step, npts, pts ); // get "pts",  vertex indices of neighbor cell
            for( int j = 0; j < npts; j++ ) {
                size_t pt = pts[j];
                if( 0 == layer.count( pt ) ) // its not in the query layer, so its a neighbor
                    neighbors.insert( pt );
            }
        }
    }

}

void   splitLayerSet( vtkPolyData* polydata, const std::set<int>&  layer, std::set<int>&  layer_one ) {

    vtkSmartPointer<vtkIdList> cellIds  = vtkSmartPointer<vtkIdList>::New();

    int len_before, len_after;
    layer_one.clear();
    layer_one.insert( *(layer.begin()) );

    do {

    len_before = layer_one.size();

    set<int> next, result;
    accumulateLayerNeighbors( polydata, layer_one, next );
    std::set_union( layer_one.begin(), layer_one.end(), next.begin(), next.end(),
                    std::inserter( result, result.end() ) );
    next.clear();
    std::set_intersection( result.begin(), result.end(), layer.begin(), layer.end(),
                    std::inserter( layer_one, layer_one.end() ) );

    len_after = layer_one.size();
    cout << "before,after:  " << len_before << ",  " << len_after << endl;
    } while( len_after > len_before );

    cout << "layer_one: " << layer_one.size() << endl;

}




} // end anonymous space


namespace vrcl {


LayerDisplayData::LayerDisplayData( ) {
    cr = 1.0;
    cg = 1.0;
    cb = 1.0;
    strLayerName = "";
    polydata = vtkSmartPointer<vtkPolyData>::New();
    mapper   = vtkSmartPointer<vtkPolyDataMapper>::New();
    actor    = vtkSmartPointer<vtkActor>::New();
}

void LayerDisplayData::Connect( const std::list<int>& L, vtkPolyData* mesh_in ) {
    polydata = VertexList2PolyLine( L, mesh_in);
    mapper->SetInput( polydata );
    actor->SetMapper( mapper );
    actor->GetProperty()->SetColor(cr,cg,cb);
    actor->GetProperty()->SetLineWidth(4);
}

void LayerDisplayData::Connect( const std::set<int>& L, vtkPolyData* mesh_in ) {
    polydata = VertexSet2PointCloud( L, mesh_in);
    mapper->SetInput( polydata );
    actor->SetMapper( mapper );
    actor->GetProperty()->SetColor(cr,cg,cb);
    actor->GetProperty()->SetPointSize(10.0);
}

LevelsetLayerActors::LevelsetLayerActors( )
{
    phi   = vtkSmartPointer<vtkDoubleArray>::New();
    label = vtkSmartPointer<vtkIntArray>::New();
}


void  LevelsetLayerActors::createFromIndexSets( vtkPolyData* mesh, const std::list<int> L0_,
                       std::set<int>& Lp1_, std::set<int>& Ln1_, std::set<int>&  Lp2_,
                       std::set<int>&   Ln2_, const std::set<int>&  Lz_ ) {
    Lz    = L0_  ;
    L0    = Lz_  ;
    Lp1   = Lp1_ ;
    Ln1   = Ln1_ ;
    Lp2   = Lp2_ ;
    Ln2   = Ln2_ ;
    mesh_handle = mesh;

    LZ_curve.cb = 0.5;
    LZ_curve.Connect(Lz,mesh);

    L_0_pts.cb = 0.5;
    L_0_pts.Connect(L0,mesh);

    Lp1_pts.cg = 0.5;
    Lp1_pts.Connect(Lp1,mesh);

    Ln1_pts.cr = 0.5;
    Ln1_pts.Connect(Ln1,mesh);

    Lp2_pts.cg = 0.5;
    Lp2_pts.cr = 0.5;
    Lp2_pts.Connect(Lp2,mesh);

    Ln1_pts.cr = 0.5;
    Ln1_pts.cb = 0.5;
    Ln2_pts.Connect(Ln2,mesh);


}


void LevelsetLayerActors::attachToRenderer(vtkRenderer *ren)
{
    ren->AddActor(LZ_curve.actor);
    ren->AddActor(L_0_pts.actor);
    ren->AddActor(Lp1_pts.actor);
    ren->AddActor(Ln1_pts.actor);
    ren->AddActor(Lp2_pts.actor);
    ren->AddActor(Ln2_pts.actor);

    this->ren_handle = ren;
}

/** evolve for one iteration, given a map of vertex indices to mesh indices.
  * presumably, at least the zero level has something to map to!
  */
void LevelsetLayerActors::evolve_sfls( const std::map<int,double>& force_per_vertex )
{
    // copy for now; TODO: fix the constness issue
    std::map<int,double>  F = force_per_vertex;

    // wipe the temporary storage
    S0.clear(); Sn1.clear(); Sp1.clear(); Sn2.clear(); Sp2.clear();

    // update phi along L0
    double* phi_ptr = phi->GetPointer(0);
    double  cfl_val = 0.3;
    for( std::set<int>::const_iterator  iter =  L0.begin(); iter != L0.end(); ++iter ) {
        int idx         = *iter;
        double val      = F[idx];
        phi_ptr[idx]    += cfl_val * val;

    }


    //    % Update 0 LS
    //      Sn1 = Lz( ( phi( Lz ) <= -0.5 ) );
    //      Sp1 = Lz( ( phi( Lz ) >= 0.5 ) );
    //      Lz( phi(Lz)<= -0.5 ) = [];
    //      Lz( phi(Lz)>= 0.5 ) = [];

}




void   initLevelSetLists( vtkPolyData* mesh, const std::list<int>& L0,
        set<int>& Lp1, set<int>& Ln1, set<int>&  Lp2, set<int>&   Ln2, set<int>&  Lz )
{

    Lz  = set<int>();
    Ln1 = set<int>();
    Lp1 = set<int>();
    Ln2 = set<int>();
    Lp2 = set<int>();

    // 1. move L0 list to set Lz
    for( std::list<int>::const_iterator  iter = L0.begin();  iter != L0.end();  ++iter ) {
        int idx = *iter;
        Lz.insert(idx);
    }


    // 2. accumulate set of neighbors of L0
    std::set<int> s1, s2, s3;
    accumulateLayerNeighbors( mesh, Lz,  s1 ); // 1-neighbors of L0: Lp1,Ln1
    accumulateLayerNeighbors( mesh, s1,  s2 ); // Lp2,Lz,Ln2

    // Fill in s1 and s2 with values
    std::set<int> result; // s1 - s2 set difference
    std::set_difference(s2.begin(), s2.end(), Lz.begin(), Lz.end(),
        std::inserter(result, result.end()));  // Lp2 and Ln2

    splitLayerSet( mesh, result, Lp2 );
    std::set_difference(result.begin(), result.end(), Lp2.begin(), Lp2.end(),
        std::inserter(Ln2, Ln2.end()));  // Lp2 and Ln2

    accumulateLayerNeighbors( mesh, Lp2,  s3 ); // neighbors of Lp2
    std::set_intersection( s3.begin(), s3.end(), s1.begin(), s1.end(),
                    std::inserter( Lp1, Lp1.end() ) );

    accumulateLayerNeighbors( mesh, Ln2,  s3 ); // neighbors of Ln2
    std::set_intersection( s3.begin(), s3.end(), s1.begin(), s1.end(),
                    std::inserter( Ln1, Ln1.end() ) );
    cout << "size of Ln2, Ln1, Lp1, Lp2: " << endl;
    cout << Ln2.size() << endl;
    cout << Ln1.size() << endl;
    cout << Lp1.size() << endl;
    cout << Lp2.size() << endl;

    // 3. assign to in/out layers


}

namespace {
    void fillIntArrayIndicesWithValue( vtkIntArray* array, const set<int>& L, int value ) {
        for( set<int>::const_iterator iter = L.begin();  iter != L.end();  ++iter ) {
            int idx = *(iter);
            array->SetValue( idx, value );
        }
    }
}


vtkSmartPointer<vtkIntArray>  initLevelSetLists(  vtkPolyData* mesh, const vector<int>& seedIdx, list<int>& L0_input,
        set<int>& Lp1, set<int>& Ln1, set<int>&  Lp2, set<int>&   Ln2, set<int>&  Lz)
{

    initialize_closed_path( mesh, seedIdx, L0_input );

    Lp1.clear(); Ln1.clear(); Lp2.clear(); Ln2.clear(); Lz.clear();
    initLevelSetLists(mesh, L0_input,Lp1,Ln1,Lp2,Ln2,Lz );

    vtkSmartPointer<vtkIntArray>  activeContourVertIdx = vtkSmartPointer<vtkIntArray>::New();
    int numVerts = mesh->GetNumberOfPoints();
    activeContourVertIdx->SetNumberOfTuples( numVerts );

    for( int k = 0; k < numVerts; k++ ) { // initial fill
        activeContourVertIdx->SetValue(k, 3.0);
    }
    fillIntArrayIndicesWithValue( activeContourVertIdx, Lz,   0 );
    fillIntArrayIndicesWithValue( activeContourVertIdx, Lp1,  1 );
    fillIntArrayIndicesWithValue( activeContourVertIdx, Ln2, -1 );
    fillIntArrayIndicesWithValue( activeContourVertIdx, Lp2,  2);
    fillIntArrayIndicesWithValue( activeContourVertIdx, Ln2, -2 );

    set<int>  outside_Ln2, Ln3;
    accumulateLayerNeighbors( mesh, Ln2, outside_Ln2 );
    std::set_difference(outside_Ln2.begin(), outside_Ln2.end(), Ln1.begin(), Ln1.end(),
        std::inserter(Ln3, Ln3.end()));  // Ln3 = Ln2_neighbs - Ln1
    fillIntArrayIndicesWithValue( activeContourVertIdx, Ln3, -3 );

    return activeContourVertIdx;
}

void GetLayerIndexSets( vtkIntArray* activeContourVertIdx, std::set<int>& Lz, std::set<int>& Lp1, std::set<int>& Ln1,
                        std::set<int>& Ln2, std::set<int>& Lp2 )
{
    int nverts = activeContourVertIdx->GetNumberOfTuples();
    for( int k = 0; k < nverts; k++ ) {
        if( activeContourVertIdx->GetValue(k) == 0 ) {
            Lz.insert(k);
        }
        else if( activeContourVertIdx->GetValue(k) == 1 ) {
            Lp1.insert(k);
        }
        else if( activeContourVertIdx->GetValue(k) ==  -1 ) {
            Ln1.insert(k);
        }
        else if( activeContourVertIdx->GetValue(k) ==  2 ) {
            Lp2.insert(k);
        }
        else if( activeContourVertIdx->GetValue(k) == -2 ) {
            Ln2.insert(k);
        }
    }


}


}
