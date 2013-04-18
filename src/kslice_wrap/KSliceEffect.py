import os
from __main__ import vtk, qt, ctk, slicer
import EditorLib
from EditorLib.EditOptions import HelpButton
from EditorLib.EditOptions import EditOptions
from EditorLib import EditUtil
from EditorLib import LabelEffect
import vtkSlicerKSliceModuleLogicPython
from copy import copy, deepcopy
from numpy import *
from KUtil import *

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
    if hasattr(slicer.modules, 'editorBot'):
      self.botButton.text = "Stop Interactive Segmentor"
    else:
      self.botButton.text = "Start Interactive Segmentor"

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
    self.widgets.append(self.locRadSpinBox)

    HelpButton(self.frame, "This is an interactive segmentation tool.")
    self.frame.layout().addStretch(1) # Add vertical spacer

  def destroy(self):
    super(KSliceEffectOptions,self).destroy()
    print("Destroy in KSliceOptions has been called")

  
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
    self.connectWidgets()
    self.updatingGUI = False
    print("made the connections in KSliceEffectOptions")

  def onStartBot(self):
    """create the bot for background editing"""
    if hasattr(slicer.modules, 'editorBot'):
      slicer.modules.editorBot.stop()
      del(slicer.modules.editorBot)
      self.botButton.text = "Start Interactive Segmentor"
    else:
      KSliceBot(self)
      self.botButton.text = "Stop Interactive Segmentor"

  def updateMRMLFromGUI(self):
    if self.updatingGUI:
      return
    disableState = self.parameterNode.GetDisableModifiedEvent()
    self.parameterNode.SetDisableModifiedEvent(1)
    super(KSliceEffectOptions,self).updateMRMLFromGUI()
    self.parameterNode.SetDisableModifiedEvent(disableState)
    if not disableState:
      self.parameterNode.InvokePendingModifiedEvent()



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
      self.labelMTimeAtStart = labelMTime
    #self.logic.step(1)
    qt.QTimer.singleShot(self.interval, self.iteration)


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
      #print(values)
    if event == 'EnterEvent':
      pass
      #print "EnterEvent in KSliceEffect."
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
        #print str(viewName) + "," + str(vo)
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
    print "labelIndex = " + str(labelIndex)
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
    print "did a bind_view_observers for view: " + str(sliceNode.GetLayoutName())
    if sliceWidget: # add obserservers and keep track of tags
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
    [concise useful desc]
  """

  def __init__(self,sliceLogic):
    self.sliceLogic = sliceLogic
    print("Made a KSliceEffectLogic")

    # self.attributes should be tuple of options:
    # 'MouseTool' - grabs the cursor
    # 'Nonmodal' - can be applied while another is active
    # 'Disabled' - not available
    self.attributes = ('MouseTool')
    self.displayName = 'KSliceEffect Effect'

    self.labVal = EditorLib.EditUtil.EditUtil().getLabel() #this gets set once, user cannot use a different color w/o stopping segmentation, starting over
    #create variables to keep track of how the label changed (automatic part or user input)
    self.acMod = 0
    self.userMod = 0

    #create a key shortcut
    s2 = qt.QKeySequence(qt.Qt.Key_E) # Press e/E to run segmentor 2d
    s3 = qt.QKeySequence(qt.Qt.Key_T) # Press t/T to run segmentor 3d
    tg = qt.QKeySequence(qt.Qt.Key_A) # toggle between the painting label and 0--erasing label 
    cp = qt.QKeySequence(qt.Qt.Key_C) # copy
    ps = qt.QKeySequence(qt.Qt.Key_V) # paste
    
    self.qtkeyconnections = []
    self.qtkeydefs = [ [s2,self.runSegment2D],
                       [s3,self.runSegment3D],
                       [tg,self.toggleDrawErase],
                       [cp,self.copyslice],
                       [ps,self.pasteslice] ] # like a cell array in matlab

    for i in [0,1,2,3,4]:
        keydef = self.qtkeydefs[i]
        s = qt.QShortcut(keydef[0], mainWindow()) # connect this qt event to mainWindow focus
        s.connect('activated()', keydef[1])
        s.connect('activatedAmbiguously()', keydef[1])
        self.qtkeyconnections.append(s)

    #  TODO: check this claim-  might be causing leaks
    #     set the image, label nodes (this will not change although the user can
    #      alter what is bgrnd/frgrnd in editor)
    labelLogic = self.sliceLogic.GetLabelLayer()
    self.labelNode = labelLogic.GetVolumeNode()
    backgroundLogic = self.sliceLogic.GetBackgroundLayer()
    self.backgroundNode = backgroundLogic.GetVolumeNode()

    #put a listener on label, so we know when user has drawn
    labelImg=self.labelNode.GetImageData()
    self.ladMod_tag=labelImg.AddObserver("ModifiedEvent", self.labModByUser)
    self.labelImg=labelImg
    #self.labArr=vtk.util.numpy_support.vtk_to_numpy(labelImg.GetPointData().GetScalars()) 
    #keep reference for easy computation of accumulation BUT this is a hacked version of getting the underlying array 
    
    #TODO: prevents other-named labels?
    self.labArr= slicer.util.array(self.backgroundNode.GetName()+'-label') 

    #put test listener on the whole window
    self.logMod_tag = self.sliceLogic.AddObserver("ModifiedEvent", self.testWindowListener)

    # a number of observers for mouse events, bound to the interactors
    self.mouse_obs,self.swLUT = bind_view_observers(self.testWindowListener)
    self.mouse_obs.append([self.sliceLogic,self.logMod_tag])
  
    #make KSlice class
    print("making a kslice")
    ksliceMod=vtkSlicerKSliceModuleLogicPython.vtkKSlice()
    ksliceMod.SetImageVol(self.backgroundNode.GetImageData())
    ksliceMod.SetLabelVol( self.labelNode.GetImageData() )

    volumesLogic  = slicer.modules.volumes.logic()
    steeredName   = self.backgroundNode.GetName() + '-UserInput'
    steeredVolume = volumesLogic.CloneVolume(slicer.mrmlScene, self.labelNode, steeredName)

    steeredArray = slicer.util.array(steeredName) #get the numpy array
    steeredArray[:]=0 # Init user input to zeros
    tmpVol = steeredVolume.GetImageData()
    tmpVol.SetScalarTypeToDouble()
    tmpVol.AllocateScalars()

    ksliceMod.SetUIVol( tmpVol )
    ksliceMod.SetCurrLabel(self.labVal)  
    ksliceMod.Initialize()

    self.UIarray=slicer.util.array(steeredName) #keep reference for easy computation of accumulation
    self.UIVol  =steeredVolume.GetImageData() # is == c++'s vtkImageData* ?
    # Confused about how info propagates UIarray to UIVol, not the other way
    # NEEDS AUTO TESTS
    
    self.ksliceMod= ksliceMod;
    self.currSlice= None
    self.ijkPlane='IJ'
    self.computeCurrSlice() #initialize the current slice to something meaningful

    self.lastRunPlane ='None'   #null setting, meaning we havent done any segmentations yet
    self.lastModBy    ='None'   #was last active contour run in 2D or 3D (cache needs to be recomputed)
    self.accumInProg    =0        #marker to know that we are accumulating user input   

    print("Making temporary slice arrays")
    volSize=self.labelImg.GetDimensions()
    def createTmpArray( dim0, dim1, nameSuffix ):
        volumesLogic = slicer.modules.volumes.logic()
        tmp          =volumesLogic.CreateAndAddLabelVolume(slicer.mrmlScene, self.backgroundNode, self.backgroundNode.GetName() + nameSuffix)
        tmp_imgDat   =tmp.GetImageData()
        tmp_imgDat.SetDimensions(volSize[dim0], volSize[dim1],1) #use just one slice to keep track of changes
        tmp_imgDat.SetScalarTypeToDouble()
        tmp_imgDat.AllocateScalars()
        tmpArr = slicer.util.array(self.backgroundNode.GetName() + nameSuffix) #get the numpy array
        tmpArr[:]=0
        return tmpArr
    
    self.ij_tmpArr=createTmpArray(0,1,'-ij_Tmp')
    self.jk_tmpArr=createTmpArray(1,2,'-jk_Tmp')
    self.ik_tmpArr=createTmpArray(0,2,'-ik_Tmp')
    
    self.i_range=arange(0,volSize[0])
    self.j_range=arange(0,volSize[1])
    self.k_range=arange(0,volSize[2])
    print("Done creating temporary slice arrays")

    # keep track of these vars so plane changes make tmpArr re-init correctly
    self.currSlice_tmp=self.currSlice
    self.ijkPlane_tmp =self.ijkPlane
 

  def testWindowListener(self, caller, event):
    interactor=caller # should be called by the slice interactor...
    
    if event == "MouseMoveEvent": # this a verbose event, dont print
      pass
    else:
      pass                                                                       #print "windowListener => processEvent( " + str(event) +" )"   
   
    if event in ("EnterEvent","LeftButtonPressEvent","RightButtonPressEvent"):
      # should be done first! sets orientation info
      sw = self.swLUT[interactor]
      if not sw:
        print "caller (interactor?) not found in lookup table!"
        pass
      else:
        viewName,orient = get_view_names(sw)
        xy = interactor.GetEventPosition()
        ijk = smart_xyToIJK(xy,sw)
        vals = get_values_at_IJK(ijk,sw)
        self.sliceLogic = sw.sliceLogic()
        ijkPlane = self.sliceIJKPlane()
        self.ksliceMod.SetOrientation(str(ijkPlane))
        self.ijkPlane = ijkPlane
        self.computeCurrSlice()
        print "ijk plane is: " + str(ijkPlane) + ", curr slice = " + str(self.currSlice)

    # self.labVal doesn't seem to work when erasing
    currLabelValue = EditorLib.EditUtil.EditUtil().getLabel() # return integer value, *scalar*
    signAccum=(-1)*(currLabelValue!=0) + (1)*(currLabelValue==0) #change sign based on drawing/erasing
    

    # Ivan: I can't get it to work with both numpy AND c++-managed ...
    bUseLabelModTrigger = False    
    if (event=="ModifiedEvent") and (self.accumInProg==1):
        # Danger: you haven't recalc'd the orientation and currSlice yet        
        # self.labVal doesn't seem to work when erasing
        if self.ijkPlane=="IJ":
            if signAccum==-1:                     #We're drawing
                deltPaint=self.labArr[self.linInd]#find the next stuff that was painted
                newLab= (self.ij_tmpArr + self.labArr[self.linInd])!=0
            elif signAccum==1:                       #user is erasing
                deltPaint=(self.ij_tmpArr - self.labArr[self.linInd])!=0 
                newLab=self.labArr[self.linInd]  
        elif self.ijkPlane=="JK":
            if signAccum==-1: 
                deltPaint=self.labArr[self.linInd]
                newLab=(self.jk_tmpArr + self.labArr[self.linInd])!=0  
            elif signAccum==1:    
                deltPaint=(self.jk_tmpArr - self.labArr[self.linInd])!=0 
                newLab=self.labArr[self.linInd] 
        elif self.ijkPlane=="IK":
            if signAccum==-1: 
                deltPaint=self.labArr[self.linInd]
                newLab=(self.ik_tmpArr + self.labArr[self.linInd])!=0  
            elif signAccum==1:    
                deltPaint=(self.ik_tmpArr - self.labArr[self.linInd])!=0 
                newLab=self.labArr[self.linInd] 
        #TODO: scale by the currLabelValue 

        # Argh, overwrites any changes to underlying vtk volume!?
        #if bUseLabelModTrigger: # trying to add this back in
        
        self.UIarray[self.linInd]+=signAccum*deltPaint
        self.labArr[self.linInd] = newLab   
        self.accumInProg=0    #done accumulating
        
        def check_U_sync( ):
          range_UIVol = self.UIVol.GetScalarRange()
          range_UIArr = [self.UIarray.min(),self.UIarray.max()]
          strInfo="min,max of UIVol and UIArr = "+str(range_UIVol)+";  "+str(range_UIArr)+", sign(uk)= " + str(signAccum)
          isGood = True
          if max( [abs(range_UIArr[0]-range_UIVol[0]),
                   abs(range_UIArr[1]-range_UIVol[1])]  )>0.5:
            Print_Bad( "FAIL SYNC! "+strInfo )
            isGood = False
          else:
            Print_Good(strInfo)
          return isGood
          
        passedTest = check_U_sync()
        
    
    if event == "RightButtonPressEvent":
        print "right mouse ..."
    
    if (event == "LeftButtonPressEvent") and (not bUseLabelModTrigger):
        self.ksliceMod.applyUserIncrement(ijk[0],ijk[1],ijk[2],-signAccum)
        print "Accumulate User Input! "+str(ijk)+str(orient)+" ("+str(viewName)+")"
  
    if (event == "LeftButtonPressEvent"):
        self.accumInProg=1
        if self.ijkPlane=="IJ":
            self.linInd=ix_([self.currSlice],  self.j_range, self.i_range)
            self.ij_tmpArr=deepcopy(self.labArr[self.linInd])            
            
        elif self.ijkPlane=="JK":
            self.linInd=ix_(self.k_range, self.j_range, [self.currSlice])
            self.jk_tmpArr=deepcopy(self.labArr[self.linInd]) 
            
        elif self.ijkPlane=="IK":
            self.linInd=ix_(self.k_range,  [self.currSlice], self.i_range)
            self.ik_tmpArr=deepcopy(self.labArr[self.linInd]) 
            
        if EditorLib.EditUtil.EditUtil().getLabel() !=0:  #need this only if erasing
            self.labArr[self.linInd]=0   

# 
#        U_ijk = self.UIVol.GetScalarComponentAsDouble(ijk[0],ijk[1],ijk[2],0)
#        print 'U at ' + str(ijk) + ' = '+str(U_ijk)
#        U_new = U_ijk - 1.0
#        self.UIVol.SetScalarComponentFromDouble(ijk[0],ijk[1],ijk[2],U_new)

       
  def labModByUser(self,caller,event):
    if self.acMod==0 :
      if 0==self.userMod:
        print("modified by user, kslice bot is running")
      self.userMod=1
    else:
      self.acMod=0    #modification came from active contour, reset variable, prepare to listen to next modification
      self.userMod=0  #print("modified by active contour")
      pass

  def toggleDrawErase(self):
    if EditorLib.EditUtil.EditUtil().getLabel() == self.labVal:
      EditorLib.EditUtil.EditUtil().setLabel(0)
    else:
      EditorLib.EditUtil.EditUtil().setLabel(self.labVal)

  def copyslice(self):
    self.copySliceView=self.sliceIJKPlane()   #ensure were pasting from within the same view later
    self.computeCurrSlice()
    self.ksliceMod.SetFromSlice(self.currSlice)
    print('copyslice')

  def pasteslice(self):
    if self.copySliceView==self.sliceIJKPlane(): #make sure user hasn't move to a different view
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
    # annoying to reset these just for slice #...
    labelLogic = self.sliceLogic.GetLabelLayer()
    self.labelNode = labelLogic.GetVolumeNode()
    backgroundLogic = self.sliceLogic.GetBackgroundLayer()
    self.backgroundNode = backgroundLogic.GetVolumeNode()

    sliceOffset = self.sliceLogic.GetSliceOffset()
    spacingVec = self.labelNode.GetSpacing()
    originVec = self.labelNode.GetOrigin()
    for orient in ( ('IJ',2),('JK',0),('IK',1) ):
      if self.ijkPlane == orient[0]:
        cs=int( round( (sliceOffset - originVec[orient[1]])/spacingVec[orient[1]]))
    self.currSlice = abs(cs) # hacky, is this always the fix if result of above is negative?


  def runSegment2D(self):
    lm = slicer.app.layoutManager()
       
    self.computeCurrSlice()

    # get the parameters from MRML
    node = EditUtil.EditUtil().getParameterNode()
    currRad = int(node.GetParameter("KSliceEffect,radius"))

    #make connections, parameter settings
    self.ksliceMod.SetCurrSlice(self.currSlice)
    self.ksliceMod.SetBrushRad(currRad)
    self.ksliceMod.SetNumIts(30)

    #execute a run
    #we're on same plane, same run type, user has not drawn => use cache (check for "same slice" is done in c++)
    useCache= (self.lastRunPlane==self.ijkPlane)& (self.lastModBy=='2D') & (self.userMod==0) 
    print "use cache?:" + str(useCache)
    self.ksliceMod.runUpdate2D(not useCache)
    
    #signal to slicer that the label needs to be updated
    labelImage=self.labelNode.GetImageData()

    #save the 'last run state' information
    self.acMod=1
    self.lastRunPlane=self.ijkPlane
    self.lastModBy='2D'    #was last active contour run in 2D or 3D (cache needs to be recomputed)

    #update vars
    labelImage.Modified()
    self.labelNode.Modified()
    #labelNode.SetModifiedSinceRead(1)

  def runSegment3D(self):
    lm = slicer.app.layoutManager()
    self.computeCurrSlice()

    # get the parameters from MRML
    node = EditUtil.EditUtil().getParameterNode()
    currRad = int(node.GetParameter("KSliceEffect,radius"))

    #make connections, parameter settings
    self.ksliceMod.SetCurrSlice(self.currSlice)
    self.ksliceMod.SetBrushRad(currRad)
    self.ksliceMod.SetNumIts(10) # should be less than 2D!

    #execute a run
    #still doing 3D, user has not drawn => use cache
    useCache= (self.lastModBy=='3D') & (self.userMod==0) 
    print "use cache?:" + str(useCache)

    self.ksliceMod.runUpdate3D(not useCache)
    
    #signal to slicer that the label needs to be updated
    labelImage=self.labelNode.GetImageData()

    #save the 'last run state' information
    self.acMod=1
    self.lastModBy='3D'    #was last active contour run in 2D or 3D (cache needs to be recomputed)

    labelImage.Modified()
    self.labelNode.Modified()
    #labelNode.SetModifiedSinceRead(1)

  def destroy(self):
    #super(KSliceEffectLogic,self).destroy()

    print("Destroy in KSliceLogic has been called")
    #disconnect key shortcut
    for i in [0,1,2,3,4]:
        keyfun = self.qtkeydefs[i]
        keydef = self.qtkeyconnections[i]
        print('disconnecting keydef: ')
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
Example of an editor extension. No module interface here, only in the Editor module
"""
    parent.acknowledgementText = """
This editor extension was developed by
<Author>, <Institution>
"""

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
