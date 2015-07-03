file( GLOB ksegm_sources src/*.cxx   src/*.cpp )
file( GLOB ksegm_headers include/*.h  )

# MACOSX_BUNDLE

message(STATUS "{QT_QTCORE_LIBRARY}  {QT_LIBRARIES}= ${QT_QTCORE_LIBRARY}  ${QT_LIBRARIES}")

set( KSEGM_LIBS
  ${OpenCV_LIBS}  ${Boost_LIBRARIES}
  vtkRendering  vtkGraphics
  vtkHybrid vtkImaging vtkIO
  vtkFiltering vtkCommon
  ${QT_LIBRARIES}
)

add_library(KSegmentor  ${ksegm_headers} ${ksegm_sources})
target_link_libraries(KSegmentor   sfm_local_chanvese   vrcl ${KSEGM_LIBS} )

add_subdirectory( tests )

