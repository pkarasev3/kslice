#include <iostream>
#include <string>
#include <list>

#include <unistd.h>
#include "KSegmentor.h"
#include "LevelSetCurve.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>

using namespace cv;
using namespace std;

int main( int argc, char* argv[] ) {
  
  Mat img;
  if( argc < 2 ) {
    img = Mat::zeros( 240, 320, CV_32FC3 );
    randn(img, Scalar::all(0.5), Scalar::all(0.1));
  } else {
    img = imread( argv[1] );
  }
  Mat phi0 = Mat::zeros( img.size(), CV_32F );
  for( int i = 0; i < img.rows; i++ ) {
    for( int j = 0; j < img.cols; j++ ) {
      if( argc < 2 ) {
        float v = 0.5 * ( ( abs( i - img.rows/2 ) < 20 ) + ( abs( j - img.cols/2 ) < 20 ) );
        img.at<cv::Point3f>(i,j) = cv::Point3f( v, v, v );
      }
      phi0.at<float>(i,j)      = -pow(30.0,2.0) + pow( float(i)-img.rows/2, 2.0 ) 
                                 + pow( float(j)-img.cols/2, 2.0 );
    }
  }
  imshow("image", img  );
  waitKey(0);
    
  Ptr<LevelSetCurve> LS = Ptr<LevelSetCurve>(new LevelSetCurve( img, phi0, string("image") ));
  
  char c = 'c';
  IplImage output = IplImage(img);
  cout<<"running a few iterations per key stroke... hit 'q' or 'Esc' to exit \n";
  while( c != 'q' && c != '\x1b') {
    int its = 10;
    LS->EvolveSparseField( its, &output );
    cvShowImage("image",&output);
    c = cvWaitKey(0);
  }
  
	return 0;
}
