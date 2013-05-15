import os
from __main__ import vtk, qt, ctk, slicer
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

def Print_Coord_Debug(xyz, RAS, xy, ijk, sliceWidget):
  print "the x,y,z coordinate is: " + str(xyz[0]) + "  " + str(xyz[1]) + "  " + str(xyz[2]) #debug
  RAS= sliceWidget.sliceView().convertXYZToRAS(xyz);
  print "the RAS coordinate is: " + str(RAS[0]) + "  " + str(RAS[1]) + "  " + str(RAS[2]) #debug
  print "the x,y used is: " + str(xy[0])+ "  " + str(xy[1]) #debug
  print "ijk is: " + str(ijk) #debug
  
# Gulag 
   #print str(viewName) + "," + str(vo)
#print("Got a %s at %s in %s", (event,str(xy),viewName))
#values = get_values_at_IJK(ijk,self.sliceWidget)   #this fails if KSliceEffect is active, user turns off label on some slice view, clicks 

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