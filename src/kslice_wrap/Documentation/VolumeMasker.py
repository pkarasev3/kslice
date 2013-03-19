from __main__ import vtk, qt, ctk, slicer
import EditorLib

#
# VolumeMasker
#

class VolumeMasker:
  def __init__(self, parent):
    import string
    parent.title = "Volume Masker"
    parent.categories = ["Work in Progress"]
    parent.contributors = ["Steve Pieper (Isomics)"]
    parent.helpText = string.Template("""
Use this module to define a masked volume based on an input volume and a label volume. This module observes the input and label volumes to recompute the masked volume when the others are modified. Masking is done in image space and does not account for transforms. Note: volumes must have same dimensions. See <a href=\"$a/Documentation/$b.$c/Modules/VolumeMasker\">$a/Documentation/$b.$c/Modules/VolumeMasker</a> for more information.
""").substitute({ 'a':parent.slicerWikiUrl, 'b':slicer.app.majorVersion, 'c':slicer.app.minorVersion })
    parent.acknowledgementText = """
Supported by NA-MIC, NAC, BIRN, NCIGT, and the Slicer Community. See http://www.slicer.org for details. Module implemented by Steve Pieper.
"""
    self.parent = parent

#
# qSlicerPythonModuleExampleWidget
#

class VolumeMaskerWidget:
  def __init__(self, parent=None):
    if not parent:
      self.parent = slicer.qMRMLWidget()
      self.parent.setLayout(qt.QVBoxLayout())
      self.parent.setMRMLScene(slicer.mrmlScene)
    else:
      self.parent = parent
    self.logic = VolumeMaskerLogic()
    
  def setup(self):
    #
    # the inputVolume volume selector
    #
    self.inputVolumeSelectorFrame = qt.QFrame(self.parent)
    self.inputVolumeSelectorFrame.setLayout(qt.QHBoxLayout())
    self.parent.layout().addWidget(self.inputVolumeSelectorFrame)

    self.inputVolumeSelectorLabel = qt.QLabel("Input Volume: ", self.inputVolumeSelectorFrame)
    self.inputVolumeSelectorLabel.setToolTip( "Select the inputVolume volume (background inputVolume scalar volume node) for statistics calculations")
    self.inputVolumeSelectorFrame.layout().addWidget(self.inputVolumeSelectorLabel)

    self.inputVolumeSelector = slicer.qMRMLNodeComboBox(self.inputVolumeSelectorFrame)
    self.inputVolumeSelector.nodeTypes = ( ("vtkMRMLScalarVolumeNode"), "" )
    self.inputVolumeSelector.addAttribute( "vtkMRMLScalarVolumeNode", "LabelMap", 0 )
    self.inputVolumeSelector.selectNodeUponCreation = False
    self.inputVolumeSelector.addEnabled = False
    self.inputVolumeSelector.removeEnabled = False
    self.inputVolumeSelector.noneEnabled = True
    self.inputVolumeSelector.showHidden = False
    self.inputVolumeSelector.showChildNodeTypes = False
    self.inputVolumeSelector.setMRMLScene( slicer.mrmlScene )
    self.inputVolumeSelectorFrame.layout().addWidget(self.inputVolumeSelector)

    
    #
    # the label volume selector
    #
    self.labelSelectorFrame = qt.QFrame()
    self.labelSelectorFrame.setLayout( qt.QHBoxLayout() )
    self.parent.layout().addWidget( self.labelSelectorFrame )

    self.labelSelectorLabel = qt.QLabel()
    self.labelSelectorLabel.setText( "Label Map: " )
    self.labelSelectorFrame.layout().addWidget( self.labelSelectorLabel )

    self.labelSelector = slicer.qMRMLNodeComboBox()
    self.labelSelector.nodeTypes = ( "vtkMRMLScalarVolumeNode", "" )
    self.labelSelector.addAttribute( "vtkMRMLScalarVolumeNode", "LabelMap", "1" )
    self.labelSelector.selectNodeUponCreation = False
    self.labelSelector.addEnabled = False
    self.labelSelector.noneEnabled = True
    self.labelSelector.removeEnabled = False
    self.labelSelector.showHidden = False
    self.labelSelector.showChildNodeTypes = False
    self.labelSelector.setMRMLScene( slicer.mrmlScene )
    self.labelSelector.setToolTip( "Pick the label map to edit" )
    self.labelSelectorFrame.layout().addWidget( self.labelSelector )

    # create and add EditColor directly to "edit label map" section
    self.editColor = EditorLib.EditColor(self.parent)

    #
    # the output volume label
    #
    self.outputVolumeSelectorFrame = qt.QFrame(self.parent)
    self.outputVolumeSelectorFrame.setLayout(qt.QHBoxLayout())
    self.parent.layout().addWidget(self.outputVolumeSelectorFrame)

    self.outputVolumeSelectorLabel = qt.QLabel("Masked Volume: ", self.outputVolumeSelectorFrame)
    self.outputVolumeSelectorFrame.layout().addWidget(self.outputVolumeSelectorLabel)
    
    # Apply button
    self.applyButton = qt.QPushButton("Apply")
    self.applyButton.toolTip = "Apply the masking."
    self.applyButton.enabled = False
    self.parent.layout().addWidget(self.applyButton)

    # Auto button
    self.autoApplyButton = qt.QCheckBox("Auto Apply")
    self.autoApplyButton.toolTip = "Apply the change when either input value changes."
    self.autoApplyButton.enabled = False
    self.parent.layout().addWidget(self.autoApplyButton)

    # Add vertical spacer
    self.parent.layout().addStretch(1)

    # connections
    self.applyButton.connect('clicked()', self.onApply)
    self.autoApplyButton.connect('toggled(bool)', self.onAutoApply)
    self.inputVolumeSelector.connect('currentNodeChanged(vtkMRMLNode*)', self.onInputSelect)
    self.labelSelector.connect('currentNodeChanged(vtkMRMLNode*)', self.onLabelSelect)
    
  def updateEnableState(self):
    enabled = bool(self.logic.inputVolumeNode) and bool(self.logic.labelNode)
    self.applyButton.enabled = enabled
    self.autoApplyButton.enabled = enabled
    if not enabled:
      self.outputVolumeSelectorLabel.text = ""


  def onInputSelect(self, node):
    self.logic.inputVolumeNode = node
    self.updateEnableState()

  def onLabelSelect(self, node):
    self.logic.labelNode = node
    self.updateEnableState()

  def updateLabelText(self):
    if self.logic.outputVolumeNode:
      self.outputVolumeSelectorLabel.text = self.logic.outputVolumeNode.GetName()
    else:
      self.outputVolumeSelectorLabel.text = ""

  def onApply(self):
    """Calculate the mask volume
"""
    if self.logic.apply():
      self.updateLabelText()

  def onAutoApply(self,state):
    """Calculate the mask volume
"""
    if state:
      if self.logic.apply():
        self.logic.autoApply()
        self.updateLabelText()
    else:
      self.logic.removeObservers()

