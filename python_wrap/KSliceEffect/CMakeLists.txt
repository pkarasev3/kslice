cmake_minimum_required(VERSION 2.8.7)

#-----------------------------------------------------------------------------
if(NOT Slicer_SOURCE_DIR)
  set(EXTENSION_NAME KSliceEffect)
  set(EXTENSION_HOMEPAGE "http://www.slicer.org/slicerWiki/index.php/Documentation/4.2/Extensions/KSliceEffect")
  set(EXTENSION_CATEGORY "Examples")
  set(EXTENSION_CONTRIBUTORS "Steve Pieper (Isomics)")
  set(EXTENSION_DESCRIPTION "This is an example of extension bundling one scripted loadable editor effect")
endif()

#-----------------------------------------------------------------------------
set(MODULE_NAME KSliceEffect)

#-----------------------------------------------------------------------------
if(NOT Slicer_SOURCE_DIR)
  find_package(Slicer REQUIRED)
  include(${Slicer_USE_FILE})
endif()

#-----------------------------------------------------------------------------
set(MODULE_PYTHON_SCRIPTS
  KSliceEffect.py
  )

set(MODULE_PYTHON_RESOURCES
  KSliceEffect.png
  )

#-----------------------------------------------------------------------------
ctkMacroCompilePythonScript(
  TARGET_NAME KSliceEffect
  SCRIPTS "${MODULE_PYTHON_SCRIPTS}"
  RESOURCES "${MODULE_PYTHON_RESOURCES}"
  DESTINATION_DIR ${CMAKE_BINARY_DIR}/${Slicer_QTSCRIPTEDMODULES_LIB_DIR}
  INSTALL_DIR ${Slicer_INSTALL_ROOT}
  )

#-----------------------------------------------------------------------------
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()

#-----------------------------------------------------------------------------
if(NOT Slicer_SOURCE_DIR)
  include(${Slicer_EXTENSION_CPACK})
endif()


#-----------------------------------------------------------------------------
#build the module python will include
find_package(VTK)
IF(NOT VTK_DIR)
  message(FATAL_ERROR "Please set VTK_DIR.")
ENDIF(NOT VTK_DIR)
include(${VTK_USE_FILE})

execute_process(COMMAND echo ${VTK_INCLUDE_DIRS})

include_directories(../../src/KViewerMain/include/ ../../src/KSegmentor/include/)
add_library(vtkKSlicePy kslice_TryWrap.cpp ../../src/KViewerMain/include/KSlice.h)
target_link_libraries( vtkKSlicePy /u4/home/ikolesov3/KSlice_slicerCont/kslice/python_wrap/KSliceEffect/../../build/lib/libKSlice.so)
