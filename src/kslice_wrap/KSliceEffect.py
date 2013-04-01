import os
from __main__ import vtk, qt, ctk, slicer
import EditorLib
from EditorLib.EditOptions import HelpButton
from EditorLib.EditOptions import EditOptions
from EditorLib import EditUtil
from EditorLib import LabelEffect
import vtkSlicerKSliceModuleLogicPython
#import KSliceEffect_GUI

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
    lm                      = slicer.app.layoutManager()
    self.redSliceWidget     = lm.sliceWidget('Red')
    self.yellowSliceWidget  = lm.sliceWidget('Yellow')
    self.greenSliceWidget   = lm.sliceWidget('Green')
    self.parameterNode=parameterNode

  def __del__(self):
    super(KSliceEffectOptions,self).__del__()

  def create(self):
    super(KSliceEffectOptions,self).create()

    #create a "Start Bot" button
    self.botButton = qt.QPushButton(self.frame)
    if hasattr(slicer.modules, 'editorBot'):
      self.botButton.text = "Stop Interactive Segmentor"
    else:
      self.botButton.text = "Start Interactive Segmentor"
    self.frame.layout().addWidget(self.botButton)
    self.botButton.connect('clicked()', self.onStartBot)

    #make an "Apply" button
    #self.apply = qt.QPushButton("Apply", self.frame)
    #self.apply.setToolTip("Apply the extension operation")
    ###add the apply button to layout, widgets list
    #self.frame.layout().addWidget(self.apply)
    #self.widgets.append(self.apply)
    #self.apply.connect('clicked()', self.logic.apply)
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
    self.frame.layout().addStretch(1)     # Add vertical spacer

  def destroy(self):
    super(KSliceEffectOptions,self).destroy()
    print("Destroy in KSliceOptions has been called")

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

  def onStartBot(self):
    """create the bot for background editing"""
    if hasattr(slicer.modules, 'editorBot'):
      slicer.modules.editorBot.stop()
      del(slicer.modules.editorBot)
      self.botButton.text = "Start Segmentation"
    else:
      KSliceCLBot(self)
      self.botButton.text = "Stop Segmentation"

  def updateMRMLFromGUI(self):
    if self.updatingGUI:
      return
    disableState = self.parameterNode.GetDisableModifiedEvent()
    self.parameterNode.SetDisableModifiedEvent(1)
    super(KSliceEffectOptions,self).updateMRMLFromGUI()
    self.parameterNode.SetDisableModifiedEvent(disableState)
    if not disableState:
      self.parameterNode.InvokePendingModifiedEvent()



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
    self.redSliceWidget=options.redSliceWidget
    self.greenSliceWidget=options.greenSliceWidget
    self.yellowSliceWidget=options.yellowSliceWidget
    self.start()

  def start(self):
    self.active = True
    self.labelMTimeAtStart = self.editUtil.getLabelVolume().GetImageData().GetMTime()
    sliceLogic = self.sliceWidget.sliceLogic()
    self.logic = KSliceEffectLogic( self.redSliceWidget.sliceLogic() )

    print("Starting")
    qt.QTimer.singleShot(self.interval, self.iteration)

  def stop(self):
    self.active = False
    self.logic.destroy()
  def iteration(self):
    """Perform an iteration of the algorithm"""
    if not self.active:
      return
    labelMTime = self.editUtil.getLabelVolume().GetImageData().GetMTime()
    if labelMTime > self.labelMTimeAtStart:
      sliceLogic = self.sliceWidget.sliceLogic()
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

#print("Got a %s at %s in %s", (event,str(xy),viewName))
  def processEvent(self, caller=None, event=None):
    """
    handle events from the render window interactor
    """
    if event == "LeftButtonPressEvent":
      xy = self.interactor.GetEventPosition()
      viewName,orient = get_view_names(self.sliceWidget)
      ijk= smart_xyToIJK(xy,self.sliceWidget)
      if not orient:
        print "Warning, unexpected view orientation!?"
      values = get_values_at_IJK(ijk,self.sliceWidget)
      print(values)
    if event == 'EnterEvent':
      print "EnterEvent in KSliceEffect."
    else:
      pass

