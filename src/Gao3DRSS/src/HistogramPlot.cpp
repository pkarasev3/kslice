#include "HistogramPlot.h"

#include <vtkSmartPointer.h>

#include <string>

#include <vtkActor.h>
#include <vtkImageAccumulate.h>
#include <vtkImageData.h>
#include <vtkImageExtractComponents.h>
#include <vtkJPEGReader.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkXYPlotActor.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include "vtkConfigure.h"
#include <vtkCellArray.h>

HistogramPlot::HistogramPlot(int numDataArray, int numBins){
    this->numDataSets=numDataArray; //this is how many data sets we expect to plot
    this->addingSetNumber=1;
    this->numBins=numBins;
}

void HistogramPlot::AddPlot(double *beginDataArray){
    if (this->addingSetNumber<=this->numDataSets){
        this->dataSetStart[this->addingSetNumber-1]=beginDataArray;
        this->addingSetNumber++;
    }else{
        std::cout<<"Added too many data sets to be plotted"<<std::endl;
    }
}
void HistogramPlot::Plot()
{

  // Create a vtkXYPlotActor
  vtkSmartPointer<vtkXYPlotActor> plot = vtkSmartPointer<vtkXYPlotActor>::New();
  plot->ExchangeAxesOff();
  plot->SetLabelFormat( "%g" );
  plot->SetXTitle( "Level" );
  plot->SetYTitle( "Frequency" );
  plot->SetXValuesToValue();

  double xmax = this->numBins;
  double ymax = .15;

  double colors[3][3] = {
    { 1, 0, 0 },
    { 0, 1, 0 },
    { 0, 0, 1 } };

  const char* labels[3] = {
    "Red", "Green", "Blue" };

  // Process the image, extracting and plotting a histogram for each
  // component
  double* beginDataSet;
  for( int i = 0; i < 1; ++i ) //this->numDataSets
    {


//    vtkSmartPointer<vtkImageAccumulate> histogram = vtkSmartPointer<vtkImageAccumulate>::New();
//    histogram->SetInputConnection( extract->GetOutputPort() );
//    histogram->SetComponentExtent( 0, static_cast<int>(range[1])-static_cast<int>(range[0])-1,0,0,0,0 );
//    histogram->SetComponentOrigin( range[0],0,0 );
//    histogram->SetComponentSpacing( 1,0,0 );
//    histogram->SetIgnoreZero( ignoreZero );
//    histogram->Update();

    beginDataSet=(this->dataSetStart)[i];

    vtkSmartPointer<vtkPoints> polyLinePoints= vtkSmartPointer<vtkPoints>::New();
    for(int j=0; j< this->numBins;j++){
        double currPoint[2];
        currPoint[0]=j;
        currPoint[1]=beginDataSet[j];
        polyLinePoints->InsertNextPoint(currPoint);
        std::cout<<"the x: " << currPoint[0]<< "and the y: "<<currPoint[1]<<std::endl;
    }

   double* bounds = polyLinePoints->GetBounds();
   double xRange[2]; xRange[0]=bounds[0]; xRange[1]=bounds[1];
   double yRange[2]; yRange[0]=bounds[2]; yRange[1]=bounds[3];
   std::cout<<"the x range is from: "<<xRange[0]<<"to: "<<xRange[1]<<std::endl;
   std::cout<<"the y range is from: "<<yRange[0]<<"to: "<<yRange[1]<<std::endl;

   // All the lines joining the plot points.
   vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
   for ( int k = 0; k < polyLinePoints->GetNumberOfPoints() ; ++k )
   {
      vtkIdType topol[2];
      topol[0] = k;
      topol[1] = k+1;
      lines->InsertNextCell( 2, topol );

      std::cout<<topol[0]<< " and " <<topol[1];
      std::cout<<"We have "<< polyLinePoints->GetNumberOfPoints()  <<" points and were are at point # "<<k<<std::endl;
   }

   // Create polydata from points & lines.
   vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
   //polydata->SetPoints(polyLinePoints);
   polydata->SetLines(lines);
   polydata->BuildCells();

   vtkIndent a=vtkIndent(0);
   polydata->PrintSelf(std::cout, a);

//    if( range[1] > xmax )
//      {
//      xmax = range[1];
//      }
//    if( histogram->GetOutput()->GetScalarRange()[1] > ymax )
//      {
//      ymax = histogram->GetOutput()->GetScalarRange()[1];
//      }

    //plot->AddInput( histogram->GetOutput() );
    plot->AddInput( polydata );

    if( this->numDataSets > 1 )
      {
      plot->SetPlotColor(i,colors[i]);
      plot->SetPlotLabel(i,labels[i]);
      plot->LegendOn();
      }
    }

  plot->SetXRange( 0, xmax );
  plot->SetYRange( 0, ymax );

  // Visualize the histogram(s)
  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();
  renderer->AddActor(plot);


  vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer( renderer );
  renderWindow->SetSize(640, 480);

  vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  interactor->SetRenderWindow( renderWindow );

  // Initialize the event loop and then start it
  interactor->Initialize();
  renderWindow->Render();
  interactor->Start();

  //return EXIT_SUCCESS;
}
