#ifndef KVIEWEROPTIONS_H_
#define KVIEWEROPTIONS_H_

#include <string>
#include <vector>
#include "vtkSmartPointer.h"
#include "vtkMetaImageReader.h"
#include "vtkTransform.h"

struct KViewerOptions
{
public:
  static double getDefaultDrawLabelMaxVal() {
    return 1000.0;
  }
private:

  vtkSmartPointer<vtkTransform> m_Transform;


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
  float     seg_time_interval;
  double m_Center[3];
  float m_CurrentAngle;

  //vtkSmartPointer<vtkMetaImageReader> labelFileReader;

  std::vector<std::string> LabelArrayFilenames;
  std::string ImageArrayFilename;

  float modelOpacity3D;
  float labelOpacity2D;
  bool  labelInterpolate;
  bool  writeCompressed;
  bool  time_triggered_seg_update;
  int   minIntensity;
  int   maxIntensity;

  double drawLabelMaxVal; // value of "inside" labelmap
  int    segmentor_iters; // # of curve evolution update iterations
  int    multilabel_paste_mode; // copy/paste, do all labels, or only active, or ?
  int    multilabel_sgmnt_mode; // run segmentor, do all labels, or only active, or ?

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

void PrintLogo( );

int GetBrushSize();

void SetupTransform();

vtkSmartPointer<vtkTransform> GetTransform()
{
  if( NULL == m_Transform ) {
    std::cout << "warning, attempted to get null transform... initializing now." << std::endl;
    m_Transform = vtkSmartPointer<vtkTransform>::New();
  }
  return m_Transform;
}



};





#endif

