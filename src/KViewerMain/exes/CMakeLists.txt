set(KV_LIBS KViewer QVTK  ${QT_LIBRARIES}  vtkRendering  vtkGraphics  vtkIO  vtkCommon  ${OpenCV_LIBS} ${Boost_LIBRARIES} )


add_executable( KViewer_desktop KViewer_desktop.cpp)
target_link_libraries( KViewer_desktop ${KV_LIBS})


add_executable( imageglob2mha imageglob2mha.cpp)
target_link_libraries( imageglob2mha ${KV_LIBS} )


add_executable( mha_decompress_resave mha_decompress_resave.cpp)
target_link_libraries( mha_decompress_resave ${KV_LIBS} )


add_executable( test8_ParametricSurface test8_ParametricSurface.cpp)
target_link_libraries( test8_ParametricSurface ${KV_LIBS} )

add_executable( test9_DataVolumeToSurface test9_DataVolumeToSurface.cpp)
target_link_libraries( test9_DataVolumeToSurface ${KV_LIBS} )

add_executable(compiler_compatibility_test  compiler_compatibility_test.cpp)
target_link_libraries(compiler_compatibility_test ${KV_LIBS})

