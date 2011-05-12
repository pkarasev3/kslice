#INCLUDE_DIRECTORIES( ${LSTVK_SOURCE_DIR} /module-code)

# Don't need ITK for now and it causes issues if ITK binaries have been deleted on windows
#FIND_PACKAGE ( ITK )
#IF (  ITK_FOUND  )
 #        INCLUDE(  ${USE_ITK_FILE}  )
#ENDIF(   ITK_FOUND   )

FIND_PACKAGE ( VTK )
IF (   VTK_FOUND   )
         INCLUDE(  ${USE_VTK_FILE}  )
ENDIF(   VTK_FOUND   )

message( STATUS "vtk use file: ${USE_VTK_FILE} ")

include_directories( include )
file( GLOB LSVTK_SRC  src/*.cxx src/*.cpp include/*.h)


ADD_LIBRARY( MeshContourEvolver STATIC ${LSVTK_SRC} )
TARGET_LINK_LIBRARIES (  MeshContourEvolver ${OUR_VTK_REQUIRED_LIBS} )

