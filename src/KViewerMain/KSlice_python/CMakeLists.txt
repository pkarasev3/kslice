#-----------------------------------------------------------------------------
if(NOT Slicer_SOURCE_DIR)
  set(EXTENSION_NAME KSlice)
  set(EXTENSION_HOMEPAGE "http://www.slicer.org/slicerWiki/index.php/Documentation/4.2/Extensions/KSlice")
  set(EXTENSION_CATEGORY "Examples")
  set(EXTENSION_CONTRIBUTORS "Jean-Christophe Fillion-Robin (Kitware), Steve Pieper (Isomics)")
  set(EXTENSION_DESCRIPTION "This is an example of extension bundling a scripted loadable module")
  set(EXTENSION_ICONURL "http://viewvc.slicer.org/viewvc.cgi/Slicer4/trunk/Extensions/Testing/KSlice/Resources/Icons/KSlice.png?revision=19437&view=co")
  set(EXTENSION_SCREENSHOTURLS "http://wiki.slicer.org/slicerWiki/images/e/e2/Slicer-r19441-KSlice-screenshot.png")
endif()

#-----------------------------------------------------------------------------
set(MODULE_NAME KSlice)


#-----------------------------------------------------------------------------
if(NOT Slicer_SOURCE_DIR)
  find_package(Slicer REQUIRED)
  include(${Slicer_USE_FILE})
endif()

find_package(VTK)
IF(NOT VTK_DIR)
  message(FATAL_ERROR "Please set VTK_DIR.")
ENDIF(NOT VTK_DIR)
include(${VTK_USE_FILE})

find_package(ITK)
if(NOT ITK_DIR)
  message(STATUS "If you need ITK, Please set ITK_DIR to build stuff that needs ITK.")
else(NOT ITK_DIR)
   include(${ITK_USE_FILE}) # only gao3d should need this ...
endif(NOT ITK_DIR)



#-----------------------------------------------------------------------------
set(MODULE_PYTHON_SCRIPTS
  KSlice.py
  )

set(MODULE_PYTHON_RESOURCES
  )

#-----------------------------------------------------------------------------
ctkMacroCompilePythonScript(
  TARGET_NAME KSlice
  SCRIPTS "${MODULE_PYTHON_SCRIPTS}"
  RESOURCES "${MODULE_PYTHON_RESOURCES}"
  DESTINATION_DIR ${CMAKE_BINARY_DIR}/${Slicer_QTSCRIPTEDMODULES_LIB_DIR}
  INSTALL_DIR ${Slicer_INSTALL_QTSCRIPTEDMODULES_LIB_DIR}
  NO_INSTALL_SUBDIR
  )

#-------------------------------------------------------------------------------
#Set up python wrapping
include_directories(../include)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${VTK_DIR}/../VTK/CMake/ /home/ikolesov3/source/Slicer4SuperBuildSource/CMake/)
execute_process(COMMAND echo ${CMAKE_MODULE_PATH})
execute_process(COMMAND echo "simple print")
execute_process(COMMAND echo ${Slicer_SOURCE_DIR})
include(vtkWrapPython)
set(KitPython_SRCS )
set(WRAP_LIST ../src/KSlice.cpp)  #the files for which we want to create a python wrapping
set(Kit_PYTHON_EXTRA_SRCS ) #no extra sources at this time
VTK_WRAP_PYTHON3(${EXTENSION_NAME}Python KitPython_SRCS "${WRAP_LIST}")
ADD_LIBRARY(${EXTENSION_NAME}PythonD ${KitPython_SRCS} ${Kit_PYTHON_EXTRA_SRCS})
PYTHON_ADD_MODULE(${EXTENSION_NAME}Python ${EXTENSION_NAME}PythonInit.cxx)


#-----------------------------------------------------------------------------
if(BUILD_TESTING)

  # this will regiester the unittest subclass in the main script
  # as a ctest (note that it is also usable as a runtime test).
  slicer_add_python_unittest(SCRIPT KSlice.py)

  # additional build-time testing goes in here
  add_subdirectory(Testing)
endif()

#-----------------------------------------------------------------------------
if(NOT Slicer_SOURCE_DIR)
  include(${Slicer_EXTENSION_CPACK})
endif()
