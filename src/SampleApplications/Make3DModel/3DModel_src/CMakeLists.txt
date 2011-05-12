ADD_EXECUTABLE(Model3D Model3D.cxx)
TARGET_LINK_LIBRARIES(Model3D vtkRendering )


ADD_EXECUTABLE(MakeVTPandShow3D MakeVTPandShow3D.cpp)
TARGET_LINK_LIBRARIES(MakeVTPandShow3D vtkRendering )
