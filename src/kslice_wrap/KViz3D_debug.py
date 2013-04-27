
import vtk,sys,os

if len(sys.argv) < 2:
    sys.exit('Usage: %s filenameImage'  % sys.argv[0])

if not os.path.exists(sys.argv[1]):
    sys.exit('ERROR: %s was not found!' % sys.argv[1]) 
 
vol_in=sys.argv[1]
 
# Prepare to read the file
 
readerVolume = vtk.vtkMetaImageReader()
readerVolume.SetFileName(vol_in)
readerVolume.Update()
 
 
# Extract the region of interest
voi = vtk.vtkExtractVOI()
if vtk.VTK_MAJOR_VERSION <= 5:
    voi.SetInput(readerVolume.GetOutput())
else:
    voi.SetInputConnection(readerVolume.GetOutputPort())
 
#voi.SetVOI(0,517, 0,228, 0,392)
voi.SetSampleRate(1,1,1)
#voi.SetSampleRate(3,3,3)
voi.Update()#necessary for GetScalarRange()
srange= voi.GetOutput().GetScalarRange()#needs Update() before!
print "Range", srange
 
contour = vtk.vtkDiscreteMarchingCubes() #for label images
if vtk.VTK_MAJOR_VERSION <= 5:
    contour.SetInput( voi.GetOutput() )
else:
    contour.SetInputConnection( voi.GetOutputPort() )
contour.ComputeNormalsOn()
 
print "max value: " + str(srange[1])
##run through all labels

#for index in range(1, int(srange[1]) + 1):
 
index = int(srange[1])
print "Doing label", index

contour.SetValue(0, index)
contour.Update() #needed for GetNumberOfPolys() !!!


smoother= vtk.vtkWindowedSincPolyDataFilter()
if vtk.VTK_MAJOR_VERSION <= 5:
    smoother.SetInput(contour.GetOutput());
else:
    smoother.SetInputConnection(contour.GetOutputPort());
smoother.SetNumberOfIterations(5);
smoother.NonManifoldSmoothingOn();
smoother.NormalizeCoordinatesOn();
smoother.Update();

 # create a rendering window and renderer
ren = vtk.vtkRenderer()
renWin = vtk.vtkRenderWindow()
renWin.AddRenderer(ren)
renWin.SetSize(512,512) 
# create a renderwindowinteractor
iren = vtk.vtkRenderWindowInteractor()
iren.SetRenderWindow(renWin)
 
style = vtk.vtkInteractorStyleTrackballCamera()
iren.SetInteractorStyle(style)
 
# mapper
mapper = vtk.vtkPolyDataMapper()
mapper.SetInput( smoother.GetOutput() )
 
# actor
actor = vtk.vtkActor()
actor.SetMapper(mapper)
 
# assign actor to the renderer
ren.AddActor(actor)
 
# enable user interface interactor
iren.Initialize()
renWin.Render()
iren.Start()


