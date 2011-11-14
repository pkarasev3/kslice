# VTK examples now require CMake 2.0 or higher.


SET( VOLREND_VTK_REQUIRED_LIBS vtkRendering  vtkGraphics vtkHybrid  vtkWidgets vtkImaging vtkIO vtkFiltering vtkCommon vtkVolumeRendering)

add_executable( ComputeAndPrintLabelVolumes ComputeAndPrintLabelVolumes.cpp )
TARGET_LINK_LIBRARIES( ComputeAndPrintLabelVolumes KViewer KSegmentor ${OpenCV_LIBS} ${VOLREND_VTK_REQUIRED_LIBS} )


set(KV_LIBS KViewer QVTK  ${QT_LIBRARIES}  vtkRendering
    vtkGraphics  vtkIO  vtkCommon  ${OpenCV_LIBS} ${Boost_LIBRARIES} )
add_executable( ComputeAndPrintStatistics ComputeAndPrintStatistics.cpp)
target_link_libraries( ComputeAndPrintStatistics ${KV_LIBS})
