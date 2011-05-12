// First include the required header files for the VTK classes we are using.
#include "vtkConeSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCamera.h"
#include "vtkLODActor.h"
#include "vtkRenderer.h"
#include "vtkMetaImageReader.h"
#include "vtkIndent.h"
#include "vtkExtractVOI.h"
#include "vtkImageData.h"  //should not need this b/c vtkMetaImageReader should inherit
#include "vtkDataSetMapper.h"
//#include "vtkAlgorithm.h"
#include "vtkProperty.h"
#include "vtkContourFilter.h"
#include "vtkPolyDataNormals.h"
#include "vtkMarchingCubes.h"
#include "vtkImagePlaneWidget.h"
#include "vtkImageActor.h"
#include "vtkImageMapper.h"
#include "vtkImageMapToColors.h"
//#include "vtkDataObject.h"
#include "vtkLookupTable.h"
#include "vtkImageShiftScale.h"
#include "vtkImageReslice.h"
#include "vtkAlgorithmOutput.h"
#include  "vtkLight.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkCallbackCommand.h"
#include <iostream> //can get (cout)
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkFiniteDifferenceGradientEstimator.h"
#include "vtkVolumeProperty.h"
#include "vtkVolumeRayCastCompositeFunction.h"
#include "vtkVolumeRayCastMIPFunction.h"
#include "vtkVolumeRayCastCompositeFunction.h"
#include "vtkVolumeRayCastMIPFunction.h"
#include "vtkXMLPolyDataWriter.h"
#include "vtkImageMedian3D.h"
#include <string>

#include "vtkImageGaussianSmooth.h"

#include "vtkCleanPolyData.h"

using namespace std;

