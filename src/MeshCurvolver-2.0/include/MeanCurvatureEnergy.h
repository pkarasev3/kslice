/*=========================================================================

  Program:   SparseFieldLevelSetContour
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Applications/CLI/SparseFieldLevelSetContour/MeanCurvatureEnergy.h $
  Language:  C++
  Date:      $Date: 2010-04-30 16:13:05 -0400 (Fri, 30 Apr 2010) $
  Version:   $Revision: 13140 $

  Copyright (c) Brigham and Women's Hospital (BWH) All Rights Reserved.

  See License.txt or http://www.slicer.org/copyright/copyright.txt for details.

==========================================================================*/
#ifndef MEANCURVATUREENERGY
#define MEANCURVATUREENERGY

#include "MeshEnergy.h"

class MeanCurvatureEnergy : public MeshEnergy
{
public:
MeanCurvatureEnergy( MeshOps::MeshData* data ) {meshdata = data;}
virtual ~MeanCurvatureEnergy( ) {}

double eval_energy( const std::vector<int>& C );
std::valarray<double> getforce( const std::list<int>& C);
std::valarray<double> getforce( const std::list<int>& C,
const std::list<int>& L_p1, const std::list<int>& L_n1,
const std::vector<double>& phi);

};



#endif
