#ifndef __KSegmentor_h
#define __KSegmentor_h


#include "vtkSmartPointer.h"
#include "vtkImageReslice.h"

//For testing
#include"vtkMetaImageWriter.h"
#include <opencv2/core/core.hpp>


class vtkImageData;

class vtkTransform;
struct LL;


namespace vrcl
{

class KSegmentor
{
protected:
          KSegmentor(bool){} //dummy constructor forderived classes

private:
      KSegmentor(); // prevent invalid initialization


      vtkSmartPointer<vtkImageData> U_l_slice_image, U_t_image,U_Integral_image;

      vtkSmartPointer<vtkImageReslice> m_Reslicer;

    public:
        KSegmentor( vtkImageData* image, vtkImageData* label, int sliceIndex,bool contInit=false );
        virtual ~KSegmentor();
        virtual void setNumIterations(int itersToRun);
        virtual void setCurrIndex(int sliceIndex);
        virtual void initializeData();
        virtual void setCurrLabelArray(vtkImageData *label);
        virtual void intializeLevelSet();
        virtual void TransformUserInputImages(vtkTransform* transform, bool invert=false );
        virtual void initializeUserInputImageWithContour();
        virtual void Update();

        /** external interface to update at a voxel */
        void accumulateUserInput( double value, int i, int j, int k );
        virtual void accumulateUserInputInUserInputImages( double value,const unsigned int element);
        virtual void copyIntegralDuringPaste( int kFrom, int kTo );
        virtual void setRadius( int radNew ) {
          rad = radNew;
        }
        virtual void saveCurrentSliceToPNG( const std::string& fileName);

        static double defaultKappaParam;

        virtual void UpdateImageSpacing(double* spacing)
        {
            for (int i=0;i<3;i++)
                m_Spacing_mm[i]=spacing[i];
        }

        virtual void GetImageSpacing(double* spacing)
        {
            for (int i=0;i<3;i++)
                spacing[i]=m_Spacing_mm[i];
        }

        virtual void SetUseEdgeBasedEnergy( bool useEdgeBased )
        {
            m_bUseEdgeBased = useEdgeBased;
        }

        virtual bool GetUseEdgeBasedEnergy( )
        {
            return m_bUseEdgeBased;
        }

        virtual void SetSaturationRange( double dmin, double dmax ) {
            if( dmax > dmin ) {
                m_SatRange[0] = dmin;
                m_SatRange[1] = dmax;
            } else {
                std::cout << "bogus min/max attempted set! " << std::endl;
            }
        }

    private:
        /** internal 'update from input' function */
        void integrateUserInput( int k );

        virtual void integrateUserInputInUserInputImage( int k );

        virtual void UpdateArraysAfterTransform();

        /** write to png file. rescale to 255, make sure it has .png ending */
        virtual void saveMatToPNG( const cv::Mat& source, const std::string& fileName );

/** Does NOT own this memory */
        vtkImageData *imageVol; //full image volume we are working with
        vtkImageData *labelVol; //full label volume (at the current time)
        unsigned short *ptrCurrImage; //ptr to current image slice
        unsigned short *ptrCurrLabel; //ptr to current label slice
        double *ptrIntegral_Image; //ptr to current image slice
        double *ptrU_t_Image; //ptr to current label slice


/** Might (??) own anything else below */
        int currSlice;       //the slice we are segmenting
        int prevSlice;
        long numberdims;     //for images =2, for volumes =3
        int *mdims;          //dimensions of "image" we are segmenting (ex.512x512x212)
        double *imgRange;    //[minImageVal, maxImageVal]
        double* labelRange;  //[minLabVal, maxLabVal]

        double *img;         // single slice!
        double *mask;        // single slice!
        double *U_I_slice;
        double *U_t_slice;

        double penaltyAlpha; //regularizer for "user constraints" experiments
        double *seed;        //again, only used in functions for "user constraints" experiments
        bool useContInit;    //for "user constraints" do we intitialize from seed or initial contour
        int iter;            //number of iterations to execute
        double lambda;       //curvature penalty
        double rad;          //radius of ball used in local-global energies
        double dthresh;
        int display;         //is the debug display on/off if ~=0, will display every X iterations

        double m_Spacing_mm[3];
        double m_SatRange[2];
        bool   m_bUseEdgeBased; // do we use edge-based energy?

        unsigned short *seg; //seg result from last run of the segmentor
        short *iList;        //row indices of points on zero level set from last run
        short *jList;        //column indices of points on zero level set from last run
        long lengthZLS;      //number of point on the zero level set from last run


        //Level Set Variables Stay persistent

        /** time-integrated user inputs */
        std::vector< cv::Mat >  U_integral;

        /** instantaneous user input (stuff that was drawn between running 's') */
        std::vector< cv::Mat >  U_t;

        double *B, *phi, *C, *label;
        double *F;
        double usum, vsum;
        int    countdown;
        long    dims[5];
        long dimz,dimy,dimx;
        LL *Lz, *Ln1, *Ln2, *Lp1, *Lp2;
        LL *Sz, *Sn1, *Sn2, *Sp1, *Sp2;
        LL *Lin2out, *Lout2in;



};






}



#endif
