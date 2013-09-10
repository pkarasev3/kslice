#ifndef __KSegmentorBase_h
#define __KSegmentorBase_h


#include "vtkSmartPointer.h"
#include "vtkImageReslice.h"
#include "KViewerOptions.h"
#include <iostream>

//#include <opencv2/core/core.hpp>
//#include <boost/shared_ptr.hpp>

class vtkImageData;

class vtkTransform;
struct LL;


namespace vrcl
{

/** write string on top of image data in-place*/
//void waterMark(const std::string& text, cv::Mat & img);

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
void copySliceFromTo( vtkImageData* label_map, int idxFrom, int idxTo, const std::string& orient );

/** remove 3D islands: erode slightly, dilate a lot, AND this with original */
vtkSmartPointer<vtkImageData>  removeImageOstrava( vtkImageData* img_dirty,
                                               int erode_sz=3, int dilate_sz=5);

class KSegmentorBase
{
    public:
        struct LLset
        {
            LL *Lz, *Ln1, *Ln2, *Lp1, *Lp2;
            LL *Lin2out, *Lout2in,*Lchanged;
            void init()
            {
                Lz=NULL;
                Ln1= NULL;
                Ln2= NULL;
                Lp1=NULL ;
                Lp2=NULL ;
                Lchanged=NULL;
                Lin2out=NULL ;
                Lout2in=NULL ;
            }
        };
        struct SFM_vars;

        static std::vector<std::string> GetSupportedEnergyNames()
        {
            std::vector<std::string> e_names;
            e_names.push_back("ChanVese");
            e_names.push_back("LocalCV");
            e_names.push_back("LocalCVLimited");
            return e_names;
        }
    public:
        virtual ~KSegmentorBase()=0;
        void SetSliceOrientationIJK(const std::string& ijk_str);
        void SetCurrentSlice(int currSlice);
        void setNumIterations(int itersToRun);
        void setCurrLabelArray(vtkImageData *label);
        //void intializeLevelSet3D();
        //void TransformUserInputImages(vtkTransform* transform, bool invert=false );
        void initializeUserInputImageWithContour(bool accumulate=true);
        virtual void Update2D(bool reInitFromMask)=0;
        virtual void Update3D(bool reInitFromMask)=0;
        double GetUmax() { if( m_Umax <= 0 ) { std::cout << "Warning, m_Umax seems wrong!\n"; } return m_Umax; }

        /** Compute Chan-Vese (mean difference) Energy LL2D.*/
        virtual double evalChanVeseCost( double& mu_i, double& mu_o  ) const;

        /** external interface to update at a voxel */
        //virtual void accumulateUserInput(double value,const unsigned int element);

        /** for 3D updates that are less frequent */
        virtual void accumulateUserInput(double value, int i, int j, int k);

        void AddPointToUpdateVector(unsigned int element){
            m_UpdateVector.push_back(element);
        }

        void AddPointToCoordinatesVector(const std::vector<unsigned int>& coord){
            m_CoordinatesVector.push_back(coord);
        }

        void PrintUpdateInfo() const {
          std::cout << "m_UpdateVector.size()      = " << m_UpdateVector.size() << std::endl;
          std::cout << "m_CoordinatesVector.size() = " << m_CoordinatesVector.size() << std::endl;
        }

        virtual void copyIntegralDuringPaste( int kFrom, int kTo );

        //void setRadius( int *radNew ) {
        //  rad = radNew;
        //}

        void setCurrIndex(int sliceIndex){
            this->currSlice=sliceIndex;
        }

        void saveCurrentSliceToPNG( double* data,  const std::string& fileName);

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
        int GetLzLength() const;

        void SetSaturationRange( double dmin, double dmax ) {
            if( dmax > dmin ) {
                m_SatRange[0] = dmin;
                m_SatRange[1] = dmax;
            } else {
                std::cout << "bogus min/max attempted set! " << std::endl;
            }
        }

        void SetPlaneCenter(const std::vector<double>& center)
        {
            m_PlaneCenter=center;
        }

        void SetPlaneNormalVector(const std::vector<double>& normal)
        {
            this->m_PlaneNormalVector=normal;
        }

        void SetDistanceWeight(float dw)
        {
            this->m_DistWeight=dw;
        }

