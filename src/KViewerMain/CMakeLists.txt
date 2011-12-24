# Use the include path and library for Qt that is used by VTK.
include_DIRECTORIES(
        ${QT_include_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# run shell script to create the "moc_KViewer.cpp" auto-generated source file
IF(WIN32)
  execute_process(COMMAND ${SHELL} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/moc_creator_win.bat  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts )
ELSE(WIN32)
  execute_process(COMMAND "sh" ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate_moc.sh  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts )
ENDIF(WIN32)

file( GLOB KVIEWER_SRCS ./src/*.cxx ./include/*.h  ./src/*.cpp )

#TODO: BAD, VTKtoITK is a reusable chunk, are we sure this is where it goes?
# e.g. sounds more like a util perhaps
# Anyway this seems broken/deprecated, revisit with VTK 5.8+ and ITK later than 3.2
#file( GLOB VTKITKConnection ../VTKtoITKConnection/src/*.cxx ../VTKtoITKConnection/src/*.txx ../VTKtoITKConnection/include/*.h)
#if(${BUILD_ITK_DEPENDENT_STUFF}) # only build if we have ITK
#  set(KVIEWER_SRCS  ${KVIEWER_SRCS}  ${VTKITKConnection})
#endif(${BUILD_ITK_DEPENDENT_STUFF})

message(STATUS "kviewer sources:  ${KVIEWER_SRCS}")
add_library( KViewer  ${KVIEWER_SRCS} ${UI_SRCS}  )

TARGET_LINK_LIBRARIES( KViewer
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
