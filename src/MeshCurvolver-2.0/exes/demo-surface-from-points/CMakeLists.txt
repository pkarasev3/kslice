add_executable(SurfaceFromPoints SurfaceFromPoints.cxx )
#  MeshContourEvolver     should be able to build and run without it!
target_link_libraries(SurfaceFromPoints   ${OUR_VTK_REQUIRED_LIBS} )
