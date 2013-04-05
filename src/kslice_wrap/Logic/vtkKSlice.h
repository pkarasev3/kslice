
#ifndef KVIEWER_GUI_H
#define KVIEWER_GUI_H


#include "KDataWarehouse.h"
#include "KViewerOptions.h"
#include "vtkSlicerKSliceModuleLogicExport.h"

class VTK_SLICER_KSLICE_MODULE_LOGIC_EXPORT vtkKSlice : public vtkObject
{



public:
  static vtkKSlice* New();
  vtkTypeRevisionMacro(vtkKSlice,vtkObject);
  
  void PrintSelf(ostream &os, vtkIndent indent);

  vtkSetObjectMacro(ImageVol, vtkImageData); //void SetImage(vtkImageData* image);
  vtkSetObjectMacro(LabelVol, vtkImageData); //void SetLabel(vtkImageData* label);
  vtkSetObjectMacro(UIVol, vtkImageData);    //void SetUI(vtkImageData* uiVol);

  vtkGetObjectMacro(UIVol, vtkImageData);

  vtkSetMacro(NumIts, int);               //void SetNumIters(int numIts);
  vtkSetMacro(BrushRad, int);             //void SetBrushRad(int rad);
  vtkSetMacro(CurrSlice, int);            //void SetCurrSlice(int currSlice);
  vtkSetMacro(DistWeight, float);         //void SetDistWeight(float distWeight);
  vtkSetMacro(FromSlice, int);            //void CopySlice(int fromSlice);
  vtkSetMacro(CurrLabel, int);            //void CopySlice(int fromSlice);

  // needs implementation
  void SetOrientation(const std::string& orient); // IJ, JK, IK
  void applyUserIncrement(int i, int j, int k, double val);

  //implemented functionality
  void PasteSlice(int toSlice);
  void Initialize();
  void runUpdate(bool reInitFromMask);
  void PrintEmpty();
  void PrintImage();
protected:
 vtkKSlice();
  virtual ~vtkKSlice();
  KDataWarehouse* dataWarehouse; //all storage arrays
  //KViewerOptions* ksliceOptions; //app parameters, variables
  bool initCorrectFlag; //if Kslice has been initialized =1, else starts out being 0
  bool m_bUseEdgeBased; //if =1, use edge based energy, else region based (default =0)
  bool contInit; //flag for initializing the
private:
  vtkImageData* ImageVol;
  vtkImageData* LabelVol;
  vtkImageData* UIVol;
  int NumIts;
  int BrushRad;
  int CurrSlice;
  int DistWeight;
  int FromSlice;
  int ToSlice;
  int CurrLabel;
  std::string Orientation;

};
#endif
