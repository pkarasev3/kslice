import os
from __main__ import vtk, qt, ctk, slicer
import EditorLib
from EditorLib.EditOptions import HelpButton
from EditorLib.EditOptions import EditOptions
from EditorLib import EditUtil
from EditorLib import LabelEffect

import pyopencl
import pyopencl.array
import numpy


#
# The Editor Extension itself.
# 
# This needs to define the hooks to become an editor effect.
#

#
# GrowCutCLOptions - see LabelEffect, EditOptions and Effect for superclasses
#

class GrowCutCLOptions(EditorLib.LabelEffectOptions): #gets instantiated when go into our module
  """ GrowCutCL-specfic gui
  """

  def __init__(self, parent=0):
    super(GrowCutCLOptions,self).__init__(parent)

    # self.attributes should be tuple of options:
    # 'MouseTool' - grabs the cursor
    # 'Nonmodal' - can be applied while another is active
    # 'Disabled' - not available
    self.attributes = ('MouseTool')
    self.displayName = 'GrowCutCL Effect'

    self.bot = None

  def __del__(self):
    super(GrowCutCLOptions,self).__del__()

  def create(self):
    super(GrowCutCLOptions,self).create()

    self.botButton = qt.QPushButton(self.frame)
    if hasattr(slicer.modules, 'editorBot'):
      self.botButton.text = "Stop Bot"
    else:
      self.botButton.text = "Start Bot"
    self.frame.layout().addWidget(self.botButton)

    self.acceptButton = qt.QPushButton(self.frame)
    self.acceptButton.text = "Accept Steered Label"
    self.frame.layout().addWidget(self.acceptButton)

    self.toleranceFrame = qt.QFrame(self.frame)
    self.toleranceFrame.setLayout(qt.QHBoxLayout())
    self.frame.layout().addWidget(self.toleranceFrame)
    self.widgets.append(self.toleranceFrame)
    self.toleranceLabel = qt.QLabel("Tolerance:", self.toleranceFrame)
    self.toleranceLabel.setToolTip("Set the tolerance of the wand in terms of background pixel values")
    self.toleranceFrame.layout().addWidget(self.toleranceLabel)
    self.widgets.append(self.toleranceLabel)
    self.toleranceSpinBox = qt.QDoubleSpinBox(self.toleranceFrame)
    self.toleranceSpinBox.setToolTip("Set the tolerance of the wand in terms of background pixel values")
    self.toleranceSpinBox.minimum = 0
    self.toleranceSpinBox.maximum = 1000
    self.toleranceSpinBox.suffix = ""
    self.toleranceFrame.layout().addWidget(self.toleranceSpinBox)
    self.widgets.append(self.toleranceSpinBox)

    self.maxPixelsFrame = qt.QFrame(self.frame)
    self.maxPixelsFrame.setLayout(qt.QHBoxLayout())
    self.frame.layout().addWidget(self.maxPixelsFrame)
    self.widgets.append(self.maxPixelsFrame)
    self.maxPixelsLabel = qt.QLabel("Max Pixels per click:", self.maxPixelsFrame)
    self.maxPixelsLabel.setToolTip("Set the maxPixels for each click")
    self.maxPixelsFrame.layout().addWidget(self.maxPixelsLabel)
    self.widgets.append(self.maxPixelsLabel)
    self.maxPixelsSpinBox = qt.QDoubleSpinBox(self.maxPixelsFrame)
    self.maxPixelsSpinBox.setToolTip("Set the maxPixels for each click")
    self.maxPixelsSpinBox.minimum = 1
    self.maxPixelsSpinBox.maximum = 1000
    self.maxPixelsSpinBox.suffix = ""
    self.maxPixelsFrame.layout().addWidget(self.maxPixelsSpinBox)
    self.widgets.append(self.maxPixelsSpinBox)

    HelpButton(self.frame, "Use this tool to label all voxels that are within a tolerance of where you click")

    self.botButton.connect('clicked()', self.onStartBot)
    self.acceptButton.connect('clicked()', self.onAccept)
    self.toleranceSpinBox.connect('valueChanged(double)', self.onToleranceSpinBoxChanged)
    self.maxPixelsSpinBox.connect('valueChanged(double)', self.onMaxPixelsSpinBoxChanged)

    # Add vertical spacer
    self.frame.layout().addStretch(1)

    # TODO: the functionality for the steered volume should migrate to
    # the edit helper class when functionality is finalized.
    backgroundVolume = self.editUtil.getBackgroundVolume()
    labelVolume = self.editUtil.getLabelVolume()
    steeredName = backgroundVolume.GetName() + '-steered'
    steeredVolume = slicer.util.getNode(steeredName)
    if not steeredVolume:
      volumesLogic = slicer.modules.volumes.logic()
      steeredVolume = volumesLogic.CloneVolume(
                           slicer.mrmlScene, labelVolume, steeredName)
    compositeNodes = slicer.util.getNodes('vtkMRMLSliceCompositeNode*')
    for compositeNode in compositeNodes.values():
      compositeNode.SetForegroundVolumeID(steeredVolume.GetID())
      compositeNode.SetForegroundOpacity(0.5)


  def destroy(self):
    super(GrowCutCLOptions,self).destroy()

  # note: this method needs to be implemented exactly as-is
  # in each leaf subclass so that "self" in the observer
  # is of the correct type 
  def updateParameterNode(self, caller, event):
    node = EditUtil.EditUtil().getParameterNode()
    if node != self.parameterNode:
      if self.parameterNode:
        node.RemoveObserver(self.parameterNodeTag)
      self.parameterNode = node
      self.parameterNodeTag = node.AddObserver("ModifiedEvent", self.updateGUIFromMRML)

  def setMRMLDefaults(self):
    super(GrowCutCLOptions,self).setMRMLDefaults()
    disableState = self.parameterNode.GetDisableModifiedEvent()
    self.parameterNode.SetDisableModifiedEvent(1)
    defaults = (
      ("tolerance", "20"),
      ("maxPixels", "200"),
    )
    for d in defaults:
      param = "GrowCutCL,"+d[0]
      pvalue = self.parameterNode.GetParameter(param)
      if pvalue == '':
        self.parameterNode.SetParameter(param, d[1])
    self.parameterNode.SetDisableModifiedEvent(disableState)

  def updateGUIFromMRML(self,caller,event):
    if self.updatingGUI:
      return
    params = ("tolerance",)
    params = ("maxPixels",)
    for p in params:
      if self.parameterNode.GetParameter("GrowCutCL,"+p) == '':
        # don't update if the parameter node has not got all values yet
        return
    self.updatingGUI = True
    super(GrowCutCLOptions,self).updateGUIFromMRML(caller,event)
    self.toleranceSpinBox.setValue( float(self.parameterNode.GetParameter("GrowCutCL,tolerance")) )
    self.maxPixelsSpinBox.setValue( float(self.parameterNode.GetParameter("GrowCutCL,maxPixels")) )
    self.updatingGUI = False

  def onToleranceSpinBoxChanged(self,value):
    if self.updatingGUI:
      return
    self.updateMRMLFromGUI()

  def onMaxPixelsSpinBoxChanged(self,value):
    if self.updatingGUI:
      return
    self.updateMRMLFromGUI()

  def updateMRMLFromGUI(self):
    if self.updatingGUI:
      return
    disableState = self.parameterNode.GetDisableModifiedEvent()
    self.parameterNode.SetDisableModifiedEvent(1)
    super(GrowCutCLOptions,self).updateMRMLFromGUI()
    self.parameterNode.SetParameter( "GrowCutCL,tolerance", str(self.toleranceSpinBox.value) )
    self.parameterNode.SetParameter( "GrowCutCL,maxPixels", str(self.maxPixelsSpinBox.value) )
    self.parameterNode.SetDisableModifiedEvent(disableState)
    if not disableState:
      self.parameterNode.InvokePendingModifiedEvent()

  def onStartBot(self):
    """create the bot for background editing"""
    if hasattr(slicer.modules, 'editorBot'):
      slicer.modules.editorBot.stop()
      del(slicer.modules.editorBot)
      self.botButton.text = "Start Bot"
    else:
      GrowCutCLBot(self) 
      self.botButton.text = "Stop Bot"

  def onAccept(self):
    """Copy steered volume into label layer"""
    if hasattr(slicer.modules, 'editorBot'):
      slicer.modules.editorBot.stop()
      del(slicer.modules.editorBot)
      self.botButton.text = "Start Bot"
    backgroundVolume = self.editUtil.getBackgroundVolume()
    labelVolume = self.editUtil.getLabelVolume()
    steeredName = backgroundVolume.GetName() + '-steered'
    labelArray = slicer.util.array(labelVolume.GetName())
    steeredArray = slicer.util.array(steeredName)
    labelArray[:] = steeredArray[:]
    labelVolume.GetImageData().Modified()
    labelVolume.Modified()

