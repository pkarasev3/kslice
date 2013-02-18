

set(KV_LIBS XKSlice QVTK  ${QT_LIBRARIES}  vtkRendering  vtkGraphics
         vtkIO  vtkCommon  ${OpenCV_LIBS} ${PYTHON_LIBRARIES} ${Boost_LIBRARIES} )


add_executable( KViewer_desktop KViewer_desktop.cpp)
target_link_libraries( KViewer_desktop ${KV_LIBS})


