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

#define SP( Y, X )  vtkSmartPointer<X> Y = vtkSmartPointer<X>::New()

static vtkSmartPointer<vtkImageData> CreateData(vtkImageData* label);

class vtkSliderCallback : public vtkCommand
{
public:
    static vtkSliderCallback *New()
    {
        return new vtkSliderCallback;
    }
    virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
        vtkSliderWidget *sliderWidget =
                reinterpret_cast<vtkSliderWidget*>(caller);
        double value = static_cast<vtkSliderRepresentation *>(sliderWidget->GetRepresentation())->GetValue();
        this->ContourFilter->GenerateValues(1, value, value);

    }
    vtkSliderCallback():ContourFilter(NULL) {}
    vtkContourFilter* ContourFilter;
};

int main(int argc, char *argv[])
{
    if( argc < 2 ) {
        cout << "usage: " << argv[0] << " labelName.mha" << endl;
    }
    std::string labelName = argv[1];
    SP(label,vtkImageData); // unsigned short
    SP(data,vtkImageData);  // double
    SP(reader,vtkMetaImageReader);
    reader->SetFileName(labelName.c_str());
    reader->Update();
    label = reader->GetOutput();
    data  = CreateData(label);

    // Create an isosurface
    vtkSmartPointer<vtkContourFilter> contourFilter =            vtkSmartPointer<vtkContourFilter>::New();
    contourFilter->SetInput(data);
    contourFilter->GenerateValues(3, 200, 1200); // (numContours, rangeStart, rangeEnd)

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
    sliderRep->SetMinimumValue(0.0);
    sliderRep->SetMaximumValue(1000.0);
    sliderRep->SetValue(500.0);
    sliderRep->SetTitleText("Contour value");
    sliderRep->SetPoint1InWorldCoordinates(-20, -20, 0);
    sliderRep->SetPoint2InWorldCoordinates(0, -20, 0);
    sliderRep->SetSliderWidth(.2);
    sliderRep->SetLabelHeight(.1);

    vtkSmartPointer<vtkSliderWidget> sliderWidget =
            vtkSmartPointer<vtkSliderWidget>::New();
    sliderWidget->SetInteractor(interactor);
    sliderWidget->SetRepresentation(sliderRep);
    sliderWidget->SetAnimationModeToAnimate();
    sliderWidget->EnabledOn();

    vtkSmartPointer<vtkSliderCallback> callback =
            vtkSmartPointer<vtkSliderCallback>::New();
    callback->ContourFilter = contourFilter;

    sliderWidget->AddObserver(vtkCommand::InteractionEvent,callback);

    vtkSmartPointer<vtkInteractorStyleUser> style =
            vtkSmartPointer<vtkInteractorStyleUser>::New();
    interactor->SetInteractorStyle(style);

    renderWindow->SetSize(800,800);
    renderWindow->Render();
    renderWindow->Render();
    interactor->Start();

    return EXIT_SUCCESS;
}

vtkSmartPointer<vtkImageData> CreateData(vtkImageData* label)
{
    SP(data,vtkImageData);  // double
    data->DeepCopy(label);
//    data->SetScalarTypeToDouble();
//    data->Update();

//    int* extent = data->GetExtent();

//    for (int z = extent[4]; z <= extent[5]; z++)
//    {
//        for (int y = extent[2]; y <= extent[3]; y++)
//        {
//            for (int x = extent[0]; x <= extent[1]; x++)
//            {
//                unsigned short* label_val = static_cast<unsigned short*>(label->GetScalarPointer(x,y,z));
//                double* data_val          = static_cast<double*>(data->GetScalarPointer(x,y,z));
//                data_val[0]               = (double) label_val[0];
//            }
//        }
//    }
    cout << "data range is: " << data->GetScalarRange()[0] << ", " << data->GetScalarRange()[1] << endl;
    return data;

}
