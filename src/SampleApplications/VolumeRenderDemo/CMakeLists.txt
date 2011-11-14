# VTK examples now require CMake 2.0 or higher.


add_executable( VolumeRenderVTKDemo VolumeRenderDemo.cxx )

SET( VOLREND_VTK_REQUIRED_LIBS vtkRendering  vtkGraphics vtkHybrid  vtkWidgets vtkImaging vtkIO vtkFiltering vtkCommon vtkVolumeRendering)
TARGET_LINK_LIBRARIES( VolumeRenderVTKDemo ${VOLREND_VTK_REQUIRED_LIBS} )

add_executable( MultiLabelVolumeRender MultiLabelVolumeRender.cpp )
TARGET_LINK_LIBRARIES( MultiLabelVolumeRender KSegmentor ${OpenCV_LIBS} ${VOLREND_VTK_REQUIRED_LIBS} )

add_executable( LabelMapAsImplicitFunction LabelMapAsImplicitFunction.cpp )
TARGET_LINK_LIBRARIES( LabelMapAsImplicitFunction ${VOLREND_VTK_REQUIRED_LIBS} )

add_executable( FlipVolumeAndSave_test  FlipVolumeAndSave_test.cpp )
TARGET_LINK_LIBRARIES( FlipVolumeAndSave_test KViewer KSegmentor ${VOLREND_VTK_REQUIRED_LIBS} )


