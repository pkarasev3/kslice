# Use the include path and library for Qt that is used by VTK.
include_DIRECTORIES(
        ${QT_include_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

include_directories( ${PYTHON_INCLUDE_DIRS} )

set(KV_LIBS vtkRendering  vtkGraphics  vtkIO  vtkCommon  vtkVolumeRendering
             ${PYTHON_LIBRARIES}  KSegmentor  ) 


set(KV_SRC                 ./src/KSandbox.cpp
                           ./include/KViewerOptions.h
                           ./include/KDataWarehouse.h
                           ./include/KSandbox.h)

# FOR WRAPPING; makes KSlice.so (explicitly set, must equal the PythonModule name)
add_library( KSlice STATIC ${KV_SRC})
target_link_libraries( KSlice  ${KV_LIBS})
set_target_properties( KSlice PROPERTIES PREFIX "" ) # want KSlice.so, not libKSlice.so
set_target_properties( KSlice PROPERTIES COMPILE_FLAGS "-fPIC" )