class VolumeMaskerLogic:
  """Implement the logic to calculate the mask volume
"""
  
  def __init__(self):
    self.inputVolumeNode = None
    self.labelNode = None
    self.outputVolumeNode = None
    self.observerTags = {}
    self.lastMaskLabel = None

  def removeObservers(self):
    if self.observerTags != {}:
      for n in self.observerTags.keys():
        n.RemoveObserver(self.observerTags[n])
    self.observerTags = {}
    
  def apply(self):
    if not (self.inputVolumeNode and self.labelNode):
      return False
    if not self.outputVolumeNode:
      volumesLogic = slicer.modules.volumes.logic()
      outputName = self.inputVolumeNode.GetName() + "-masked"
      self.outputVolumeNode = volumesLogic.CloneVolume(slicer.mrmlScene, self.inputVolumeNode, outputName)
    self.performMasking()
    return True

  def autoApply(self):
    self.removeObservers()
    parameterNode = EditorLib.EditUtil.EditUtil().getParameterNode()
    self.observerTags[parameterNode] = parameterNode.AddObserver(
        vtk.vtkCommand.ModifiedEvent, self.conditionalPerformMasking)
    nodes = (self.inputVolumeNode, self.labelNode)
    for n in nodes:
      self.observerTags[n] = n.AddObserver(n.ImageDataModifiedEvent, self.performMasking)

  def conditionalPerformMasking(self,object=None,event=None):
    """Since we cannot observe for just the label value
changing on the parameter node, we check the label value
or else we would be re-masking for every small edit
parameter change.
"""
    label = EditorLib.EditUtil.EditUtil().getLabel()
    if label != self.lastMaskLabel:
      self.performMasking()

  def performMasking(self,object=None,event=None):
    import numpy
    label = EditorLib.EditUtil.EditUtil().getLabel()
    a = slicer.util.array(self.inputVolumeNode.GetID())
    la = slicer.util.array(self.labelNode.GetID())
    ma = slicer.util.array(self.outputVolumeNode.GetID())
    mask = numpy.ndarray.copy(la)
    # TODO: can there be one vector operation that sets
    # everything to 0 or 1?
    mask[mask == label] = 1
    mask[mask != 1] = 0
    ma[:] = a * mask
    self.outputVolumeNode.GetImageData().Modified()
    self.lastMaskLabel = label



class Slicelet(object):
  """A slicer slicelet is a module widget that comes up in stand alone mode
implemented as a python class.
This class provides common wrapper functionality used by all slicer modlets.
"""
  # TODO: put this in a SliceletLib
  # TODO: parse command line arge


  def __init__(self, widgetClass=None):
    self.parent = qt.QFrame()
    self.parent.setLayout( qt.QVBoxLayout() )

    # TODO: should have way to pop up python interactor
    self.buttons = qt.QFrame()
    self.buttons.setLayout( qt.QHBoxLayout() )
    self.parent.layout().addWidget(self.buttons)
    self.addDataButton = qt.QPushButton("Add Data")
    self.buttons.layout().addWidget(self.addDataButton)
    self.addDataButton.connect("clicked()",slicer.app.ioManager().openAddDataDialog)
    self.loadSceneButton = qt.QPushButton("Load Scene")
    self.buttons.layout().addWidget(self.loadSceneButton)
    self.loadSceneButton.connect("clicked()",slicer.app.ioManager().openLoadSceneDialog)

    if widgetClass:
      self.widget = widgetClass(self.parent)
      self.widget.setup()
    self.parent.show()

class VolumeMaskerSlicelet(Slicelet):
  """ Creates the interface when module is run as a stand alone gui app.
"""

  def __init__(self):
    super(VolumeMaskerSlicelet,self).__init__(VolumeMaskerWidget)


if __name__ == "__main__":
  # TODO: need a way to access and parse command line arguments
  # TODO: ideally command line args should handle --xml

  import sys
  print( sys.argv )

  slicelet = VolumeMaskerSlicelet()
