# VTK examples now require CMake 2.0 or higher.


set( VOLREND_VTK_REQUIRED_LIBS vtkRendering  vtkGraphics vtkHybrid  vtkWidgets vtkImaging vtkIO vtkFiltering vtkCommon vtkVolumeRendering)

add_executable( ComputeAndPrintLabelVolumes ComputeAndPrintLabelVolumes.cpp )
target_link_libraries( ComputeAndPrintLabelVolumes KViewer KSegmentor ${OpenCV_LIBS} ${VOLREND_VTK_REQUIRED_LIBS} )


set(KV_LIBS KViewer QVTK  ${QT_LIBRARIES}  vtkRendering
    vtkGraphics  vtkIO  vtkCommon  ${OpenCV_LIBS} ${Boost_LIBRARIES} )
add_executable( ComputeAndPrintStatistics ComputeAndPrintStatistics.cpp)
target_link_libraries( ComputeAndPrintStatistics ${KV_LIBS})


if(${BUILD_ITK_DEPENDENT_STUFF})
    include(${ITK_USE_FILE})
    add_executable( ResampleVolumesToBeIsotropic ResampleVolumesToBeIsotropic.cpp)
    target_link_libraries( ResampleVolumesToBeIsotropic ${KV_LIBS}  ITKIO ITKCommon ITKBasicFilters)

# Version of ITK mismatch? Anyway this app doesn't achieve intended goal
# add_executable( AntiAliasBinaryImage AntiAliasBinaryImage.cpp)
# target_link_libraries( AntiAliasBinaryImage ${KV_LIBS}  ITKIO ITKCommon ITKBasicFilters)

endif(${BUILD_ITK_DEPENDENT_STUFF})
