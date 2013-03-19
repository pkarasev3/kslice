"""Helper functions for developing - see bottom for key bindings"""

def load_default_volume():
  if not getNodes().has_key('moving'):
    #fileName = slicer.app.slicerHome + "/share/MRML/Testing/TestData/moving.nrrd"
    fileName = os.environ['HOME'] + "/Dropbox/data/faces/neutral.nrrd"
    vl = slicer.modules.volumes.logic()
    volumeNode = vl.AddArchetypeScalarVolume (fileName, "moving", 0)

    # automatically select the volume to display
    appLogic = slicer.app.applicationLogic()
    selNode = appLogic.GetSelectionNode()
    selNode.SetReferenceActiveVolumeID(volumeNode.GetID())
    appLogic.PropagateVolumeSelection()


def LabelStatistics():
  print "SlicerRC - LabelStatistics setup..."
  import imp, sys, os
  path = '%s/../../Slicer/Modules/Scripted/Scripts' % slicer.app.slicerHome
  if not sys.path.__contains__(path):
    sys.path.insert(0,path)

  mod = "LabelStatistics"
  sourceFile = path + "/LabelStatistics.py"
  fp = open(sourceFile, "r")
  globals()[mod] = imp.load_module(mod, fp, sourceFile, ('.py', 'r', imp.PY_SOURCE))
  fp.close()

  globals()['ls'] = globals()[mod].LabelStatisticsWidget()

def endoscopy():
  print "SlicerRC - endoscopy setup..."
  import imp, sys, os
  scriptPath = '%s/../../Slicer/Modules/Scripted/Scripts' % slicer.app.slicerHome
  if not sys.path.__contains__(scriptPath):
    sys.path.insert(0,scriptPath)

  mod = "Endoscopy"
  sourceFile = scriptPath + "/Endoscopy.py"
  fp = open(sourceFile, "r")
  globals()[mod] = imp.load_module(mod, fp, sourceFile, ('.py', 'r', imp.PY_SOURCE))
  fp.close()

  globals()['e'] = e = globals()[mod].EndoscopyWidget()

def labelStatistics():
  print "SlicerRC - labelStatistics setup..."
  import imp, sys, os
  scriptPath = '%s/../../Slicer/Modules/Scripted/Scripts' % slicer.app.slicerHome
  if not sys.path.__contains__(scriptPath):
    sys.path.insert(0,scriptPath)

  mod = "LabelStatistics"
  sourceFile = scriptPath + "/LabelStatistics.py"
  fp = open(sourceFile, "r")
  globals()[mod] = imp.load_module(mod, fp, sourceFile, ('.py', 'r', imp.PY_SOURCE))
  fp.close()

  globals()['l'] = l = globals()[mod].LabelStatisticsWidget()


def editor():
  print "SlicerRC - editor setup..."
  import imp, sys, os
  import slicer
  tcl("set ::guipath %s/../../Slicer/Base/GUI/Tcl" % slicer.app.slicerHome )
  tcl("if { [lsearch $::auto_path $::guipath] == -1 } { set ::auto_path [list %s/../../Slicer/Base/GUI/Tcl $::auto_path] } " % slicer.app.slicerHome)
  tcl("package forget SlicerBaseGUITcl")
  tcl("package require SlicerBaseGUITcl")
  #tcl("EffectSWidget::RemoveAll")
  #tcl("EffectSWidget::Add DrawEffect")

  if 0 and not getNodes().has_key('2006-spgr'):
    slicer.mrmlScene.SetURL('/home/pieper/data/edit/edit-small.mrml')
    slicer.mrmlScene.Connect()

  if 0 and not getNodes().has_key('CTA-cardio'):
    slicer.mrmlScene.SetURL('/home/pieper/data/edit/edit.mrml')
    slicer.mrmlScene.Connect()

  if 0 and not slicer.util.getNodes('MR-head*'):
    slicer.util.loadVolume('/Users/pieper/data/MR-head.nrrd')

  if not slicer.util.getNodes('kidney*'):
    slicer.util.loadVolume('/Users/pieper/Dropbox/data/kidney.nrrd')

  editorLibPath = '%s/../../Slicer/Modules/Scripted/EditorLib' % slicer.app.slicerHome
  if not sys.path.__contains__(editorLibPath):
    sys.path.insert(0, editorLibPath)
  editorPath = '%s/../../Slicer/Modules/Scripted/Scripts' % slicer.app.slicerHome
  if not sys.path.__contains__(editorPath):
    sys.path.insert(0,editorPath)


  modules = (
      "EditColor", "EditOptions", "EditBox", "ColorBox", "HelperBox",
      "LabelEffect", "PaintEffect", "DrawEffect",
      )
  for mod in modules:
    sourceFile = editorLibPath + "/" + mod + ".py"
    fp = open(sourceFile, "r")
    globals()[mod] = imp.load_module(mod, fp, sourceFile, ('.py', 'r', imp.PY_SOURCE))
    fp.close()
    exec("globals()['EditorLib'].%s = globals()['%s'].%s" % (mod,mod,mod))

  mod = "Editor"
  sourceFile = editorPath + "/Editor.py"
  fp = open(sourceFile, "r")
  globals()[mod] = imp.load_module(mod, fp, sourceFile, ('.py', 'r', imp.PY_SOURCE))
  fp.close()

  # hide the current widget
  
  mainWindow = slicer.util.mainWindow()
  mainWindow.moduleSelector().selectModule('Editor')

  parent = findChildren(text='Create and Select*')[0].parent()
  for child in parent.children():
    try:
      child.hide()
    except AttributeError:
      pass

  globals()['e'] = e = globals()[mod].EditorWidget(parent)
  e.setup()
  e.helper.setMasterVolume(slicer.util.getNode('MR-head'))
  #e.toolsBox.reloadExtensions()

