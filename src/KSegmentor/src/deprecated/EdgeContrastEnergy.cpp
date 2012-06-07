#include "EdgeContrastEnergy.h"

#include <opencv2/core/core.hpp>
#include <opencv2/legacy/compat.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <iostream>

using namespace cv;
using std::vector;
using std::valarray;
using std::cout;
using std::endl;

double EdgeContrastEnergy::Evaluate( const vector<int>& C )
{
	double energy = 0.0;
	for( size_t i = 0; i < C.size(); i++ ) {
		int idx = C[i];
		//if( bUseSmoothInEval )
		energy = energy + this->NormGradSq_smooth->data.fl[ idx ];
		/*else
			energy = energy + this->NormGradSq->data.fl[ idx ];*/
	}
	energy = energy / C.size();
	return energy;
}

EdgeContrastEnergy::EdgeContrastEnergy(IplImage* img, const vector<int>& params)
{
	Initialize(img, params);
	lambda = 0.0; // kappa vs. data term tradeoff
	this->iFilterSize = 15; // size of smoothing window for the ||gradient||

	CvMat* Ix = cvCreateMat( img->height, img->width, CV_32FC1 );
	CvMat* Iy = cvCreateMat( img->height, img->width, CV_32FC1 );
	this->NormGradSq = cvCreateMat(	img->height, img->width, CV_32FC1 );
	this->NormGradSq_smooth = cvCreateMat(	img->height, img->width, CV_32FC1 );
	
	cvSobel( floatImg, Ix, 1, 0, 1 );
	cvSobel( floatImg, Iy, 0, 1, 1 );
	CvMat* temp1 = cvCreateMat( img->height, img->width, CV_32FC1 );
	CvMat* temp2 = cvCreateMat( img->height, img->width, CV_32FC1 );
	CvMat* temp3 = cvCloneMat( Ix );
	CvMat* temp4 = cvCloneMat( Iy );
	cvMul( Ix, temp3, temp1 );
	cvMul( Iy, temp4, temp2 );
	cvAdd( temp1, temp2, temp3 );
	cvPow( temp3, this->NormGradSq, 0.5 );
	cvSmooth( NormGradSq, NormGradSq_smooth, CV_GAUSSIAN, this->iFilterSize, this->iFilterSize );
	F_x = cvCreateMat( img->height, img->width, CV_32FC1 );
	F_y = cvCreateMat( img->height, img->width, CV_32FC1 );
	cvSobel( temp1, F_x, 1, 0, 1 );
	cvSobel( temp1, F_y, 0, 1, 1 );
	cvReleaseMat( &Ix );
	cvReleaseMat( &Iy );
	cvReleaseMat( &temp1 );
	cvReleaseMat( &temp2 );
	cvReleaseMat( &temp3 );
	cvReleaseMat( &temp4 );

	/*DisplayImgRescale( F_x, "F_x", 0);
	int breakhere = 1;
	DisplayImgRescale( F_y, "F_x", 0);
	breakhere = 1;*/

}

EdgeContrastEnergy::~EdgeContrastEnergy()
{
	// to do: verify the parent constructor gets called correctly

	cvReleaseMat( &F_x );
	cvReleaseMat( &F_y );
	cvReleaseMat( &NormGradSq );
	cvReleaseMat( &NormGradSq_smooth );

}

valarray<double> EdgeContrastEnergy::GetSpeed(const vector<int>& C, CvMat* phi)
{
	if( !phi ) { // cannot have a null phi for this energy
		cout<<"Error, cannot use a null levelset function with EdgeContrastEnergy, exiting \n";
		exit(1);
	}
	
	valarray<double> F( C.size() );
	CvMat* phi_x = cvCreateMat( phi->rows, phi->cols, CV_32FC1 );
	CvMat* phi_y = cvCreateMat( phi->rows, phi->cols, CV_32FC1 );
	
	cvSobel( phi, phi_x, 1, 0, 1 );
	cvSobel( phi, phi_y, 0, 1, 1 );

	for( size_t i = 0; i < C.size(); i++ ) {
		int idx = C[i];
		double dPhidx = phi_x->data.fl[idx];
		double dPhidy = phi_y->data.fl[idx];
		double dFdx = F_x->data.fl[idx];
		double dFdy = F_y->data.fl[idx];
		//F[i] = dFdx + dFdy;
		F[i] = dPhidx * dFdx + dPhidy * dFdy;
		//F[i] = dPhidx * dPhidx + dPhidy * dPhidy;
		//F[i] = 1 + 0.1*(rand()%5);
		//F[i] = 1;
		//F[i] = i;
	}
	F = F / (abs(F)).max() * (1 - lambda); /* TODO: add a kappa term! */
	cvReleaseMat( &phi_x );
	cvReleaseMat( &phi_y );
	return F;
}
