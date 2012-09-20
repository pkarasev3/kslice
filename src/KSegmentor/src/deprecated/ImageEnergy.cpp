#include "ImageEnergy.h"

#include <opencv2/core/core.hpp>
#include <opencv2/legacy/compat.hpp>
#include <vector>
#include <valarray>

using namespace cv;
using std::vector;
using std::valarray;

// GRAYSCALE IMAGE ASSUMED!
ImageEnergy::ImageEnergy(IplImage *img, const vector<int> &params)
{
	Initialize( img, params );
}

void ImageEnergy::Initialize( IplImage* img, const vector<int>& params )
{
	this->myImg = cvCloneImage( img ); // assign the image (unsigned 8-bit single channel)
	
	// create the floating-point version for numerics (32-bt floating point single channel)
	this->floatImg = cvCreateMat( img->height, img->width, CV_32FC1 );
	int i,j; 
	for( i = 0; i < img->height; i++ ) {
		for( j = 0; j < img->width; j++ ) {
			CvScalar s = cvGet2D( img, i, j );
			double ds = s.val[0];
			this->floatImg->data.fl[ i*img->width + j ] = ds * 1.0 / 255.0;
		}
	}

}

ImageEnergy::~ImageEnergy() 
{
	cvReleaseImage( &(this->myImg) );
	cvReleaseMat( &(this->floatImg) );
}

valarray<double> ImageEnergy::GetKappa(CvMat* phi, const vector<int>& C)
{
	// K = (dx2.*dyy + dy2.*dxx - 2*dx.*dy.*dxy)./(dx2 + dy2 + eps).^(3/2);
	valarray<double> kappa(C.size());
	int w = phi->width;
	int h = phi->height;
	for( int ii = 0; ii < C.size(); ii++ ) {
		int i = C[ii];
		int row = i / w;
		int col = i % w;
		if( row - 1 < 0 || row + 1 >= h || col - 1 < 0 || col+1 >= w )
			continue;
		double left = phi->data.fl[i-1];
		double right = phi->data.fl[i+1];
		double up = phi->data.fl[i-w];
		double down = phi->data.fl[i+w];
		double here = phi->data.fl[i];
		double eps = 1e-6;
		double upleft = phi->data.fl[i-1-w];
		double upright = phi->data.fl[i+1-w];
		double downleft = phi->data.fl[i+w-1];
		double downright = phi->data.fl[i+1+w];
		
		double dx = (right-left)/2;
		double dy = (up - down)/2;
		double dxx = left + right - 2*here;
		double dyy = up + down - 2*here;
		double dxy = (upleft + downright - upright - downleft)/4;

		kappa[ii] = (dx*dx*dyy + dy*dy*dxx-2*dx*dy*dxy) / pow( ( dx*dx+dy*dy+eps ), 1.5 );
	}

	return kappa;
}
