#!/bin/bash

echo "generating QT .pro,  Makefile, ui headers. "
echo "assumption: vtk, qt libraries are in your library path"
./qmake.build.sh  $1


echo "building GUI executable"
./main.build.sh  

rm Makefile

