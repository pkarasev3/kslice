import os
from copy import copy, deepcopy

class KUtil:
  """
  This class is the 'hook' for slicer to detect and recognize the extension
  """
  def __init__(self, parent):
    from __main__ import vtk, qt, ctk, slicer # hide the import here so 
                                              #the rest can be tested easily w/o all heavy libs
    parent.title = "KUtil"
    parent.categories = [""]
    parent.contributors = [""]
    parent.helpText = ""
    parent.acknowledgementText = ""
    parent.hidden = True
    pass

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[101m'
    ENDC = '\033[0m'

    def disable(self):
        self.HEADER = ''
        self.OKBLUE = ''
        self.OKGREEN = ''
        self.WARNING = ''
        self.FAIL = ''
        self.ENDC = ''

def Print_Bad( strInput ):
  print bcolors.FAIL +  "(FAIL) " + strInput + bcolors.ENDC
  
def Print_Good( strInput ):
  print bcolors.OKGREEN +"(OK)   "+ strInput + bcolors.ENDC  

# Gulag 
 #volumesLogic = slicer.modules.volumes.logic()
        #tmp          =volumesLogic.CreateAndAddLabelVolume(slicer.mrmlScene, self.backgroundNode, self.backgroundNode.GetName() + nameSuffix)
        #tmp_imgDat   =tmp.GetImageData()
        #tmp_imgDat.SetDimensions(volSize[dim0], volSize[dim1],1) #use just one slice to keep track of changes
        #tmp_imgDat.SetScalarTypeToDouble()
        #tmp_imgDat.AllocateScalars()
        #tmpArr = slicer.util.array(self.backgroundNode.GetName() + nameSuffix) #get the numpy array
        #tmpArr[:]=0
        #tmpArr = 
        
#To use code like this, you can do something like
#
#print bcolors.WARNING + "Warning: No active frommets remain. Continue?" 
#      + bcolors.ENDC
