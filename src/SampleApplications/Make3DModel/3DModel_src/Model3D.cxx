/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: Cone.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
//
// This example creates a polygonal model of a cone, and then renders it to
// the screen. It will rotate the cone 360 degrees and then exit. The basic
// setup of source -> mapper -> actor -> renderer -> renderwindow is 
// typical of most VTK programs.
//

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


//#include <conio.h>

vtkImageActor* imageActor;
vtkRenderWindowInteractor *iren;
vtkImageReslice *imageReslicer;
int drawZpos;
double dispSlicePos;

void redraw_func( vtkObject* caller, unsigned long eventID, void* ClientData, void* data )
{
	switch (eventID) {
		case vtkCommand::MouseWheelForwardEvent:
		case vtkCommand::MouseWheelBackwardEvent:
			break;
		default:
			break;
	}
}

void clickhandle( vtkObject* caller, unsigned long eventID, void* ClientData, void* data )
{
	switch (eventID ) {
		case vtkCommand::KeyPressEvent:
			int X, Y;
			iren->GetEventPosition( X, Y );
//	                const char keypressed = *(iren -> GetKeySym());
//			switch( keypressed )
//			{
//			    case 'w':
//                    drawZpos--;
//                    imageActor->SetPosition(0,0,drawZpos);
//                    cout<<drawZpos<<"\n";
//                    break;
//                case 'W':
//                    drawZpos++; // for 'ventura' suggested 59 here
//                    imageActor->SetPosition(0,0,drawZpos);
//                     cout<<drawZpos<<"\n";
//                    break;
//                case 's': // for 'ventura' suggested 77 here
//                    dispSlicePos -= 1.0; // update the position from which a slice is read
//                    imageReslicer->SetResliceAxesOrigin(0,0,dispSlicePos );
//                     cout<<dispSlicePos<<"\n";
//	                break;
//                case 'S':
//                    dispSlicePos += 1.0; // update the position from which a slice is read
//                    imageReslicer->SetResliceAxesOrigin(0,0,dispSlicePos );
//                    cout<<dispSlicePos<<"\n";
//                    break;
//                case 'g':
//					cout<<"The point we clicked has coordinates: \n";
//
//                default:
//                    break;
//			}
//			break;
		default:
			cout<<"Bogus Event! \n";
			break;
	}
}

using namespace std; //so dont have to write std::cout, uses all func, obj in std library


int main(int argc,      // Number of strings in array argv
          char *argv[],   // Array of command-line argument strings
          char *envp[] )
