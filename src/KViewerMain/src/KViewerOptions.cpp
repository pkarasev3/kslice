#include "KViewerOptions.h"
#include "getopt_pp_standalone.h"
#include "Logger.h"
#include "KSandbox.h"
#include "vtkImageData.h"
#include "KDataWarehouse.h"
#include "vtkMetaImageReader.h"
#include "boost/foreach.hpp"
#include "boost/program_options.hpp"
#include <string>
#include <QMainWindow>
#include <QMenu>
#include <QString>
#include <QFileDialog>

#ifndef WIN32
#include <sys/types.h>
#include <unistd.h>
#endif

using namespace GetOpt;
using namespace vrcl;
using std::string;
using std::vector;

namespace po = boost::program_options;

void KViewerOptions::LoadImage( ){
  QString path;
  path = QFileDialog::getOpenFileName(    NULL,  
                                          "Choose an Image file to open",    "../data/",   "*.mha" );
  this->ImageArrayFilename = path.toStdString();
  //Should it be initialized somewhere else?
  m_Transform =vtkSmartPointer<vtkTransform>::New();

}

void KViewerOptions::LoadLabel( const std::string& path ){

  vector<string>::iterator it = std::find(LabelArrayFilenames.begin(),LabelArrayFilenames.end(),path);
  if( it != LabelArrayFilenames.end() ) {
    this->LabelArrayFilenames.push_back(path);
  } else {
    cout << path << " label file is already loaded! "<< endl;
  }
}



int KViewerOptions::GetBrushSize(){
  return (this->paintBrushRad);
}

void KViewerOptions::setFromArgs(int argc, char **argv){

  PrintLogo();

  GetOpt_pp ops(argc, argv); // parse the command line string into chunks

  // set default values if they weren't passed as command line args
  paintBrushRad       = 7; // size of brush (radius)
  paintBrushThreshold = 0.05; // only draw if within X times image range of click point
  modelOpacity3D      = 0.3;
  labelOpacity2D      = 0.7;
  labelInterpolate    = true;
  writeCompressed     = true;

  minIntensity        = 1;
  maxIntensity        = 5000;
  segmentor_iters     = 30;
  time_triggered_seg_update = true;
  seg_time_interval =0.3;  //time interval for performing (automatic) segmentation while painting
  m_CurrentAngle=0;

  LabelArrayFilenames.push_back("");
  drawLabelMaxVal     = getDefaultDrawLabelMaxVal();

  ops >> Option('q', "modelOpacity3D",      modelOpacity3D);
  ops >> Option('X', "labelInterpolate",    labelInterpolate);
  ops >> Option('n', "minIntensity",        minIntensity);
  ops >> Option('x', "maxIntensity",        maxIntensity);
  ops >> Option('p', "paintBrushRad",       paintBrushRad);
  ops >> Option('c', "writeCompressed",     writeCompressed);
  ops >> Option('M', "drawLabelMaxVal",     drawLabelMaxVal);

  po::options_description commands("Allowed options");
  commands.add_options()
      ("Labels,L",po::value< std::vector<std::string> >(&LabelArrayFilenames)->multitoken(),
       "which labels to pre-load, e.g. --Labels=a.mha b.mha c.mha")
      ("Image,I",po::value<std::string>(&ImageArrayFilename)->default_value(""),"image volume file")
      ("CurveIters,C",po::value<int>(&segmentor_iters)->default_value(30),"# of curve evolution update iterations")
      ("MultiLabelPasteMode,P",po::value<int>(&multilabel_paste_mode)->default_value(0),"copy/paste, do all labels [1] or only active [0], or ?")
      ("MultiLabelSegmentMode,S",po::value<int>(&multilabel_sgmnt_mode)->default_value(0),"segmentor, do all labels [1] or only active [0], or ?")
      ("labelOpacity2D,w",po::value<float>(&labelOpacity2D)->default_value(0.5),"2D label opacity initial. note that [o,p] keys adjust it live.")
      ("help","print help")
      ;

  po::variables_map vm;
  po::store( parse_command_line(argc, argv, commands, po::command_line_style::unix_style ), vm);
                                // bitwise XOR => disallow short style po::command_line_style::unix_style ^ po::command_line_style::allow_short),
  po::notify(vm);

  if (vm.count("help")) {
    cout << commands << "\n";
    exit(1);
  }

  if( ! LabelArrayFilenames.empty() ) {
    cout << "attempting to preload labels: " << endl;
    BOOST_FOREACH(string labelName, LabelArrayFilenames)
        std::cout << labelName << std::endl;
  }

  // must have image file names!
  if( ImageArrayFilename.empty()  )  {
    loadImageTrigger = 1;
  } else {
    loadImageTrigger = 0;
  }



}


void KViewerOptions::PrintLogo()
{



  cout << "\n\n                            ,--.\n                           {    }\n                          K,   }\n                          /  ~Y`\n                     ,   /   /\n                    {_'-.__/\n                      `/-.__L._\n                      /  ' /`\\_}\n                     / ' /\n             ____   /  ' /\n      ,-'~~~~    ~~/  ' /_\n    ,'             ``~~~  ',\n   (                       Y\n  {                         I\n {      -     СМЕРть Шпионам    `,\n |       ',                   )\n |        |   ,..__      __. Y\n |    .,_./  Y ' / ^Y   J   )|\n \\           |' $$  |   | $$||\n \\          L_/    . _ (_,.'(\n   \\,   ,      ^^""' / |      )\n     \\_  \\          /,L]     /\n      '-_~-,       ` `   ./`\n          `'{_            )\n              ^^[]..___,.--`\n\n" << endl;


}

