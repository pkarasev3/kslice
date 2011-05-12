
ADD_EXECUTABLE( DemoStandaloneMeshCurvolver DemoStandaloneMeshCurvolver.cxx )
TARGET_LINK_LIBRARIES ( DemoStandaloneMeshCurvolver  MeshContourEvolver ${OUR_VTK_REQUIRED_LIBS} )

ADD_EXECUTABLE( SurfaceCurvatureNoGui SurfaceCurvatureNoGui.cxx)
TARGET_LINK_LIBRARIES ( SurfaceCurvatureNoGui MeshContourEvolver ${OUR_VTK_REQUIRED_LIBS} )

