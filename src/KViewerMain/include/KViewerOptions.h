#ifndef KVIEWEROPTIONS_H_
#define KVIEWEROPTIONS_H_

#include <string>
#include <vector>
#include "vtkSmartPointer.h"
#include "vtkMetaImageReader.h"

struct KViewerOptions
{

public:
  int     numSlices;
  int     imgHeight;
  int     imgWidth;
  double  sliceZSpace;
  double  sliderMax;
  double  sliderMin;
  int     paintBrushRad;
  double  paintBrushThreshold; 
  int     loadImageTrigger;


  vtkSmartPointer<vtkMetaImageReader> labelFileReader;
  std::string LabelArrayFilename;
  std::string ImageArrayFilename;
  float modelOpacity3D;
  float labelOpacity2D;
  bool  labelInterpolate;
  bool  writeCompressed;
  int   minIntensity;
  int   maxIntensity;

//////////////////////

  // BAD  (these are "State" not "Options" )
  std::vector<double> imageOrigin;
  std::vector<double> imageSpacing;
  std::vector<int>    imageExtent;

  /////////////////////////

  
  // METHODS

  /** send args from main() to me to set params
    */
  void setFromArgs(int argc, char **argv);

  /** print help about args
    */
  void help();

void LoadImage( );

void LoadLabel(const std::string& loadLabel);

int GetBrushSize();



};





#endif

