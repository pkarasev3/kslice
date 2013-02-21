# Use the include path and library for Qt that is used by VTK.
include_DIRECTORIES(
        ${QT_include_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)


find_package(Boost COMPONENTS filesystem system python REQUIRED)
message("Include dirs of boost: " ${Boost_INCLUDE_DIRS} )
message("Libs of boost: " ${Boost_LIBRARIES} )

#find_package(PythonLibs REQUIRED) #this is now done by find_package(Slicer)
#message("Include dirs of Python: " ${PYTHON_INCLUDE_DIRS} )
#message("Libs of Python: " ${PYTHON_LIBRARIES} )

include_directories(
    ${Boost_INCLUDE_DIRS}
    ${PYTHON_INCLUDE_DIRS}
)

#set(Boost_USE_STATIC_LIBS ON)
#set(Boost_USE_MULTITHREADED ON)
#set(Boost_USE_STATIC_RUNTIME ON)

set(KV_LIBS vtkRendering  vtkGraphics  vtkIO  vtkCommon  vtkVolumeRendering
     KSegmentor  vrcl  ${OpenCV_LIBS} # should not need opencv now, see pk's branch
                       #${Boost_LIBRARIES}
                       ${PYTHON_LIBRARIES} )

set(KV_SRC                 ./src/KSandbox.cpp
                           ./src/KSlice.cpp
                           ./include/KSlice.h
                           ./include/KViewerOptions.h
                           ./include/KDataWarehouse.h
                           ./include/KSandbox.h)
# STANDALONE; makes libXKSlice.so
#add_library( XKSlice STATIC ${KV_SRC})
#set_target_properties( XKSlice PROPERTIES COMPILE_FLAGS "-fPIC" )
#target_link_libraries( XKSlice ${KV_LIBS})

# FOR WRAPPING; makes KSlice.so (explicitly set, must equal the PythonModule name)
add_library( KSlice SHARED ${KV_SRC})
target_link_libraries( KSlice  ${KV_LIBS})
set_target_properties( KSlice PROPERTIES PREFIX "" ) # want KSlice.so, not libKSlice.so
set_target_properties( KSlice PROPERTIES COMPILE_FLAGS "-fPIC" )

# STANDALONE
add_subdirectory(exes)
#add_subdirectory(KSlice_python)