        void SetCurvaturePenalty(double lambdaPenalty)
        {
            this->lambda=lambdaPenalty;
        }

        void SetThreshWeight(float dw)
        {
            this->m_ThreshWeight=dw;
        }

        void SetEnergyChanVese( )
        {
            this->m_EnergyName = GetSupportedEnergyNames()[0];
        }
        void SetEnergyLocalCV( )
        {
            this->m_EnergyName = GetSupportedEnergyNames()[1];
        }
        void SetEnergyLocalCVLimited()
        {
            this->m_EnergyName = GetSupportedEnergyNames()[2];
        }

        vtkImageData* GetUIVol() { return U_Integral_image; }

        vtkImageData* U_Integral_image;

        virtual void OnUserPaintsLabel()=0;
    protected:
        vtkSmartPointer<vtkImageData> U_l_slice_image, U_t_image;

        /** two arrays for storing the location of user edits; Vanya: it !!DRAMATICALLY!!
                                                   improves speed and thus pracical usability*/
        std::vector<unsigned int > m_UpdateVector;
        std::vector< std::vector<unsigned int> > m_CoordinatesVector;


        void InitializeVariables(vtkImageData* image, vtkImageData* label, vtkImageData* UIVol,
                                 bool contInit, int currSlice, int numIts, float distWeight, double lambdaPenalty,
                                 int brushRad, int currLabel, double *imgSpacing);

        void InitializeMaskAndImage();
        //void UpdateMask(bool bForceUpdateAll = false);

        virtual void initializeData()=0;

        //virtual void integrateUserInput()=0;

        //virtual void UpdateArraysAfterTransform()=0;

        void CreateLLs(LLset& ll);

        std::vector<float> cache_phi;

        /** write to png file. rescale to 255, make sure it has .png ending */
        void saveMatToPNG( double* data, const std::string& fileName );

        /** Does NOT own this memory */
        vtkImageData *imageVol; //full image volume we are working with
        vtkImageData *labelVol; //full label volume (at the current time)
        std::vector<double> m_PlaneNormalVector, m_PlaneCenter;
        float m_DistWeight,m_ThreshWeight;

        std::string m_EnergyName;
        std::string m_IJK_orient; // IJ, JK, IK
        std::string prevMode;
        std::string last2DOrient;
        double m_Umax;

        /** struct containing formerly global low-level crap in sfm_local library */
        //boost::shared_ptr<SFM_vars>  m_SFM_vars;
        SFM_vars* m_SFM_vars;

public:
        /** Might (??) own anything else below */
        int currSlice;       //the slice we are segmenting
        int prevSlice;
        long numberdims;     //for images =2, for volumes =3
        int *mdims;          //dimensions of "image" we are segmenting (ex.512x512x212)
        //double *imgRange;    //[minImageVal, maxImageVal]
        //double* labelRange;  //[minLabVal, maxLabVal]

        //all the 2D slices
        double* imgSlice;
        short* maskSlice; //changing types
        float* phiSlice;
        short* U_I_slice;
        short* labelSlice;


        //3D variables
        double *img;
        short *mask;
        float *phi;
        short *ptrIntegral_Image;
        short* label;

        double* m_CustomSpeedImgPointer;
        double penaltyAlpha; //regularizer for "user constraints" experiments
        double *seed;        //again, only used in functions for "user constraints" experiments
        bool useContInit;    //for "user constraints" do we intitialize from seed or initial contour
        int iter;            //number of iterations to execute
        double lambda;       //curvature penalty
        int rad[3];          //radius of ball used in local-global energies
        int currLabel;       //the label value to mark inside contour
        int display;         //is the debug display on/off if ~=0, will display every X iterations

        double m_Spacing_mm[3];
        double m_SatRange[2];
        bool   m_bUseEdgeBased; // do we use edge-based energy?

        double *B, *C;
        double *F;
        double usum, vsum;
        int    countdown;
        long    dims[5];
        long dimz,dimy,dimx;
        LL *Sz, *Sn1, *Sn2, *Sp1, *Sp2;


        LLset LL2D,LL3D;


};






}

#endif


#if  0
vtkSmartPointer<vtkImageReslice> m_Reslicer;
m_Reslicer = vtkSmartPointer<vtkImageReslice>::New();
#endif
