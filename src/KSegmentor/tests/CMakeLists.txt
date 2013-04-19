find_package( Boost COMPONENTS system filesystem program_options timer thread)
find_package( OpenMP )
link_directories ( ${Boost_LIBRARY_DIRS} )
include_directories ( ${Boost_INCLUDE_DIRS} )


add_executable(ksegmentor_test_KSeg3D  ksegmentor_test_KSegmentor3D.cpp)
target_link_libraries(ksegmentor_test_KSeg3D  -lboost_program_options ${Boost_LIBRARIES}
                      KSegmentor vrcl ${KSEGM_LIBS} )

add_executable(ksegmentor_test_InputAndRunSeg  ksegmentor_test_InputAndRunSeg.cpp)
target_link_libraries(ksegmentor_test_InputAndRunSeg  -lboost_program_options ${Boost_LIBRARIES}
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
           -O  ${CMAKE_SOURCE_DIR}/../data/tmpOut_ksegTest2.mha
        )

set(GetWebData CACHE bool true "wget data for real test, e.g. brain tumor (anonymous, legit, etc)")
if(GetWebData)
  #file(DOWNLOAD https://www.dropbox.com/s/azqa7lsdy88mzgs/DS004.mha  ${CMAKE_SOURCE_DIR}/../data/DS004.mha)
 # file(DOWNLOAD https://www.dropbox.com/s/azqa7lsdy88mzgs/DS004_Label0.mha  ${CMAKE_SOURCE_DIR}/../data/DS004_Label0.mha)
endif()

# Case 1: many inner loop iters
set(CITERS_LONG  "100")
set(NITERS_LONG  "5")

# Case 2:  many  key presess etc
set(CITERS_SHORT "10"  )
set(NITERS_SHORT "10"  )

macro(kseg3d_testGen  TEST_NAME ENERGY_TYPE      CITERS     NITERS)
add_test( ${TEST_NAME}
          ${CMAKE_BINARY_DIR}/bin/ksegmentor_test_InputAndRunSeg
           -I  ${CMAKE_SOURCE_DIR}/../data/DS004.mha
           -L  ${CMAKE_SOURCE_DIR}/../data/DS004_Label0.mha
           -C  ${CITERS}  # inner iteration of level set
           -N  ${NITERS}  # outer iterations, i.e. like key presses
           -E  ${ENERGY_TYPE}
           -O  ${CMAKE_SOURCE_DIR}/../data/tmpOut_${TEST_NAME}.mha
        )
endmacro()


kseg3d_testGen("SpeedTest_KSeg3D_Localized_CITERS_${CITERS_SHORT}"   "LocalCV" ${CITERS_SHORT}  ${NITERS_SHORT} )
kseg3d_testGen("SpeedTest_KSeg3D_Localized_CITERS_${CITERS_LONG}"   "LocalCV" ${CITERS_LONG}   ${NITERS_LONG}  )
kseg3d_testGen("SpeedTest_KSeg3D_ChanVese_CITERS_${CITERS_SHORT}"   "ChanVese" ${CITERS_SHORT}  ${NITERS_SHORT}  )
kseg3d_testGen("SpeedTest_KSeg3D_ChanVese_CITERS_${CITERS_LONG}"   "ChanVese" ${CITERS_LONG}   ${NITERS_LONG}   )
