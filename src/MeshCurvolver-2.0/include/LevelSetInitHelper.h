
#ifndef LEVELSETINITHELPER_H_
#define LEVELSETINITHELPER_H_

#include "vtkSmartPointer.h"
#include "vtkFloatArray.h"
#include "vtkIntArray.h"
#include <list>
#include <set>
#include <map>

#include "MeshOps.h"

namespace vrcl
{

void GetLayerIndexSets( vtkIntArray* array, std::set<int>& L0, std::set<int>& Lp1, std::set<int>& Ln1,
                        std::set<int>& Ln2, std::set<int>& Lp2 ) ;

void   initLevelSetLists( vtkPolyData* mesh, const std::list<int>& L0,
                          std::set<int>& Lp1, std::set<int>& Ln1, std::set<int>&  Lp2,
                          std::set<int>&   Ln2, std::set<int>&  Lz );

/** \brief initialize the 'layers' for sparse field level set algorithm
 */
vtkSmartPointer<vtkIntArray>  initLevelSetLists(  vtkPolyData* mesh, const std::vector<int>& seedIdx, std::list<int>& L0_input,
        std::set<int>& Lp1, std::set<int>& Ln1, std::set<int>&  Lp2, std::set<int>&   Ln2, std::set<int>&  Lz);



struct  LayerDisplayData {

public:
    vtkSmartPointer<vtkPolyData>  polydata;
    vtkSmartPointer<vtkPolyDataMapper> mapper;
    vtkSmartPointer<vtkActor> actor;
    std::string               strLayerName;

    float cr,cg,cb;

    LayerDisplayData( );
    ~LayerDisplayData( ) { }

    void Connect( const std::list<int>& L, vtkPolyData* mesh_in ) ;

    void Connect( const std::set<int>& L, vtkPolyData* mesh_in ) ;

};

struct  LevelsetLayerActors {

public:

    LevelsetLayerActors() ;
    ~LevelsetLayerActors() { }

    LayerDisplayData LZ_curve;
    LayerDisplayData L_0_pts;
    LayerDisplayData Lp1_pts;
    LayerDisplayData Lp2_pts;
    LayerDisplayData Ln1_pts;
    LayerDisplayData Ln2_pts;

    // persistent layers
    std::set<int>   L0, Lp1, Lp2, Ln1, Ln2;
    std::list<int>  Lz;

    // temporary containers
    std::set<int> S0, Sn1, Sn2, Sp1, Sp2;

    // 'levelset function'
    vtkSmartPointer<vtkDoubleArray> phi;

    // label of vertices, e.g. {3,2,1,0,-1,-2,-3}
    vtkSmartPointer<vtkIntArray> label;

    // does not own these, volatile pointers!
    vtkRenderer* ren_handle;
    vtkPolyData* mesh_handle;

    /** initialize actor, polydata, points, mapper, from index lists into "mesh"
      */
    void createFromIndexSets( /*const*/ vtkPolyData* mesh, const std::list<int> L0,
                           std::set<int>& Lp1, std::set<int>& Ln1, std::set<int>&  Lp2,
                           std::set<int>&   Ln2, const std::set<int>&  Lz );

    /** after creation, attach the actors to a renderer
      */
    void attachToRenderer( /* NOT const*/ vtkRenderer* ren );

    /** evolve for one iteration, given a map of vertex indices to "Force" values.
      * presumably, at least the zero level has something to map to!
      * force is computed somewhere else (for flexibility)
      */
    void evolve_sfls( const std::map<int,double>& force_per_vertex );

};






};




#endif /* LEVELSETINITHELPER_H_ */