def get_view_names( sw ):
    viewName    = sw.sliceLogic().GetSliceNode().GetName()
    lm          = slicer.app.layoutManager()
    orient      = lm.sliceWidget(viewName).sliceOrientation;
    valid_orient= ('Axial','Sagittal','Coronal','Reformat')
    viewOrient  = None
    for vo in valid_orient:
      if vo == orient:
        viewOrient = vo
        #print str(viewName) + "," + str(vo)
    return viewName,viewOrient


def smart_xyToIJK(xy,sliceWidget):
  xyz = sliceWidget.sliceView().convertDeviceToXYZ(xy);
  ll  = sliceWidget.sliceLogic().GetLabelLayer()
  xyToIJK = ll.GetXYToIJKTransform().GetMatrix()
  ijkFloat = xyToIJK.MultiplyPoint(xyz+(1,))[:3]
  print( ijkFloat )
  ijk = []
  for element in ijkFloat:
    try:
      index = int(round(element))
    except ValueError:
      index = 0
    ijk.append(index)
  return ijk


def get_values_at_IJK( ijk, sliceWidget):
  labelLogic = sliceWidget.sliceLogic().GetLabelLayer()
  volumeNode = labelLogic.GetVolumeNode()
  imageData  = volumeNode.GetImageData()
  if not imageData:
    return "No Label Image"
  dims = imageData.GetDimensions()
  print "current view dims = " + str(dims)
  wasOutOfFrame = False
  values        = {'label':None,'U':None}
  for ele in xrange(3):
    ijk
    if ijk[ele] < 0 or ijk[ele] >= dims[ele]:
      print "Out of Frame"
      wasOutOfFrame=True
  if not wasOutOfFrame and volumeNode.GetLabelMap():
    labelIndex = int(imageData.GetScalarComponentAsDouble(ijk[0], ijk[1], ijk[2], 0))
    print "labelIndex = " + str(labelIndex)
    values['label'] = labelIndex
  # TODO get the user-integral value too
  return values


