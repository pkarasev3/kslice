#include <vtkSmartPointer.h>
#include <vtkInteractorStyleUser.h>
#include <vtkProperty.h>
#include <vtkOutlineFilter.h>
#include <vtkCommand.h>
#include <vtkSliderWidget.h>
#include <vtkSliderRepresentation.h>
#include <vtkSliderRepresentation3D.h>
#include <vtkImageData.h>
#include <vtkCellArray.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkContourFilter.h>
#include <vtkXMLImageDataWriter.h>
#include <vtkMetaImageReader.h>
#include <vtkXMLPolyDataWriter.h>

#define SP( X )  vtkSmartPointer<X>

class vtkSliderCallback : public vtkCommand
{
public:
  static vtkSliderCallback *New()
  {
    return new vtkSliderCallback();
  }
  virtual void Execute(vtkObject *caller, unsigned long, void*)
  {
    time_t seconds;
    seconds = time(NULL);
    if( abs( double( seconds - previous_seconds ) ) < 1000 ) {
      std::cout<<".";
      return;
    } else {
      previous_seconds = seconds;
    }
    vtkSliderWidget *sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
    double value = static_cast<vtkSliderRepresentation *>(sliderWidget->GetRepresentation())->GetValue();
    this->ContourFilter->GenerateValues(1, value, value);

  }
  vtkSliderCallback():ContourFilter(NULL) { previous_seconds = 0; }
  vtkContourFilter* ContourFilter;
  time_t previous_seconds;

};

int main(int argc, char * argv[])
{
  if( argc != 2 ) {
    std::cout << "fail, must use " << argv[0] << " labelVolume.mha " << std::endl;
  } else {
    std::cout << "attempting to read " << argv[1] << std::endl;
  }
  vtkSmartPointer<vtkImageData> data = SP(vtkImageData)::New();
  SP(vtkMetaImageReader) reader      = SP(vtkMetaImageReader)::New();
  reader->SetFileName( argv[1] );
  reader->Update();
  data                               = reader->GetOutput();

  // Create an isosurface
  vtkSmartPointer<vtkContourFilter> contourFilter =
    vtkSmartPointer<vtkContourFilter>::New();
  contourFilter->SetInput(data);

  double label_range[2];
  data->GetScalarRange( label_range );
  std::cout << "scalar range: " << label_range[0] << ", " << label_range[1] << std::endl;
  contourFilter->GenerateValues(label_range[0], label_range[1], 10); // (numContours, rangeStart, rangeEnd)

  // Map the contours to graphical primitives
  vtkSmartPointer<vtkPolyDataMapper> contourMapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  contourMapper->SetInput(contourFilter->GetOutput());

  // Create an actor for the contours
  vtkSmartPointer<vtkActor> contourActor =
    vtkSmartPointer<vtkActor>::New();
  contourActor->SetMapper(contourMapper);

  // Create the outline
  vtkSmartPointer<vtkOutlineFilter> outlineFilter =
    vtkSmartPointer<vtkOutlineFilter>::New();
  outlineFilter->SetInput(data);
  vtkSmartPointer<vtkPolyDataMapper> outlineMapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  outlineMapper->SetInputConnection(outlineFilter->GetOutputPort());
  vtkSmartPointer<vtkActor> outlineActor =
    vtkSmartPointer<vtkActor>::New();
  outlineActor->SetMapper(outlineMapper);

  // Visualize
  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  vtkSmartPointer<vtkRenderWindowInteractor> interactor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  interactor->SetRenderWindow(renderWindow);

  renderer->AddActor(contourActor);
  renderer->AddActor(outlineActor);

  vtkSmartPointer<vtkSliderRepresentation3D> sliderRep =
    vtkSmartPointer<vtkSliderRepresentation3D>::New();
  sliderRep->SetMinimumValue(label_range[0]);
  sliderRep->SetMaximumValue(label_range[1]);
  sliderRep->SetValue(label_range[1]/2);
  sliderRep->SetTitleText("Contour value");
  sliderRep->SetPoint1InWorldCoordinates(-20, -40, 0);
  sliderRep->SetPoint2InWorldCoordinates(0, -40, 0);
  sliderRep->SetSliderWidth(10);
  sliderRep->SetLabelHeight(0.5);

  vtkSmartPointer<vtkSliderWidget> sliderWidget =
    vtkSmartPointer<vtkSliderWidget>::New();
  sliderWidget->SetInteractor(interactor);
  sliderWidget->SetRepresentation(sliderRep);
  sliderWidget->SetAnimationModeToAnimate();
  sliderWidget->EnabledOn();

  vtkSmartPointer<vtkSliderCallback> callback =
    vtkSmartPointer<vtkSliderCallback>::New();
  callback->ContourFilter = contourFilter;
  callback->ContourFilter->GenerateValues(1, label_range[1]/2, label_range[1]/2);
  sliderWidget->AddObserver(vtkCommand::InteractionEvent,callback);

  SP(vtkRenderWindowInteractor) renderWindowInteractor = SP(vtkRenderWindowInteractor)::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderWindow->SetSize(500,500);
  renderWindow->Render();
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
