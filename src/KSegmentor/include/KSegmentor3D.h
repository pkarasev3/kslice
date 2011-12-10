#ifndef __KSegmentor3D_h
#define __KSegmentor3D_h


#include "vtkSmartPointer.h"
#include "vtkImageReslice.h"

#include "KSegmentorBase.h"

//For testing
#include"vtkMetaImageWriter.h"
#include <opencv2/core/core.hpp>


class vtkImageData;

class vtkTransform;
struct LL;


namespace vrcl
{



class KSegmentor3D : public KSegmentorBase
{


    private:
    KSegmentor3D(){}
    typedef KSegmentor3D Self;

    public:
        static KSegmentor3D* CreateSegmentor(vtkImageData *image, vtkImageData *label, bool contInit);
        virtual ~KSegmentor3D();
        void initializeData();
        void intializeLevelSet();
        void Update();

        /** external interface to update at a voxel */
        void accumulateUserInputInUserInputImages( double value,const unsigned int element);

    private:
        /** internal 'update from input' function */
        void integrateUserInput( int k );

        void integrateUserInputInUserInputImage( int k );

        void UpdateArraysAfterTransform();
};






}



#endif
