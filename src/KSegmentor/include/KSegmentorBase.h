#ifndef __KSegmentorBase_h
#define __KSegmentorBase_h


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

//Should these methods be in here??

/** write string on top of image data in-place*/
void waterMark(const std::string& text, cv::Mat & img);

/** compute volume (non-zero elements) in label map, insert to string "xyz mL^3
    optionally, pass "left" or "right" to do only lateral or medial side. */
void getVolumeAsString( const std::vector<double>& imageSpacing,
                                 vtkImageData* label_map,
                                 std::string & volume, bool numberOnly = false,
                                 const std::string& right_left_both = std::string("both"), double center_line = -1.0);

void getXYZExtentsAsString( const std::vector<double>& imageSpacing,
                                 vtkImageData* label_map,
                                 std::string & volumeString, std::vector<double>& minXYZ,
                                 std::vector<double>& maxXYZ, bool numberOnly = false );

/** in-place, copy a slice from index a to index b in volume */
void copySliceFromTo( vtkImageData* label_map, int idxFrom, int idxTo );

/** remove 3D islands: erode slightly, dilate a lot, AND this with original */
vtkSmartPointer<vtkImageData>  removeImageOstrava( vtkImageData* img_dirty,
                                               int erode_sz=3, int dilate_sz=5);
class KSegmentorBase
{

    public:
        virtual ~KSegmentorBase()=0;
        void setNumIterations(int itersToRun);
        void setCurrLabelArray(vtkImageData *label);
        void intializeLevelSet();
        void TransformUserInputImages(vtkTransform* transform, bool invert=false );
        void initializeUserInputImageWithContour(bool accumulate=true);
        virtual void Update()=0;

        /** external interface to update at a voxel */

        virtual void accumulateUserInputInUserInputImages( double value,const unsigned int element);

        void AddPointToUpdateVector(unsigned int element){
            m_UpdateVector.push_back(element);
        }

        void AddPointToCoordinatesVector(std::vector<unsigned int> coord){
            m_CoordinatesVector.push_back(coord);
        }

        virtual void copyIntegralDuringPaste( int kFrom, int kTo );

        void setRadius( int radNew ) {
          rad = radNew;
        }

        void setCurrIndex(int sliceIndex){
            this->currSlice=sliceIndex;
        }

        void saveCurrentSliceToPNG( const std::string& fileName);

        static double defaultKappaParam;

        void UpdateImageSpacing(double* spacing)
        {
            for (int i=0;i<3;i++)
                m_Spacing_mm[i]=spacing[i];
        }

        void GetImageSpacing(double* spacing)
        {
            for (int i=0;i<3;i++)
                spacing[i]=m_Spacing_mm[i];
        }

       void SetUseEdgeBasedEnergy( bool useEdgeBased )
        {
            m_bUseEdgeBased = useEdgeBased;
        }

        bool GetUseEdgeBasedEnergy( )
        {
            return m_bUseEdgeBased;
        }

        void SetSaturationRange( double dmin, double dmax ) {
            if( dmax > dmin ) {
                m_SatRange[0] = dmin;
                m_SatRange[1] = dmax;
            } else {
                std::cout << "bogus min/max attempted set! " << std::endl;
            }
        }

        void SetPlaneCenter(double* center)
        {
            this->m_PlaneCenter=center;
        }

        void SetPlaneNormalVector(double* normal)
        {
            this->m_PlaneNormalVector=normal;
        }

        void SetDistanceWeight(float dw)
        {
            this->m_DistWeight=dw;
        }


    protected:
        vtkSmartPointer<vtkImageData> U_l_slice_image, U_t_image,U_Integral_image;

        vtkSmartPointer<vtkImageReslice> m_Reslicer;

        std::vector<unsigned int > m_UpdateVector;
        std::vector< std::vector<unsigned int> > m_CoordinatesVector;

        void InitializeVariables(KSegmentorBase* segPointer,vtkImageData *image, vtkImageData *label, bool contInit);
        void InitializeMaskAndImage();
        void UpdateMask();

        virtual void initializeData()=0;

        virtual void integrateUserInputInUserInputImage()=0;

        virtual void UpdateArraysAfterTransform()=0;


        /** write to png file. rescale to 255, make sure it has .png ending */
        void saveMatToPNG( const cv::Mat& source, const std::string& fileName );

        /** Does NOT own this memory */
        vtkImageData *imageVol; //full image volume we are working with
        vtkImageData *labelVol; //full label volume (at the current time)
        double* m_PlaneNormalVector, *m_PlaneCenter;
        float m_DistWeight;

public:
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

         double* m_CustomSpeedImgPointer;

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
        LL *Lin2out, *Lout2in,*Lchanged;
};






}



#endif
