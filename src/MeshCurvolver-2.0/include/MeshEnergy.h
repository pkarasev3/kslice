/*=========================================================================

  Program:   SparseFieldLevelSetContour
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Applications/CLI/SparseFieldLevelSetContour/MeshEnergy.h $
  Language:  C++
  Date:      $Date: 2010-04-30 16:13:05 -0400 (Fri, 30 Apr 2010) $
  Version:   $Revision: 13140 $

  Copyright (c) Brigham and Women's Hospital (BWH) All Rights Reserved.

  See License.txt or http://www.slicer.org/copyright/copyright.txt for details.

==========================================================================*/
#ifndef ENERGY
#define ENERGY

#include "MeshOps.h"
#include <list>

class MeshEnergy
{
public:
MeshEnergy() { }
MeshEnergy( const MeshEnergy & );
virtual ~MeshEnergy() { }
// evaluate energy at every point indexed by C
virtual double eval_energy( const std::vector<int>& C ) = 0;

// get the force F at the vertices for sparse field method
virtual  std::valarray<double> getforce( const std::list<int>& C ) = 0;
virtual  std::valarray<double> getforce( const std::list<int>& C,
const std::list<int>& L_p1, const std::list<int>& L_n1,
const std::vector<double>& phi) = 0;

public:
MeshOps::MeshData* meshdata;

protected:

void GetKappa( const  std::vector<int>& C, const  std::vector<double>& phi,
 std::valarray<double>& kappa);

void GetNormalsTangentPlane( const  std::vector<int>& C, const  std::vector<double>& phi,
 std::valarray<double>& ne1,  std::valarray<double>& ne2, MeshOps::MeshData* meshdata );

};

#endif

