#ifndef __vtkSysInfo_h
#define __vtkSysInfo_h

// SlicerLogic includes
#include "vtkSlicerBaseLogic.h"

// VTK includes
#include <vtkObject.h>

// VTKSYS includes
#include <vtksys/SystemInformation.hxx>

// STD includes
#include <string>

#include "vtkSlicerKSliceModuleLogicExport.h"

class VTK_SLICER_KSLICE_MODULE_LOGIC_EXPORT vtkSysInfo : public vtkObject
{
public:
  static vtkSysInfo *New();
  vtkTypeRevisionMacro(vtkSysInfo,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  void PrintEmpty();

protected:

  vtkSysInfo();
  virtual ~vtkSysInfo();
  vtkSysInfo(const vtkSysInfo&);
//  void operator=(const vtkSysInfo&);
//
//  //private:
//
//  vtksys::SystemInformation SystemInformation;
//  std::string StringHolder;

};
#endif
