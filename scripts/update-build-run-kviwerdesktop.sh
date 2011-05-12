#!/bin/bash

cd ../
builddir=`pwd`
cd ../
git pull
cd $builddir
cmake .
make -j8
cd scripts
../bin/KViewer_desktop 
