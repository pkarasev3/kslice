#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

#if 0
 size_t N() const {
    return 4; // x,y,vx,vy
  }

  virtual void outputCallback(const vector<cv::Mat> &particles,
                              const vector<double> &weights)
  {
    // Assign the "low dimensional outputs"

    track_box.x      = Icurr.cols / 2;
    track_box.y      = Icurr.rows / 2;
    track_box.width  = 20;
    track_box.height = 20;

    Icurr.copyTo(Iprev);
    Icurr_smooth.copyTo(Iprev_smooth);
  }

  // **** Functions specific to this incarnation of TrackingProblem
public:
  /** draw the track_box on top of an image, for display */
  void drawTrackBox( cv::Mat& outimg ) {
    dIdt.copyTo(outimg);

    cv::rectangle(outimg, track_box, Scalar(255,255,255),3,CV_AA);
  }

  // **** Data specific to this incarnation of TrackingProblem

private:
  Mat dIdt;
  Mat Icurr;
  Mat Iprev;
  Mat Iprev_smooth;
  Mat Icurr_smooth;

  cv::Rect  track_box;

};

#endif

int main( int argc, char* argv [] )
{
  
  Mat img = imread(argv[1]);
  Mat img2 = img2.clone();
  imwrite("test_savepng.png",img2);

#if 0
  for (;;)
  {
    capture >> frame; // grab frame data from webcam
    if (frame.empty())
      continue;

    image_container[0] = frame;
    image_container[1] = frame;
    pft.processNewData(image_container, meta_data );

    prob->drawTrackBox(image_container[1]);
    cv::imshow(window_name,image_container[1]);

    //cv::Point2f pt_xy;
    //cout << "tracking at:  x = " << pt_xy.x << ", y = " << pt_xy.y << endl;

    char key = cv::waitKey(10);
    if( 'q' == key ) // quick if we hit q key
      break;


  }
#endif
  return 0;
}

