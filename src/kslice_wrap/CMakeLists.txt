

enable_testing()



#-----------------------------------------------------------------------------
set(MODULE_NAME KSlice)
set(MODULE_TITLE ${MODULE_NAME})

string(TOUPPER ${MODULE_NAME} MODULE_NAME_UPPER)

#-----------------------------------------------------------------------------
#if(NOT Slicer_SOURCE_DIR)
#  find_package(Slicer REQUIRED)
#  include(${Slicer_USE_FILE})
#endif()

#-----------------------------------------------------------------------------
add_subdirectory(Logic)
add_subdirectory(Widgets)

#-----------------------------------------------------------------------------
set(MODULE_EXPORT_DIRECTIVE "Q_SLICER_QTMODULES_${MODULE_NAME_UPPER}_EXPORT")

# Current_{source,binary} and Slicer_{Libs,Base} already included
set(MODULE_INCLUDE_DIRECTORIES
  ${CMAKE_CURRENT_SOURCE_DIR}/Logic
  ${CMAKE_CURRENT_BINARY_DIR}/Logic
  ${CMAKE_CURRENT_SOURCE_DIR}/Widgets
  ${CMAKE_CURRENT_BINARY_DIR}/Widgets
  )

set(MODULE_SRCS
  qSlicer${MODULE_NAME}Module.cxx
  qSlicer${MODULE_NAME}Module.h
  qSlicer${MODULE_NAME}ModuleWidget.cxx
  qSlicer${MODULE_NAME}ModuleWidget.h
  )

set(MODULE_MOC_SRCS
  qSlicer${MODULE_NAME}Module.h
  qSlicer${MODULE_NAME}ModuleWidget.h
  )

set(MODULE_UI_SRCS
  Resources/UI/qSlicer${MODULE_NAME}Module.ui
  )

set(MODULE_TARGET_LIBRARIES
  vtkSlicer${MODULE_NAME}ModuleLogic
  qSlicer${MODULE_NAME}ModuleWidgets
  )

set(MODULE_RESOURCES
  Resources/qSlicer${MODULE_NAME}Module.qrc
  )

link_directories(${KSLICE_LIB_PATH})

#-----------------------------------------------------------------------------
slicerMacroBuildQtModule(
  NAME ${MODULE_NAME}
  TITLE ${MODULE_TITLE}
  EXPORT_DIRECTIVE ${MODULE_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${MODULE_INCLUDE_DIRECTORIES}
  SRCS ${MODULE_SRCS}
  MOC_SRCS ${MODULE_MOC_SRCS}
  UI_SRCS ${MODULE_UI_SRCS}
  TARGET_LIBRARIES ${MODULE_TARGET_LIBRARIES}
  RESOURCES ${MODULE_RESOURCES}
  )

#-----------------------------------------------------------------------------
##Set up the Editor module


set(MODULE_PYTHON_SCRIPTS
 #__init__
  KUtil
  KSliceEffect
  )

set(MODULE_PYTHON_RESOURCES
  KSliceEffect.png
  )

#-----------------------------------------------------------------------------
#ctkMacroCompilePythonScript(
 # TARGET_NAME KUtil
 # SCRIPTS "KUtil.py"
 # RESOURCES ""
 # DESTINATION_DIR ${CMAKE_BINARY_DIR}/${Slicer_QTSCRIPTEDMODULES_LIB_DIR}
#  INSTALL_DIR ${Slicer_INSTALL_ROOT}
 # )

set(CMAKE_INSTALL_PREFIX  ${Slicer_DIR} CACHE PATH  "cmake install prefix")

ctkMacroCompilePythonScript(
  TARGET_NAME KSliceEffect
  SCRIPTS "${MODULE_PYTHON_SCRIPTS}"
  RESOURCES "${MODULE_PYTHON_RESOURCES}"
  DESTINATION_DIR ${CMAKE_BINARY_DIR}/${Slicer_QTSCRIPTEDMODULES_LIB_DIR}
  INSTALL_DIR ${Slicer_INSTALL_BIN_DIR}
  )


#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()

#-----------------------------------------------------------------------------
#if(NOT Slicer_SOURCE_DIR)   # why? look at slicerRT
include(${Slicer_EXTENSION_CPACK})
#endif()
#-----------------------------------------------------------------------------
set(CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${CMAKE_BINARY_DIR};${EXTENSION_NAME};ALL;/")

