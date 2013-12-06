import os
from __main__ import vtk, qt, ctk, slicer
import EditorLib
from EditorLib.EditOptions import HelpButton
from EditorLib.EditOptions import EditOptions
from EditorLib import EditUtil
from EditorLib import LabelEffect


from copy import copy, deepcopy
import numpy as np
from KUtil import KUtil, Print_Bad, Print_Good

# The Editor Extension itself.
# Must define the interface hooks to be come an editor effect.

#
# KSliceEffectOptions - see LabelEffect, EditOptions and Effect for superclasses
#

class KSliceEffectOptions(EditorLib.LabelEffectOptions):
  """ KSliceEffect-specfic gui
"""

  def __init__(self, parent=0):
    super(KSliceEffectOptions,self).__init__(parent)
    print("Made a KSliceEffectOptions")

    #save a layout manager, get all 3 slice widgets
    editUtil = EditorLib.EditUtil.EditUtil()
    parameterNode = editUtil.getParameterNode()
    lm = slicer.app.layoutManager()
    self.redSliceWidget = lm.sliceWidget('Red')
    self.yellowSliceWidget = lm.sliceWidget('Yellow')
    self.greenSliceWidget = lm.sliceWidget('Green')
    self.parameterNode=parameterNode

  def __del__(self):
    super(KSliceEffectOptions,self).__del__()

  def create(self):
    super(KSliceEffectOptions,self).create()

    #create a "Start Bot" button
    self.botButton = qt.QPushButton(self.frame)

    self.frame.layout().addWidget(self.botButton)
    self.botButton.connect('clicked()', self.onStartBot)

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
    self.locRadSpinBox.connect('valueChanged(double)', self.onRadiusSpinBoxChanged)
    self.widgets.append(self.locRadSpinBox)

    self.numItsLabel = qt.QLabel("Number Iterations:", self.locRadFrame)
    self.numItsLabel.setToolTip("Set the number of iterations to evolve contour.")
    self.locRadFrame.layout().addWidget(self.numItsLabel)
    self.widgets.append(self.numItsLabel)

    self.numItsSpinBox = qt.QDoubleSpinBox(self.locRadFrame)
    self.numItsSpinBox.setToolTip("Set the number of iterations to evolve contour.")
    self.numItsSpinBox.minimum = 0
    self.numItsSpinBox.maximum = 50
    self.numItsSpinBox.suffix = ""
    self.locRadFrame.layout().addWidget(self.numItsSpinBox)
    self.numItsSpinBox.connect('valueChanged(double)', self.onNumItsSpinBoxChanged)
    self.widgets.append(self.numItsSpinBox)

    HelpButton(self.frame, "TO USE: \n Start the interactive segmentor and initialize the segmentation with any other editor tool. \n KEYS: \n Press the following keys to interact: \n C: copy label slice \n V: paste label slice \n Q: evolve contour in 2D \n E: evolve contour in 3D \n A: toggle between draw/erase modes" )
    self.frame.layout().addStretch(1) # Add vertical spacer

    if hasattr(slicer.modules, 'editorBot'):
      self.botButton.text = "Stop Interactive Segmentor"
      if self.locRadFrame:
        self.locRadFrame.hide()
    else:
      self.botButton.text = "Start Interactive Segmentor"
      if self.locRadFrame:
        self.locRadFrame.show()

  def destroy(self):
    super(KSliceEffectOptions,self).destroy()

  def onRadiusSpinBoxChanged(self,value):
    self.parameterNode.SetParameter("KSliceEffect,radius", str(value))
    #self.locRadSpinBox.setValue( float(self.parameterNode.GetParameter("KSliceEffect,radius")) )
    self.updateMRMLFromGUI()


  def onNumItsSpinBoxChanged(self,value):
    self.parameterNode.SetParameter("KSliceEffect,numIts", str(value))
    self.updateMRMLFromGUI()


  def updateParameterNode(self, caller, event):
    '''# in each leaf subclass so that "self" in the observer
       # is of the correct type
    '''
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
      ("numIts", "10")
    )
    for d in defaults:
      param = "KSliceEffect,"+d[0]
      pvalue = self.parameterNode.GetParameter(param)
      if pvalue == '':
        self.parameterNode.SetParameter(param, d[1])
    self.parameterNode.SetDisableModifiedEvent(disableState)

  def updateGUIFromMRML(self,caller,event):
    self.updatingGUI = True
    super(KSliceEffectOptions,self).updateGUIFromMRML(caller,event)
    self.disconnectWidgets()
    self.locRadSpinBox.setValue( float(self.parameterNode.GetParameter("KSliceEffect,radius")) )
    self.numItsSpinBox.setValue( float(self.parameterNode.GetParameter("KSliceEffect,numIts")) )
    self.connectWidgets()
    self.updatingGUI = False

  def onStartBot(self):
    """create the bot for background editing"""
    print("entered 'onStartBot'")
    if hasattr(slicer.modules, 'editorBot'):
      slicer.modules.editorBot.stop()
      del(slicer.modules.editorBot)
      if self.botButton:
        self.botButton.text = "Start Interactive Segmentor"
      if self.locRadFrame:
        self.locRadFrame.show()
    else:
      KSliceBot(self)
      slicer.modules.editorBot.logic.emergencyStopFunc = self.botEstop;   #save the function that stops bot, destroys KSlice, if things go wrong 
      if self.botButton:
        self.botButton.text = "Stop Interactive Segmentor"
      if self.locRadFrame:
        self.locRadFrame.hide()

  def updateMRMLFromGUI(self):
    if self.updatingGUI:
      return
    disableState = self.parameterNode.GetDisableModifiedEvent()
    self.parameterNode.SetDisableModifiedEvent(1)
    super(KSliceEffectOptions,self).updateMRMLFromGUI()
    self.parameterNode.SetDisableModifiedEvent(disableState)
    if not disableState:
      self.parameterNode.InvokePendingModifiedEvent()

  def botEstop(self):
    if hasattr(slicer.modules, 'editorBot'):
      slicer.modules.editorBot.stop()
      del(slicer.modules.editorBot)
      if self.botButton:
        self.botButton.text = "Start Interactive Segmentor"
      if self.locRadFrame:
        self.locRadFrame.show()