#
# GrowCutCLBot
#
 
# TODO: move the concept of a Bot into the Effect class
# to manage timer.  Also put Bot status indicator and controls into
# an Editor interface.  For now, use slicer.modules.editorBot
# to enforce singleton instance for now.
#class GrowCutCLBot(EditorLib.LabelEffectBot):

class GrowCutCLBot(object): #stays active even when running the other editor effects
  """
  Task to run in the background for this effect.
  Receives a reference to the currently active options
  so it can access tools if needed.
  """
  def __init__(self,options):
    self.editUtil = EditUtil.EditUtil()
    self.sliceWidget = options.tools[0].sliceWidget
    if hasattr(slicer.modules, 'editorBot'):
      slicer.modules.editorBot.active = False
      del(slicer.modules.editorBot)
    slicer.modules.editorBot = self
    self.interval = 100
    self.active = False
    self.start()

  def start(self):
    self.active = True
    self.labelMTimeAtStart = self.editUtil.getLabelVolume().GetImageData().GetMTime()
    sliceLogic = self.sliceWidget.sliceLogic()
    self.logic = GrowCutCLLogic(sliceLogic)
    qt.QTimer.singleShot(self.interval, self.iteration)

  def stop(self):
    self.active = False

  def iteration(self):
    """Perform an iteration of the GrowCutCL algorithm"""
    if not self.active:
      return
    labelMTime = self.editUtil.getLabelVolume().GetImageData().GetMTime()
    if labelMTime > self.labelMTimeAtStart:
      sliceLogic = self.sliceWidget.sliceLogic()
      self.logic = GrowCutCLLogic(sliceLogic)
      self.labelMTimeAtStart = labelMTime
    self.logic.step(1)
    qt.QTimer.singleShot(self.interval, self.iteration)
    


