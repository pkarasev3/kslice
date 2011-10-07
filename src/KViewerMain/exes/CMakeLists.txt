set(KV_LIBS KViewer QVTK  ${QT_LIBRARIES}  vtkRendering  vtkGraphics  vtkIO  vtkCommon  ${OpenCV_LIBS}  )

# MACOSX_BUNDLE - wtf?
add_executable( KViewer_desktop KViewer_desktop.cpp)
target_link_libraries( KViewer_desktop ${KV_LIBS})


add_executable( imageglob2mha imageglob2mha.cpp)
target_link_libraries( imageglob2mha ${KV_LIBS} )


add_executable( mha_decompress_resave mha_decompress_resave.cpp)
target_link_libraries( mha_decompress_resave ${KV_LIBS} )

