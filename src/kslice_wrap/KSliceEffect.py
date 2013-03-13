import os
from __main__ import vtk, qt, ctk, slicer
import EditorLib
from EditorLib.EditOptions import HelpButton
from EditorLib.EditOptions import EditOptions
from EditorLib import EditUtil
from EditorLib import LabelEffect
import vtkSlicerKSliceModuleLogicPython

#
# The Editor Extension itself.
#
# This needs to define the hooks to be come an editor effect.
#

#
# KSliceEffectOptions - see LabelEffect, EditOptions and Effect for superclasses
#

class KSliceEffectOptions(EditorLib.LabelEffectOptions):
  """ KSliceEffect-specfic gui
  """

  def __init__(self, parent=0):
    super(KSliceEffectOptions,self).__init__(parent)
    print("Made a KSliceEffectOptions")

    #save a layout manager, get just the red slice
    editUtil = EditorLib.EditUtil.EditUtil()
    parameterNode = editUtil.getParameterNode()
    lm = slicer.app.layoutManager()
    redSliceWidget = lm.sliceWidget('Red')
    self.parameterNode=parameterNode
    self.logic=KSliceEffectLogic( redSliceWidget.sliceLogic() )
    
  def __del__(self):
    super(KSliceEffectOptions,self).__del__()

  def create(self):
    super(KSliceEffectOptions,self).create()

    #create a "Start Bot" button
    self.botButton = qt.QPushButton(self.frame)
    if hasattr(slicer.modules, 'editorBot'):
      self.botButton.text = "Stop Bot"
    else:
      self.botButton.text = "Start Bot"
    self.frame.layout().addWidget(self.botButton)
    self.botButton.connect('clicked()', self.onStartBot)

    #make an "Apply" button
    self.apply = qt.QPushButton("Apply", self.frame)
    self.apply.setToolTip("Apply the extension operation")
    ###add the apply button to layout, widgets list
    self.frame.layout().addWidget(self.apply)
    self.widgets.append(self.apply)
    self.apply.connect('clicked()', self.logic.apply)

    #make radius option

    self.locRadFrame = qt.QFrame(self.frame)
    self.locRadFrame.setLayout(qt.QHBoxLayout())
    self.frame.layout().addWidget(self.locRadFrame)
    self.widgets.append(self.locRadFrame)

    self.locRadLabel = qt.QLabel("Radius:", self.locRadFrame)
    self.locRadLabel.setToolTip("Set the radius used to compute local energy.")
    self.locRadFrame.layout().addWidget(self.locRadLabel)
    self.widgets.append(self.locRadLabel)

    self.locRadSpinBox = qt.QDoubleSpinBox(self.locRadFrame)
    self.locRadSpinBox.setToolTip("Set the radius used to compute local energy.")
    self.locRadSpinBox.minimum = 0
    self.locRadSpinBox.maximum = 50
    self.locRadSpinBox.suffix = ""
    self.locRadFrame.layout().addWidget(self.locRadSpinBox)
    self.widgets.append(self.locRadSpinBox)

    HelpButton(self.frame, "This is an interactive segmentation tool.")



    # Add vertical spacer
    self.frame.layout().addStretch(1)

  def destroy(self):
    super(KSliceEffectOptions,self).destroy()

  # note: this method needs to be implemented exactly as-is
  # in each leaf subclass so that "self" in the observer
  # is of the correct type
  def updateParameterNode(self, caller, event):
    node = EditUtil.EditUtil().getParameterNode()
    if node != self.parameterNode:
      if self.parameterNode:
        node.RemoveObserver(self.parameterNodeTag)
      self.parameterNode = node
      self.parameterNodeTag = node.AddObserver(vtk.vtkCommand.ModifiedEvent, self.updateGUIFromMRML)

  def setMRMLDefaults(self):
    super(KSliceEffectOptions,self).setMRMLDefaults()

    disableState = self.parameterNode.GetDisableModifiedEvent()
    self.parameterNode.SetDisableModifiedEvent(1)
    defaults = (
      ("radius", "5"),
    )
    for d in defaults:
      param = "KSliceEffect,"+d[0]
      pvalue = self.parameterNode.GetParameter(param)
      if pvalue == '':
        self.parameterNode.SetParameter(param, d[1])
    self.parameterNode.SetDisableModifiedEvent(disableState)

  def updateGUIFromMRML(self,caller,event):
    print("making the connections")
    #params = ("radius",)
    #for p in params:
    #  if self.parameterNode.GetParameter("KSliceEffect,"+p) == '':
    #    # don't update if the parameter node has not got all values yet
    #    return

    
    self.updatingGUI = True
    super(KSliceEffectOptions,self).updateGUIFromMRML(caller,event)
    self.disconnectWidgets()
    self.locRadSpinBox.setValue( float(self.parameterNode.GetParameter("KSliceEffect,radius")) )
    self.connectWidgets()
    self.updatingGUI = False

    print("made the connections")

  def onApply(self):
    print('Processing image')
    
    self.logic.apply()

  def onStartBot(self):
    """create the bot for background editing"""
    if hasattr(slicer.modules, 'editorBot'):
      slicer.modules.editorBot.stop()
      del(slicer.modules.editorBot)
      self.botButton.text = "Start Bot"
    else:
      KSliceCLBot(self)  
      self.botButton.text = "Stop Bot"


  def updateMRMLFromGUI(self):
    if self.updatingGUI:
      return
    disableState = self.parameterNode.GetDisableModifiedEvent()
    self.parameterNode.SetDisableModifiedEvent(1)
    super(KSliceEffectOptions,self).updateMRMLFromGUI()
    self.parameterNode.SetDisableModifiedEvent(disableState)
    if not disableState:
      self.parameterNode.InvokePendingModifiedEvent()



