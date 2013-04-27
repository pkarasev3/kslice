cmake_minimum_required(VERSION 2.8)



project(KSlice3D-slicer)
cmake_policy(SET CMP0012 NEW)

enable_testing()

#-----------------------------------------------------------------------------
set(MODULE_NAME KSlice)
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
#set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Debug;Release") #what does this line do??

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Debug")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin CACHE PATH "Single Directory for all Executables")
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib CACHE PATH "Single Directory for all Libraries")


if(WIN32)
  set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MDd /Zi /Ob0 /Od /RTC1" CACHE STRING "extra compiler flags for debug" FORCE )
  set(CMAKE_C_FLAGS_DEBUG "/D_DEBUG /MDd /Zi /Ob0 /Od /RTC1" CACHE STRING "extra compiler flags for debug" FORCE )
else(WIN32)
  set(CMAKE_C_FLAGS_DEBUG "-pedantic " CACHE STRING "extra compiler flags for debug" FORCE )
  set(CMAKE_C_FLAGS "-fPIC -g3 -std=c++0x -Wall -pipe -DVTK_EXCLUDE_STRSTREAM_HEADERS -Wl,--no-undefined -fopenmp" CACHE STRING "compiler flags always used" FORCE)
  set(CMAKE_CXX_FLAGS "-fPIC -std=c++0x -Wall -pipe -DVTK_EXCLUDE_STRSTREAM_HEADERS -Wl,--no-undefined -fopenmp" CACHE STRING "compiler flags always used" FORCE)
  set(CMAKE_CXX_FLAGS_DEBUG "-g3 " CACHE STRING "extra compiler flags for debug" FORCE )
endif(WIN32)

find_package( Boost COMPONENTS system filesystem program_options )
  find_package( OpenMP )
  link_directories ( ${Boost_LIBRARY_DIRS} )
  include_directories ( ${Boost_INCLUDE_DIRS} )







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

if(KSlice_Build_SlicerWrap)
    set(KSLICE_LIB_PATH  ${CMAKE_BINARY_DIR}/lib/ CACHE PATH "where are vrcl and KSegmentor libraries")
    add_subdirectory(kslice_wrap)
    add_subdirectory(KViewerMain/exes)    # STANDALONE
endif()


set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS " Compiler: ${CMAKE_COMPILER} ")
message(STATUS " C++ flags (Release): ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS " C++ flags (Debug): ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS " QT_LIBRARIES: ${QT_LIBRARIES} " )
message(STATUS " qt include dir: ${QT_include_DIR} " )
message(STATUS " fyi, bunch of image data is here ---- tsfat.bme.gatech.edu:/u4/public/karasev/data_medical/mha_grantmuller_kslice/mha_all/ " )


