cmake_minimum_required(VERSION 2.6)
project(KSlice3D)

find_package(VTK)
IF(NOT VTK_DIR)
  message(FATAL_ERROR "Please set VTK_DIR.")
ENDIF(NOT VTK_DIR)
include(${VTK_USE_FILE})

find_package(ITK)
if(NOT ITK_DIR)
  message(STATUS "If you need ITK, Please set ITK_DIR to build stuff that needs ITK.")
else(NOT ITK_DIR)
  # include(${ITK_USE_FILE}) # only gao3d should need this ...
endif(NOT ITK_DIR)


find_package(OpenCV  REQUIRED)
message(STATUS "  OpenCV_LIBS = ${OpenCV_LIBS} ;
                   you also want to have had libgtk2.0-dev *Before* building opencv!" )

set(CMAKE_VERBOSE_MAKEFILE CACHE bool true FORCE)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin CACHE PATH  "Single Directory for all Executables")
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib CACHE PATH  "Single Directory for all Libraries")

# semi-hack: copy the scripts dir so the relative structure still jives, and temp files can be wiped easily
set(SCRIPT_PATH ${CMAKE_SOURCE_DIR}/../scripts CACHE PATH  "scripts folder")
set(DATA_PATH ${CMAKE_SOURCE_DIR}/../data CACHE PATH  "data folder")
execute_process(COMMAND "sh"  ${SCRIPT_PATH}/link_build_dir_to_scripts_and_data.sh  "
                ${CMAKE_SOURCE_DIR}"  "${CMAKE_BINARY_DIR}"   "${SCRIPT_PATH}"  ${DATA_PATH})

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "configuration (default: Debug)")
set(BUILD_QTGUI_DEMO_SH  CACHE bool false)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# run shell script to force-clean... delete everything in /bin and /lib, and generated moc files
# more work to build but enables sloppy user to have installed a bunch of conflicting QT libs
message(STATUS "    force clean this round? (set to 0 or 1) " )
set(FORCE_CLEAN_SH  CACHE bool true)
if(${FORCE_CLEAN_SH})
message(STATUS "   running force_clean.sh once and setting flag back to false! " )
execute_process(COMMAND "sh"  ${SCRIPT_PATH}/force_clean.sh  "${EXECUTABLE_OUTPUT_PATH}"  "${LIBRARY_OUTPUT_PATH}" )
set(FORCE_CLEAN_SH  CACHE bool true)
else(${FORCE_CLEAN_SH})
message(STATUS " not running force_clean! " )
endif(${FORCE_CLEAN_SH})


# use what QVTK built with!
SET(QT_QMAKE_EXECUTABLE ${VTK_QT_QMAKE_EXECUTABLE} CACHE FILEPATH "")
SET(QT_MOC_EXECUTABLE ${VTK_QT_MOC_EXECUTABLE} CACHE FILEPATH "")
SET(QT_UIC_EXECUTABLE ${VTK_QT_UIC_EXECUTABLE} CACHE FILEPATH "")
SET(DESIRED_QT_VERSION 4 CACHE FILEPATH "")

find_package(Qt)
IF(QT_USE_FILE)
  include(${QT_USE_FILE})
ELSE(QT_USE_FILE)
  SET(QT_LIBRARIES   ${QT_QT_LIBRARY})
ENDIF(QT_USE_FILE)



if(WIN32)
  set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MDd /Zi /Ob0 /Od /RTC1" CACHE STRING "extra compiler flags for debug" FORCE )
  set(CMAKE_C_FLAGS_DEBUG "/D_DEBUG /MDd /Zi /Ob0 /Od /RTC1" CACHE STRING "extra compiler flags for debug" FORCE )
