add_executable( ksegmentor_test1 ksegmentor_test1.cpp)
target_link_libraries( ksegmentor_test1  ${KSEGM_LIBS}  KSegmentor )

add_executable( savepng_test savepng_test.cpp)
target_link_libraries( savepng_test ${KSEGM_LIBS} )
