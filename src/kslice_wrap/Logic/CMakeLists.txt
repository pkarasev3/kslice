project(vtkSlicer${MODULE_NAME}ModuleLogic)

set(KIT ${PROJECT_NAME})
enable_testing()

set(${KIT}_EXPORT_DIRECTIVE "VTK_SLICER_${MODULE_NAME_UPPER}_MODULE_LOGIC_EXPORT")

set(${KIT}_INCLUDE_DIRECTORIES
  ${CMAKE_CURRENT_SOURCE_DIR}/../../KSegmentor/include/
  ${CMAKE_CURRENT_SOURCE_DIR}/../../KViewerMain/include/
  ${CMAKE_CURRENT_SOURCE_DIR}/../../util/vrcl/include/
  )

set(${KIT}_SRCS
  vtkSlicer${MODULE_NAME}Logic.cxx
  vtkSlicer${MODULE_NAME}Logic.h
  vtkKSlice.cpp 
  vtkKSlice.h 
)

link_directories(${KSLICE_LIB_PATH})

message(STATUS "Slicer_SOURCE_DIR is:  [ ${Slicer_SOURCE_DIR} ]...")

set(${KIT}_TARGET_LIBRARIES
  ${ITK_LIBRARIES}
    KSegmentor  sfm_local_chanvese
  )

#-----------------------------------------------------------------------------
SlicerMacroBuildModuleLogic(
  NAME ${KIT}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${${KIT}_INCLUDE_DIRECTORIES}
  SRCS ${${KIT}_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  )


add_executable(KSlice_smoketest      
  vtkKSlice.cpp 
  vtkKSlice.h
  KSlice_smoketest.cpp)
link_directories(${KSLICE_LIB_PATH})
target_link_libraries(KSlice_smoketest
  ${ITK_LIBRARIES} KSegmentor  sfm_local_chanvese  KSlice
)


# Add your test after this line, using SIMPLE_TEST( <testname> )
add_test(KSlice_smoketest_1 KSlice_smoketest  "test_img1.mha    2D")
add_test(KSlice_smoketest_2 KSlice_smoketest  "test_img2.mha    3D")

