
add_executable(ksegmentor_test_KSeg3D  ksegmentor_test_KSegmentor3D.cpp)
target_link_libraries(ksegmentor_test_KSeg3D -lboost_program_options ${Boost_LIBRARIES}
                      KSegmentor vrcl ${KSEGM_LIBS} )


add_test( ksegmentor_test_KSeg3D_1  # Name of Test
          ${CMAKE_BINARY_DIR}/bin/ksegmentor_test_KSeg3D  # Name of binary to run
           -I  ${CMAKE_SOURCE_DIR}/../data/noisy_parab_plane_intersect.mha
           -L  ${CMAKE_SOURCE_DIR}/../data/label_parab_slice0.mha
           -C  50
        )

add_test( ksegmentor_test_KSeg3D_2  # Name of Test
          ${CMAKE_BINARY_DIR}/bin/ksegmentor_test_KSeg3D  # Name of binary to run
           -I  ${CMAKE_SOURCE_DIR}/../data/noisy_parab_plane_intersect.mha
           -L  ${CMAKE_SOURCE_DIR}/../data/label_parab_slice0.mha
           -C  50
           -O  ${CMAKE_SOURCE_DIR}/../data/tmpOutput_kseg_test2.mha
        )




