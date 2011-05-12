ADD_EXECUTABLE(SurfaceFromPoints SurfaceFromPoints.cxx )
#  MeshContourEvolver     should be able to build and run without it!
TARGET_LINK_LIBRARIES (SurfaceFromPoints   ${OUR_VTK_REQUIRED_LIBS} )
