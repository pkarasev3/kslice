# Use the include path and library for Qt that is used by VTK.
include_directories(
        ${QT_include_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_definitions( -DKVIEWER_LIB )

if(WIN32)
    set(ghetto_moc_cmd  ${QT_MOC_EXECUTABLE}  include/KViewer.h  -o  src/moc_KViewer.cpp)    
else()
    set(ghetto_moc_cmd  "sh  ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_moc.sh")
endif(WIN32)

set(ghetto_moc_workdir  ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_command(
    OUTPUT   ${CMAKE_CURRENT_SOURCE_DIR}/src/moc_KViewer.cpp                          
    COMMAND  ${ghetto_moc_cmd} #${QT_MOC_EXECUTABLE}  include/KViewer.h  -o  src/moc_KViewer.cpp
    DEPENDS  ${CMAKE_CURRENT_SOURCE_DIR}/include/KViewer.h
             ${CMAKE_CURRENT_SOURCE_DIR}/include/GUISetup.h
             ${CMAKE_CURRENT_SOURCE_DIR}/include/KViewerOptions.h
    WORKING_DIRECTORY ${ghetto_moc_workdir}
    COMMENT "...running ghetto_moc_cmd=${ghetto_moc_cmd}, working dir=${ghetto_moc_workdir}"  VERBATIM
)



set(QT_WRAP_UI "YES")

set(UI src/ParameterWidget.ui)

include_directories(${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${CMAKE_CURRENT_BINARY_DIR}/src
                    )
set(MOC_HDR  include/ParameterWidget.h)

QT4_WRAP_CPP(mocSRC  ${MOC_HDR} OPTIONS ${MOC_WARNING_FLG})
QT4_WRAP_UI(mocUI    ${UI} )
message(STATUS "QT moc: generated mocSRC=${mocSRC}, mocUI=${mocUI}")

if(0) # if have resources like images ...
    QT4_ADD_RESOURCES(resSrc ${QRC})
endif()

set(KVIEWER_SRCS
    src/KDataWarehouse.cpp
    src/KInteractiveLabelMap.cpp
    src/KSandbox.cpp
    src/KViewer.cpp
    src/KViewerOptions.cpp
    src/KVolumeRenderView.cpp
    src/KWidget_2D_left.cpp
    src/KWidget_3D_right.cpp
    src/KvtkImageInteractionCallback.cpp
    src/ParameterWidget.cpp
    src/moc_KViewer.cpp
)

set_source_files_properties( src/moc_KViewer.cpp PROPERTIES GENERATED 1)

set(KVIEWER_HDRS
    include/GUISetup.h
    include/KDataWarehouse.h
    include/KInteractiveLabelMap.h
    include/KSandbox.h
    include/KViewer.h
    include/KViewerOptions.h
    include/KVolumeRenderView.h
    include/KWidget_2D_left.h
    include/KWidget_3D_right.h
    include/KvtkImageInteractionCallback.h
    include/ParameterWidget.h
    include/kv_export.h
)


message(STATUS "kviewer sources:  ${KVIEWER_SRCS}")
add_library( KViewer  ${KVIEWER_SRCS} ${KVIEWER_HDRS} ${mocSRC} ${mocUI}  )

target_link_libraries( KViewer
  QVTK
  ${QT_LIBRARIES}
  vtkRendering
  vtkGraphics
  vtkIO
  vtkCommon
  vtkVolumeRendering
  KSegmentor
  vrcl
)


add_subdirectory(exes)
