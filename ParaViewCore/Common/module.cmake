vtk_module(vtkPVCommon
  GROUPS
    ParaViewCore
  DEPENDS
    vtkCommonCore
    vtkIOXMLParser
    vtkClientServer
  TEST_DEPENDS
    vtkTestingCore
  TEST_LABELS
    PARAVIEW
)
