#include "KViewerOptions.h"
#include "getopt_pp_standalone.h"
#include "Logger.h"
#include "KSandbox.h"
#include "vtkImageData.h"
#include "KDataWarehouse.h"
#include "vtkMetaImageReader.h"
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

void KViewerOptions::LoadImage( ){
  QString path;
  path = QFileDialog::getOpenFileName(    NULL,  
              "Choose an Image file to open",    "../data/",   "*.mha" );
  this->ImageArrayFilename = path.toStdString();
}

void KViewerOptions::LoadLabel( const std::string& path ){
  this->LabelArrayFilename = path;
}

int KViewerOptions::GetBrushSize(){
   return (this->paintBrushRad);
}

void KViewerOptions::setFromArgs(int argc, char **argv){
    GetOpt_pp ops(argc, argv); // parse the command line string into chunks

    // set default values if they weren't passed as command line args
    paintBrushRad       = 10; // size of brush (radius)
    paintBrushThreshold = 0.05; // only draw if within X times image range of click point
    modelOpacity3D      = 0.8;
    labelOpacity2D      = 0.2;
    labelInterpolate    = true;
    writeCompressed     = true;
    minIntensity        = 1;
    maxIntensity        = 5000;
    LabelArrayFilename  = "";

    ops >> Option('L', "LabelArrayFilename",  LabelArrayFilename);
    ops >> Option('I', "ImageArrayFilename",  ImageArrayFilename);
    ops >> Option('q', "modelOpacity3D",      modelOpacity3D);
    ops >> Option('w', "labelOpacity2D",      labelOpacity2D);
    ops >> Option('X', "labelInterpolate",    labelInterpolate);
    ops >> Option('n', "minIntensity",        minIntensity);
    ops >> Option('x', "maxIntensity",        maxIntensity);
    ops >> Option('p', "paintBrushRad",       paintBrushRad);
    ops >> Option('c', "writeCompressed",     writeCompressed);
   

    // must have image file names!
    if( ImageArrayFilename.empty()  )  {
      loadImageTrigger = 1;
    } else {
      loadImageTrigger = 0;
    }



}

