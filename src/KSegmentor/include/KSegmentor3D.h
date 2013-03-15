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
        virtual void initializeData();
        virtual void Update3D();

        /** run 2D seg */
        virtual void Update2D();

        /** external interface to update at a voxel */
        virtual void accumulateCurrentUserInput( double value,const unsigned int element, double weight=1.0);

        virtual void OnUserPaintsLabel();

    private:

        KSegmentor3D(){}
        typedef KSegmentor3D Self;

        /** internal 'update from input' function */
        virtual void integrateUserInput();

        virtual void UpdateArraysAfterTransform();

};






}



#endif