#
# GrowCutCLTool
#
 
class GrowCutCLTool(EditorLib.LabelEffectTool):
  """
  One instance of this will be created per-view when the effect
  is selected.  It is responsible for implementing feedback and
  label map changes in response to user input.
  This class observes the editor parameter node to configure itself
  and queries the current view for background and label volume
  nodes to operate on.
  """

  def __init__(self, sliceWidget, threeDWidget=None):
    super(GrowCutCLTool,self).__init__(sliceWidget, threeDWidget)

  def cleanup(self):
    super(GrowCutCLTool,self).cleanup()

  def processEvent(self, caller=None, event=None):
    """
    handle events from the render window interactor
    """
    # this tool doesn't care about user events
    pass

#
# GrowCutCLLogic
#
 
class GrowCutCLLogic(EditorLib.LabelEffectLogic):
  """
  This class contains helper methods for a given effect
  type.  It can be instanced as needed by an GrowCutCLTool
  or GrowCutCLOptions instance in order to compute intermediate
  results (say, for user feedback) or to implement the final 
  segmentation editing operation.  This class is split
  from the GrowCutCLTool so that the operations can be used
  by other code without the need for a view context.
  """

  def __init__(self,sliceLogic):
    self.sliceLogic = sliceLogic

    import os
    os.environ['PYOPENCL_COMPILER_OUTPUT'] = '1'

    #
    # get the label and background volume nodes
    #
    labelLogic = self.sliceLogic.GetLabelLayer()
    self.labelNode = labelLogic.GetVolumeNode()
    self.labelNode.SetModifiedSinceRead(1)
    backgroundLogic = self.sliceLogic.GetBackgroundLayer()
    self.backgroundNode = backgroundLogic.GetVolumeNode()

    #
    # Get the numpy array for the bg, steered and label
    #
    import vtk.util.numpy_support
    self.backgroundImage = self.backgroundNode.GetImageData()
    self.labelImage = self.labelNode.GetImageData()
    self.shape = list(self.backgroundImage.GetDimensions())
    self.shape.reverse()
    self.backgroundArray = vtk.util.numpy_support.vtk_to_numpy(
        self.backgroundImage.GetPointData().GetScalars()).reshape(self.shape)
    self.labelArray = vtk.util.numpy_support.vtk_to_numpy(
        self.labelImage.GetPointData().GetScalars()).reshape(self.shape)
    self.backgroundArrayMax = self.backgroundArray.max()
    steeredName = self.backgroundNode.GetName() + '-steered'
    self.steeredNode = slicer.util.getNode(steeredName)
    self.steeredArray = slicer.util.array(steeredName)

    print("Creating Context...")
    self.clContext = None
    for platform in pyopencl.get_platforms():
        for device in platform.get_devices():
            if pyopencl.device_type.to_string(device.type) == "CPU":
               self.clContext = pyopencl.Context([device])
               print ("using: %s" % pyopencl.device_type.to_string(device.type))
               break;

    if not self.clContext:
      print ("preferred context not available")
      self.clContext = pyopencl.create_some_context()
    print("Creating Queue...")
    self.clQueue = pyopencl.CommandQueue(self.clContext)

    # background and labels arrays in device
    self.backgroundArray_dev = pyopencl.array.to_device(
        self.clQueue, self.backgroundArray)
    self.labelArray_dev = pyopencl.array.to_device(
        self.clQueue, self.labelArray)

    # theta is the segmentation strength
    binaryLabels = numpy.logical_not(numpy.logical_not(self.labelArray))
    theta = float(2**15) * numpy.array(binaryLabels,dtype=numpy.dtype('float32'))
    self.theta_dev = pyopencl.array.to_device(self.clQueue,theta)
    thetaNext = numpy.copy(numpy.array(theta, dtype=numpy.dtype('float32')))
    self.thetaNext_dev = pyopencl.array.to_device(self.clQueue,thetaNext)
    self.labelNext_dev = pyopencl.array.zeros_like(self.labelArray_dev)

    # candidates is a bitmap of pixels to consider on the next iteration
    candidates = self.labelArray.copy()
    self.candidates_dev = pyopencl.array.to_device(self.clQueue,candidates)
    candidatesNext = candidates.copy()
    self.candidatesNext_dev = pyopencl.array.to_device(
        self.clQueue,candidatesNext)
    self.candidatesInitialized = False

    # Get the source code and subsitute in the variables approproate
    # the current data - pyopencl will rebuild the binary if needed
    # or will reuse and existing binary if the code checksum matches
    inPath = os.path.dirname(slicer.modules.growcutcl.path) + "/GrowCutCL.cl.in"
    fp = open(inPath)
    sourceIn = fp.read()
    fp.close()

    slices, rows, columns = self.shape
    source = sourceIn % { 
        'slices' : slices, 
        'rows' : rows, 
        'columns' : columns,
        }
    self.clProgram = pyopencl.Program(self.clContext, source).build()
    self.pendingEvent = None

    # set the work size to include the extent of the labeled region
    labeledPoints = numpy.transpose(numpy.where( self.labelArray != 0 ))
    if len(labeledPoints) == 0:
      self.work_offset = (0,0,0)
      self.work_size = self.shape
    else:
      # Get the bounding box
      minCorner = labeledPoints.min(0)
      maxCorner = labeledPoints.max(0)+1

      origin = minCorner
      size = maxCorner - minCorner
      # workaround for some boost/numpy issue
      # http://boost.2283326.n4.nabble.com/extracting-a-numpy-int32-type-td2701573.html
      self.work_offset = (int(origin[0]),int(origin[1]),int(origin[2]))
      self.work_size = (int(size[0]),int(size[1]),int(size[2]))

  def step(self, iterations=1):

    if self.pendingEvent:
      if self.pendingEvent.command_execution_status == 0:
        # TODO: is there an attribute for CL_COMPLETE?
        # TODO: also catch error codes here
        #
        # put data back in node
        #
        self.steeredArray[:] = self.labelNext_dev.get()
        self.steeredNode.GetImageData().Modified()
        self.steeredNode.Modified()
      else:
        return

    #
    # get the parameters from MRML
    #
    # TODO: handl any real growcut parameters
    node = EditUtil.EditUtil().getParameterNode()
    tolerance = float(node.GetParameter("GrowCutCL,tolerance"))
    maxPixels = float(node.GetParameter("GrowCutCL,maxPixels"))

    #
    # initialize candidates if needed
    #
    if not self.candidatesInitialized:
      self.clProgram.clearShort(
          self.clQueue, self.shape, None, self.candidatesNext_dev.data).wait()
      print("self.candidatesNext_dev mean ", self.candidatesNext_dev.get().mean())
      self.clProgram.initialCandidates(
          self.clQueue, self.shape, None, self.labelArray_dev.data, 
          self.candidates_dev.data).wait()
      print("self.labelArray_dev mean ", self.labelArray_dev.get().mean())
      print("self.candidates_dev mean ", self.candidates_dev.get().mean())
      self.candidatesInitialized = True

    #
    # run iterations
    # - save the pending event from the last call so that we can 
    #   know when the execution is finished (so the main event loop
    #   is not blocked on this execution)
    #
    for iteration in xrange(iterations):
      self.clProgram.growCut(self.clQueue, self.work_size, None,
          self.backgroundArray_dev.data, self.labelArray_dev.data, 
          self.theta_dev.data, 
          self.thetaNext_dev.data, self.labelNext_dev.data, 
          self.candidates_dev.data, self.candidatesNext_dev.data,
          self.backgroundArrayMax, global_offset=self.work_offset)
      self.clProgram.copyShort(self.clQueue, self.work_size, None,
          self.labelNext_dev.data, self.labelArray_dev.data, global_offset=self.work_offset)
      self.clProgram.copyShort(self.clQueue, self.work_size, None,
          self.candidatesNext_dev.data, self.candidates_dev.data, global_offset=self.work_offset)
      self.clProgram.clearShort(self.clQueue, self.work_size, None,
          self.candidatesNext_dev.data, global_offset=self.work_offset)
      self.pendingEvent = self.clProgram.copyFloat(self.clQueue, self.work_size, None,
          self.thetaNext_dev.data, self.theta_dev.data, global_offset=self.work_offset)



