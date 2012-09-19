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
    public:
        static KSegmentor3D* CreateSegmentor(vtkImageData *image, vtkImageData *label, bool contInit);
        virtual ~KSegmentor3D();
        void initializeData();
        virtual void Update3D();
        virtual void Update2D();

        /** external interface to update at a voxel */
        void accumulateUserInputInUserInputImages( double value,const unsigned int element);

    private:

        KSegmentor3D(){}
        typedef KSegmentor3D Self;

        /** internal 'update from input' function */

        void integrateUserInputInUserInputImage();

        void UpdateArraysAfterTransform();

};






}



#endif
