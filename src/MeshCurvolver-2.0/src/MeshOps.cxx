/*=========================================================================

  Program:   SparseFieldLevelSetContour
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Applications/CLI/SparseFieldLevelSetContour/MeshOps.cxx $
  Language:  C++
  Date:      $Date: 2010-04-30 16:13:05 -0400 (Fri, 30 Apr 2010) $
  Version:   $Revision: 13140 $

  Copyright (c) Brigham and Women's Hospital (BWH) All Rights Reserved.

  See License.txt or http://www.slicer.org/copyright/copyright.txt for details.

==========================================================================*/
#include "MeshOps.h"
#include "vtkSmartPointer.h"
#include "Logger.h"

using namespace vrcl;
using std::vector;
using std::list;
using std::map;
using std::valarray;
using std::string;

namespace MeshOps {

// TODO: What is the point of this function ???
int CountVertsOnMesh( vtkPolyData* poly ) {
  int num = 0;
  vector<int> idx(0);
  vtkCellArray* faces = poly->GetPolys();
  vtkPoints* verts = poly->GetPoints();
  vector<bool> alreadyFound( verts->GetNumberOfPoints() );
  faces->SetTraversalLocation(0);
  for( int i = 0; i < faces->GetNumberOfCells(); i++ )
    {
    vtkIdType numpts = 0;
    vtkIdType* ptsIds = NULL;
    //faces->GetCell(i, numpts, ptsIds );
    faces->GetNextCell(numpts, ptsIds );
    for( ::size_t k = 0; k < numpts; k++ )
      {
      int pt = ptsIds[k];
      if( alreadyFound[pt] )
        {
        continue;
        }
      alreadyFound[pt] = true;
      num++;
      }
    }

  return num;
}


void MeshData::ComputeCurvatureData( )
{
  vtkPoints*    verts = this->polydata->GetPoints();
  int numverts = verts->GetNumberOfPoints();

  if (numverts < 1)    {
      std::cerr << "ComputeCurvatureData: invalid number of vertices found in the polydata: " << numverts << std::endl;
    return;
   }  else    {
    std::cout << "ComputeCurvatureData: allocating arrays to be of size " << numverts << std::endl;
  }
  this->MeanCurv.resize(numverts);
  this->dkde2.resize(numverts);
  this->dkde1.resize(numverts);
  this->dkmag.resize(numverts);
  this->nx.resize(numverts);
  this->ny.resize(numverts);
  this->nz.resize(numverts);

  this->adj = vector<AdjData>( numverts );
  this->adjimm = vector<AdjData>( numverts );

  ComputeAdjacency(   );
  ComputeNormals(   );
  ComputeCurvature(   );
  SmoothCurvature(   );
  ComputeGradCurvatureTangentPlane(   );

  // Store the data arrays, get read into vtkFloatArrays later on!
  // std::map< const std::string&, std::valarray<double>* > namedMeshArrays;

  namedMeshArrays.clear();
  namedMeshArrays["mean_curvature"] = MeanCurv;
  namedMeshArrays["dkde1"]          = dkde1;
  namedMeshArrays["dkde2"]          = dkde2;
  namedMeshArrays["dkmag"]          = dkmag;
  namedMeshArrays["nx"]             =    nx;
  namedMeshArrays["ny"]             =    ny;
  namedMeshArrays["nz"]             =    nz;

  double dmax = MeanCurv.max();
  double dmin = MeanCurv.min();
  Lout() << "dmax: " << dmax << "  dmin: " << dmin ;
  LENDL
  double breakhere = 1;
}

void MeshData::SmoothCurvature( )
{
  if ( this->MeanCurv.size() == 0)    {
    std::cerr << "SmoothCurvature: No mean curvature computed on mesh " << std::endl;
    return;
    }
  if ( this->adjimm.size() == 0)
    {
    std::cerr << "SmoothCurvature: No mean adjacency computed on mesh " << std::endl;
    return;
    }

  std::cout<<"Smoothing curvature...\n";
  int iterations = this->smoothH_its;
  vtkPoints*    verts = this->polydata->GetPoints();
  int numverts = verts->GetNumberOfPoints();
  valarray<double> tempK = this->MeanCurv;
  for( int i = 0; i < iterations; i++ )
    {
    for( int k = 0; k < numverts; k++ )
      {
      double sumK = 0.0;
      ::size_t numneigh = this->adjimm[k].myNeighbs.size();
      for( ::size_t j = 0; j < numneigh; j++ )
        {
        int idx = this->adjimm[k].myNeighbs[j];
        sumK += this->MeanCurv[idx];
        }
      if( 0 == numneigh ) {
        numneigh = 1;
      }
      sumK = sumK / numneigh;
      tempK[k] = sumK;
      }
    this->MeanCurv = tempK;
    }

}

void MeshData::ComputeGradCurvatureTangentPlane(  )
{

  if (this->nx.size() == 0 ||     this->ny.size() == 0 ||     this->nz.size() == 0)    {
    std::cerr << "ComputeGradCurvatureTangentPlane: no normals on mesh data " << std::endl;
    return;
    }
  vtkPoints*    verts = this->polydata->GetPoints();
  int numverts = verts->GetNumberOfPoints();

  for( int k = 0; k < numverts; k++ )    {
    vector<double> nhat(3);
    nhat[0] = this->nx[k];
    nhat[1] = this->ny[k];
    nhat[2] = this->nz[k];
// step 1. create the rotation matrix that orients the current normal as [0,0,1]'.

    double phiang = atan2( nhat[0], nhat[1] );
    vector<double> rotate1(9);
    rotate1[0] = cos(phiang); rotate1[1] = -sin(phiang); rotate1[2] = 0;
    rotate1[3] = sin(phiang); rotate1[4] = cos(phiang); rotate1[5] = 0;
    rotate1[6] = 0; rotate1[7] = 0; rotate1[8] = 1.0;
    vector<double> nhat_a(3);
    pkmult( nhat, rotate1, nhat_a );
    double ytilde = nhat_a[1];
    double theta = M_PI_2 - atan2(nhat[2],ytilde);
    vector<double> rotate2(9);
    rotate2[0] = 1.0; rotate2[1] = 0; rotate2[2] = 0;
    rotate2[3] = 0; rotate2[4] = cos(theta); rotate2[5] = -sin(theta);
    rotate2[6] = 0; rotate2[7] = sin(theta); rotate2[8] = cos(theta);
    vector<double> nhat_b(3);
    pkmult( nhat_a, rotate2, nhat_b );
// nhat_b should now be [0 0 1]'

    double thispt[3];
    verts->GetPoint( k, thispt );
// apply rotate2 * rotate1 to each *translated* neighbor of this k-th point
    ::size_t num_neigh = this->adj[k].myNeighbs.size();

  if( num_neigh < 1 ) {
     this->dkde1[k] = 0.0;
     this->dkde2[k] = 0.0;
     continue;
  }


    double vec[3];
    vector<double> vv(3);
    vector<double> vv_(3);
    valarray<double> xdata(num_neigh);
    valarray<double> ydata(num_neigh);
    valarray<double> zdata(num_neigh);
// step 2. create temporary set of vectors as copies of neighboring points
// translated to origin
// step 3. apply the rotation to all these points
    for ( ::size_t i = 0; i < num_neigh; i++ )
      {
      int idx = this->adj[k].myNeighbs[i];
      verts->GetPoint( idx, vec );
      vv[0] = vec[0] - thispt[0];
      vv[1] = vec[1] - thispt[1];
      vv[2] = vec[2] - thispt[2];
      pkmult( vv, rotate1, vv_ );
      pkmult( vv_, rotate2, vv );
      xdata[i] = vv[0];
      ydata[i] = vv[1];
      zdata[i] = this->MeanCurv[idx] - this->MeanCurv[k]; //vv[2];
// zero reference H at the vertex where we are forming tangent plane
      }

// step 4. find least-squares fit for H(x,y) = ax + by
    valarray<double> RHS(2);
    valarray<double> ATA(4);
    ATA[0] = (xdata * xdata).sum();
    ATA[1] = (xdata * ydata).sum();
    ATA[2] = ATA[1];
    ATA[3] = (ydata * ydata).sum();

    RHS[0] = (xdata * zdata).sum();
    RHS[1] = (ydata * zdata).sum();

    int maxits = 1000;
    valarray<double> ab = RHS; // initial guess
    valarray<double> LHS(2);
    pkmult2( ab, ATA, LHS );
    double res = sqrt( ( (LHS - RHS)*(LHS - RHS) ).sum() );
    double tol = 1e-8;
    int iter = 0;
    while( iter < maxits && res > tol )
      {
      iter++;
      ab[0] = (RHS[0] - ( ab[1]*ATA[1] ) )/ ATA[0];
      ab[1] = (RHS[1] - ( ab[0]*ATA[2] ) )/ ATA[3];
      pkmult2( ab, ATA, LHS );
      res = sqrt( ( (LHS - RHS)*(LHS - RHS) ).sum() );
      }
    this->dkde1[k] = ab[0];
    this->dkde2[k] = ab[1];
// step 5. differentiate the plane along principal directions

    }
  this->dkmag = sqrt( this->dkde1 * this->dkde2 );
}



void MeshData::ComputeCurvature(  )
{
  if ( this->nx.size() == 0 || this->ny.size() == 0 || this->nz.size() == 0) {
    std::cerr << "Compute Curvature: no normals on mesh data\n";
    return;
  }
  std::cout<<"Computing curvature...\n";
  vtkPoints*    verts = this->polydata->GetPoints();
  int numverts = verts->GetNumberOfPoints();

  for( int k = 0; k < numverts; k++ ) {
    vector<double> nhat(3);
    nhat[0] = this->nx[k];
    nhat[1] = this->ny[k];
    nhat[2] = this->nz[k];
// step 1. create a rotation matrix that orients the current normal as [0,0,1]'.

    double phiang = atan2( nhat[0], nhat[1] );
    vector<double> rotate1(9);
    rotate1[0] = cos(phiang); rotate1[1] = -sin(phiang); rotate1[2] = 0;
    rotate1[3] = sin(phiang); rotate1[4] = cos(phiang); rotate1[5] = 0;
    rotate1[6] = 0; rotate1[7] = 0; rotate1[8] = 1.0;
    vector<double> nhat_a(3);
    pkmult( nhat, rotate1, nhat_a );
    double ytilde = nhat_a[1];
    double theta = M_PI_2 - atan2(nhat[2],ytilde);
    vector<double> rotate2(9);
    rotate2[0] = 1.0; rotate2[1] = 0; rotate2[2] = 0;
    rotate2[3] = 0; rotate2[4] = cos(theta); rotate2[5] = -sin(theta);
    rotate2[6] = 0; rotate2[7] = sin(theta); rotate2[8] = cos(theta);
    vector<double> nhat_b(3);
    pkmult( nhat_a, rotate2, nhat_b );
// nhat_b should now be [0 0 1]'

    double thispt[3];
    verts->GetPoint( k, thispt );
// apply rotate2 * rotate1 to each *translated* neighbor of this k-th point
    ::size_t num_neigh = this->adj[k].myNeighbs.size();

  if( num_neigh < 1 ) {
    this->MeanCurv[k] = 0.0;
    continue;
  }

  double vec[3];
    vector<double> vv(3);
    vector<double> vv_(3);
    valarray<double> xdata(num_neigh);
    valarray<double> ydata(num_neigh);
    valarray<double> zdata(num_neigh);
// step 2. create temporary set of vectors as copies of neighboring points
// translated to origin
// step 3. apply the rotation to all these points
    for ( ::size_t i = 0; i < num_neigh; i++ )
      {
      int idx = this->adj[k].myNeighbs[i];
      verts->GetPoint( idx, vec );
      vv[0] = vec[0] - thispt[0];
      vv[1] = vec[1] - thispt[1];
      vv[2] = vec[2] - thispt[2];
      pkmult( vv, rotate1, vv_ );
      pkmult( vv_, rotate2, vv );
      xdata[i] = vv[0];
      ydata[i] = vv[1];
      zdata[i] = vv[2];

      }
// step 4. find least-squares fit for z(x,y) = ax^2 + bxy + cy^2

    valarray<double> RHS(3); // A'z
    RHS[0] = ( xdata * xdata * zdata  ).sum();
    RHS[1] = ( xdata * ydata * zdata  ).sum();
    RHS[2] = ( ydata * ydata * zdata  ).sum();

    double tik_delta = 1e-1 * abs(RHS).min();

    vector<double> ATA(9); // A'A
    ATA[0] = tik_delta + (xdata * xdata * xdata * xdata).sum();
    ATA[1] = (xdata * xdata * xdata * ydata).sum();
    ATA[2] = (xdata * xdata * ydata * ydata).sum();
    ATA[3] = (xdata * ydata * xdata * xdata).sum();
    ATA[4] = tik_delta + (xdata * ydata * xdata * ydata).sum();
    ATA[5] = (xdata * ydata * ydata * ydata).sum();
    ATA[6] = (ydata * ydata * xdata * xdata).sum();
    ATA[7] = (ydata * ydata * xdata * ydata).sum();
    ATA[8] = tik_delta + (ydata * ydata * ydata * ydata).sum();

    int maxits = 1000;
    valarray<double> abc = RHS; // initial guess
    valarray<double> LHS(3);
    pkmult( abc, ATA, LHS );
    double res = sqrt( ( (LHS - RHS)*(LHS - RHS) ).sum() );
    double tol = 1e-8;
    int iter = 0;
    while( iter < maxits && res > tol )     {
      iter++;
      abc[0] = (RHS[0] - ( abc[1]*ATA[1] + abc[2]*ATA[2] ) )/ ATA[0];
      abc[1] = (RHS[1] - ( abc[0]*ATA[3] + abc[2]*ATA[5] ) )/ ATA[4];
      abc[2] = (RHS[2] - ( abc[0]*ATA[6] + abc[1]*ATA[7] ) )/ ATA[8];
      pkmult( abc, ATA, LHS );
      res = sqrt( ( (LHS - RHS)*(LHS - RHS) ).sum() );
    }

   this->MeanCurv[k] = -1*(abc[0] + abc[2]);
   //Lout() << MeanCurv[k] ;
   //LENDL;
//    this->MeanCurv[k] = abc[0] + abc[2];
// step 5. mean curvature is a + c
    }
}

vector<int> MeshData::InitPath( vector<int> pts)
{
// given several seed points, form a contour via
// some shortest straight euclidean path
// return a vector containing all indices of the initalized contour
  ::size_t numPts = pts.size();
  if( numPts < 3 )
    {
    std::cerr<<"Must have at least 3 pts to define closed contour! \n";
    return vector<int>(0);
    }

  vector<int> C(0);

  if (this->polydata->GetPointData()->GetScalars("InitialCurvature") != NULL)
    {
    // use the initial curvature that came in with the poly data
    vtkDataArray *curvScalars = this->polydata->GetPointData()->GetScalars("InitialCurvature");
    for (int i = 0; i < curvScalars->GetNumberOfTuples(); i++)
      {
      C.push_back((int)(floor(curvScalars->GetTuple1(i))));
      }
    return C;
    }


  vtkPoints*    verts = this->polydata->GetPoints();
  int numverts = verts->GetNumberOfPoints();
  if ( this->adjimm.size() == 0)
    {
    std::cerr << "InitPath: No mean adjacency computed on mesh\n";
    return C;
    }
  double thispt[3];
  double thatpt[3];

  unsigned int srtIdx = 0;
  unsigned int endIdx = 1;
  int Cpt = pts[srtIdx];
  C.push_back( Cpt );
  while( srtIdx < numPts )
    {
// look at the neighbors of initPt
// push the one with the closest distance to nextPt
// onto the stack of C
    int nextPt = pts[endIdx];
    verts->GetPoint( nextPt, thatpt ); // point where we're path finding towards
    while( Cpt != nextPt )
      { // path-find until you get to the nextPt
      vector<int> neigh_pts = this->adjimm[Cpt].myNeighbs;
      double minDist = 1e9;
      int minIdx = Cpt;
      for( ::size_t k = 0; k < neigh_pts.size(); k++ )
        {
        int idx = neigh_pts[k];
        int idx_count = 0;
        if( idx != nextPt )
          {
          idx_count = count( C.begin(), C.end(), idx );
          }
        // for each neighbor, measure the distance to the nextPt
        // keep the index of least distance
        verts->GetPoint( idx, thispt ); // current neighbor candidate
        double dist = pow(thatpt[0]-thispt[0],2.0)+pow(thatpt[1]-thispt[1],2.0)+pow(thatpt[2]-thispt[2],2.0);
        if( (idx_count == 0 ) && (idx != Cpt) && dist < minDist )
          {
          minDist = dist;
          minIdx = idx;
          }
        }
  // ok now we know the index of the best neighbor.
  // push it onto the path stack and make it the new current point
      Cpt = minIdx;
      if( (minIdx != nextPt) && count( C.begin(), C.end(), Cpt ) != 0 )
        {
        std::cerr<<"Error, path finder stuck in a loop. Try another initialization. \n";
        return vector<int>(0);
        }
      //if( Cpt != nextPt ) // later debug: make sure the nextPt later gets put on the stack
      C.push_back( Cpt );
      }
    srtIdx++;
    endIdx++;
    if( endIdx == numPts )
      {
      endIdx = 0;
      }
    }

  throw "Should Not be here! \n" ;


  return C;
}

void MeshData::ComputeNormals( )
{
  bool bTextInputNormals = false;

  if( bTextInputNormals )
    {
    string filename = "..\\data\\n3.dat";
    ReadNormals( filename, this->nx, this->ny, this->nz );

    }
  else
    {
    vtkCellArray* faces = this->polydata->GetPolys();
    vtkPoints*    verts = this->polydata->GetPoints();

    int numfaces = faces->GetNumberOfCells();
    int numverts = verts->GetNumberOfPoints();
    valarray<double> fnx( numverts );
    valarray<double> fny( numverts );
    valarray<double> fnz( numverts );
    vector<int> vertcount(numverts); // number of times vertex has been added to
    double pt0[3];
    double pt1[3];
    double pt2[3];

    double v0[3];
    double v1[3];
    for( int i = 0; i < numfaces; i++ )
    {   // for every face
        // get the indices of points in this face
        // get the points at these indices
        // compute normal as cross product

        //vtkCell* cell = this->polydata->GetCell( i );
      vtkIdType npts;
      vtkIdType* pts;
      faces->GetCell(4*i,npts, pts );
      verts->GetPoint( pts[0], pt0 );
      verts->GetPoint( pts[1], pt1 );
      verts->GetPoint( pts[2], pt2 );

      v0[0] = pt1[0] - pt0[0];
      v0[1] = pt1[1] - pt0[1];
      v0[2] = pt1[2] - pt0[2];
      v1[0] = pt2[0] - pt0[0];
      v1[1] = pt2[1] - pt0[1];
      v1[2] = pt2[2] - pt0[2];

      double x = v0[1]*v1[2] - v0[2]*v1[1];
      double y = -v0[0]*v1[2] + v0[2]*v1[0];
      double z = v0[0]*v1[1] - v0[1]*v1[0];
      double norm = sqrt(x*x+y*y+z*z) + 1e-9;
      fnx[pts[0]] += x/norm;
      fny[pts[0]] += y/norm;
      fnz[pts[0]] += z/norm;
      fnx[pts[1]] += x/norm;
      fny[pts[1]] += y/norm;
      fnz[pts[1]] += z/norm;
      fnx[pts[2]] += x/norm;
      fny[pts[2]] += y/norm;
      fnz[pts[2]] += z/norm;
      vertcount[pts[0]] += 1;
      vertcount[pts[1]] += 1;
      vertcount[pts[2]] += 1;
    }

    if( ! bTextInputNormals ) {
      for( int i = 0;  i < numverts; i++ ) {
        vertcount[i] += 1e-9;
        this->nx[i] = fnx[i] / vertcount[i] ;
        this->ny[i] = fny[i] / vertcount[i] ;
        this->nz[i] = fnz[i] / vertcount[i] ;
      }
    }
    else {
      std::cerr << "Normals not computed for mesh data, numverts = " << numverts << ", normal array sizes were allocated to be x = " << this->nx.size() << ", y = " << this->ny.size() << ", z = " << this->nz.size() << "\n";
     }
  }
}

void MeshData::ComputeAdjacency(  )
{
    std::cout<<"Computing adjacency data... " << std::endl;
  int NUMADJ = this->adj_levels; // how levels to traverse in adding neighbors
  this->polydata->BuildLinks();
  int numverts = this->polydata->GetNumberOfPoints();
  vtkCellArray* faces = this->polydata->GetPolys();
  if (faces->GetNumberOfCells() == 0)
    {
    std::cerr << "No cells on the poly data polys" << std::endl;
    return;
    }
  vtkSmartPointer<vtkIdList> cellIds = vtkSmartPointer<vtkIdList>::New();

  // for every face, make all vertices on the face store in the adjimm list
  for( int i = 0; i < numverts; i++ ) { // NOT NEEDED !?
    this->adjimm[i].myNeighbs    = vector<int>(1);
    this->adjimm[i].myNeighbs[0] = i;
    this->adjimm[i].myIdx = i;
  }

  int numfaces = faces->GetNumberOfCells();
  for( int i = 0; i < numfaces; i++ ) {
    vtkIdType npts;
    vtkIdType* pts;
    faces->GetCell(i*4,npts, pts );
    int vert0 = pts[0];    int vert1 = pts[1];    int vert2 = pts[2];
    for( int k = 0 ; k < 3; k++ ) {
      for( int kk = 0; kk < 3; kk++ ) {
        if( 0 == count( this->adjimm[pts[kk]].myNeighbs.begin(), this->adjimm[pts[kk]].myNeighbs.end(),pts[k] ) ) {
          this->adjimm[pts[kk]].myNeighbs.push_back( pts[k] );
        }
      }
    }
  }

  for( int i = 0; i < numverts; i++ ) {
    this->polydata->GetPointCells( i, cellIds );
    this->adj[i].myNeighbs = vector<int>(0);
    size_t iMaxExpectedNeigh = 64;
    this->adj[i].myNeighbs.reserve( iMaxExpectedNeigh );
    this->adj[i].myIdx = i;
    int iAdjCellCount = cellIds->GetNumberOfIds();
    if( 0 == (i % 10000 ) ) {
      std::cout<<"Storing immediate neighbors... "<<double(i)/numverts*100<<"% " << std::endl;
    }
    for( int k = 0; k < iAdjCellCount; k++ ) {
      int id = cellIds->GetId( k ); // add every point in this cell...
      vtkIdType npts;
      vtkIdType* pts;
      faces->GetCell(id*4,npts, pts );
      int c0 = count( this->adj[i].myNeighbs.begin(),this->adj[i].myNeighbs.end(),pts[0] );
      int c1 = count( this->adj[i].myNeighbs.begin(),this->adj[i].myNeighbs.end(),pts[1] );
      int c2 = count( this->adj[i].myNeighbs.begin(),this->adj[i].myNeighbs.end(),pts[2] );
      if( c0 == 0 ) {
        this->adj[i].myNeighbs.push_back( pts[0] );
        }
      if( c1 == 0 ) {
        this->adj[i].myNeighbs.push_back( pts[1] );
        }
      if( c2 == 0 ) {
        this->adj[i].myNeighbs.push_back( pts[2] );
       }
      }
    }


  // every neigbhor array appends the neigbhor arrays of its neighbors to itself
  MeshData* tempdata = new MeshData();
  tempdata->adj = this->adj;
  for( ::size_t its = 0; its < NUMADJ; its++ )  { // how many levels deep to append
    std::cout<<" adding level "<<its<<" to adjacency..." << std::endl;
    for( ::size_t i = 0; i < numverts; i++ ) { // for every vertex
      if( 0 == (i % 10000 ) ) {
        std::cout<<"Storing next level neighbors... "<<double(i)/numverts*100<<"% " << std::endl;
        }
      ::size_t len = this->adj[i].myNeighbs.size(); // length of my neighbor array
      for( ::size_t k = 0; k < len; k++ )
        { // for every neigbhor index
        int idx = this->adj[i].myNeighbs[k];
        vector<int>* others = &(this->adj[idx].myNeighbs); // get neighbor's neighbor array
        ::size_t otherlen = (others)->size(); // length of neighbor's neighbor array
        for( ::size_t j = 0; j < otherlen; j++ ) { // for every element in neighbor's neigbhor array
          int ptId = (*others)[j];
          int num = count( this->adj[i].myNeighbs.begin(),this->adj[i].myNeighbs.end(),ptId );
          int num2 = count( tempdata->adj[i].myNeighbs.begin(),tempdata->adj[i].myNeighbs.end(),ptId );
          if( (num + num2) == 0 ) { // if I don't have his neigbhor yet, add it to my list of neighbors
            tempdata->adj[i].myNeighbs.push_back( ptId );
           }
          }
        }
    }
    this->adj = tempdata->adj;
  }
  delete tempdata;
}



}
