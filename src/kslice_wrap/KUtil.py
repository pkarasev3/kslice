import os
from __main__ import vtk, qt, ctk, slicer
from copy import copy, deepcopy

class KUtil:
  """
  This class is the 'hook' for slicer to detect and recognize the extension
  """
  def __init__(self, parent):
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
    FAIL = '\033[91m'
    ENDC = '\033[0m'

    def disable(self):
        self.HEADER = ''
        self.OKBLUE = ''
        self.OKGREEN = ''
        self.WARNING = ''
        self.FAIL = ''
        self.ENDC = ''

def Print_Bad( strInput ):
  print bcolors.FAIL + strInput + bcolors.ENDC
  
def Print_Good( strInput ):
  print bcolors.OKGREEN + strInput + bcolors.ENDC  

#To use code like this, you can do something like
#
#print bcolors.WARNING + "Warning: No active frommets remain. Continue?" 
#      + bcolors.ENDC
