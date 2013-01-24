set(KV_LIBS KViewer QVTK  ${QT_LIBRARIES}  vtkRendering  vtkGraphics  vtkIO  vtkCommon  ${OpenCV_LIBS} ${Boost_LIBRARIES} )


add_executable( KViewer_desktop KViewer_desktop.cpp)
target_link_libraries( KViewer_desktop ${KV_LIBS})


