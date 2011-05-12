
set( MY_ITK_LIBS   ITKCommon ITKBasicFilters ITKIO itkvnl   )

file( GLOB  GAO3DRSS_SRC  include/*.txx include/*.h  src/*.cpp)
add_library( Gao3DRSS   ${GAO3DRSS_SRC}  )
target_link_libraries( Gao3DRSS  ${MY_ITK_LIBS} vrcl)


add_subdirectory( tests )
