#include_directories( ${LSTVK_SOURCE_DIR} /module-code)

# Don't need ITK for now and it causes issues if ITK binaries have been deleted on windows
#find_package( ITK )
#if(  ITK_FOUND  )
 #        include(  ${USE_ITK_FILE}  )
#endif(   ITK_FOUND   )

find_package( VTK )
if(   VTK_FOUND   )
         include(  ${USE_VTK_FILE}  )
endif(   VTK_FOUND   )

message( STATUS "vtk use file: ${USE_VTK_FILE} ")

include_directories( include )
file( GLOB LSVTK_SRC  src/*.cxx src/*.cpp include/*.h)


add_library( MeshContourEvolver STATIC ${LSVTK_SRC} )
target_link_libraries(  MeshContourEvolver ${OUR_VTK_REQUIRED_LIBS} )

