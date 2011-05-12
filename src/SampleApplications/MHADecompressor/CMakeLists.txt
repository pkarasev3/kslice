# This is the root ITK CMakeLists file.
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

# This project is designed to be built outside the Insight source tree.
PROJECT(FileIO)

# Find ITK.
FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})

find_package(VTK REQUIRED)

ADD_EXECUTABLE(MHADecompressor ImageReadExtractWrite.cxx )
TARGET_LINK_LIBRARIES(MHADecompressor   ITKIO)
TARGET_LINK_LIBRARIES(MHADecompressor   ITKCommon)

add_executable( dicom2mha   dicom2mha.cpp )
target_link_libraries( dicom2mha   vtkRendering  vtkGraphics  vtkIO  vtkCommon )
