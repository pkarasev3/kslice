#get volume 
#volNode=slicer.util.loadVolume("/raid1/home/pkarasev3/Dropbox/Quad study/ANON0084.mha")

filenameImage="../data/DS004.mha" # /home/kslice/kslice/data/ANON0084.mha"
# Let's try to use one that's in git, or a few downloadable by URL
volNode=slicer.util.loadVolume(filenameImage)

#volNode=slicer.util.loadVolume("/home/kslice/source/kslice/data/images/husney.mha")
if not volNode:
   volNode=slicer.util.loadVolume("/u4/home/ikolesov3/Data/AdaptRadioData/origMHA/ANON0084/ANON0084.mha")

# /u4/home/ikolesov3/Data/AdaptRadioData/origMHA/ANON0084/ANON0084.mha");  

#create a label volume, make it active
volumesLogic = slicer.modules.volumes.logic();
imgNode = getNode('DS004');
labNode = volumesLogic.CreateAndAddLabelVolume( slicer.mrmlScene, imgNode, imgNode.GetName() + '-label' );
selectionNode = slicer.app.applicationLogic().GetSelectionNode()
selectionNode.SetReferenceActiveVolumeID( imgNode.GetID() )
selectionNode.SetReferenceActiveLabelVolumeID( labNode.GetID() )
slicer.app.applicationLogic().PropagateVolumeSelection(0)

#enter editor
mW=slicer.util.mainWindow()
mW.moduleSelector().selectModule('Editor')

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
paintTool.paintAddPoint(450,350)
paintTool.paintApply()
paintTool.cleanup()
paintTool = None

