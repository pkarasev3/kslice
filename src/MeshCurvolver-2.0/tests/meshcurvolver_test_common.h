#ifndef MESHCURVOLVER_TEST_COMMON_H
#define MESHCURVOLVER_TEST_COMMON_H

#include "vtkPoints.h"
#include "vtkMath.h"
#include <vector>


vtkPoints* createSyntheticBumps( int num_pts = 5000 );

vtkPoints* createSyntheticDonut( int num_pts = 5000 );

vtkPoints* createSyntheticStarBlob( int Npts = 5000 );

/**
  */
void getSyntheticSeedPoints( std::vector< std::vector<float> >&  pts3D );

void getSyntheticSeedPoints2( std::vector< std::vector<float> >&  pts3D );


#endif
