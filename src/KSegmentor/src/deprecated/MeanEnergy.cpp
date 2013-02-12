#include "MeanEnergy.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/legacy/compat.hpp>
#include <iostream>

using namespace cv;
using std::vector;
using std::valarray;
using std::cout;
using std::endl;

MeanEnergy::MeanEnergy(IplImage* img, const vector<int>& params)
{
	Initialize(img, params);
        lambda = 0.4; // kappa vs. data term tradeoff
	mask = cvCreateMat( img->height, img->width, CV_8U );
	maskn = cvCloneMat(mask);
}

MeanEnergy::~MeanEnergy()
{
	// to do: verify the parent constructor gets called correctly
	cvReleaseMat( &mask );
	cvReleaseMat( &maskn );

}

double MeanEnergy::Evaluate( CvMat* phi )
{

	int w = phi->width;
	int h = phi->height;
	//CvMat* mask = cvCreateMat( h, w, CV_8U );
	for( int i = 0; i < w*h; i++ ) {
		if( (phi->data.fl[i] > 0) ) {
			mask->data.ptr[i] =  0;
			Aout += 1.0;
		}
		else {
			mask->data.ptr[i] =  1;
			Ain += 1.0;
		}
	}
	
	cvNot( mask, maskn );

	mean_in = cvMean( myImg, mask );
	mean_out = cvMean( myImg, maskn );

	double t1 = 0.0;
	double t2 = 0.0;
	for( int i = 0; i < w*h; i++ ) {
		if( phi->data.fl[i] < 0.0 )
			t1 += pow(this->floatImg->data.fl[i] - mean_in,2.0);
		else
			t2 += pow(this->floatImg->data.fl[i] - mean_out,2.0);
	}

	return t1+t2;
	
	
}

valarray<double> MeanEnergy::GetSpeed(const vector<int>& C, CvMat* phi)
{
	if( !phi ) { // cannot have a null phi for this energy
		cout<<"Error, cannot use a null levelset function with EdgeContrastEnergy, exiting \n";
		exit(1);
	}

	int w = phi->width;
	int h = phi->height;
	
	for( int i = 0; i < w*h; i++ ) {
		if( (phi->data.fl[i] > 0) ) {
			mask->data.ptr[i] =  0;
			Aout += 1.0;
		}
		else {
			mask->data.ptr[i] =  1;
			Ain += 1.0;
		}
	}

	
	cvNot( mask, maskn );

	mean_in = cvMean( myImg, mask );
	mean_out = cvMean( myImg, maskn );
	
	valarray<double> F( C.size() );	
	for( int i = 0; i < C.size(); i++ ) {
		int idx = C[i];
		CvScalar s = cvGet2D( myImg, idx / w, idx % w );
		F[i] = pow( double(s.val[0] - mean_in),2.0) - pow( double(s.val[0] - mean_out),2.0);
	}
	valarray<double> kappa = GetKappa(phi, C);
	F = F / (abs(F)).max() * (1 - lambda) + lambda * kappa / (abs(kappa)).max();

	
	return F;
}
