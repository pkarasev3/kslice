#!/bin/bash
cd ../
builddir=`pwd`
cmake .
make -j8
cd scripts
../bin/KViewer_desktop 
