#ifndef __KSegmentor3D_h
#define __KSegmentor3D_h


#include "vtkSmartPointer.h"
#include "vtkImageReslice.h"

#include "KSegmentorBase.h"
#include "KViewerOptions.h"
#include "KSandbox.h"
#include "energy3c.h"

//For testing
#include"vtkMetaImageWriter.h"


class vtkImageData;

class vtkTransform;
struct LL;

class KSegmentor3D: public vrcl::KSegmentorBase{
    public:

        KSegmentor3D(vtkImageData* image, vtkImageData* label, vtkImageData* UIVol,
                     bool contInit, int currSlice, int numIts, float distWeight, int brushRad, int currLabel, double *imgSpacing);

        virtual ~KSegmentor3D();
        void initializeData();
        /** compute view plane's normal and center */
        void CalcViewPlaneParams( );
        virtual void Update3D(bool reInitFromMask);
        virtual void Update2D(bool reInitFromMask);

        /** external interface to update at a voxel */
        void accumulateCurrentUserInput( double value,
                                         const unsigned int element,
                                         double weight=1.0 );
        virtual void OnUserPaintsLabel();

    private:
        /** internal 'update from input' function */
        void integrateUserInput();
        bool firstPassInit; // only load image to double *once*
        energy3c *segEngine;
        //void UpdateArraysAfterTransform();
};

#endif



