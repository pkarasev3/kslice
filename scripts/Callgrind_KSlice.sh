#
valgrind --tool=callgrind -v  ../build/bin/ksegmentor_test_InputAndRunSeg -C 5 -E LocalCV -N 1  -I  ../data/DS004.mha -L ../data/DS004_Label0.mha -O /tmp/cachegrindout.mha
                     

#           -I  ${CMAKE_SOURCE_DIR}/../data/DS004.mha\
#           -L  ${CMAKE_SOURCE_DIR}/../data/DS004_Label0.mha\
#           -C  ${CITERS}  # in
#           -N  ${NITERS}  # outer iterations, i.e. like key presses
#           -E  ${ENERGY_TYPE}
#           -O  ${CMAKE_SOURCE_DIR}/../data/tmpOut_${TEST_NAME}.mha
