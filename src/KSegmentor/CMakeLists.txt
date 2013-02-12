file( GLOB ksegm_sources src/*.cxx   src/*.cpp )
file( GLOB ksegm_headers include/*.h  )

# MACOSX_BUNDLE

set( KSEGM_LIBS
  ${OpenCV_LIBS}
  vtkRendering  vtkGraphics
  vtkHybrid vtkImaging vtkIO
  vtkFiltering vtkCommon
)

add_library(KSegmentor  ${ksegm_headers} ${ksegm_sources})
target_link_libraries(KSegmentor   sfm_local_chanvese   vrcl ${KSEGM_LIBS} )

add_subdirectory( tests )

