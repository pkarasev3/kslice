# TODO: either done right as cmake files or hack a script to import the .pro
# project into the top level tree



# run shell script to force-clean
execute_process(COMMAND "sh" ${CMAKE_CURRENT_SOURCE_DIR}/build.sh
             ${EXECUTABLE_OUTPUT_PATH}  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}  )

#TODO: seems like too many libraries, why are there link errors without them
set( VTK_LIBS    vtkCommon vtksys QVTK  vtkViews vtkWidgets
vtkInfovis vtkRendering vtkGraphics vtkImaging vtkIO vtkFiltering
vtkDICOMParser vtksqlite   vtkexoIIc  vtkNetCDF  vtkmetaio  vtkftgl # vtkfreetype
vtkHybrid  vtkalglib vtkexpat vtkverdict vtkParallel VPIC Cosmo )

file( GLOB   sources *.h *.cpp *.cxx  )
message(STATUS "grabbed qmake files:  ${sources}, linking to qt libs: ${QT_LIBRARIES} " )

ADD_EXECUTABLE(qtGUI_Demo ${sources} )
TARGET_LINK_LIBRARIES(qtGUI_Demo  ${QT_LIBRARIES}  ${VTK_LIBS} QtOpenGL pthread )
