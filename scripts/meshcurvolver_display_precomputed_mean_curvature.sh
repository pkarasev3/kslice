#!/bin/bash

outputName="KViewer_polydata_with_mean_curvature_2.vtp"
inputName="KViewer_polydata_with_mean_curvature.vtp"
colormapName="mean_curvature"
../bin/DemoStandaloneMeshCurvolver  -o  $outputName  -i  $inputName -c $colormapName
