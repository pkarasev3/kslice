#ifndef __KSegmentor_h
#define __KSegmentor_h


#include "vtkSmartPointer.h"
#include <opencv2/core/core.hpp>

class vtkImageData;
struct LL;
// Don't include shit we can forward-declare!


namespace vrcl
{

/** write string on top of image data in-place*/
void waterMark(const std::string& text, cv::Mat & img);

/** compute volume (non-zero elements) in label map, insert to string "xyz mL^3*/
void getVolumeAsString( const std::vector<double>& imageSpacing,
                                 vtkImageData* label_map,
                                 std::string & volume );

/** in-place, copy a slice from index a to index b in volume */
void copySliceFromTo( vtkImageData* label_map, int idxFrom, int idxTo );

class KSegmentor
{
    private:
      KSegmentor(); // prevent invalid initialization
    public:
        KSegmentor( vtkImageData* image, vtkImageData* label, int sliceIndex );
        virtual ~KSegmentor();
        void setNumIterations(int itersToRun);
        void setCurrIndex(int sliceIndex);
        void initializeData();
        void setCurrLabelArray(vtkImageData *label);
        void intializeLevelSet();
        void Update();

        /** external interface to update at a voxel */
        void accumulateUserInput( double value, int i, int j, int k );
        void copyIntegralDuringPaste( int kFrom, int kTo );
        void setRadius( int radNew ) {
          rad = radNew;
        }
        void saveCurrentSliceToPNG( const std::string& fileName);

    private:
        /** internal 'update from input' function */
        void integrateUserInput( int k );

        /** write to png file. rescale to 255, make sure it has .png ending */
        void saveMatToPNG( const cv::Mat& source, const std::string& fileName );

/** Does NOT own this memory */
        vtkImageData *imageVol; //full image volume we are working with
        vtkImageData *labelVol; //full label volume (at the current time)
        unsigned short *ptrCurrImage; //ptr to current image slice
        unsigned short *ptrCurrLabel; //ptr to current label slice


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
