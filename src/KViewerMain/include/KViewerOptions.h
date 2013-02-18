#ifndef KVIEWEROPTIONS_H_
#define KVIEWEROPTIONS_H_

#include <string>
#include <vector>
#include "vtkSmartPointer.h"
#include "vtkMetaImageReader.h"
#include "vtkTransform.h"

struct KViewerOptions
{

public:
static double getDefaultDrawLabelMaxVal()
{
      return 1000.0;
}

    int BrushRad; //radius of the local ball
    int NumIts; //number of iterations the contour will evolve for
    int CurrSlice; //index of the current slice in the volume
    float DistWeight; //slow down speed away from view/edit plane
    int FromSlice; //put this slice into storage, to paste later
    int ToSlice; //paste the cached slice to this one
    bool initCorrectFlag; //if Kslice has been initialized =1, else starts out being 0
    bool m_bUseEdgeBased; //if =1, use edge based energy, else region based (default =0)
    bool contInit; //flag for initializing the

};





#endif

