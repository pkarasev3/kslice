set( OUR_VTK_REQUIRED_LIBS vtkRendering  vtkGraphics vtkHybrid vtkImaging vtkIO vtkFiltering vtkCommon)

file( GLOB LSVTK_SRC  src/*.cxx src/*.cpp include/*.h)
add_library( MeshContourEvolver ${LSVTK_SRC} )
target_link_libraries(  MeshContourEvolver vrcl  ${OpenCV_LIBS}  ${OUR_VTK_REQUIRED_LIBS} )


add_subdirectory(exes/demo-standalone-code)

add_subdirectory(exes/demo-surface-from-points)

add_subdirectory(tests)


