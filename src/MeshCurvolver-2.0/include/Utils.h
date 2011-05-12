/*=========================================================================


  Program:   SparseFieldLevelSetContour
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Applications/CLI/SparseFieldLevelSetContour/Utils.h $
  Language:  C++
  Date:      $Date: 2010-04-30 16:13:05 -0400 (Fri, 30 Apr 2010) $
  Version:   $Revision: 13140 $

  Copyright (c) Brigham and Women's Hospital (BWH) All Rights Reserved.

  See License.txt or http://www.slicer.org/copyright/copyright.txt for details.

==========================================================================*/


#ifndef UTILS_H
#define UTILS_H
#include <stdlib.h>
#include <valarray>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <list>
#include "vtkPolyData.h"

#ifndef M_PI_2
#define M_PI_2     1.57079632679489661923
#endif

// This stuff is deprecated, no new usages!

void ProcessTriDataFiles( const std::vector<std::string>& filenames, std::valarray<int> &CurIdx,
std::valarray<double> &CurVertX, std::valarray<double> &CurVertY, std::valarray<double> &CurVertZ );

void ReadNormals( const std::string& filename, std::valarray<double>& nx, std::valarray<double>& ny, std::valarray<double>& nz );

void pkmult( const std::vector<double>& vecin, const std::vector<double>& mat, std::vector<double>& vecout );
void pkmult( const std::valarray<double>& vecin, const std::vector<double>& mat, std::valarray<double>& vecout );
void pkmult2( const std::valarray<double>& vecin, const std::valarray<double>& mat, std::valarray<double>& vecout );

void SelfUnion( std::vector<int>& vec );

void DropIdx( const std::vector<int>& L_zp, const std::vector<int>& L_zn, std::vector<int>& L_z );

void Crash();
void PrintPolyDataArrayNames( vtkPolyData* polyMesh );

std::vector<int> listToSTDVector( const std::list<int>& ListIn );
#endif
