#!/bin/bash

# arg1: cmake executable output dir
# arg2: cmake library output dir

qBinPath=$1
echo "Destination: $qBinPath"

qmake -project "SOURCES = main.cpp  gui.cpp"
qmake -project "HEADERS = gui.h"
qmake -project "FORMS = gui.ui"
qmake -project "LIBS  += -L/usr/local/lib/vtk-5.6 -lvtkCommon -lvtksys -lQVTK -lvtkQtChart -lvtkViews -lvtkWidgets -lvtkInfovis -lvtkRendering -lvtkGraphics -lvtkImaging -lvtkIO -lvtkFiltering -lvtkDICOMParser -lvtksqlite   -lvtkexoIIc  -lvtkNetCDF  -lvtkmetaio  -lvtkftgl -lvtkfreetype -lvtkHybrid  -lvtkalglib -lvtkexpat -lvtkverdict -lvtkParallel -lVPIC -lCosmo"

qVTK_DIR=/usr/local/include/vtk-5.6

qmake -project "INCLUDEPATH+=$qVTK_DIR"
qmake -project "DESTDIR = $qBinPath" 

# create Makefile and .pro
qmake-qt4  `pwd`/qtGUI_Demo.pro  -spec linux-g++  CONFIG+=debug


