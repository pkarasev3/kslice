
add_executable( DemoStandaloneMeshCurvolver DemoStandaloneMeshCurvolver.cxx )
target_link_libraries( DemoStandaloneMeshCurvolver  MeshContourEvolver ${OUR_VTK_REQUIRED_LIBS} )

add_executable( SurfaceCurvatureNoGui SurfaceCurvatureNoGui.cxx)
target_link_libraries( SurfaceCurvatureNoGui MeshContourEvolver ${OUR_VTK_REQUIRED_LIBS} )