# TODO: move the concept of a Bot into the Effect class
# to manage timer.  Also put Bot status indicator and controls into
# an Editor interface.  For now, use slicer.modules.editorBot
# to enforce singleton instance for now.
#class GrowCutCLBot(EditorLib.LabelEffectBot):

class KSliceCLBot(object): #stays active even when running the other editor effects
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
    #self.logic = GrowCutCLLogic(sliceLogic)
    print("Starting")
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
      #self.logic = GrowCutCLLogic(sliceLogic)
      print("Should be performing seg")
      self.labelMTimeAtStart = labelMTime
    #self.logic.step(1)
    qt.QTimer.singleShot(self.interval, self.iteration)


#
# KSliceEffectTool
#

class KSliceEffectTool(LabelEffect.LabelEffectTool):
  """
  One instance of this will be created per-view when the effect
  is selected.  It is responsible for implementing feedback and
  label map changes in response to user input.
  This class observes the editor parameter node to configure itself
  and queries the current view for background and label volume
  nodes to operate on.
  """

  def __init__(self, sliceWidget):
    super(KSliceEffectTool,self).__init__(sliceWidget)

  def cleanup(self):
    super(KSliceEffectTool,self).cleanup()

  def processEvent(self, caller=None, event=None):
    """
    handle events from the render window interactor
    """
    if event == "LeftButtonPressEvent":
      xy = self.interactor.GetEventPosition()
      #sliceLogic = self.sliceWidget.sliceLogic()
      #logic = KSliceEffectLogic(sliceLogic)
      #logic.apply(xy)
      print("Got a %s at %s in %s", (event,str(xy),self.sliceWidget.sliceLogic().GetSliceNode().GetName()))
      self.abortEvent(event)
    else:
      pass


#
# KSliceEffectLogic
#