int main( int argc, char* argv[] )
{
  cout << argv[0] << " file1.mha   file2.mha   opacity_float(e.g. 3.0)" << endl;

  char* FemurLabelArrayFilename=argv[1];
  float modelOpacity3D = 0.9;
  if( argc == 3 ) {
    modelOpacity3D=atof(argv[2]); //convert character array to float
  }

  int imgPlacementZ=0;

  vtkMetaImageReader *labFemurReader= vtkMetaImageReader::New();

  int canReadFemurLab=labFemurReader->CanReadFile(FemurLabelArrayFilename); //will return 0 if cannot read file

  if (canReadFemurLab==0) {			//I want to make sure to check both files at the same time

    cout<<"Could not read file"<<FemurLabelArrayFilename<<"\n";
    return 0;
  }
  cout<<"LabelArrayFilenameay: "<<FemurLabelArrayFilename<<"\n";
  cout<<"Image Placement Z: "<<imgPlacementZ<<"\n";

  //if we are here we can succesfully read both files so define the readers
  labFemurReader->SetFileName(FemurLabelArrayFilename);


  labFemurReader->Update();

  vtkImageData* data = labFemurReader->GetOutput();
  vtkImageMedian3D* med = vtkImageMedian3D::New();
  med->SetInput( data );
  med->SetKernelSize( 3, 3, 3 );
  med->Update();

  vtkImageGaussianSmooth* gauss = vtkImageGaussianSmooth::New();
  gauss->SetInput( med->GetOutput() );
  gauss->SetStandardDeviations( 1.0, 1.0, 1.0 );
  gauss->Update();
///////////////////////Making the 3D Femur Actor//////////////////////////
  
  //if ever want to take out portion of the Volume, use the extractor
  vtkExtractVOI *labelSubVolumeExtractor1=vtkExtractVOI::New();
  labelSubVolumeExtractor1->SetInput( gauss->GetOutput() );

  //vtkMarchingCubes  is designed to excel when receiving a volume input, while vtkContourFilter
  //can work acceptably with 2-D or 3-D data. Used to make isosurfaces (surfaces separating different values)
  vtkMarchingCubes *cube_marcher1=vtkMarchingCubes::New();
  cube_marcher1->SetInput(labelSubVolumeExtractor1->GetOutput());
  cube_marcher1->ComputeNormalsOn();
  cube_marcher1->ComputeGradientsOff();
  cube_marcher1->ComputeScalarsOn();
  cube_marcher1->SetValue(0,1);

  //Using to smooth out the edges make it more "biologically feasible"
  vtkSmoothPolyDataFilter *smoother1=vtkSmoothPolyDataFilter::New();
  smoother1->SetInput(cube_marcher1->GetOutput());
  smoother1->SetEdgeAngle(60);
  smoother1->SetNumberOfIterations(50);

  //vtkPolyDataNormals compute normals for polygonal mesh. Sharp edges can be split
  //and points duplicated with separate normals to give crisp (rendered) surface definition
  vtkPolyDataNormals *labSurfNormals1=vtkPolyDataNormals::New();
  vtkPolyData*     polyData = smoother1->GetOutput();

  vtkXMLPolyDataWriter* writer = vtkXMLPolyDataWriter::New();
  writer->SetInput( polyData );
  string strName = string(argv[1]) + ".vtp";
  writer->SetFileName( strName.c_str() );
  writer->Write();

  labSurfNormals1->SetInput(smoother1->GetOutput());
  labSurfNormals1->SetFeatureAngle(60);


  vtkDataSetMapper *labelMapper1=vtkDataSetMapper::New(); //or use vtkPolyDataMapper--check the capabilities of each
  labelMapper1->SetInput(labSurfNormals1->GetOutput());
  labelMapper1->SetImmediateModeRendering(1);
  labelMapper1->ScalarVisibilityOff();


  //make and actor and set its properties
  vtkLODActor *labelActor3D1 = vtkLODActor::New(); //actor types takes away detail if interaction is fast
  labelActor3D1->SetMapper( labelMapper1 );


  vtkProperty *propSetter1=vtkProperty::New();
  propSetter1=labelActor3D1->GetProperty();
  propSetter1->SetRepresentationToSurface();//->SetRepresentationToSurface();
  propSetter1->SetAmbient(0);
  propSetter1->SetDiffuse(1);
  propSetter1->SetDiffuseColor(1,.49,.25);
  propSetter1->SetSpecular(.1);
  propSetter1->SetSpecularPower(1);
  propSetter1->SetSpecularColor(.7,.49,.25);

  propSetter1->SetInterpolationToFlat();
  propSetter1->SetOpacity(1-modelOpacity3D);

  //propSetter1->SetColor(.8,.4,.4);//rarely change these, so keep as constant

  ///////////////////////DONE Making the 3D Femur Actor//////////////////////////


  //////////////////////Make transfer functions to determine how data will look///////////////////////
  vtkPiecewiseFunction *opacityTransferFunction=vtkPiecewiseFunction::New();
  opacityTransferFunction->ClampingOff();
  opacityTransferFunction->AddPoint(750,1-modelOpacity3D);
  opacityTransferFunction->AddPoint(1300, 1);

 vtkPiecewiseFunction *opacityGradientTransferFunction=vtkPiecewiseFunction::New();
  opacityGradientTransferFunction->AddPoint(0,0);
  opacityGradientTransferFunction->AddPoint(5,0);
  opacityGradientTransferFunction->AddPoint(20,0);
  opacityGradientTransferFunction->ClampingOn();

  vtkColorTransferFunction *colorTransferFunction=vtkColorTransferFunction::New();
  colorTransferFunction->AddRGBPoint(750, 1, 0, 0);
  colorTransferFunction->AddRGBPoint(1300,0, 1, 0);

  vtkVolumeProperty *volumeProperty=vtkVolumeProperty::New();
  volumeProperty->SetColor(colorTransferFunction);
  volumeProperty->SetScalarOpacity(opacityTransferFunction);

  volumeProperty->ShadeOn();
  volumeProperty->SetAmbient(0.05);
  volumeProperty->SetDiffuse(0.1);
  volumeProperty->SetSpecular(0.05);
  volumeProperty->SetSpecularPower(20);
  volumeProperty->SetInterpolationTypeToLinear();

  vtkRenderer *ren1= vtkRenderer::New();
  ren1->AddActor( labelActor3D1 );
  ren1->SetBackground(1,1,1);
  vtkRenderWindow *renWin = vtkRenderWindow::New();
  renWin->AddRenderer( ren1 );
  renWin->SetSize( 800, 800 );

  vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(renWin);

//  iren->AddObserver( vtkCommand::KeyPressEvent, cbc );
//  iren->AddObserver( vtkCommand::RightButtonPressEvent, cbc );
//  iren->AddObserver( vtkCommand::MouseWheelBackwardEvent, idle );
//  iren->AddObserver( vtkCommand::MouseWheelForwardEvent, idle );

  iren->Initialize();
  iren->Start();


}
