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
#include "KSegmentor.h"
#include "llist.h"
#include "sfm_local_chanvese_mex.h"

namespace po = boost::program_options;
using namespace cv;
using namespace std;

#define SP( Y, X )  vtkSmartPointer<X> Y = vtkSmartPointer<X>::New()


namespace {

struct KSegTest2_Options
{

    KSegTest2_Options(int argc, char* argv[])
    {
        po::options_description commands("Allowed options");
        commands.add_options()
                ("Labels,L",po::value< vector<string> >(&labelFilenames)->multitoken(),
                 "which labels to load, e.g. --Labels=a.mha b.mha c.mha")
                ("Outputs,O",po::value< vector<string> >(&outputFilenames)->multitoken(),
                 "which labels to write, e.g. --Outputs=a_out.mha b_out.mha c_out.mha")
                ("Image,I",po::value<string>(&imageFilename)->default_value(""),
                 "image volume file")
                ("CurveIters,C",po::value<int>(&segmentor_iters)->default_value(200),
                 "# of curve evolution update iterations")
                ("SmoothnessKappa,k",po::value<double>(&smoothness_kappa)->default_value(0.5),
                 "smoothness weight, between 0 and a reasonably small integer")
                ("verbose,v",po::value<bool>(&verbose)->default_value(false),
                 "smoothness weight, between 0 and a reasonably small integer")
                ("EnergyName,E",po::value<string>(&energyFunctionalName)->default_value("chanvese"),
                 "name of energy function to test, e.g. chanvese or kappa. ")
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
            cout << "will try to write labels: " << endl;
            BOOST_FOREACH(string labelName, outputFilenames)
                    std::cout << labelName << std::endl;
        }
    }

    int            segmentor_iters;
    double         smoothness_kappa;
    bool           verbose;
    vector<string> labelFilenames;
    vector<string> outputFilenames;
    string         imageFilename;
    string         energyFunctionalName;


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

//extern double ain, aout, auser; // means
//extern double *pdfin, *pdfout, *pdfuser;
//extern long numdims;
//extern double engEval;
//extern bool UseInitContour;
//extern double *Ain, *Aout, *Sin, *Sout; //local means
//[phi C L] = ls_sparse(img,mask,iterations,display,lam,rad,dthresh);
void mexFunctionReplacement(long numberdims, int *mdims,
                            double *img, double *mask_in,
                            unsigned short* label_out,
                            int contour_iters, double smoothness_lambda)
{

    double *phi, *label;
    long    dims[5];
    long dimz,dimy,dimx;
    LL *Lz, *Ln1, *Ln2, *Lp1, *Lp2;
    LL *Lin2out, *Lout2in;

    int numdims=numberdims;

    dims[2] = 1;
    dims[1] = 1;
    switch(numdims)
    {
    case 3: dimz = (int)mdims[2]; dims[2] = dimz;
    case 2: dimx = (int)mdims[1]; dims[1] = dimx;
    case 1: dimy = (int)mdims[0]; dims[0] = dimy;
    }
    dims[3] = dims[0]*dims[1];
    dims[4] = dims[0]*dims[1]*dims[2];


    // level-set function
    phi  = new double[mdims[0]*mdims[1]*mdims[2]];
    // 'local' version of label for the sfm code (seems redundant...)
    label= new double[mdims[0]*mdims[1]*mdims[2]];


    //create linked lists
    Lz      = ll_create();
    Ln1     = ll_create();
    Ln2     = ll_create();
    Lp1     = ll_create();
    Lp2     = ll_create();
    Lin2out = ll_create();
    Lout2in = ll_create();

    //initialize lists, phi, and labels
    ls_mask2phi3c(mask_in,phi,label,dims,Lz,Ln1,Ln2,Lp1,Lp2);

    short plhs[1];
    int display = (int) false;

    int energyToUse = (int) (0 != static_energyFunctionalName.compare(string("chanvese")) );
    switch ( energyToUse )
    {
    case 0:
        cout << "running chanvese() ... " << endl;
        chanvese(img,phi,label,dims,
                 Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in,
                 10,0.5,plhs,display);
        break;
    case 1:
        cout << "running kappa() ... " << endl;
        kappa(img,phi,label,dims,
              Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in,
              contour_iters,smoothness_lambda,plhs,display);
    default:
        break;
    }

    double labelMaxVal = 1000.0;
    //threshold phi to find segmentation label,
    // assign it to appropriate range of label!
    // shift and scale from [-3,3] to [0,1] x max_label
    long elemNum=0;
    for (int k=0; k<=mdims[2]-1; k++) {
        for (int j=0; j<=mdims[1]-1; j++)  {
            for (int i=0; i<=mdims[0]-1; i++) {
                double phi_val = phi[elemNum];
                double phi_out = (-phi_val + 3.0) / 6.0;
                label_out[elemNum] =
                        (unsigned short) ( ( (phi_out > 0.95) +
                                             (phi_out > 0.8) +
                                             (phi_out > 0.65) +
                                             (phi_out > 0.5) )
                                           * labelMaxVal / 4.0 );
                elemNum++;
            }
        }
    }

    //destroy linked lists
    ll_destroy(Lz);
    ll_destroy(Ln1);
    ll_destroy(Ln2);
    ll_destroy(Lp1);
    ll_destroy(Lp2);
    ll_destroy(Lin2out);
    ll_destroy(Lout2in);

    delete phi;
    delete label;

    // The output has been written to ptrLabel_out!

    return;
}

void  RunTest_Segment3D( vtkImageData* image, vtkImageData* label, int contour_iters = 100, double smoothness_lambda = 0.5)
{
    PrintSizes();

    std::vector<int> mdims(3);
    image->GetDimensions(&(mdims[0]));
    cout << "Image Dimensions: " << cv::Mat(mdims) << endl;
    double* d_imagePtr=new double[ mdims[0]*mdims[1]*mdims[2] ];
    double* d_labelPtr=new double[ mdims[0]*mdims[1]*mdims[2] ];

    assert( image->GetNumberOfPoints() == label->GetNumberOfPoints() );

    unsigned short *us_imagePtr = static_cast<unsigned short*>(image->GetScalarPointer());
    unsigned short *us_labelPtr = static_cast<unsigned short*>(label->GetScalarPointer());


    //Loop Alternate Syntax: ptr[k*mdims[1]*mdims[0] +j*mdims[0]+i];
    long elemNum=0;
    for (int k=0; k<=mdims[2]-1; k++) {
        for (int j=0; j<=mdims[1]-1; j++) {
            for (int i=0; i<=mdims[0]-1; i++,elemNum++) {
                d_imagePtr[elemNum]=(double)  us_imagePtr[elemNum];
                d_labelPtr[elemNum]=(double)  us_labelPtr[elemNum];
            }
        }
    }
    // Run it.
    int numdims = 3;
    mexFunctionReplacement( numdims, &(mdims[0]),
                            d_imagePtr, d_labelPtr, us_labelPtr,
                            contour_iters, smoothness_lambda);

    delete d_imagePtr;
    delete d_labelPtr;

    label->Update();

}



}

