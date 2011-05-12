#include "vtkInitClosedPath.h"
#include "MeshOps.h"
#include "Logger.h"
#include "path_finder.h"
#include "polydata_helper.h"
#include "LevelSetInitHelper.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkCellLocator.h"
#include "vtkFloatArray.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkPolygon.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkTriangleFilter.h"
#include "vtkSmartPointer.h"

#include <list>
#include <vector>
#include <algorithm>
#include <set>
#include <map>

using std::list;
using std::vector;
using std::set;
using std::map;
using std::string;
using std::valarray;
using namespace vrcl;




//================================================================


const std::string vtkInitClosedPath::CONTOUR_ARRAY_NAME      =  "ActiveContourVertexLabels";
const std::string vtkInitClosedPath::DISTANCEMAP_ARRAY_NAME  =  "DistanceMapsFromSeeds";

vtkCxxRevisionMacro(vtkInitClosedPath, "$Revision: 1.00 $");
vtkStandardNewMacro(vtkInitClosedPath);

vtkInitClosedPath::vtkInitClosedPath()
{
  this->activeContourVertIdx  = vtkSmartPointer<vtkIntArray>::New();   // vertex labels
  this->distanceMapsFromSeeds = vtkSmartPointer<vtkFloatArray>::New(); // distance maps, ntuples = nseeds
  this->activeContourSeedIdx  = vtkSmartPointer<vtkIntArray>::New();   // small list of initial points
}

void vtkInitClosedPath::SetSource(vtkPolyData *source)
{
  this->SetInput(1, source);
}

vtkPolyData *vtkInitClosedPath::GetSource()
{
  if (this->GetNumberOfInputConnections(1) < 1)
    {
    return NULL;
    }
  return vtkPolyData::SafeDownCast(
    this->GetExecutive()->GetInputData(1, 0));
}

  // Where the bulk of work is done!
int vtkInitClosedPath::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
   // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // update progress bar at some intervals
  this->UpdateProgress(0.15);

    // get the input and ouptut
  vtkPolyData *input = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  // Check input
  ::size_t num_verts=input->GetNumberOfPoints();
  ::size_t numCells=input->GetNumberOfCells();
  if (num_verts < 1 || numCells < 1)
    {
    vtkErrorMacro(<<"No data to smooth!")
    return 1;
    }

  { // copy everything through first, before updating...
    output->CopyStructure(input);
    output->GetPointData()->PassData(input->GetPointData());
    output->GetCellData()->PassData(input->GetCellData());
    vtkDebugMacro(<<"Analyzing ____ ...")
  }

  vtkDataArray* contourIdxArrayIn = input->GetPointData()->GetArray( CONTOUR_ARRAY_NAME.c_str() );
  if( NULL == contourIdxArrayIn )  {

    size_t numInit ;
    std::vector<int> seedIdx;

      activeContourVertIdx->SetName( CONTOUR_ARRAY_NAME.c_str() );
      activeContourVertIdx->SetNumberOfComponents(1);
      activeContourVertIdx->SetNumberOfTuples( num_verts );
      numInit = this->activeContourSeedIdx->GetNumberOfTuples();
      seedIdx = std::vector<int>(numInit);
      for( ::size_t k = 0; k < numInit; k++ )
        seedIdx[k] = activeContourSeedIdx->GetValue(k);

    // Now, link them together...
    output->BuildLinks();
    vtkCellArray* faces = output->GetPolys();
    if (faces->GetNumberOfCells() == 0) {
      std::cerr << "No cells on the poly data polys \n";
      return 0;
    }

    std::cout << "...assigning distance maps to vertices from seed points " << std::endl;
    compute_distance_map( output, this->distanceMapsFromSeeds, seedIdx );
    std::cout << " done! " << std::endl;

    vtkDataArray* contourIdxArrayIn = input->GetPointData()->GetArray("ActiveContourVertexIndices");
    vtkSmartPointer<vtkIntArray>  activeContourVertIdx = vtkSmartPointer<vtkIntArray>::New();
    activeContourVertIdx = vtkIntArray::SafeDownCast( contourIdxArrayIn );

    std::set<int>  Lp1;
    std::set<int>  Ln1;
    std::set<int>  Ln2;
    std::set<int>  Lp2;
    std::set<int>  Lz;
    std::list<int> L0;

    // activeContourVertIdx = initLevelSetLists(  output, seedIdx, L0, Lp1, Ln1, Lp2, Ln2, Lz );
    // output->GetPointData()->AddArray( activeContourVertIdx );

    // add distance maps array
    distanceMapsFromSeeds->SetName( DISTANCEMAP_ARRAY_NAME.c_str() );
    output->GetPointData()->AddArray( distanceMapsFromSeeds );


} else {
    LOUT "array seems to exist already... passing through! " LENDL
}

  // update progress bar at some intervals
  this->UpdateProgress(1.0);

  return 1;
 }

int vtkInitClosedPath::FillInputPortInformation(int port,
                                                      vtkInformation *info)
{
  if (!this->Superclass::FillInputPortInformation(port, info))    {
    return 0;
    }

  if (port == 1)    {
    info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
    }
  return 1;
}


void vtkInitClosedPath::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Truth: " << 42 << "\n";
  if ( this->GetSource() )    {
      os << indent << "Source: " << static_cast<void *>(this->GetSource()) << "\n";
    }
  else  {
    os << indent << "Source (none)\n";
    }
}
