include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
set(MeshcurvolverTestCommon   meshcurvolver_test_common.cpp)

add_executable( test1_MeshCurvolverBasic            test1.cxx  ${MeshcurvolverTestCommon} )
add_executable( test2_MeshCurvolverComputeGeometry  test2_compute_geom.cxx ${MeshcurvolverTestCommon})
add_executable( test3_MeshCurvolverInitLevelset     test3_init_LZ_donut.cxx ${MeshcurvolverTestCommon})
add_executable( test4_MeshCurvolverGlyphs           test4_glyphs.cxx ${MeshcurvolverTestCommon})
add_executable( test5_MeshCurvolverDistanceFuncs    test5_distance_functions_from_markers.cxx ${MeshcurvolverTestCommon})
add_executable( test6_MeshCurvolverInitLevelset     test6_init_LZ_hard.cxx ${MeshcurvolverTestCommon})
add_executable( test7_MeshCurvolverEvolveLevelset     test7_evolve_LZ.cxx ${MeshcurvolverTestCommon})

target_link_libraries( test1_MeshCurvolverBasic            MeshContourEvolver ${OUR_VTK_REQUIRED_LIBS} )
target_link_libraries( test2_MeshCurvolverComputeGeometry  MeshContourEvolver ${OUR_VTK_REQUIRED_LIBS} )
target_link_libraries( test3_MeshCurvolverInitLevelset     MeshContourEvolver ${OUR_VTK_REQUIRED_LIBS} )
target_link_libraries( test4_MeshCurvolverGlyphs           MeshContourEvolver ${OUR_VTK_REQUIRED_LIBS} )
target_link_libraries( test5_MeshCurvolverDistanceFuncs    MeshContourEvolver ${OUR_VTK_REQUIRED_LIBS} )
target_link_libraries( test6_MeshCurvolverInitLevelset     MeshContourEvolver ${OUR_VTK_REQUIRED_LIBS} )
target_link_libraries( test7_MeshCurvolverEvolveLevelset     MeshContourEvolver ${OUR_VTK_REQUIRED_LIBS} )

if(EXISTS valgrind_demo.cpp)
    add_executable( valgrind_demo valgrind_demo.cpp )
endif()

