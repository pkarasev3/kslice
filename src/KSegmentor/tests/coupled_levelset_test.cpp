#include <iostream>
#include <string>
#include <list>


#if !defined(_WIN32)
#include <unistd.h>
#endif
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>

#include "KoupledKurvolver.h"
#include "boost/shared_ptr.hpp"
#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"

namespace po = boost::program_options;
using namespace cv;
using namespace std;

int main( int argc, char* argv[] )
{

  // Load/Generate Image.

  // Create and Initialize Segmentor.
  KoupledKurvolver::Options  opts(argc,argv);
  std::shared_ptr<KoupledKurvolver>  coupled_levelsets( new KoupledKurvolver(opts) );
  coupled_levelsets->Print();

  // Run it.

  return 0;

}
