#ifndef MEANENERGY
#define MEANENERGY

#include "ImageEnergy.h"

class MeanEnergy : public ImageEnergy
{
public:
	MeanEnergy( IplImage* img, const vector<int>& params );
	~MeanEnergy();
	double Evaluate( const vector<int>& C ) { return 0.0; }
	double Evaluate( CvMat* interiorMask, CvMat* exteriorMask, const vector<int>& C ) {return 0.0;} //invalid for this energy
	double Evaluate( ) { return 0.0; } // invalid for this energy
	double Evaluate( const vector<int>& C, CvMat* phi = NULL ) { return 0.0; } // invalid
	double Evaluate( CvMat* phi );
	valarray<double> GetSpeed( const vector<int>& C, CvMat* interiorMask,
	    CvMat* exteriorMask, CvMat* phi = NULL) { return valarray<double>(0); } // invalid for this energy
	valarray<double> GetSpeed( const vector<int>& C, CvMat* phi = NULL  );
	
private:
	double lambda; //param between 0 and 1, weighting the image data vs. curve smoothness
	double mean_in;
	double mean_out;
	CvMat* mask;
	CvMat* maskn;
	

};


#endif
