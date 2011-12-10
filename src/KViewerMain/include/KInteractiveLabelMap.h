#ifndef KINTERACTIVELABELMAP_H
#define KINTERACTIVELABELMAP_H

#include "vtkSmartPointer.h"
#include "vtkImageActor.h"
#include "vtkImageData.h"
#include "vtkImageReslice.h"
#include "vtkImageShiftScale.h"
#include "vtkImageMapToColors.h"
#include "vtkLookupTable.h"
#include "KSegmentorBase.h"
#include "KSegmentor3D.h"

struct KViewerOptions;
class  KWidget_2D_left;
using  vrcl::KSegmentorBase;

class KInteractiveLabelMap
{
public:
    KInteractiveLabelMap();

    /** Smartpointers: stuff that I own */
    vtkSmartPointer<vtkImageActor>        labelActor2D;
    vtkSmartPointer<vtkImageReslice>      labelReslicer;
    vtkSmartPointer<vtkLookupTable>       labelLUT;
    vtkSmartPointer<vtkImageMapToColors>  colorMap;
    vtkSmartPointer<vtkImageData>         labelDataArray;
    vtkSmartPointer<vtkImageShiftScale>   label2D_shifter_scaler;
    cv::Ptr<KSegmentorBase>                   ksegmentor;


    /** raw pointers: stuff someone else owns */
    vtkImageData*                         imageVolume;
    KViewerOptions*                       kv_opts;
    
    /** W x H x nZ */
    std::vector<int>    imageDimensions; 
    
    std::vector<double> imageSpacing; 
    
    /** how opaque or transperent to draw myself */
    float labelOpacity2D;
    
    /** true or false interpolate myself while drawing (speed vs visual niceness) */
    bool  labelInterpolate;

    
    
    /** ghetto but allows flexibility: reverse handle on the widget */
    void RegisterSourceWidget( KWidget_2D_left* kwidget2D, bool regNewImage );


private:
    //no copying!
    KInteractiveLabelMap(const KInteractiveLabelMap&)   {  }
    void operator=(const KInteractiveLabelMap&)         {  }
    
    KWidget_2D_left*    sourceWidget;  // reverse handle on my source

    /** get a handle on the state-data in kv_opts */
    void RegisterState( KViewerOptions* kv_opts_in) {
      kv_opts = kv_opts_in; 
    }
//Changed visibility for testing purposes
public:
    /** get a handle on a new image and create a blank labelmap of 
      * the appropriate dimensions, type, spacing, and so on. */
    void RegisterNewImage( vtkImageData* image);

    void SetupLabelView(int index =0);
    void UpdateResliceTransform();
};

#endif // KINTERACTIVELABELMAP_H