def bind_view_observers( handlerFunc ):
  layoutManager  = slicer.app.layoutManager()
  sliceNodeCount = slicer.mrmlScene.GetNumberOfNodesByClass('vtkMRMLSliceNode')
  ObserverTags   = []
  SliceWidgetLUT = {}   # for  sw = SliceWidget[caller] in handlerFunc
  for nodeIndex in xrange(sliceNodeCount):
    sliceNode   = slicer.mrmlScene.GetNthNodeByClass(nodeIndex, 'vtkMRMLSliceNode')
    sliceWidget = layoutManager.sliceWidget(sliceNode.GetLayoutName())
    print "did a bind_view_observers for view: " + str(sliceNode.GetLayoutName())
    if sliceWidget:  # add obserservers and keep track of tags
      style = sliceWidget.sliceView().interactor()
      SliceWidgetLUT[style] = sliceWidget
      events = ("LeftButtonPressEvent","MouseMoveEvent",
                    "RightButtonPressEvent","EnterEvent", "LeaveEvent")
      for event in events: # override active effect w/ priority
        tag = style.AddObserver(event, handlerFunc, 2.0)
        ObserverTags.append([style,tag])
  return ObserverTags,SliceWidgetLUT

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
    self.attributes  = ('MouseTool')
    self.displayName = 'KSliceEffect Effect'

    #create variables to keep track of how the label changed (automatic part or user input)
    self.acMod   = 0
    self.userMod = 0

    #create a key shortcut
    k = qt.QKeySequence(qt.Qt.Key_E)   # Press e/E to run segmentor
    cp= qt.QKeySequence(qt.Qt.Key_C)  # copy
    ps= qt.QKeySequence(qt.Qt.Key_V)  # paste

    self.qtkeyconnections = []
    self.qtkeydefs = [     [k,self.runSegment2D],
                           [cp,self.copyslice],
                           [ps,self.pasteslice] ] # like a cell array in matlab
    for i in [0,1,2]:
        keydef = self.qtkeydefs[i]
        s = qt.QShortcut(keydef[0], mainWindow())  # connect this qt event to mainWindow focus
        s.connect('activated()', keydef[1])
        s.connect('activatedAmbiguously()', keydef[1])
        self.qtkeyconnections.append(s)

    #set the image, label nodes (this will not change although the user can alter what is bgrnd/frgrnd in editor)
    labelLogic = self.sliceLogic.GetLabelLayer()
    self.labelNode = labelLogic.GetVolumeNode()
    backgroundLogic = self.sliceLogic.GetBackgroundLayer()
    self.backgroundNode = backgroundLogic.GetVolumeNode()

    #put a listener on label, so we know when user has drawn
    labelImg=self.labelNode.GetImageData()
    self.ladMod_tag=labelImg.AddObserver("ModifiedEvent", self.labModByUser)
    self.labelImg=labelImg

    #put test listener on the whole window
    # Don't think we need this!! This gets called on window resize, change label shown, etc
    self.logMod_tag = self.sliceLogic.AddObserver("ModifiedEvent", self.testWindowListener)

    # a number of observers for mouse events, bound to the interactors
    self.mouse_obs,self.swLUT = bind_view_observers(self.testWindowListener)
    self.mouse_obs.append([self.sliceLogic,self.logMod_tag])
  
    #make KSlice class
    print("making a kslice")
    #import vtkSlicerKSliceModuleLogicPython
    ksliceMod=vtkSlicerKSliceModuleLogicPython.vtkKSlice()
    ksliceMod.SetImageVol(self.backgroundNode.GetImageData())
    ksliceMod.SetLabelVol( self.labelNode.GetImageData() )

    volumesLogic = slicer.modules.volumes.logic()
    steeredName = self.backgroundNode.GetName() + '-steered'
    steeredVolume = volumesLogic.CloneVolume(slicer.mrmlScene, self.labelNode, steeredName)

    steeredArray = slicer.util.array(steeredName) #get the numpy array
    steeredArray[:]=0 #initialize user input
    tmpVol         = steeredVolume.GetImageData()
    tmpVol.SetScalarTypeToDouble()
    tmpVol.AllocateScalars()
    ksliceMod.SetUIVol( tmpVol )
    ksliceMod.Initialize() # kind of heavy-duty

    self.UIVol    = steeredVolume.GetImageData() # is == c++'s vtkImageData* ?
    self.ksliceMod= ksliceMod;
    self.currSlice= None
    self.ijkPlane='IJ'
    self.computeCurrSlice() #initialize the current slice to something meaningful


  def testWindowListener(self, caller, event):
    interactor=caller # should be called by the slice interactor...
    if event == "MouseMoveEvent":       # this a verbose event, dont print
      pass
    else:
      print "windowListener => processEvent( " + str(event) +" )"
    if event in ("EnterEvent","LeftButtonPressEvent","RightButtonPressEvent"):
      sw = self.swLUT[interactor]
      if not sw:
        print "caller (interactor?) not found in lookup table!"
        pass
      else:
        viewName,orient = get_view_names(sw)
        xy              = interactor.GetEventPosition()
        ijk             = smart_xyToIJK(xy,sw)      
        vals            = get_values_at_IJK(ijk,sw)
        self.sliceLogic = sw.sliceLogic()
        ijkPlane        = self.sliceIJKPlane()
        print "ijk plane is: " + str(ijkPlane)
        self.ksliceMod.SetOrientation(str(ijkPlane))
        self.ijkPlane = ijkPlane
        self.computeCurrSlice()
        #if 0==vals['label']:
        #self.currSlice = self.computeCurrSliceSmarter(ijk)
        print "smarter curr slice = " + str(self.currSlice)
        if event == "LeftButtonPressEvent":
          print "Accumulate User Input! "+str(ijk)+str(orient)+" ("+str(viewName)+")"        
          self.ksliceMod.applyUserIncrement(ijk[0],ijk[1],ijk[2],+1.0)
        # right click for negative ?
        #else:
        #  self.ksliceMod.applyUserIncrement(ijk[0],ijk[1],ijk[2],-1.0)

  def labModByUser(self,caller,event):
    if self.acMod==0 :
      if 0==self.userMod:
        print("modified by user, kslice bot is running")
      self.userMod=1
    else:
      self.acMod=0  #modification came from active contour, reset variable, prepare to listen to next modification
      self.userMod=0
      #print("modified by active contour")
      pass

  def copyslice(self):
    self.computeCurrSlice()
    self.ksliceMod.SetFromSlice(self.currSlice)
    print('copyslice')

  def pasteslice(self):
    self.computeCurrSlice()
    self.ksliceMod.PasteSlice(self.currSlice)
    labelImage=self.labelNode.GetImageData()
    self.labelNode.Modified()
    labelImage.Modified()
    print('pasteslice')

  def computeCurrSliceSmarter(self,ijk):
    ns=-1
    for orient in ( ('IJ',2),('JK',0),('IK',1) ):
      if self.ijkPlane == orient[0]:
        ns = ijk[ orient[1] ]
    return ns

  def computeCurrSlice(self):
    # annoying to reset these...    
    labelLogic = self.sliceLogic.GetLabelLayer()
    self.labelNode = labelLogic.GetVolumeNode()
    backgroundLogic = self.sliceLogic.GetBackgroundLayer()
    self.backgroundNode = backgroundLogic.GetVolumeNode()

    sliceOffset = self.sliceLogic.GetSliceOffset() 
    spacingVec  = self.labelNode.GetSpacing()
    originVec   = self.labelNode.GetOrigin()
    for orient in ( ('IJ',2),('JK',0),('IK',1) ):
      if self.ijkPlane == orient[0]:
        cs=int( round( (sliceOffset - originVec[orient[1]])/spacingVec[orient[1]]))
    #self.currSlice=computeCurrSliceSmarter
    self.currSlice = abs(cs) # hacky, is this always the fix if result of above is negative? 
    print "currSlice = " + str(self.currSlice) + ", offset= " + str(sliceOffset)


  def runSegment2D(self):
    # TODO: clarify this function's name, RunSegment2D
    #get slider information, a.__dict__ and a.children() are useful commands
    #imgLayer= self.sliceLogic.GetBackgroundLayer();
    #imgNode= imgLayer.GetVolumeNode();
    #labelLogic = self.sliceLogic.GetLabelLayer()
    #labelNode = labelLogic.GetVolumeNode()
    lm = slicer.app.layoutManager()
    
    
    self.computeCurrSlice()

    # get the parameters from MRML
    node = EditUtil.EditUtil().getParameterNode()
    currRad = int(node.GetParameter("KSliceEffect,radius"))

    #make connections, parameter settings
    self.ksliceMod.SetCurrSlice(self.currSlice)
    self.ksliceMod.SetBrushRad(currRad)
    self.ksliceMod.SetNumIts(50)

    #debug prints: lets see how this went ...
    #print(self.currSlice)
    #print(currRad)


    #execute a run
    self.ksliceMod.runUpdate(self.userMod)
    #print("1")

    #signal to slicer that the label needs to be updated
    labelImage=self.labelNode.GetImageData()

    self.acMod=1

    labelImage.Modified()
    #labelNode.SetModifiedSinceRead(1)
    self.labelNode.Modified()

  def destroy(self):
    #super(KSliceEffectLogic,self).destroy()

    print("Destroy in KSliceLogic has been called")
    #disconnect key shortcut
    for i in [0,1,2]:
        keyfun = self.qtkeydefs[i]
        keydef = self.qtkeyconnections[i]
        print('disconnecting keydef:  ')
        print(keyfun)
        keydef.disconnect('activated()', keyfun[1])
        keydef.disconnect('activatedAmbiguously()', keyfun[1])

    #***delete steeredArray
    for style,tag in self.mouse_obs:
        style.RemoveObserver(tag)

    #remove label observer
    self.labelImg.RemoveObserver(self.ladMod_tag)
    #remove logic observer
    self.sliceLogic.RemoveObserver(self.logMod_tag)

    self.ksliceMod.FastDelete()




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
    self.tool    = KSliceEffectTool
    self.logic   = KSliceEffectLogic

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

'''
  def get_IJK_orientation(self,xy,ijk,sw):
    xy0 = (0,0)
    if 1.0 < (abs(xy[0]-xy0[0])+abs(xy[1]-xy0[1])):
      xy0=(2,2)
    i0,j0,k0 = smart_xyToIJK( xy0, sw)
    i1,j1,k1 = ijk[:]
    if i0 == i1 and j0 == j1 and k0 == k1:
      return None
    if i0 == i1:
      return 'JK'
    if j0 == j1:
      return 'IK'
    if k0 == k1:
      return 'IJ'
    return None
'''
