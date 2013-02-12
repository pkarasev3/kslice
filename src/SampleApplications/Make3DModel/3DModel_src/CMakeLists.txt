add_executable(Model3D Model3D.cxx)
target_link_libraries(Model3D vtkRendering )


add_executable(MakeVTPandShow3D MakeVTPandShow3D.cpp)
target_link_libraries(MakeVTPandShow3D vtkRendering )
