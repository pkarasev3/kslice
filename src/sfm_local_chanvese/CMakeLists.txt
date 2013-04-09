file( GLOB  SFM_LOCAL_CHANVESE_SRC  include/*.txx include/*.h  src/*.cpp)
add_library( sfm_local_chanvese  SHARED ${SFM_LOCAL_CHANVESE_SRC}  )
target_link_libraries( sfm_local_chanvese )
