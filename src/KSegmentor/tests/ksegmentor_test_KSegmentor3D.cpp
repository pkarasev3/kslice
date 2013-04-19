#include <iostream>
#include <string>
#include <list>

#ifndef WIN32
#include <unistd.h>
#endif

#include "boost/shared_ptr.hpp"
#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"
#include "vtkMetaImageReader.h"
#include "vtkMetaImageWriter.h"
#include "boost/foreach.hpp"
#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include "KSegmentor3D.h"

//#include <initializer_list>

namespace po = boost::program_options;
using namespace std;

#define SP( Y, X )  vtkSmartPointer<X> Y = vtkSmartPointer<X>::New()

namespace {

struct KSegTest2_Options
{
    KSegTest2_Options(int argc, char* argv[])
    {
        po::options_description commands("Allowed options");
        commands.add_options()
                ("Label,L",po::value< string >(&inputFilename),
                 "which labels to load, e.g. --Label=patela.mha (Required)")
                ("Output,O",po::value< string >(&outputFilename),
                 "which labels to write, e.g. --Output=a_out.mha")
                ("Image,I",po::value<string>(&imageFilename)->default_value(""),
                 "image volume file")
                ("CurveIters,C",po::value<int>(&segmentor_iters)->default_value(10),
                 "# of curve evolution update iterations")
                ("SmoothnessKappa,k",po::value<double>(&smoothness_kappa)->default_value(0.5),
                 "smoothness weight, between 0 and a reasonably small integer")
                ("verbose,v",po::value<bool>(&verbose)->default_value(false),
                 "verbosity, higher to display more crap")
                ("EnergyName,E",po::value<string>(&energyFunctionalName)->default_value("ChanVese"),
                 "name of energy function to test, e.g. chanvese or kappa. ")
                ("help","print help");

        po::variables_map vm;
        po::store( parse_command_line(argc, argv, commands, po::command_line_style::unix_style ), vm);
        po::notify(vm);

        if (vm.count("help")) {
            cout << commands << "\n";
            exit(1);
        }

        if( ! inputFilename.empty() )
        {
            cout << "attempting to load image: " << imageFilename << endl;
            cout << "attempting to load label: " << inputFilename << std::endl;
            cout << "will try to write label:  " << outputFilename << endl;

        }
    }

    int            segmentor_iters;
    double         smoothness_kappa;
    bool           verbose;
    string         outputFilename;
    string         inputFilename;
    string         imageFilename;
    string         energyFunctionalName;


};

string static_energyFunctionalName;


}

int main( int argc, char* argv[] )
{
    using namespace vrcl;

    cout << " Sample usage: "
         << argv[0] << "-I ./data/emorySE06.mha -L  LabelXYZ.mha  " << endl;

    // Create and Initialize Segmentor.
    KSegTest2_Options  opts(argc,argv);
    static_energyFunctionalName = opts.energyFunctionalName;

    SP(image,vtkImageData);
    SP(label,vtkImageData);
    SP(UIVol,vtkImageData);
    SP(imgReader,vtkMetaImageReader);
    SP(lblReader,vtkMetaImageReader);

    imgReader->SetFileName(opts.imageFilename.c_str());
    //imgReader->SetDataScalarTypeToUnsignedShort();
    imgReader->Update();

    if(! imgReader->CanReadFile(imgReader->GetFileName()) ){ return -1; }

    double Ebest = 1e99;
    double Efirst;
    boost::shared_ptr<KSegmentor3D>  kseg; /** Correct Storage?? */
    int result = 0;

    lblReader->SetFileName(opts.inputFilename.c_str());
    // lblReader->SetDataScalarTypeToUnsignedShort();
    lblReader->Update();
    if(! lblReader->CanReadFile(lblReader->GetFileName()) ){ return -2; }

    image = imgReader->GetOutput();
    label = lblReader->GetOutput();
    { // make an initial UIVol
        UIVol->DeepCopy(label);
        UIVol->SetScalarTypeToDouble();
        UIVol->AllocateScalars();
    }

    kseg = boost::shared_ptr<KSegmentor3D>(
                new KSegmentor3D(image,label,UIVol,true,0,50,0.005,3/*brush rad*/,1/**label value*/));
    kseg->setNumIterations( opts.segmentor_iters );
    kseg->SetEnergyChanVese( );

    vector<double> defaultPlaneCenter(3,0.0);
    vector<double> defaultPlaneNormal({1.0,1.0,1.0});
    vector<int> Lz_len; // look at length as correctness check

    for( int k=0; k<10; k++ ) {
        kseg->setCurrIndex( 2 ); // slice index
        kseg->PrintUpdateInfo();

        kseg->SetPlaneCenter(&(defaultPlaneCenter[0]));
        kseg->SetPlaneNormalVector(&(defaultPlaneNormal[0]));

        kseg->Update3D( (k==0)/*is it first time run?*/ );
        Lz_len.push_back( kseg->GetLzLength() );
        double mu_in,mu_out;
        double E = kseg->evalChanVeseCost(mu_in,mu_out);
        cout << "Iter of Update3D()  = " <<k<<", E= " <<E<<", mu_i = " << mu_in << ", mu_o = " << mu_out << endl;
        if( 1==k )      { Efirst= E; }
        if( E < Ebest ) { Ebest = E; }
    }

    if( ! opts.outputFilename.empty() )
    {   /** Save to file if -O  XXX.mha was given */
        string outputFilename = opts.outputFilename;
        SP(lblWriter,vtkMetaImageWriter);
        lblWriter->SetCompression(true);
        lblWriter->SetInput(label);
        lblWriter->SetFileName( outputFilename.c_str() );
        lblWriter->Write();
        cout << "Done! ATtempted to save output label MHA in: " << lblWriter->GetFileName() << endl;
    } else { cout<<"No output filename(s) given with -O flag; Not saving final MHA."
                << endl; }
    if(Ebest < Efirst) {
      // result = 0; // [All Good] code for running with "make test" or "ctest -V"
    }else if( Lz_len.back() <= Lz_len[1] ) {
      //  result = 0; // contour should only expand here!
    }

    return result;

}


#if 0         // Death Row


//  How to use this file in CTest:

add_executable(ksegmentor_test_KSeg3D  ksegmentor_test_KSegmentor3D.cpp)
target_link_libraries(ksegmentor_test_KSeg3D ${Boost_LIBRARIES} KSegmentor vrcl ${KSEGM_LIBS} )


add_test( ksegmentor_test1_KSeg3D  # Name of Test
          ${CMAKE_SOURCE_DIR}/bin/ksegmentor_test_KSeg3D  # Name of binary to run
-I  ${CMAKE_SOURCE_DIR}/data/noisy_parab_plane_intersect.mha
-L  ${CMAKE_SOURCE_DIR}/data/label_parab_slice0.mha
-C  10
)

add_test( ksegmentor_test2_KSeg3D  # Name of Test
          ${CMAKE_SOURCE_DIR}/bin/ksegmentor_test_KSeg3D  # Name of binary to run
-I  ${CMAKE_SOURCE_DIR}/data/noisy_parab_plane_intersect.mha
-L  ${CMAKE_SOURCE_DIR}/data/label_parab_slice0.mha
-C  50
-o  ${CMAKE_SOURCE_DIR}/data/out_test_tmp.mha
)
#endif
