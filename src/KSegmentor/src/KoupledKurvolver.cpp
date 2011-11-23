#include "KSandbox.h"
#include "vtkImageData.h"
#include "KDataWarehouse.h"
#include "vtkMetaImageReader.h"
#include "boost/foreach.hpp"
#include "boost/program_options.hpp"
#include "KoupledKurvolver.h"
#include <string>

using std::string;
using std::vector;
namespace po = boost::program_options;

KoupledKurvolver::Options::Options(int ac, char *av[])
{
  // parse args with boost::program_options
}

/** initialize and allocate things based on options,
       e.g. number of labels, observer versus tanh(g(U)), etc */
KoupledKurvolver::KoupledKurvolver(const KoupledKurvolver::Options &opts_in)
{


}

/** Print info about self */
void KoupledKurvolver::Print()
{


}
