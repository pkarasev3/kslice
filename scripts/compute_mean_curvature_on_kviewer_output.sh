#!/bin/bash

outputName="KViewer_polydata_with_mean_curvature.vtp"
inputName="KViewer_polydata.vtp"
../bin/SurfaceCurvatureNoGui  -i $inputName  -o $outputName
