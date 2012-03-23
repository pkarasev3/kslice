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

const std::string tmp_file_name = "kslice.tmp";

std::string ExtractDirectory( const std::string& path )  {
  return path.substr( 0, path.find_last_of( '/' ) +1 ); // note: no dice on windows, needs backslash
}

void KViewerOptions::LoadImage( )
{
  std::string data_path = "../data/";
  std::ifstream  cache_reader(tmp_file_name.c_str());
  cache_reader.open(tmp_file_name.c_str(),ifstream::in);

  if(cache_reader.is_open())
  { // cache file exists, read it for the previous dir
    cache_reader.close();
    std::ifstream data(tmp_file_name.c_str());
    std::string line;
    while(std::getline(data,line)) {
      std::stringstream  lineStream(line);
      std::string        cell;
      std::getline(lineStream,cell); //,'\n
      cout << "read cache line: " << cell << endl;
      if( !cell.empty() )
        data_path = cell;
    }
    data.close();
  }
  QString path;
  path = QFileDialog::getOpenFileName( NULL,"Choose an Image file to open",data_path.c_str(),"*.mha" );
  this->ImageArrayFilename = path.toStdString();

  // request by Grant: save the last location, too lazy to click
  std::ofstream  cache_writer;
  cache_writer.open(tmp_file_name.c_str(),ofstream::out);
  cache_writer << ExtractDirectory(path.toStdString()) << std::endl;
  cache_writer.close();

  cout << "loaded image ... ";

}

void KViewerOptions::LoadLabel( const std::string& path ){

  vector<string>::iterator it = std::find(LabelArrayFilenames.begin(),LabelArrayFilenames.end(),path);
  if( it != LabelArrayFilenames.end() || LabelArrayFilenames.size()==0) {
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
  modelOpacity3D      = 0.5;
  labelOpacity2D      = 0.7;
  labelInterpolate    = true;
  writeCompressed     = true;

  minIntensity        = 1;
  maxIntensity        = 5000;
  segmentor_iters     = 10;
  time_triggered_seg_update = true;
  seg_time_interval =0.3;  //time interval for performing (automatic) segmentation while painting
  distWeight=0.3;
  m_CurrentAngle=0;
  //m_PlaneCenter = new double[3];

  //LabelArrayFilenames.push_back("");
  drawLabelMaxVal     = getDefaultDrawLabelMaxVal();

  // Using these is deprecated. Do they even work? Move to boost program options parser if they are desired.
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
      ("Opacity3D,q",po::value<float>(&modelOpacity3D)->default_value(0.4),"model opacity 3D")
      ("Image,I",po::value<std::string>(&ImageArrayFilename)->default_value(""),"image volume file")
      ("CurveIters,C",po::value<int>(&segmentor_iters)->default_value(40),"# of curve evolution update iterations")
      ("MultiLabelPasteMode,P",po::value<int>(&multilabel_paste_mode)->default_value(0),"copy/paste, do all labels [1] or only active [0], or ?")
      ("MultiLabelSegmentMode,S",po::value<int>(&multilabel_sgmnt_mode)->default_value(0),"segmentor, do all labels [1] or only active [0], or ?")
      ("labelOpacity2D,w",po::value<float>(&labelOpacity2D)->default_value(0.5),"2D label opacity initial. note that [o,p] keys adjust it live.")
      ("imgThreshWeight,W",po::value<float>(&m_ThreshWeight)->default_value(0.0),"image threshold weight (truncate label after segmentation)")
      ("time_triggered_seg_update,t",po::value<bool>(&time_triggered_seg_update)->default_value(0),"True or False (0,1), continous levelset integration?")
      ("resampleToCubicVoxels,r",po::value<bool>(&m_bResampleImageAtStartupToCubicVoxels)->default_value(0),"(NOT IMPLEMENTED!!!) True or False (0,1), resample image at startup to cubic voxels?")
      ("useEdgeBased,e",po::value<bool>(&m_bUseEdgeBased)->default_value(0),"Use Edge Based Cost !?")
       //currently not used
      // ("CustomSpeedImg,c",po::value<std::string>(&m_SpeedImageFileName)->default_value(""),"custom speed image")
      ("DrawSpreadOffViewPlane,d",po::value<int>(&m_DrawSpreadOffViewPlane)->default_value(0),"how much +/- from viewed slice to draw in (unobservable area off-plane)")
      ("help","print help");

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