def fileScan():
  print "SlicerRC - fileScan setup..."
  import imp, sys, os
  p = '%s/../../Slicer/Modules/Scripted/Scripts' % slicer.app.slicerHome
  if not sys.path.__contains__(p):
    sys.path.insert(0,p)


  mod = "FileScan"
  sourceFile = p + "/FileScan.py"
  fp = open(sourceFile, "r")
  globals()[mod] = imp.load_module(mod, fp, sourceFile, ('.py', 'r', imp.PY_SOURCE))
  fp.close()

  globals()['e'] = e = globals()[mod].FileScanWidget()

def performance():
  print "SlicerRC - performance setup..."
  import os
  execfile(slicer.app.slicerHome + "/../../Slicer/Modules/Scripted/Scripts/PerformanceTests.py")
  globals()['p'] = p = globals()[mod].PerformanceTestsWidget()


  #load_default_volume()

  #reslicing(10)
  #timeProbe()
  #global slt
  #slt = sliceLogicTest()

def slicr_setup():
  print "slicr - server setup..."

  load_default_volume()
  import imp, sys, os
  p = '/Users/pieper/Dropbox/webgl/slicr'
  if not sys.path.__contains__(p):
    sys.path.insert(0,p)


  mod = "slicr"
  sourceFile = p + "/slicr.py"
  fp = open(sourceFile, "r")
  globals()[mod] = imp.load_module(mod, fp, sourceFile, ('.py', 'r', imp.PY_SOURCE))
  fp.close()

  globals()['s'] = globals()[mod].slicr_command_processor()
  globals()['s'].start()

def DICOM_setup():
  print "SlicerRC - DICOM setup..."
  import imp, sys, os
  path = '%s/../../Slicer/Modules/Scripted/Scripts' % slicer.app.slicerHome
  if not sys.path.__contains__(path):
    sys.path.insert(0,path)

  if False:
    # TODO: reload dicomlib
    dicomLibPath = '%s/../../Slicer/Modules/Scripted/DICOMLib' % slicer.app.slicerHome
    if not sys.path.__contains__(dicomLibPath):
      sys.path.insert(0, dicomLibPath)


    modules = ("DICOMServers", "DICOMDataExchange", "DICOMWidgets")
    for mod in modules:
      sourceFile = dicomLibPath + "/" + mod + ".py"
      fp = open(sourceFile, "r")
      globals()[mod] = imp.load_module(mod, fp, sourceFile, ('.py', 'r', imp.PY_SOURCE))
      fp.close()
      exec("globals()['DICOMLib'].%s = globals()['%s'].%s" % (mod,mod,mod))

  mod = "DICOM"
  sourceFile = path + "/DICOM.py"
  fp = open(sourceFile, "r")
  globals()[mod] = imp.load_module(mod, fp, sourceFile, ('.py', 'r', imp.PY_SOURCE))
  fp.close()

  globals()['DICOM'] = globals()[mod].DICOMWidget()

def grabToClipboard(widget):
  pixmap = qt.QPixmap.grabWidget(widget)
  slicer.app.clipboard().setPixmap(pixmap)

def grabMainToClipboard():
  grabToClipboard(slicer.util.mainWindow())

def grabPythonToClipboard():
  grabToClipboard(slicer.util.pythonShell())


def setupMacros():
  """Set up hot keys for various development scenarios"""
  
  import qt
  global load_default_volume, multivolume, endoscopy, labelStatistics, editor, fileScan, performance, slicr_setup, DICOM_setup
  global grabPythonToClipboard, grabMainToClipboard, grabToClipboard
  
  print "SlicerRC - Install custom keyboard shortcuts"
  
  import sys
  if sys.platform == 'darwin':
    modifier = 'Shift+Meta'
  else:
    modifier = 'Shift+Ctrl'

  macros = (
    ("0", loadSlicerRCFile),
    ("1", LabelStatistics),
    ("2", endoscopy),
    ("3", editor),
    ("4", fileScan),
    ("5", performance),
    ("6", slicr_setup),
    ("7", DICOM_setup),
    ("p", grabPythonToClipboard),
    ("c", grabMainToClipboard),
    )
      
  windows = (slicer.util.mainWindow(), slicer.util.pythonShell())
  for window in windows:
    for keys,f in macros:
      k = qt.QKeySequence( "%s+%s" % (modifier,keys) )
      s = qt.QShortcut(k,mainWindow())
      s.connect('activated()', f)
      s.connect('activatedAmbiguously()', f)
      print "SlicerRC - '%s' -> '%s'" % (keys, f.__name__)
      
# Install macros
if mainWindow(verbose=False): setupMacros()


# Display current time
from time import gmtime, strftime
print "Slicer RC file loaded [%s]" % strftime("%d/%m/%Y %H:%M:%S", gmtime())

# always show shell when using macros
pythonShell()
