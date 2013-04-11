#ifndef __KSegmentor3D_h
#define __KSegmentor3D_h


#include "vtkSmartPointer.h"
#include "vtkImageReslice.h"

#include "KSegmentorBase.h"
#include "KViewerOptions.h"

//For testing
#include"vtkMetaImageWriter.h"
//#include <opencv2/core/core.hpp>


class vtkImageData;

class vtkTransform;
struct LL;


class KSegmentor3D: public vrcl::KSegmentorBase{
    public:
        //static KSegmentor3D* CreateSegmentor(vtkImageData *image, vtkImageData *label, vtkImageData* UIVol, bool contInit);
        KSegmentor3D(vtkImageData* image, vtkImageData* label, vtkImageData* UIVol,
                     bool contInit, int currSlice, int numIts, float distWeight, int brushRad, int currLabel);

        virtual ~KSegmentor3D();
        void initializeData();
        //virtual void Update3DUnknown(bool reInitFromMask); //this function should be deleted
        virtual void Update3D(bool reInitFromMask);
        virtual void Update2D(bool reInitFromMask);


        /** external interface to update at a voxel */
        void accumulateCurrentUserInput( double value,const unsigned int element, double weight=1.0 );
        virtual void OnUserPaintsLabel();

    private:
        /** internal 'update from input' function */
        void integrateUserInput();
        //void UpdateArraysAfterTransform();
};

#endif
