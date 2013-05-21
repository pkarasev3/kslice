cmake_minimum_required(VERSION 2.8)



#roject(KSlice3D-slicer)
#make_policy(SET CMP0012 NEW)

enable_testing()

#-----------------------------------------------------------------------------
set(EXTENSION_NAME KSlice)

set(KSLICE_VERSION_MAJOR "1")
set(KSLICE_VERSION_MINOR "0")
set(KSLICE_VERSION ${KSLICE_VERSION_MAJOR}.${KSLICE_VERSION_MINOR})
set(EXTENSION_STATUS "Beta")
set(EXTENSION_DEPENDS "NA") 
set(EXTENSION_SCREENSHOTURLS   "http://www.slicer.org/slicerWiki/images/thumb/2/2c/DrawLabel.png/500px-DrawLabel.png  http://www.na-mic.org/Wiki/images/thumb/d/d0/TendonSegment.png/800px-TendonSegment.png  http://www.na-mic.org/Wiki/images/thumb/b/b8/FiducialSnapshotX.png/800px-FiducialSnapshotX.png")
# "boost") # Specified as a space separated list or 'NA' if any
#set(EXTENSION_BUILD_SUBDIRECTORY  

set(MIDAS_PACKAGE_URL   http://slicer.kitware.com/midas3)
set(MIDAS_PACKAGE_EMAIL  karasevpa@gmail.com)
set(MIDAS_PACKAGE_API_KEY  tqsS/lB5zAE7xcI3i52P0NFVvJx0x8EQ)
set(EXTENSION_HOMEPAGE     http://wiki.slicer.org/slicerWiki/index.php/Documentation/Nightly/Modules/KSliceInteractiveSegmenter)
set(EXTENSION_CATEGORY    "Segmentation")
set(EXTENSION_ICONURL    http://www.slicer.org/slicerWiki/images/b/bc/KSliceEffect.png)
set(EXTENSION_CONTRIBUTORS  "Ivan Kolesov (GT), Peter Karasev (GT), Patricio Vela (GT), Allen Tannenbaum (UAB), Karl Fritscher (BWH & MGH), Steve Pieper (Isomics)")
set(EXTENSION_DESCRIPTION   "This extension provides an interactive 3D segmentation tool.")
#-----------------------------------------------------------------------------

set(KSlice_Build_SlicerWrap TRUE CACHE BOOL TRUE)
if(KSlice_Build_SlicerWrap)
  if(NOT Slicer_SOURCE_DIR)
    find_package(Slicer REQUIRED)
    include(${Slicer_USE_FILE})
  endif()
endif()

if(NOT KSlice_Build_SlicerWrap)
  find_package(VTK REQUIRED)
  if(NOT VTK_DIR)
    message(FATAL_ERROR "Please set VTK_DIR, or build with Slicer.")
  endif(NOT VTK_DIR)
  include(${VTK_USE_FILE})
endif()


SET(BUILD_SHARED_LIBS ON CACHE INTERNAL ON)
set(CMAKE_VERBOSE_MAKEFILE CACHE bool true FORCE)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Debug;Release")
 #Q: what does this line do??  A: enables dropdown box in cmake-gui, instead of needed to type

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Debug")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin CACHE PATH "Single Directory for all Executables")
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib CACHE PATH "Single Directory for all Libraries")


if(WIN32)
  set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MDd /Zi /Ob0 /Od /RTC1" CACHE STRING "extra compiler flags for debug" FORCE )
  set(CMAKE_C_FLAGS_DEBUG "/D_DEBUG /MDd /Zi /Ob0 /Od /RTC1" CACHE STRING "extra compiler flags for debug" FORCE )
else(WIN32)
  set(CMAKE_C_FLAGS_DEBUG "-pedantic " CACHE STRING "extra compiler flags for debug" FORCE )
  set(CMAKE_C_FLAGS "-fPIC -g3 -Wall -pipe -DVTK_EXCLUDE_STRSTREAM_HEADERS -Wl,--no-undefined -fopenmp" CACHE STRING "compiler flags always used" FORCE)
  set(CMAKE_CXX_FLAGS "-fPIC -Wall -pipe -DVTK_EXCLUDE_STRSTREAM_HEADERS -Wl,--no-undefined -fopenmp" CACHE STRING "compiler flags always used" FORCE)
  set(CMAKE_CXX_FLAGS_DEBUG "-g3 " CACHE STRING "extra compiler flags for debug" FORCE )
endif(WIN32)







message(STATUS "CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}")

set( KSlice_include  ${CMAKE_CURRENT_SOURCE_DIR}/util/vrcl/include
                     ${CMAKE_CURRENT_SOURCE_DIR}/KViewerMain/include
                     ${CMAKE_CURRENT_SOURCE_DIR}/KSegmentor/include
                     ${CMAKE_CURRENT_SOURCE_DIR}/sfm_local_chanvese/include
)


# Use the include path and library for Qt that is used by VTK.
include_directories( ${QT_include_DIR} ${KSlice_include} )

#add_subdirectory(util/vrcl)
add_subdirectory(KViewerMain)
add_subdirectory(KSegmentor)
add_subdirectory(sfm_local_chanvese)

message(STATUS "Check that this dir actually contains libraries! On windows, will have subdirs! ${KSLICE_LIB_PATH}" )
if(KSlice_Build_SlicerWrap)
    set(KSLICE_LIB_PATH  ${CMAKE_BINARY_DIR}/lib/ CACHE PATH "where are vrcl and KSegmentor libraries")
    add_subdirectory(kslice_wrap)
    add_subdirectory(KViewerMain/exes)    # STANDALONE
endif()


set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_INSTALL_PREFIX  ${Slicer_DIR} CACHE PATH  "cmake install prefix")

message(STATUS " Compiler: ${CMAKE_COMPILER} ")
message(STATUS " C++ flags (Release): ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS " C++ flags (Debug): ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS " QT_LIBRARIES: ${QT_LIBRARIES} " )
message(STATUS " qt include dir: ${QT_include_DIR} " )
#
