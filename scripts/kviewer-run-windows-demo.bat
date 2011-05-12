#!/bin/bash
set opacity="0.7"
set paintBrushSize="10"
set kvLabel="../data/test2_output_shamil_basayev.mha"
set kvImage="/u4/home/ikolesov3/SVN_tann_lab/tann-lab/trunk/KSlice/data/gao_3d_rss/grayscale.mha"
set kvLabel="..\data\ACLMapCorHeader.mhd"
set kvImage="..\data\venturaHeader.mhd"
set maxVal="3000"
set minVal="0"
..\bin\Release\KViewer_desktop  -p   %paintBrushSize%  -L   %kvLabel%  -q %opacity%   -x %maxVal%   -Z 0  -I  %kvImage%  -n %minVal%
