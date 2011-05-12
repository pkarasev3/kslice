#!/bin/bash

echo `date` > meshcurvolver.test.out

../bin/SurfaceFromPoints >> meshcurvolver.test.out
echo $?
../bin/test1_MeshCurvolverBasic >> meshcurvolver.test.out
echo $?
../bin/test2_MeshCurvolverComputeGeometry >> meshcurvolver.test.out
echo $?
../bin/test3_MeshCurvolverInitLevelset >> meshcurvolver.test.out
echo $?
