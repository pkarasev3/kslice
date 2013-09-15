
if(False)

# this directory is questionable on all platforms it seems

set(KV_LIBS KSlice QVTK  ${QT_LIBRARIES}  vtkRendering  vtkGraphics
         vtkIO  vtkCommon   ) #${OpenCV_LIBS}

set(KV_SRC      ${CMAKE_CURRENT_SOURCE_DIR}/../../kslice_wrap/Logic/vtkKSlice.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/../../kslice_wrap/Logic/vtkKSlice.h
                ${CMAKE_BINARY_DIR}/kslice_wrap/Logic/vtkSlicerKSliceModuleLogicExport.h ) #a file called vtkSlicerKSliceModuleLogicExport.h gets created

# Tell cmake to delay looking for this file (this is needed, but not enough)
SET_SOURCE_FILES_PROPERTIES(${CMAKE_BINARY_DIR}/kslice_wrap/Logic/vtkSlicerKSliceModuleLogicExport.h  PROPERTIES GENERATED 1)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../../kslice_wrap/Logic/
                     ${CMAKE_BINARY_DIR}/kslice_wrap/Logic/)

if(WIN32)
	message(STATUS "skipping KViewer standalone on windows")
	# why: probably the app needs to define something like "VTK_IMPORT_HEADERS" to use .dll's correctly
else()
	add_executable( KViewer_desktop KViewer_desktop.cpp ${KV_SRC} )
	target_link_libraries( KViewer_desktop ${KV_LIBS})
    add_dependencies(KViewer_desktop  vtkSlicerKSliceModuleLogic) # let the source files generate first ... (This is not enough...)
endif(WIN32)

endif()
