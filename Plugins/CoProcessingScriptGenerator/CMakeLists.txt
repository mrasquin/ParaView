project(CoProcessingScriptGeneratorPlugin)

#--------------------------------------------------
# Find and Use ParaView
#--------------------------------------------------
IF (ParaView_SOURCE_DIR)
  INCLUDE_DIRECTORIES(
    ${VTK_INCLUDE_DIRS}
  )
ELSE (ParaView_SOURCE_DIR)
  FIND_PACKAGE(ParaView REQUIRED)
  INCLUDE(${PARAVIEW_USE_FILE})
ENDIF (ParaView_SOURCE_DIR)
 
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/Resources)

# Plugins for ParaView that add support to export state for CoProcessing etc.
#------------------------------------------------------------------------------
SET (CP_SOURCES
  pqCPPluginManager.cxx
  pqCPPluginManager.h
  pqCPWritersMenuManager.cxx
  pqCPWritersMenuManager.h
  pqCPActionsGroup.cxx
  pqCPActionsGroup.h
  pqCPExportStateWizard.cxx
  pqCPExportStateWizard.h
)

SET (UI_RESOURCES
  Resources/ExportStateWizard.ui
  Resources/ImageOutputInfo.ui)

INCLUDE(${QT_USE_FILE})

QT4_WRAP_CPP(MOC_SRCS
  pqCPPluginManager.h
  pqCPWritersMenuManager.h
  pqCPActionsGroup.h
  pqCPExportStateWizard.h
)

QT4_WRAP_UI(UI_SRCS
  ${UI_RESOURCES})

#------------------------------------------------------------------------------
SET (PARAVIEW_PLUGIN_IFACES)
SET (PARAVIEW_PLUGIN_SRCS)

SET (PARAVIEW_PLUGIN_IFACES2)
SET (PARAVIEW_PLUGIN_SRCS2)

# This is a macro for adding QActionGroup subclasses automatically as toolbars.
ADD_PARAVIEW_ACTION_GROUP(
  PARAVIEW_PLUGIN_IFACES
  PARAVIEW_PLUGIN_SRCS
  CLASS_NAME pqCPActionsGroup
  GROUP_NAME "MenuBar/CoProcessing")

ADD_PARAVIEW_AUTO_START(
  PARAVIEW_PLUGIN_IFACES2
  PARAVIEW_PLUGIN_SRCS2
  CLASS_NAME pqCPPluginManager)


ADD_PARAVIEW_PLUGIN(
  CoProcessingPlugin
  #"${CoProcessing_VERSION_MAJOR}.${CoProcessing_VERSION_MINOR}.${CoProcessing_VERSION_PATCH}"
  "0.1.0"
  SERVER_MANAGER_XML Resources/servermanager.xml
  GUI_INTERFACES
    ${PARAVIEW_PLUGIN_IFACES}
    ${PARAVIEW_PLUGIN_IFACES2}
  GUI_SOURCES
    ${CP_SOURCES}
    ${MOC_SRCS}
    ${UI_RESOURCES}
    ${PARAVIEW_PLUGIN_SRCS}
    ${PARAVIEW_PLUGIN_SRCS2}
)

IF (BUILD_TESTING)
  include(ParaViewTestingMacros)
  # this test is redundant with the CoProcessingFullWorkflow test
  # in the coprocessing library but that may not be built so we
  # add it here as well. this test though is a subset of that test.
  IF ( BUILD_SHARED_LIBS )
    add_client_tests("pv"
      TEST_SCRIPTS ${CMAKE_CURRENT_SOURCE_DIR}/Testing/CoProcessingGUI.xml )
    SET_TESTS_PROPERTIES(pv.CoProcessingGUI PROPERTIES RUN_SERIAL ON)
  ELSE ( BUILD_SHARED_LIBS )
    add_client_tests("pv"
      TEST_SCRIPTS ${CMAKE_CURRENT_SOURCE_DIR}/Testing/CoProcessingGUI-static.xml )
    SET_TESTS_PROPERTIES(pv.CoProcessingGUI-static PROPERTIES RUN_SERIAL ON)
  ENDIF ( BUILD_SHARED_LIBS )
ENDIF (BUILD_TESTING)