//(string LabelArrayFilename, string ImageArrayFilename, float modelOpacity3D,int minIntensity, int maxIntensity, int imgPlacementZ)
{
  //THE INPUTS EXPLAINED:
  //string FemurLabelArrayFilename: location of the label map
  //string TunnelLabelArrayFilename: location of corresponding medical image
  //float modelOpacity3D: opacity of the 3D model ( from 0 to 1)
  //int minIntensity: bottom cutoff of Look Up Table used to display medical image(reference Image)
  //int maxIntensity: top cutoff of Look Up Table used to display medical image
  //int imgPlacementZ: z coord of reference slice

  //make the readers
  char* FemurLabelArrayFilename=argv[1];
  char* TunnelLabelArrayFilename=argv[2];
  float modelOpacity3D=atof(argv[3]); //convert character array to float

  cout<<"crashing, parse my args right while removing crasher" << endl;

  int imgPlacementZ=atoi(argv[6]);
  drawZpos = imgPlacementZ;

  vtkMetaImageReader *labFemurReader= vtkMetaImageReader::New();
  vtkMetaImageReader *labTunnelReader= vtkMetaImageReader::New();
  
  int canReadFemurLab=labFemurReader->CanReadFile(FemurLabelArrayFilename); //will return 0 if cannot read file
  int canReadTunnelImg=labTunnelReader->CanReadFile(TunnelLabelArrayFilename); //will return 0 if cannot read file
  //for some reason if do reader->SetFileName() and then reader->CanReadFile(), when use reader get an error!

  if (canReadFemurLab==0) {			//I want to make sure to check both files at the same time
	  if( canReadTunnelImg==0){
		cout<<"Could not read file "<<TunnelLabelArrayFilename<<" \n";
	  }
		cout<<"Could not read file"<<FemurLabelArrayFilename<<"\n";
		return 0;
  }else if(canReadTunnelImg==0){
	  cout<<"Could not read file"<<TunnelLabelArrayFilename<<"\n";
	  return 0;	
  }
  
  cout<<"LabelArrayFilenameay: "<<FemurLabelArrayFilename<<"\n";
  cout<<"ImageArrayFilenameay: "<<TunnelLabelArrayFilename<<"\n";
  cout<<"Image Placement Z: "<<imgPlacementZ<<"\n";

  //if we are here we can succesfully read both files so define the readers
  labFemurReader->SetFileName(FemurLabelArrayFilename);
  labTunnelReader->SetFileName(TunnelLabelArrayFilename);

  labFemurReader->Update(); //WHY DO I NEED THIS? otherwise get memory read error?
  labTunnelReader->Update(); //WHY DO I NEED THIS? otherwise get memory read error?

  // Set the callback functions. Right click the args and goto-definition to see them.
  vtkCallbackCommand* cbc = vtkCallbackCommand::New();
  vtkCallbackCommand* idle = vtkCallbackCommand::New();
  cbc->SetCallback( clickhandle );
  idle->SetCallback( redraw_func );



  ///////////////////////Making the 3D Femur Actor//////////////////////////
  
  //if ever want to take out portion of the Volume, use the extractor
  vtkExtractVOI *labelSubVolumeExtractor1=vtkExtractVOI::New();
  labelSubVolumeExtractor1->SetInput(labFemurReader->GetOutput());

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
  smoother1->SetNumberOfIterations(300);

  //vtkPolyDataNormals compute normals for polygonal mesh. Sharp edges can be split
  //and points duplicated with separate normals to give crisp (rendered) surface definition
  vtkPolyDataNormals *labSurfNormals1=vtkPolyDataNormals::New();
  labSurfNormals1->SetInput(smoother1->GetOutput());  //input used to be "cube_marcher->GetOutput()
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
  propSetter1->SetDiffuseColor(1,1,1);//(1,.49,.25);
  propSetter1->SetSpecular(.1);
  propSetter1->SetSpecularPower(1);
  propSetter1->SetSpecularColor(.7,.49,.25);

  propSetter1->SetInterpolationToFlat();
  propSetter1->SetOpacity(1-modelOpacity3D);

  //propSetter1->SetColor(.8,.4,.4);//rarely change these, so keep as constant

  ///////////////////////DONE Making the 3D Femur Actor//////////////////////////


  
  ///////////////////////Making the 3D Tunnel Actor//////////////////////////
  
  //if ever want to take out portion of the Volume, use the extractor
  vtkExtractVOI *labelSubVolumeExtractor2=vtkExtractVOI::New();
  labelSubVolumeExtractor2->SetInput(labTunnelReader->GetOutput());

  //vtkMarchingCubes  is designed to excel when receiving a volume input, while vtkContourFilter
  //can work acceptably with 2-D or 3-D data. Used to make isosurfaces (surfaces separating different values)
  vtkMarchingCubes *cube_marcher2=vtkMarchingCubes::New();
  cube_marcher2->SetInput(labelSubVolumeExtractor2->GetOutput());
  cube_marcher2->ComputeNormalsOn();
  cube_marcher2->SetValue(0,1);

  //Using to smooth out the edges make it more "biologically feasible"
  vtkSmoothPolyDataFilter *smoother2=vtkSmoothPolyDataFilter::New();
  smoother2->SetInput(cube_marcher2->GetOutput());
  smoother2->SetEdgeAngle(30);
  smoother2->SetNumberOfIterations(50);


  //vtkPolyDataNormals compute normals for polygonal mesh. Sharp edges can be split
  //and points duplicated with separate normals to give crisp (rendered) surface definition
  vtkPolyDataNormals *labSurfNormals2=vtkPolyDataNormals::New();
  labSurfNormals2->SetInput(smoother2->GetOutput());  //input used to be "cube_marcher->GetOutput()
  labSurfNormals2->SetFeatureAngle(60);


  vtkDataSetMapper *labelMapper2=vtkDataSetMapper::New(); //or use vtkPolyDataMapper--check the capabilities of each
  labelMapper2->SetInput(labSurfNormals2->GetOutput());
  labelMapper2->SetImmediateModeRendering(1);
  labelMapper2->ScalarVisibilityOff();


  //make and actor and set its properties
  vtkLODActor *labelActor3D2 = vtkLODActor::New(); //actor types takes away detail if interaction is fast
  labelActor3D2->SetMapper( labelMapper2 );


  vtkProperty *propSetter2=vtkProperty::New();
  propSetter2=labelActor3D2->GetProperty();
  propSetter2->SetRepresentationToSurface();//->SetRepresentationToSurface();
  propSetter2->SetAmbient(0);
  propSetter2->SetDiffuse(1);
  propSetter2->SetDiffuseColor(1,.5,.5);
  propSetter2->SetSpecular(0);
  propSetter2->SetSpecularPower(1);
  propSetter2->SetSpecularColor(1,1,1);

  propSetter2->SetInterpolationToGouraud();




  //propSetter->SetInterpolationToGouraud();
  //propSetter2->SetOpacity(modelOpacity3D);
  //propSetter2->SetColor(.2,.4,.4);//rarely change these, so keep as constant


  ///////////////////////DONE Making the 3D Tunnel Actor//////////////////////////



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
  ren1->AddActor( labelActor3D2);
  ren1->SetBackground(.8,.8,.8);
  vtkRenderWindow *renWin = vtkRenderWindow::New();
  renWin->AddRenderer( ren1 );
  renWin->SetSize( 300, 300 );

  iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(renWin);

  iren->AddObserver( vtkCommand::KeyPressEvent, cbc );
  iren->AddObserver( vtkCommand::RightButtonPressEvent, cbc );
  iren->AddObserver( vtkCommand::MouseWheelBackwardEvent, idle );
  iren->AddObserver( vtkCommand::MouseWheelForwardEvent, idle );

  iren->Initialize();
  iren->Start();


  // Free up any objects we created. All instances in VTK are deleted by using the Delete() method.

  ren1->Delete();
  renWin->Delete();

  return 0;
}


