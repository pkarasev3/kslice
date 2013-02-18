// A class to make abstract an energy for the purposes of segmentation
// A number of energies can be derived from this class

// an energy can certaintly be evaluated, and can return a dE/dC speed evaluation
// evaluation can be defined either in terms of the image alone, or with a contour
// where the contour for evalutation purposes is given as a three binary masks,
// interior exterior and boundary.
// by making masks like this we can immediately allow for 'thin' regions around contours rather than
// evaluating entire exteriors / interiors

#ifndef IMAGEENERGY
#define IMAGEENERGY

#include <opencv2/core/core.hpp>
#include <vector>
#include <valarray>

using namespace cv;
using std::vector;
using std::valarray;

class ImageEnergy {
public:
	ImageEnergy( ) {}
	ImageEnergy( IplImage* img, const vector<int>& params );
	virtual ~ImageEnergy();
	virtual double Evaluate( CvMat* interiorMask, CvMat* exteriorMask, 
                           const vector<int>& C ) = 0;
	virtual double Evaluate( const vector<int>& C ) = 0;
	virtual double Evaluate( ) = 0;
	virtual double Evaluate( const vector<int>& C, CvMat* phi = NULL ) = 0;
	virtual double Evaluate( CvMat* phi ) = 0;
	virtual valarray<double> GetSpeed( const vector<int>& C, CvMat* phi = NULL  ) = 0;
	virtual valarray<double> GetSpeed( const vector<int>& C, CvMat* interiorMask, 
                                     CvMat* exteriorMask, CvMat* phi = NULL) = 0;


protected:
	virtual void Initialize( IplImage* img, const vector<int>& params );
	virtual valarray<double> GetKappa(CvMat* phi, const vector<int>& C);
	IplImage* myImg;
	CvMat* floatImg;

};


#endif
