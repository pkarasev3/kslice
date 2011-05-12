/*=========================================================================

  Program:   SparseFieldLevelSetContour
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Applications/CLI/SparseFieldLevelSetContour/MeshOps.h $
  Language:  C++
  Date:      $Date: 2009-08-07 14:35:53 -0400 (Fri, 07 Aug 2009) $
  Version:   $Revision: 10184 $

  Copyright (c) Brigham and Women's Hospital (BWH) All Rights Reserved.

  See License.txt or http://www.slicer.org/copyright/copyright.txt for details.

==========================================================================*/
#ifndef MESHOPS
#define MESHOPS

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <valarray>
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkRenderer.h"
#include "vtkDebugLeaks.h"
#include "vtkTestUtilities.h"
#include "vtkFieldData.h"
#include "Utils.h"
#include "vtkPolyDataNormals.h"
#include "vtkPolyDataMapper.h"
#include <vector>
#include <algorithm>
#include <map>

namespace MeshOps {

// TODO: this function seems pointless; clarify purpose or delete
int CountVertsOnMesh( vtkPolyData* poly );

struct AdjData { // slightly redundant, for debug clarity.
int myIdx;
std::vector<int> myNeighbs;

// TODO: use thsi struture inside MeshData instead of vectors of adjdata
// std::map<int, std::list<int>  >  neighbors_of;
};


class MeshData {
public:

    MeshData() {
        adj_levels  = 1;
        smoothH_its = 50;
    };

    ~MeshData() { };


     std::valarray<double> MeanCurv;
     std::valarray<double> nx;
     std::valarray<double> ny;
     std::valarray<double> nz;

	// derivatives w.r.t. intrinsic basis on manifold
     std::valarray<double> dkde1;
     std::valarray<double> dkde2;
     std::valarray<double> dkmag;
     std::valarray<double> kappa;

  std::map< const char* , std::valarray<double> > namedMeshArrays;

  vtkPolyData*               polydata;
  vtkPolyDataMapper*         mapper;
  std::vector<AdjData>       adj;    // neighbors N steps into tree
  std::vector<AdjData>       adjimm; // immediate neighbors

	int adj_levels;
	int smoothH_its;

public:
	void ComputeCurvatureData( ); // top level function

private:
	void ComputeAdjacency( );
	void ComputeNormals( );
	void ComputeCurvature( );
	void SmoothCurvature( );
	void ComputeGradCurvatureTangentPlane( );
    std::vector<int> InitPath( std::vector<int> pts);

private:

};

}

#endif
