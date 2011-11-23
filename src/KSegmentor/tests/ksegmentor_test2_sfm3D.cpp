#include <iostream>
#include <string>
#include <list>

#include <unistd.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>

#include "KoupledKurvolver.h"
#include "boost/shared_ptr.hpp"
#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"
#include "vtkMetaImageReader.h"
#include "boost/foreach.hpp"
#include "vtkSmartPointer.h"
#include "vtkImageData.h"

namespace po = boost::program_options;
using namespace cv;
using namespace std;

namespace {

struct KSegTest2_Options
{

  void set_from_args(int argc, char* argv[]) {
    po::options_description commands("Allowed options");
    commands.add_options()
        ("Labels,L",po::value< std::vector<std::string> >(&labelFilenames)->multitoken(),
         "which labels to pre-load, e.g. --Labels=a.mha b.mha c.mha")
        ("Image,I",po::value<std::string>(&imageFilename)->default_value(""),"image volume file")
        ("CurveIters,C",po::value<int>(&segmentor_iters)->default_value(30),"# of curve evolution update iterations")
        ("Image,I",po::value<std::string>(&energyFunctionalName)->default_value("kappa"),"name of energy function to test; RTFC for valid types.")
        ("help","print help");

    po::variables_map vm;
    po::store( parse_command_line(argc, argv, commands, po::command_line_style::unix_style ), vm);
    po::notify(vm);

    if (vm.count("help")) {
      cout << commands << "\n";
      exit(1);
    }

    if( ! labelFilenames.empty() )
    {
      cout << "attempting to load image: " << imageFilename << endl;
      cout << "attempting to load labels: " << endl;
      BOOST_FOREACH(string labelName, labelFilenames)
          std::cout << labelName << std::endl;
    }
  }

  int            segmentor_iters;
  vector<string> labelFilenames;
  string         imageFilename;
  string         energyFunctionalName;


};


}

int main( int argc, char* argv[] )
{

  // Load/Generate Image.

  // Create and Initialize Segmentor.
  KoupledKurvolver::Options  opts(argc,argv);
  boost::shared_ptr<KoupledKurvolver>  coupled_levelsets( new KoupledKurvolver(opts) );
  coupled_levelsets->Print();

  // Run it.

  return 0;

}
