# This is the root ITK CMakeLists file.
cmake_minimum_required(VERSION 2.4)

# This project is designed to be built outside the Insight source tree.
project(FileIO)

# Find ITK.
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(VTK REQUIRED)

add_executable(MHADecompressor ImageReadExtractWrite.cxx )
target_link_libraries(MHADecompressor   ITKIO)
target_link_libraries(MHADecompressor   ITKCommon)

add_executable( dicom2mha   dicom2mha.cpp )
target_link_libraries( dicom2mha   vtkRendering  vtkGraphics  vtkIO  vtkCommon )
