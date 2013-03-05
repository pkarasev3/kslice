#get volume 
volNode=slicer.util.loadVolume("/u4/home/ikolesov3/Data/AdaptRadioData/origMHA/ANON0084/ANON0084.mha");  

#create a label volume, make it active
volumesLogic = slicer.modules.volumes.logic();
imgNode = getNode('ANON0084');
labNode = volumesLogic.CreateAndAddLabelVolume( slicer.mrmlScene, imgNode, imgNode.GetName() + '-label' );
selectionNode = slicer.app.applicationLogic().GetSelectionNode()
selectionNode.SetReferenceActiveVolumeID( imgNode.GetID() )
selectionNode.SetReferenceActiveLabelVolumeID( labNode.GetID() )
slicer.app.applicationLogic().PropagateVolumeSelection(0)


# got to the editor and do some drawing
editUtil = EditorLib.EditUtil.EditUtil()
parameterNode = editUtil.getParameterNode()
lm = slicer.app.layoutManager()
paintEffect = EditorLib.PaintEffectOptions()
paintEffect.setMRMLDefaults()
paintEffect.__del__()
sliceWidget = lm.sliceWidget('Red')
paintTool = EditorLib.PaintEffectTool(sliceWidget)
editUtil.setLabel(1)
paintTool.paintAddPoint(100,100)
paintTool.paintApply()
editUtil.setLabel(2)
paintTool.paintAddPoint(450,350)
paintTool.paintApply()
paintTool.cleanup()
paintTool = None



#create a KSlice Effect
#lm = slicer.app.layoutManager();

a=slicer.modules.editorExtensions['KSliceEffect'] #this is a KSliceEffectExtension
b=a(); #make an instance
c= b.options()  #this is a KSliceEffectOptions
c.onApply() #debug statement printed

kslice=vtkSlicerKSliceModuleLogicPython.vtkKSlice()
kslice.SetImageVol(imgNode.GetImageData())
kslice.SetLabelVol(labNode.GetImageData())
kslice.SetUIVol(labNode.GetImageData())
kslice.Initialize()
kslice.SetCurrSlice(0)
kslice.runUpdate()
kslice.PrintImage()

#connect the button shortcuts
global updateKSlice
def updateKSlice():
     print("KSlice updated")
     kslice.runUpdate()

#global updateKSlice
k = qt.QKeySequence(qt.Qt.Key_E)
s = qt.QShortcut(k, mainWindow())
s.connect('activated()', updateKSlice)
s.connect('activatedAmbiguously()', updateKSlice)


#get slider information, a.__dict__ and a.children() are useful commands
a=lm.sliceWidget('Red')
d=a.mrmlSliceNode()
d.GetSliceOffset() #gets the current slice location, just need spacing to figure out which slice currently working on 
e=imgNode.GetSpacing() 
e[2]
f=imgNode.GetOrigin()