int main( int argc, char* argv[] )
{

    cout << " Sample usage: "
         << argv[0] << "-I ./data/emorySE06.mha -L `ls ./*.mha | grep 0000` " << endl;

    // Create and Initialize Segmentor.
    KSegTest2_Options  opts(argc,argv);
    static_energyFunctionalName = opts.energyFunctionalName;

    SP(image,vtkImageData);
    SP(label,vtkImageData);
    SP(imgReader,vtkMetaImageReader);
    SP(lblReader,vtkMetaImageReader);

    imgReader->SetFileName(opts.imageFilename.c_str());
    imgReader->SetDataScalarTypeToUnsignedShort();
    imgReader->Update();

    for( int idx = 0; idx < (int) opts.labelFilenames.size(); idx++ )
    {
    lblReader->SetFileName(opts.labelFilenames[idx].c_str());
        lblReader->SetDataScalarTypeToUnsignedShort();
        lblReader->Update();

        image = imgReader->GetOutput();
        label = lblReader->GetOutput();

        if( opts.verbose )
        {
            //vtkIndent indentVTK=new vtkIndent(0);
            image->PrintSelf(std::cout, vtkIndent(0));
            label->PrintSelf(std::cout, vtkIndent(0));
        }
        cout << "running RunTest_Segment3D, iters = " << opts.segmentor_iters << ", kappa = " << opts.smoothness_kappa << endl;
        RunTest_Segment3D(image,label,opts.segmentor_iters,opts.smoothness_kappa); // should update label in-place

        SP(lblWriter,vtkMetaImageWriter);
        string outputFilename = "output_ksegTest2_" + boost::lexical_cast<string>(idx) + ".mha";
        if( ! opts.outputFilenames.empty() ) {
            outputFilename = opts.outputFilenames[idx];
        }
        lblWriter->SetCompression(true);
        lblWriter->SetInput(label);
        lblWriter->SetFileName( outputFilename.c_str() );
        lblWriter->Write();
        cout << "Done! Output attempted in " << lblWriter->GetFileName() << endl;
    }
    return 0;

}
