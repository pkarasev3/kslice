#include "vtkSysInfo.h"
#include <vtkObjectFactory.h>

vtkCxxRevisionMacro(vtkSysInfo, "$Revision: $");
vtkStandardNewMacro(vtkSysInfo);

vtkSysInfo::vtkSysInfo()
{
}

vtkSysInfo::~vtkSysInfo()
{
}

void vtkSysInfo::PrintSelf(ostream& os, vtkIndent indent)
{

  Superclass::PrintSelf(os, indent);
}

void vtkSysInfo::PrintEmpty()
{
    std::cout<<"This empty print works" <<std::endl;
}
