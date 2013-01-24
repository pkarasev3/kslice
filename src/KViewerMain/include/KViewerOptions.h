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

    int brushRad; //radius of the local ball
    int numIters; //number of iterations the contour will evolve for
    int sliceNum; //index of the current slice in the volume
    bool initCorrectFlag; //if Kslice has been initialized =1, else starts out being 0
    bool m_bUseEdgeBased; //if =1, use edge based energy, else region based (default =0)
    float distWeight; // slow down speed away from view/edit plane
};





#endif