#if 0
// PURGATORY
 int minIntensity=atoi(argv[4]);
  int maxIntensity=atoi(argv[5]);
//volumeProperty->SetInterpolationTypeToNearest();
  //ren1->AddActor( imageActor1);
  //ren1->SetBackground(.8,.8,1);

//  vtkVolumeRayCastCompositeFunction  *compositeFunction=vtkVolumeRayCastCompositeFunction::New();
//
//  vtkVolumeRayCastMIPFunction *mipFunction=vtkVolumeRayCastMIPFunction::New();
//  mipFunction->SetMaximizeMethodToOpacity();


  /////////////////////////////Display the Actors//////////////////////////////////////////////////////////

  //vtkFiniteDifferenceGradientEstimator *GradientEstimator=vtkFiniteDifferenceGradientEstimator::New();

  //volumeProperty->SetGradientOpacity(opacityGradientTransferFunction);


  //labelActor3D1->SetOrigin(0,0,0);
  //labelActor3D2->SetOrigin(0,0,0);

  // Create the Renderer and assign actors to it. A renderer is like a viewport. It is part or all of a window
  // on the screen and it is responsible for drawing the actors it has.  We also set the background color here.

//  vtkLight *light=vtkLight::New();
//  light->SetFocalPoint( 0.0,  0.0,  0.0);
//  light->SetPosition(1.0 ,0.0 ,0.0);
//  light->SetColor(0.0, 1.0, 0.0);
//  light->SetIntensity(0.5);
//  ren1->AddLight(light);




  //Render automatically instantiates cameras and lights
  //vtkLight *lightObj=vtkLight::New();
  //lightObj->SetColor(1,1,1);
  //lightObj->SetFocalPoint(ren1->GetActiveCamera()->GetFocalPoint());
  //lightObj->SetPosition(ren1->GetActiveCamera()->GetPosition());
  //ren1->AddLight(lightObj);


  // Finally we create the render window which will show up on the screen. We put our renderer into the render
  // window using AddRenderer. We also set the size to be 300 pixels by 300.
  //renWin->Render(); EVERYTHING in vtk is passive until told to act!!!

//Interactor allow some user commands: try pressing t,j to change motion type
  // Wait for user to end program, but not necessary here b/c runs until close the RenderWindow anyways
  //std::cout <<"Please hit any key to terminate program" << std::endl;
  //while( !_getch() );

#endif
