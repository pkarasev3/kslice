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

#define SP( Y, X )  vtkSmartPointer<X> Y = vtkSmartPointer<X>::New()


namespace {

struct Test_Options
{

    Test_Options(int argc, char* argv[])
    {
        po::options_description commands("Allowed options");
        commands.add_options()
                ("Image,I",po::value<string>(&imageFilename)->default_value(""),"image volume file")
                ("help","print help");
        po::variables_map vm;
        po::store( parse_command_line(argc, argv, commands, po::command_line_style::unix_style ), vm);
        po::notify(vm);

        if (vm.count("help")) {
            cout << commands << "\n";
            exit(1);
        }
    }
    string         imageFilename;
};

string static_energyFunctionalName;

void PrintSizes()
{
    printf("sizeof(char) == %d\n", (int) sizeof(char));
    printf("sizeof(short) == %d\n",(int) sizeof(short));
    printf("sizeof(int) == %d\n", (int) sizeof(int));
    printf("sizeof(long) == %d\n", (int) sizeof(long));
    printf("sizeof(long long) == %d\n", (int) sizeof(long long));
    printf("sizeof(double) == %d\n", (int) sizeof(double));
}

}

int main( int argc, char* argv[] )
{

    cout << " Sample usage: "
         << argv[0] << "-I ./data/emorySE06.mha -L `ls ./*.mha | grep 0000` " << endl;

    // Create and Initialize Segmentor.
    Test_Options  opts(argc,argv);

    SP(image,vtkImageData);
    SP(imgReader,vtkMetaImageReader);

    bool isEqual1 = (  opts.imageFilename == "" );
    bool isEqual2 = (! opts.imageFilename.compare("") );
    bool isEmpty  = opts.imageFilename.empty();

    cout << "Comparing filename " << opts.imageFilename ;
    cout << " ... as         == \"\" ... " << isEqual1 << endl;
    cout << " ... as ! .compare(\"\")  ... " << isEqual2 << endl;
    cout << " ... as .empty()  ... " << isEmpty << endl;

    return 0;

}
