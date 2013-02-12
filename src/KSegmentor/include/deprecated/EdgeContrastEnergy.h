#ifndef EDGECONTRASTENERGY
#define EDGECONTRASTENERGY

#include "ImageEnergy.h"

class EdgeContrastEnergy : public ImageEnergy
{
public:
	EdgeContrastEnergy( IplImage* img, const vector<int>& params );
	~EdgeContrastEnergy();
	double Evaluate( const vector<int>& C );
	double Evaluate( CvMat* interiorMask, CvMat* exteriorMask, const vector<int>& C ) {return 0.0;} //invalid for this energy
	double Evaluate( ) { return 0.0; } // invalid for this energy
	valarray<double> GetSpeed( const vector<int>& C, CvMat* interiorMask, CvMat* exteriorMask, CvMat* phi = NULL) { return valarray<double>(0); } // invalid for this energy
	valarray<double> GetSpeed( const vector<int>& C, CvMat* phi = NULL  );
	
private:
	double lambda; //param between 0 and 1, weighting the image data vs. curve smoothness
	CvMat* F_x;
	CvMat* F_y;
	CvMat* NormGradSq;
	CvMat* NormGradSq_smooth;
	int iFilterSize;
	double dFilterSigma;
	bool bUseSmoothInEval;

};

#endif