class KSliceEffectLogic(LabelEffect.LabelEffectLogic):
  """
  This class contains helper methods for a given effect
  type.  It can be instanced as needed by an KSliceEffectTool
  or KSliceEffectOptions instance in order to compute intermediate
  results (say, for user feedback) or to implement the final
  segmentation editing operation.  This class is split
  from the KSliceEffectTool so that the operations can be used
  by other code without the need for a view context.
  """

  def __init__(self,sliceLogic):
    self.sliceLogic = sliceLogic
    #print(self.sliceLogic)
    print("Made a KSliceEffectLogic")

    # self.attributes should be tuple of options:
    # 'MouseTool' - grabs the cursor
    # 'Nonmodal' - can be applied while another is active
    # 'Disabled' - not available
    self.attributes = ('MouseTool')
    self.displayName = 'KSliceEffect Effect'


    #create variables to keep track of how the label changed (automatic part or user input)
    self.acMod   = 0
    self.userMod = 0

    #create a key shortcut
    k = qt.QKeySequence(qt.Qt.Key_E)
    s = qt.QShortcut(k, mainWindow())
    s.connect('activated()', self.apply)
    s.connect('activatedAmbiguously()', self.apply)


    #set the image, label nodes (this will not change although the user can alter what is bgrnd/frgrnd in editor)
    labelLogic = self.sliceLogic.GetLabelLayer()
    labelNode = labelLogic.GetVolumeNode()
    backgroundLogic = self.sliceLogic.GetBackgroundLayer()
    backgroundNode = backgroundLogic.GetVolumeNode()

    #put a listener on label, so we know when user has drawn
    labelImg=labelNode.GetImageData()
    labelImg.AddObserver("ModifiedEvent", self.labModByUser)


    #make KSlice class
    print("making a kslice")
    import vtkSlicerKSliceModuleLogicPython  
    ksliceMod=vtkSlicerKSliceModuleLogicPython.vtkKSlice()
    ksliceMod.SetImageVol(backgroundNode.GetImageData())
    ksliceMod.SetLabelVol( labelNode.GetImageData() )
    ksliceMod.SetUIVol( labelNode.GetImageData() )          #this is WRONG!!!
    ksliceMod.Initialize() #this should not be done every time!!
    self.ksliceMod=ksliceMod;

    print("were here")
    ksliceMod.PrintEmpty()
 

  def labModByUser(self,caller,event):
    #print(caller)
    #print(event)
    #print("2")
    #print(self.acMod)
    #print(self.userMod)
    if self.acMod==0 :  
      self.userMod=1
      print("modified by user")
    else:
      self.acMod=0  #modification came from active contour, reset variable, prepare to listen to next modification
      print("modified by active contour")
      pass


  def apply(self):
    #get slider information, a.__dict__ and a.children() are useful commands
    #imgLayer= self.sliceLogic.GetBackgroundLayer();
    #imgNode= imgLayer.GetVolumeNode();
    labelLogic = self.sliceLogic.GetLabelLayer()
    labelNode = labelLogic.GetVolumeNode()    
    sliceOffset = self.sliceLogic.GetSliceOffset() #gets the current slice location, just need spacing to figure out which slice currently working on 
    spacingVec  = labelNode.GetSpacing()
    originVec   = labelNode.GetOrigin()

    currSlice=round( (sliceOffset - originVec[2] + spacingVec[2]/2)/spacingVec[2]) #slider picks coordinate midway between slices, so need to add 1/2*spacing  


    
    # get the parameters from MRML
    node = EditUtil.EditUtil().getParameterNode()
    currRad = float(node.GetParameter("KSliceEffect,radius"))

    #make connections, parameter settings
    self.ksliceMod.SetCurrSlice(currSlice)
    self.ksliceMod.SetBrushRad(currRad)
    self.ksliceMod.SetNumIts(50)
    
    #debug prints: lets see how this went ...
    print(currSlice)  
    print(currRad)


    #execute a run
    self.ksliceMod.runUpdate(self.userMod)
    print("1")    

    #signal to slicer that the label needs to be updated
    labelImage=labelNode.GetImageData()
    
    self.acMod=1  
    labelImage.Modified()
    #labelNode.SetModifiedSinceRead(1)
    labelNode.Modified()



#
# The KSliceEffectExtension class definition
#

class KSliceEffectExtension(LabelEffect.LabelEffect):
  """Organizes the Options, Tool, and Logic classes into a single instance
  that can be managed by the EditBox
  """

  def __init__(self):
    # name is used to define the name of the icon image resource (e.g. KSliceEffect.png)
    self.name = "KSliceEffect"
    # tool tip is displayed on mouse hover
    self.toolTip = "Interactive Segmentation Tool"


    self.options = KSliceEffectOptions
    self.tool = KSliceEffectTool
    self.logic = KSliceEffectLogic

""" Test:

sw = slicer.app.layoutManager().sliceWidget('Red')
import EditorLib
pet = EditorLib.KSliceEffectTool(sw)

"""

#
# KSliceEffect
#

class KSliceEffect:
  """
  This class is the 'hook' for slicer to detect and recognize the extension
  as a loadable scripted module
  """
  def __init__(self, parent):
    parent.title = "Editor KSliceEffect Effect"
    parent.categories = ["Developer Tools.Editor Extensions"]
    parent.contributors = ["Steve Pieper (Isomics)"] # insert your name in the list
    parent.helpText = """
    Example of an editor extension.  No module interface here, only in the Editor module
    """
    parent.acknowledgementText = """
    This editor extension was developed by
    <Author>, <Institution>
    based on work by:
    Steve Pieper, Isomics, Inc.
    based on work by:
    Jean-Christophe Fillion-Robin, Kitware Inc.
    and was partially funded by NIH grant 3P41RR013218.
    """

    # TODO:
    # don't show this module - it only appears in the Editor module
    #parent.hidden = True

    # Add this extension to the editor's list for discovery when the module
    # is created.  Since this module may be discovered before the Editor itself,
    # create the list if it doesn't already exist.
    try:
      slicer.modules.editorExtensions
    except AttributeError:
      slicer.modules.editorExtensions = {}
    slicer.modules.editorExtensions['KSliceEffect'] = KSliceEffectExtension




#
# KSliceEffectWidget
#

class KSliceEffectWidget:
  def __init__(self, parent = None):
    self.parent = parent

  def setup(self):

    # don't display anything for this widget - it will be hidden anyway
    pass

  def enter(self):
    pass

  def exit(self):
    pass


