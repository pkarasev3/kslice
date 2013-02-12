# VTK examples now require CMake 2.0 or higher.


add_executable( VolumeRenderVTKDemo VolumeRenderDemo.cxx )

set( VOLREND_VTK_REQUIRED_LIBS vtkRendering  vtkGraphics vtkHybrid  vtkWidgets vtkImaging vtkIO vtkFiltering vtkCommon vtkVolumeRendering)
target_link_libraries( VolumeRenderVTKDemo ${VOLREND_VTK_REQUIRED_LIBS} )

add_executable( MultiLabelVolumeRender MultiLabelVolumeRender.cpp )
target_link_libraries( MultiLabelVolumeRender KViewer KSegmentor ${OpenCV_LIBS} ${VOLREND_VTK_REQUIRED_LIBS} )

add_executable( LabelMapAsImplicitFunction LabelMapAsImplicitFunction.cpp )
target_link_libraries( LabelMapAsImplicitFunction ${VOLREND_VTK_REQUIRED_LIBS} )

add_executable( FlipVolumeAndSave_test  FlipVolumeAndSave_test.cpp )
target_link_libraries( FlipVolumeAndSave_test KViewer KSegmentor ${VOLREND_VTK_REQUIRED_LIBS} )