class KSliceBot(object): #stays active even when running the other editor effects
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

    self.redSliceWidget=options.redSliceWidget
    self.greenSliceWidget=options.greenSliceWidget
    self.yellowSliceWidget=options.yellowSliceWidget
    self.start()

  def start(self):
    sliceLogic = self.sliceWidget.sliceLogic()
    self.logic = KSliceEffectLogic( self.redSliceWidget.sliceLogic() )

  def stop(self):

    self.logic.destroy()
    


#
# KSliceEffectTool
#

class KSliceEffectTool(LabelEffect.LabelEffectTool):
  """
One instance of this will be created per-view when the effect
is selected. It is responsible for implementing feedback and
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
      viewName,orient = get_view_names(self.sliceWidget)
      ijk= smart_xyToIJK(xy,self.sliceWidget)
      if not orient:
        print "Warning, unexpected view orientation!?"
    if event == 'EnterEvent':
      pass      #print "EnterEvent in KSliceEffect."
    else:
      pass

def get_view_names( sw ):
    viewName = sw.sliceLogic().GetSliceNode().GetName()
    lm = slicer.app.layoutManager()
    orient = lm.sliceWidget(viewName).sliceOrientation;
    valid_orient= ('Axial','Sagittal','Coronal','Reformat')
    viewOrient = None
    for vo in valid_orient:
      if vo == orient:
        viewOrient = vo
    return viewName,viewOrient


def smart_xyToIJK(xy,sliceWidget):
  xyz = sliceWidget.sliceView().convertDeviceToXYZ(xy);
  ll = sliceWidget.sliceLogic().GetLabelLayer()
  xyToIJK = ll.GetXYToIJKTransform().GetMatrix()
  ijkFloat = xyToIJK.MultiplyPoint(xyz+(1,))[:3]
  #print( ijkFloat )
  ijk = []
  for element in ijkFloat:
    try:
      index = int(round(element))
    except ValueError:
      index = 0
    ijk.append(index)
    #Print_Coord_Debug(xyz, RAS, xy, ijk, sliceWidget)
  return ijk


def get_values_at_IJK( ijk, sliceWidget):
  labelLogic = sliceWidget.sliceLogic().GetLabelLayer()
  volumeNode = labelLogic.GetVolumeNode()
  imageData = volumeNode.GetImageData()
  if not imageData:
    return "No Label Image"
  dims = imageData.GetDimensions()
  #print "current view dims = " + str(dims)
  wasOutOfFrame = False
  values = {'label':None,'U':None}
  for ele in xrange(3):
    ijk
    if ijk[ele] < 0 or ijk[ele] >= dims[ele]:
      #print "Out of Frame"
      wasOutOfFrame=True
  if not wasOutOfFrame and volumeNode.GetLabelMap():
    labelIndex = int(imageData.GetScalarComponentAsDouble(ijk[0], ijk[1], ijk[2], 0))
    #print "labelIndex = " + str(labelIndex)
    values['label'] = labelIndex
  # TODO get the user-integral value too
  return values


def bind_view_observers( handlerFunc ):
  layoutManager = slicer.app.layoutManager()
  sliceNodeCount = slicer.mrmlScene.GetNumberOfNodesByClass('vtkMRMLSliceNode')
  ObserverTags = []
  SliceWidgetLUT = {} # for sw = SliceWidget[caller] in handlerFunc
  for nodeIndex in xrange(sliceNodeCount):
    sliceNode = slicer.mrmlScene.GetNthNodeByClass(nodeIndex, 'vtkMRMLSliceNode')
    sliceWidget = layoutManager.sliceWidget(sliceNode.GetLayoutName())
    #print "did a bind_view_observers for view: " + str(sliceNode.GetLayoutName())
    if sliceWidget: # add obserservers and keep track of tags
      style = sliceWidget.sliceView().interactor()
      SliceWidgetLUT[style] = sliceWidget
      events = ("LeftButtonPressEvent","MouseMoveEvent", "RightButtonPressEvent","EnterEvent", "LeaveEvent")
      for event in events: # override active effect w/ priority
        tag = style.AddObserver(event, handlerFunc, 2.0)
        ObserverTags.append([style,tag])
  return ObserverTags,SliceWidgetLUT

#
# KSliceEffectLogic
#

class KSliceEffectLogic(LabelEffect.LabelEffectLogic):
  """
    [concise useful desc]
  """

  def __init__(self,sliceLogic):
    import vtkSlicerKSliceModuleLogicPython
    self.fullInitialized=False                                                          #tracks if completed the initializtion (so can do stop correctly)
    self.sliceLogic = sliceLogic

    #disconnect all shortcuts that may exist, to allow KSlice's to work, reconnect once bot is turned off
    slicer.modules.EditorWidget.removeShortcutKeys()

    print("Made a KSliceEffectLogic")


    # self.attributes should be tuple of options:
    # 'MouseTool' - grabs the cursor
    # 'Nonmodal' - can be applied while another is active
    # 'Disabled' - not available
    self.attributes = ('MouseTool')
    self.displayName = 'KSliceEffect Effect'

    #this gets set once, user cannot use a different color w/o stopping segmentation, starting over
    #create variables to keep track of how the label changed (automatic part or user input)
    self.labVal = EditorLib.EditUtil.EditUtil().getLabel() 
    Print_Good("Init KSliceLogic with Label Fixed to " + str(self.labVal) )
    self.acMod = 0
    self.userMod = 0
    self.emergencyStopFunc = None
    self.inFact=1                      #weight of the user input  (small=soft user suggestion, large=essentially hard constraint)

    self.dialogBox=qt.QMessageBox()                                                    #will display messages to draw users attention if he does anything wrong
    self.dialogBox.setWindowTitle("KSlice Interactive Segmentor Error")
    self.dialogBox.setWindowModality(qt.Qt.NonModal)                                   #will allow user to continue interacting with Slicer

    # TODO: check this claim-  might be causing leaks
    #       set the image, label nodes (this will not change although the user can
    #       alter what is bgrnd/frgrnd in editor)
    # Confused about how info propagates UIarray to UIVol, not the other way, NEEDS AUTO TESTS

    #labelLogic          = self.sliceLogic.GetLabelLayer()
    #backgroundLogic     = self.sliceLogic.GetBackgroundLayer()
    self.editUtil       = EditUtil.EditUtil()
    self.labelNode      = self.editUtil.getLabelVolume()                                #labelLogic.GetVolumeNode()
    self.backgroundNode = self.editUtil.getBackgroundVolume()                           #backgroundLogic.GetVolumeNode()

    #perform safety check on right images/labels being selected,                        #set up images
    if type(self.backgroundNode)==type(None) or type(self.labelNode)==type(None):       #if red slice doesnt have a label or image, go no further
       self.dialogBox.setText("Either Image (must be Background Image) or Label not set in slice views.")
       self.dialogBox.show()
       
       if self.emergencyStopFunc:
           self.emergencyStopFunc()
       return
        
    volumesLogic        = slicer.modules.volumes.logic()
    steeredName         = self.backgroundNode.GetName() + '-UserInput'                    #the name we want it to have

    self.labelImg       = self.labelNode.GetImageData()                                   # vtkImageData*
    self.labelName      = self.labelNode.GetName()                                        # record name of label so user, cant trick us
    self.labArr         = slicer.util.array(self.labelName)                               # numpy array with label, TODO: prevents other-named labels?
    self.imgName        = self.backgroundNode.GetName()

    if self.sliceViewMatchEditor(self.sliceLogic)==False:                                 # do nothing, exit function if user has played with images
      if self.emergencyStopFunc:
          self.emergencyStopFunc()
      return

    self.imgSpacing	= self.backgroundNode.GetSpacing()		                  # get the pixel spacing
    steeredVolume       = volumesLogic.CloneVolume(slicer.mrmlScene, self.labelNode, steeredName)
    self.uiName         = steeredVolume.GetName()                                         # the name that was actually assigned by slicer
    steeredArray        = slicer.util.array(self.uiName )                                 # get the numpy array
    steeredArray[:]     = 0                                                               # Init user input to zeros
    self.UIarray        = steeredArray                                                    # keep reference for easy computation of accumulation
    self.uiImg          = steeredVolume.GetImageData() 




    #create key shortcuts, make the connections
    s2 = qt.QKeySequence(qt.Qt.Key_Q) # Press q/Q to run segmentor 2d
    s3 = qt.QKeySequence(qt.Qt.Key_F) # Press f/F to run segmentor 3d
    s4 = qt.QKeySequence(qt.Qt.Key_T) # Press t/T to run segmentor 2.5d
    s5 = qt.QKeySequence(qt.Qt.Key_E) # Press e/E to run segmentor(chan-vese) 3D
    tg = qt.QKeySequence(qt.Qt.Key_A) # toggle between the painting label and 0--erasing label 
    cp = qt.QKeySequence(qt.Qt.Key_C) # copy
    ps = qt.QKeySequence(qt.Qt.Key_V) # paste
    wf = qt.QKeySequence(qt.Qt.Key_B) # flip weight factor between soft, ~hard user constraints
    si = qt.QKeySequence(qt.Qt.Key_I) # interpolate volume


    print  " keys for 2d, CV 3D, 3d, 2.5d, are      Q, E, F, T"
    print  " toggle, copy, paste:                   A, C, V "
    print  " interpolate:                           I"
                     
    self.qtkeyconnections = []
    self.qtkeydefs = [ [s2,self.runSegment2D],
                       [s3,self.runSegment3DLocCV],
                       [s4,self.runSegment2p5D],
                       [s5,self.runSegment3DCV],
                       [tg,self.toggleDrawErase],
                       [cp,self.copyslice],
                       [ps,self.pasteslice],
                       [wf,self.toggleInputFactor],
                       [si,self.interpVolume] ] # like a cell array in matlab



    for keydef in self.qtkeydefs:
        s = qt.QShortcut(keydef[0], mainWindow()) # connect this qt event to mainWindow focus
        #s.setContext(1)
        s.connect('activated()', keydef[1])
        #s.connect('activatedAmbiguously()', keydef[1])
        self.qtkeyconnections.append(s)

     


    # a number of observers for various events
    self.ladMod_tag            = self.labelImg.AddObserver("ModifiedEvent", self.labModByUser)                   # put a listener on label, so we know when user has drawn
    self.logMod_tag            = self.sliceLogic.AddObserver("ModifiedEvent", self.updateLabelUserInput)         # put test listener on the whole window
    self.mouse_obs, self.swLUT = bind_view_observers(self.testWindowListener)
    self.mouse_obs.append([self.sliceLogic,self.logMod_tag])
  
    # make KSlice class
    print(self.imgSpacing)

    print("making a kslice")
    ksliceMod=vtkSlicerKSliceModuleLogicPython.vtkKSlice()
    ksliceMod.SetImageVol( self.backgroundNode.GetImageData())
    ksliceMod.SetLabelVol( self.labelImg )
    ksliceMod.SetUIVol( self.uiImg )
    ksliceMod.SetCurrLabel(self.labVal)
    node    = EditUtil.EditUtil().getParameterNode()        # get the parameters from MRML
    currRad = int(float(node.GetParameter("KSliceEffect,radius")))
    self.numIts  = int(float(node.GetParameter("KSliceEffect,numIts")))
    ksliceMod.SetBrushRad(currRad)                          # only get to set radius at the beginning
    ksliceMod.SetNumIts(self.numIts)
    ksliceMod.SetSpacing(self.imgSpacing)
    ksliceMod.SetLambdaPenalty(0)                           # ensure effect of user input is local only, with no penalty on curvature
    ksliceMod.Initialize()
    self.ksliceMod= ksliceMod;


    # initialize state variables
    #self.currSlice    = None
    self.ijkPlane     = 'IJ'
    self.sw           = slicer.app.layoutManager().sliceWidget('Red')
    self.interactor   = self.sw.sliceView().interactor() #initialize to red slice interactor
    self.computeCurrSliceSmarter()     #initialize the current slice to something meaningful
    self.lastRunPlane = 'None'   #null setting, meaning we havent done any segmentations yet
    self.lastModBy    = 'None'   #was last active contour run in 2D or 3D (cache needs to be recomputed)
    self.accumInProg  = 0        #marker to know that we are accumulating user input   

    # make cache slices for the three different planes
    def createTmpArray(dim0, dim1, nameSuffix):
      Print_Good( "making array " + nameSuffix )
      return np.zeros([volSize[dim0],volSize[dim1],1])   #tmpArr

    print("Making temporary slice arrays")
    volSize=self.labelImg.GetDimensions()
    self.ij_tmpArr=createTmpArray(0,1,'-ij_Tmp')
    self.jk_tmpArr=createTmpArray(1,2,'-jk_Tmp')
    self.ik_tmpArr=createTmpArray(0,2,'-ik_Tmp')

    self.i_range=np.arange(0,volSize[0])
    self.j_range=np.arange(0,volSize[1])
    self.k_range=np.arange(0,volSize[2])
    self.linInd=np.ix_([self.currSlice],  self.j_range, self.i_range) #indices for elements of slice, in the 3D array



    # keep track of these vars so plane changes make tmpArr re-init correctly
    self.currSlice_tmp=self.currSlice
    self.ijkPlane_tmp =self.ijkPlane
    
    # debug 
    range_img = self.backgroundNode.GetImageData().GetScalarRange()
    Print_Good( str(range_img) )
 

    self.fullInitialized=True                                                          #tracks if completed the initializtion (so can do stop correctly)

  def check_U_sync(self):
    #self.uiImg.Update()
    range_UIVol = self.uiImg.GetScalarRange()
    range_UIArr = [self.UIarray.min(),self.UIarray.max()]
    strInfo="min,max of UIVol and UIArr = "+str(range_UIVol)+";  "+str(range_UIArr)+", sign(uk)= "
    isGood = True
    if max( [abs(range_UIArr[0]-range_UIVol[0]),
             abs(range_UIArr[1]-range_UIVol[1])]  )>0.5:
      Print_Bad( "FAIL SYNC! "+strInfo )
      isGood = False
    else:
      Print_Good(strInfo)
    return isGood

  def sliceViewMatchEditor(self, sliceLogic):
    #if self.dialogBox==type(None):                              #something deleted teh dialogBox, this function then breaks, bail
    #    if self.emergencyStopFunc:
    #        self.emergencyStopFunc()
    #    return False
    
    imgNode    = sliceLogic.GetBackgroundLayer().GetVolumeNode()
    labelNode  = sliceLogic.GetLabelLayer().GetVolumeNode()

    if type(imgNode)==type(None) or type(labelNode)==type(None) :
        self.dialogBox.setText("Either image (must be Background Image) or label not set in slice views.") 
        self.dialogBox.show()
        if self.emergencyStopFunc:
            self.emergencyStopFunc()
        return False 

    dimImg=self.backgroundNode.GetImageData().GetDimensions()
    dimLab=self.labelNode.GetImageData().GetDimensions()

    if not (dimImg[0]==dimLab[0] and dimImg[1]==dimLab[1] and dimImg[2]==dimLab[2]):    #if sizes dont match up(doing this b/c cant reach HelperBox parameters
        self.dialogBox.setText("Mismatched label to image.")
        self.dialogBox.show()
        if self.emergencyStopFunc:
            self.emergencyStopFunc()
        return False

    #print(self.imgName)
    #print(self.labelName)
    #print( sliceLogic.GetBackgroundLayer().GetVolumeNode().GetName() )
    #print( sliceLogic.GetLabelLayer().GetVolumeNode().GetName() )


    if (self.imgName== imgNode.GetName()) and (self.labelName == labelNode.GetName()):
        return True
    else:
        self.dialogBox.setText("Set image to values used for starting the KSlice bot or stop bot.")
        self.dialogBox.show()
        if self.emergencyStopFunc:
            self.emergencyStopFunc()
        return False

  def testWindowListener(self, caller, event):
    interactor=caller 						       # should be called by the slice interactor...
    self.sw         = self.swLUT[interactor]
    self.interactor = interactor 
    
    if self.sliceViewMatchEditor(self.sliceLogic)==False:              #do nothing, exit function if user has played with images
      return

    if event == "MouseMoveEvent": # this a verbose event, dont print
      self.computeCurrSliceSmarter()                                   #just in case, so were always up to date
      pass
    else:
      pass  #print "windowListener => processEvent( " + str(event) +" )"   
   
    if event in ("EnterEvent","LeftButtonPressEvent","RightButtonPressEvent"):
      # should be done first! sets orientation info
      if not self.sw:
        print "caller (interactor?) not found in lookup table!"
        pass
      else:
        viewName,orient = get_view_names(self.sw)
        xy = interactor.GetEventPosition()
        ijk = smart_xyToIJK(xy,self.sw)                               #ijk computation leads to "computeCurrSliceSmarter" not working
        vals = get_values_at_IJK(ijk,self.sw)
        self.sliceLogic = self.sw.sliceLogic()                        #this is a hack, look at init function, self.sliceLogic already defined as just "Red" slice
        ijkPlane = self.sliceIJKPlane()
        self.ksliceMod.SetOrientation(str(ijkPlane))
        self.ijkPlane = ijkPlane
        self.computeCurrSliceSmarter()                    
     
    #make sure accumulation has been done BEFORE clearing cache variables and recording over them (fixes multiple left clicks required by the "DrawEffect" tool
    if event == "LeftButtonPressEvent" and self.accumInProg==0:      

      self.accumInProg=1
      if self.ijkPlane=="IJ":
          self.linInd=np.ix_([self.currSlice],  self.j_range, self.i_range)
          self.ij_tmpArr=deepcopy(self.labArr[self.linInd])            
          
      elif self.ijkPlane=="JK":
          self.linInd=np.ix_(self.k_range, self.j_range, [self.currSlice])
          self.jk_tmpArr=deepcopy(self.labArr[self.linInd]) 
          
      elif self.ijkPlane=="IK":
          self.linInd=np.ix_(self.k_range,  [self.currSlice], self.i_range)
          self.ik_tmpArr=deepcopy(self.labArr[self.linInd]) 
          
      if EditorLib.EditUtil.EditUtil().getLabel() !=0:  #need this only if erasing
          self.labArr[self.linInd]=0
      else:
          self.labArr[self.linInd]=1

  def updateLabelUserInput(self, caller, event):
    print("updating label user input")

    if self.sliceViewMatchEditor(self.sliceLogic)==False:    
      return  #do nothing, exit function if user has played with images

    currLabelValue = EditorLib.EditUtil.EditUtil().getLabel()    # return integer value, *scalar*
    signAccum=(-1)*(currLabelValue!=0) + (1)*(currLabelValue==0) # change sign based on drawing/erasing

    bUseLabelModTrigger = False    
    if (event=="ModifiedEvent") and (self.accumInProg==1):
        # Danger: you haven't recalc'd the orientation and currSlice yet        
        # self.labVal doesn't seem to work when erasing
        print("passed update checks")
        if self.ijkPlane=="IJ":
            if signAccum==-1:                                            # We're drawing
                deltPaint=self.labArr[self.linInd]                       # find the next stuff that was painted
                newLab= (self.ij_tmpArr + self.labArr[self.linInd])!=0
            elif signAccum==1:                                           # user is erasing
                deltPaint=(self.labArr[self.linInd])==0                   #(self.ij_tmpArr - self.labArr[self.linInd])!=0 
                newLab=(deltPaint==0)*(self.ij_tmpArr!=0)               #self.labArr[self.linInd]  
        elif self.ijkPlane=="JK":
            if signAccum==-1: 
                deltPaint=self.labArr[self.linInd]
                newLab=(self.jk_tmpArr + self.labArr[self.linInd])!=0  
            elif signAccum==1:    
                deltPaint=(self.labArr[self.linInd])==0                   #(self.jk_tmpArr - self.labArr[self.linInd])!=0 
                newLab=(deltPaint==0)*(self.jk_tmpArr!=0)               #self.labArr[self.linInd] 
        elif self.ijkPlane=="IK":
            if signAccum==-1: 
                deltPaint=self.labArr[self.linInd]
                newLab=(self.ik_tmpArr + self.labArr[self.linInd])!=0  
            elif signAccum==1:    
                deltPaint=(self.labArr[self.linInd])==0                 #(self.ik_tmpArr - self.labArr[self.linInd])!=0 
                newLab=(deltPaint==0)*(self.ik_tmpArr!=0)             #self.labArr[self.linInd] 
        
        # Argh, overwrites any changes to underlying vtk volume!?
        #if bUseLabelModTrigger: # trying to add this back in

        ''' If you Enable this, It must pass the U Sync check! '''
        self.UIarray[self.linInd]+=signAccum*self.inFact*deltPaint
        self.labArr[self.linInd] = self.labVal * newLab   
        self.accumInProg=0                                              # done accumulating
        self.uiImg.Modified()
        #self.check_U_sync()
    
    if event == "RightButtonPressEvent":
        print "right mouse ..."
    
    if (event == "LeftButtonPressEvent") and (not bUseLabelModTrigger):
        self.ksliceMod.applyUserIncrement(ijk[0],ijk[1],ijk[2],-signAccum)
        print "Accumulate User Input! "+str(ijk)+str(orient)+" ("+str(viewName)+")"
  
      

# 
#        U_ijk = self.uiImg.GetScalarComponentAsDouble(ijk[0],ijk[1],ijk[2],0)
#        print 'U at ' + str(ijk) + ' = '+str(U_ijk)
#        U_new = U_ijk - 1.0
#        self.uiImg.SetScalarComponentFromDouble(ijk[0],ijk[1],ijk[2],U_new)

       
  def labModByUser(self,caller,event):
    if self.acMod==0 :
      if self.userMod==0:
        print("modified by user, kslice bot is running")
      self.userMod=1
    else:
      self.acMod=0                                                    #modification came from active contour, reset variable, prepare to listen to next modification
      self.userMod=0                                                  #print("modified by active contour")
      pass

  def toggleDrawErase(self):
    if EditorLib.EditUtil.EditUtil().getLabel() == self.labVal:
      EditorLib.EditUtil.EditUtil().setLabel(0)
    else:
      EditorLib.EditUtil.EditUtil().setLabel(self.labVal)

  def toggleInputFactor(self):
    if self.inFact == 1:
      self.inFact=10
      print("Input factor is 10")
    elif self.inFact==10:
      self.inFact=1
      print("Input factor is 1")

  def interpVolume(self):
	self.ksliceMod.interpolateLabelVolume()

  def copyslice(self):
    if self.sliceViewMatchEditor(self.sliceLogic)==False:              #do nothing, exit function if user has played with images
      return

    self.copySliceView=self.sliceIJKPlane()                            #ensure were pasting from within the same view later
    self.computeCurrSliceSmarter()
    self.ksliceMod.SetFromSlice(self.currSlice)
    print('copy slice')

  def pasteslice(self):
    if self.sliceViewMatchEditor(self.sliceLogic)==False:              #do nothing, exit function if user has played with images
      return

    if self.copySliceView==self.sliceIJKPlane():                       #make sure user hasn't move to a different view
      self.computeCurrSliceSmarter()
      self.ksliceMod.PasteSlice(self.currSlice)
      self.labelNode.Modified()
      self.labelImg.Modified()
      print('paste slice')

  def computeCurrSliceSmarter(self):
    #strange thing happens, at the very instant the user mouse enters a slice view, this function
    #results in currSlice being off by 1, after click event/move event it works correctly(uncomment the print line below
    #to notice this effect)
    xy = self.interactor.GetEventPosition()
    ijk = smart_xyToIJK(xy,self.sw)                                     #ijk computation leads to "computeCurrSliceSmarter" not working

    ns=-1
    for orient in ( ('IJ',2),('JK',0),('IK',1) ):
      if self.ijkPlane == orient[0]:
        ns = ijk[ orient[1] ]
    
    self.currSlice = ns
    #print"Computation of current slice yields:" + str(ns) 

  def computeCurrSlice(self):
    # annoying to reset these just for slice #...
    #labelLogic = self.sliceLogic.GetLabelLayer()
    #self.labelNode = labelLogic.GetVolumeNode()
    #backgroundLogic = self.sliceLogic.GetBackgroundLayer()
    self.backgroundNode = self.backgroundNode                   #backgroundLogic.GetVolumeNode()

    sliceOffset = self.sliceLogic.GetSliceOffset()
    spacingVec = self.labelNode.GetSpacing()
    originVec = self.labelNode.GetOrigin()
    for orient in ( ('IJ',2),('JK',0),('IK',1) ):
      if self.ijkPlane == orient[0]:
        cs=int( round( (sliceOffset - originVec[orient[1]])/spacingVec[orient[1]]))
    self.currSlice = abs(cs) # hacky, is this always the fix if result of above is negative?


  def runSegment2D(self):
    if self.sliceViewMatchEditor(self.sliceLogic)==False:              #do nothing, exit function if user has played with images
      return

    print("doing 2D segmentation")
    self.computeCurrSliceSmarter()
    

    #make connections, parameter settings
    self.ksliceMod.SetCurrSlice(self.currSlice)
    self.ksliceMod.SetNumIts(self.numIts)

    #execute a run, we're on same plane, same run type, user has not drawn => use cache (check for "same slice" is done in c++)
    useCache= (self.lastRunPlane==self.ijkPlane)& (self.lastModBy=='2D') & (self.userMod==0) 
    self.ksliceMod.runUpdate2D(not useCache)
    print "use cache?:" + str(useCache)    

    #save the 'last run state' information
    self.acMod=1
    self.lastRunPlane=self.ijkPlane
    self.lastModBy='2D'                             # was last active contour run in 2D or 3D (cache needs to be recomputed)
    
    #update vars
    self.labelImg.Modified()                        # This triggers a Modified Event on Label => windowListener call
    self.labelNode.Modified()                       # labelNode.SetModifiedSinceRead(1)
    
    #self.check_U_sync()                            # turn the debug off

  def runSegment3DLocCV(self):
    if self.sliceViewMatchEditor(self.sliceLogic)==False:              #do nothing, exit function if user has played with images
      return

    print("doing 3D local chan-vese segmentation")
    self.computeCurrSliceSmarter()

    #make connections, parameter settings
    self.ksliceMod.SetCurrSlice(self.currSlice)
    self.ksliceMod.SetNumIts(self.numIts)                    # should be less than 2D!

    #execute a run, still doing 3D, user has not drawn => use cache
    useCache= ( (self.lastModBy=='3DLocCV') & (self.userMod==0) )
    self.ksliceMod.runUpdate3DLocCV(not useCache)
    print "use cache?:" + str(useCache)

    #save the 'last run state' information
    self.acMod=1
    self.lastModBy='3DLocCV'                                    # was last active contour run in 2D or 3D (cache needs to be recomputed)

    self.labelImg.Modified()
    self.labelNode.Modified()                              # labelNode.SetModifiedSinceRead(1)

    #self.check_U_sync()                                   # turn the debug off
  def runSegment3DCV(self):
    if self.sliceViewMatchEditor(self.sliceLogic)==False:              #do nothing, exit function if user has played with images
      return

    print("doing 3D chan-vese segmentation")
    self.computeCurrSliceSmarter()

    #make connections, parameter settings
    self.ksliceMod.SetCurrSlice(self.currSlice)
    self.ksliceMod.SetNumIts(self.numIts)                    # should be less than 2D!

    #execute a run, still doing 3D, user has not drawn => use cache
    useCache= ( (self.lastModBy=='3DCV') & (self.userMod==0) )
    self.ksliceMod.runUpdate3DCV(not useCache)
    print "use cache?:" + str(useCache)

    #save the 'last run state' information
    self.acMod=1
    self.lastModBy='3DCV'                                    # was last active contour run in 2D or 3D (cache needs to be recomputed)

    self.labelImg.Modified()
    self.labelNode.Modified()                              # labelNode.SetModifiedSinceRead(1)

    #self.check_U_sync()                                   # turn the debug off



  def runSegment2p5D(self):
    if self.sliceViewMatchEditor(self.sliceLogic)==False:              #do nothing, exit function if user has played with images
      return

    print("doing 2.5D segmentation")
    self.computeCurrSliceSmarter()

    self.ksliceMod.SetCurrSlice(self.currSlice)
    self.ksliceMod.SetNumIts(self.numIts)                          # should be less than 2D!
    self.ksliceMod.SetDistWeight(0.2)                     # weight evolution by distancef rom view-plane

    #still doing 3D, user has not drawn => use cache
    useCache= (self.lastModBy=='25D') & (self.userMod==0) 
    self.ksliceMod.runUpdate2p5D(not useCache)
    print "use cache?:" + str(useCache)

    #save the 'last run state' information
    self.acMod=1
    self.lastModBy='25D'                                   # was last active contour run in 2D or 3D (cache needs to be recomputed)
    #self.check_U_sync()                                  # very slow operation
    
    self.labelImg.Modified()                              # signal to slicer that the label needs to be updated
    self.labelNode.Modified()


  def destroy(self):
    # PK to IK: can you purge the deprecated comments? This func is confusing to review. 
    #super(KSliceEffectLogic,self).destroy()
    if self.fullInitialized==False:                                                          #if initialized, remove, otherwise do nothing
        return
    
    print("Destroy in KSliceLogic has been called")
    #disconnect key shortcut
    for i in range(len(self.qtkeydefs)):
        keyfun = self.qtkeydefs[i]
        keydef = self.qtkeyconnections[i]
        #print('disconnecting keydef: ')
        #print(keydef)
        test1=keydef.disconnect('activated()', keyfun[1])
        test2=keydef.disconnect('activatedAmbiguously()', keyfun[1])
        keydef.setParent(None)
        #why is this necessary for full disconnect (if removed, get the error that more and more keypresses are required if module is repetedly erased and created
        keydef.delete() #this causes errors
        #print "disconnected 'activated'?:" + str(test1)
        #print "disconnected 'activatedAmbiguously'?:" + str(test2)                

    #remove observers
    for style,tag in self.mouse_obs:
        style.RemoveObserver(tag)

    #remove label observer
    self.labelImg.RemoveObserver(self.ladMod_tag)

    #remove logic observer
    self.sliceLogic.RemoveObserver(self.logMod_tag)
    
    #removing the ui Node
    slicer.mrmlScene.RemoveNode( getNode(self.uiName) ) 

    # destroy 
    self.ksliceMod=None    #self.ksliceMod.FastDelete()
    self.dialogBox=None
    self.UIarray=None      # keep reference for easy computation of accumulation
    self.uiImg=None

    #put back the editor shortcuts we removed
    slicer.modules.EditorWidget.installShortcutKeys()

    print("Deletion completed")

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
    parent.dependencies = ["Editor"]
    parent.title = "Editor KSliceEffect Effect"
    parent.categories = ["Developer Tools.Editor Extensions"]
    parent.contributors = ["Ivan Kolesov, Peter Karasev, Patricio Vela (Georgia Institute of Technology) , Allen Tannenbaum (University of Alabama Birmingham), and Steve Pieper (Isomics, Inc.)"] # insert your name in the list
    parent.helpText = """Interactive segmentation editor extension."""
    parent.acknowledgementText = """ This editor extension was developed by Ivan Kolesov,  
    Peter Karasev, Patricio Vela (Georgia Institute of Technology),
Allen Tannenbaum (University of Alabama Birmingham), and Steve Pieper (Isomics, Inc.).  """

  
    # TODO:
    # don't show this module - it only appears in the Editor module
    #parent.hidden = True

    # Add this extension to the editor's list for discovery when the module
    # is created. Since this module may be discovered before the Editor itself,
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


