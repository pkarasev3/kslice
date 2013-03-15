add_executable( ksegmentor_test_sfm2D3D ksegmentor_test_sfm2D3D.cpp)
target_link_libraries( ksegmentor_test_sfm2D3D  ${Boost_LIBRARIES} ${KSEGM_LIBS}  KSegmentor sfm_local_chanvese vrcl )

add_executable( savepng_test savepng_test.cpp)
target_link_libraries( savepng_test ${KSEGM_LIBS} )

add_executable(coupled_levelset_test coupled_levelset_test.cpp)
target_link_libraries(coupled_levelset_test ${Boost_LIBRARIES} KSegmentor  sfm_local_chanvese  vrcl ${KSEGM_LIBS} )

add_executable(ksegmentor_test_KSeg3D  ksegmentor_test_KSegmentor3D.cpp)
target_link_libraries(ksegmentor_test_KSeg3D ${Boost_LIBRARIES} KSegmentor vrcl ${KSEGM_LIBS} )


add_test( ksegmentor_test1_KSeg3D  # Name of Test
          ${CMAKE_BINARY_DIR}/bin/ksegmentor_test_KSeg3D  # Name of binary to run
           -I  ${CMAKE_BINARY_DIR}/data/noisy_parab_plane_intersect.mha
           -L  ${CMAKE_BINARY_DIR}/data/label_parab_slice0.mha
           -C  10
        )

add_test( ksegmentor_test2_KSeg3D  # Name of Test
          ${CMAKE_BINARY_DIR}/bin/ksegmentor_test_KSeg3D  # Name of binary to run
           -I  ${CMAKE_BINARY_DIR}/data/noisy_parab_plane_intersect.mha
           -L  ${CMAKE_BINARY_DIR}/data/label_parab_slice0.mha
           -C  50
           -O  ${CMAKE_BINARY_DIR}/data/tmpOutput_kseg_test2.mha
        )