#
# The GrowCutCLExtension class definition 
#

class GrowCutCLExtension(object):  #the editor turned on, this runs 
  """Organizes the Options, Tool, and Logic classes into a single instance
  that can be managed by the EditBox
  """

  def __init__(self):
    # name is used to define the name of the icon image resource (e.g. GrowCutCL.png)
    self.name = "GrowCutCL"
    # tool tip is displayed on mouse hover
    self.toolTip = "GrowCutCL: steered segmenter"

    self.options = GrowCutCLOptions
    self.tool = GrowCutCLTool
    self.logic = GrowCutCLLogic

#
# GrowCutCL
#

class GrowCutCL: #this part runs during module discovery process
  """
  This class is the 'hook' for slicer to detect and recognize the extension
  as a loadable scripted module
  """
  def __init__(self, parent):
    parent.title = "Editor GrowCutCL Effect"
    parent.categories = ["Developer Tools.Editor Extensions"]
    parent.contributors = ["Steve Pieper"]
    parent.helpText = """
    An OpenCL implementation of GrowCut as an EditorEffect with a Bot to 
    provide automated segmentation.
    """
    parent.acknowledgementText = """
    This editor extension was developed by 
    Steve Pieper, Isomics, Inc.
    """

    # don't show this module - it only appears in the Editor module
    parent.hidden = True

    # Add this extension to the editor's list for discovery when the module
    # is created.  Since this module may be discovered before the Editor itself,
    # create the list if it doesn't already exist.
    try:
      slicer.modules.editorExtensions
    except AttributeError:
      slicer.modules.editorExtensions = {}
    slicer.modules.editorExtensions['GrowCutCL'] = GrowCutCLExtension

#
# GrowCutCLWidget
#

class GrowCutCLWidget:
  def __init__(self, parent = None):
    self.parent = parent
    
  def setup(self):
    # don't display anything for this widget - it will be hidden anyway
    pass

  def enter(self):
    pass
    
  def exit(self):
    pass