else(WIN32)
  set(CMAKE_C_FLAGS_DEBUG "-g3 -pedantic -Werror" CACHE STRING "extra compiler flags for debug" FORCE )
  set(CMAKE_C_FLAGS "-std=c++0x -Wall -pipe -DVTK_EXCLUDE_STRSTREAM_HEADERS -Wl,--no-undefined -fopenmp" CACHE STRING "compiler flags always used" FORCE)
  set(CMAKE_CXX_FLAGS "-std=c++0x -Wall -pipe -DVTK_EXCLUDE_STRSTREAM_HEADERS -Wl,--no-undefined -fopenmp" CACHE STRING "compiler flags always used" FORCE)
  set(CMAKE_CXX_FLAGS_DEBUG "-g3 " CACHE STRING "extra compiler flags for debug" FORCE )
  find_package( Boost COMPONENTS filesystem program_options )
  find_package( OpenMP )
  link_directories ( ${Boost_LIBRARY_DIRS} )
  include_directories ( ${Boost_INCLUDE_DIRS} )
endif(WIN32)



set(TOTENKOPF_WERROR  CACHE bool false)
if(${TOTENKOPF_WERROR})
  set(CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG} "-Werror")
  set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG} "-Werror")
  message(STATUS  "using 'errors are warnings!' flag in gcc..." )
endif(${TOTENKOPF_WERROR})



message(STATUS "CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}")

set( KSlice_include  ${CMAKE_CURRENT_SOURCE_DIR}/util/getopt
                     ${CMAKE_CURRENT_SOURCE_DIR}/util/vrcl/include
                     ${CMAKE_CURRENT_SOURCE_DIR}/KViewerMain/include
                     ${CMAKE_CURRENT_SOURCE_DIR}/KSegmentor/include
                     ${CMAKE_CURRENT_SOURCE_DIR}/Gao3DRSS/include
                     ${CMAKE_CURRENT_SOURCE_DIR}/sfm_local_chanvese/include
                     ${CMAKE_CURRENT_SOURCE_DIR}/MeshCurvolver-2.0/include
                    # ${CMAKE_CURRENT_SOURCE_DIR}/VTKtoITKConnection/include
   )


# Use the include path and library for Qt that is used by VTK.
include_directories(
        ${QT_include_DIR}
        ${KSlice_include}
)

add_subdirectory(util/getopt)  # DEPRECATED, Use boost::program_options instead, or raw argv[k] when lazy
add_subdirectory(util/vrcl)
add_subdirectory(KViewerMain)
add_subdirectory(SampleApplications)
add_subdirectory(KSegmentor)

set(BUILD_ITK_DEPENDENT_STUFF  CACHE bool false)
if(${BUILD_ITK_DEPENDENT_STUFF})
  message(STATUS "   found ITK, building with it! " )
  add_subdirectory(SampleApplications/Make3DModel)
  #add_subdirectory(Gao3DRSS)
  #add_subdirectory(SampleApplications/MHADecompressor)
else(${BUILD_ITK_DEPENDENT_STUFF})
  message(STATUS "   no ITK for you " )
endif(${BUILD_ITK_DEPENDENT_STUFF})

add_subdirectory(sfm_local_chanvese)
add_subdirectory(MeshCurvolver-2.0)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS "    build qtGUI_Demo? (set to 0 or 1) " )
if(${BUILD_QTGUI_DEMO_SH})
    message(STATUS "   yes, building qtGUI_Demo!  " )
    add_subdirectory(SampleApplications/qtGUI_Demo)
else(${BUILD_QTGUI_DEMO_SH})
    message(STATUS "   no qtGUI_Demo for you " )
endif(${BUILD_QTGUI_DEMO_SH})

message(STATUS "    Compiler:                  ${CMAKE_COMPILER} ")
message(STATUS "    C++ flags (Release):       ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "    C++ flags (Debug):         ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "    QT_LIBRARIES:              ${QT_LIBRARIES} " )
message(STATUS "    qt include dir:            ${QT_include_DIR} "  )

message(STATUS " fyi, bunch of image data is here ----   tsfat.bme.gatech.edu:/u4/public/karasev/data_medical/mha_grantmuller_kslice/mha_all/ " )

#...
