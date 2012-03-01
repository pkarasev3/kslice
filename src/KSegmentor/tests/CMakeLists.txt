add_executable(ksegmentor_test1_sfls_opencv ksegmentor_test1_sfls_opencv.cpp)
target_link_libraries(ksegmentor_test1_sfls_opencv  ${KSEGM_LIBS}  KSegmentor )

add_executable( ksegmentor_test2_sfm2D3D ksegmentor_test2_sfm2D3D.cpp)
target_link_libraries( ksegmentor_test2_sfm2D3D  ${Boost_LIBRARIES} ${KSEGM_LIBS}  KSegmentor sfm_local_chanvese vrcl )

add_executable( savepng_test savepng_test.cpp)
target_link_libraries( savepng_test ${KSEGM_LIBS} )

add_executable(coupled_levelset_test1 coupled_levelset_test1.cpp)
target_link_libraries(coupled_levelset_test1 ${Boost_LIBRARIES} KSegmentor  sfm_local_chanvese  vrcl ${KSEGM_LIBS} )

