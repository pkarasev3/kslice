
#include "polydata_helper.h"
#include "vtkPolyData.h"
#include "vtkFloatArray.h"
#include "vtkIntArray.h"
#include "vtkPointData.h"
#include "vtkSmartPointer.h"
#include "Logger.h"
#include "vtkPolyLine.h"
#include "vtkCellArray.h"
#include "vtkPointSource.h"
#include "vtkPolyDataNormals.h"
#include <vector>
#include <string>
#include <list>
#include <set>

using std::list;
using std::string;
using std::stringstream;
using std::cout;
using std::endl;
using std::cerr;
using std::vector;
using std::set;

namespace {

    inline void shift_by_normal( double pt[3], double* p_normal ) {
        pt[0] = pt[0] + 0.1 * p_normal[0];
        pt[1] = pt[1] + 0.1 * p_normal[1];
        pt[2] = pt[2] + 0.1 * p_normal[2];
    }

}

namespace vrcl
{

    vtkSmartPointer<vtkPolyData> VertexList2PolyLine( const std::list<int>&  idx, vtkPolyData* polydata_in )
    {
      double pt[3];

      //create a vtkPoints object and store the points in it
      vtkSmartPointer<vtkPoints> line_points = vtkSmartPointer<vtkPoints>::New();
      int num_verts                          = polydata_in->GetNumberOfPoints( );
      line_points->SetNumberOfPoints( idx.size() );
      cout << " grabbing poly line of length " << idx.size() << " from mesh of # verts " << num_verts << endl;

      int k = 0;
      for( std::list<int>::const_iterator  iter = idx.begin();  iter != idx.end(); ++iter ) {
          int pt_id = (int) (*iter);
          polydata_in->GetPoint( pt_id, pt );
          line_points->InsertPoint( k, pt );
          k++;
      }

      vtkSmartPointer<vtkPolyLine> polyLine = vtkSmartPointer<vtkPolyLine>::New();
      polyLine->GetPointIds()->SetNumberOfIds( idx.size() );
      for(unsigned int i = 0; i < idx.size(); i++) {
        polyLine->GetPointIds()->SetId(i,i);
      }

      //Create a cell array to store the lines in and add the lines to it
      vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
      cells->InsertNextCell(polyLine);

      //Create a polydata to store everything in
      vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();

      //add the points to the dataset
      polyData->SetPoints(line_points);

      //add the lines to the dataset
      polyData->SetLines(cells);

      vtkSmartPointer<vtkPointSource> source = vtkSmartPointer<vtkPointSource>::New();
      // source->SetInput( polyData );
      source->SetNumberOfPoints( idx.size() );
      source->SetRadius(5.0);
      source->Update();

      //Create a polydata to store everything in
      vtkSmartPointer<vtkPolyData> polyDataOut = vtkSmartPointer<vtkPolyData>::New();

      polyDataOut = source->GetOutput();
      polyDataOut->SetPoints( line_points );
      polyDataOut->SetLines( cells );

      return polyDataOut;

  }

    vtkSmartPointer<vtkPolyData> VertexSet2PointCloud( const std::set<int>&  idx, vtkPolyData* polydata_in )
    {
      double pt[3];

      //create a vtkPoints object and store the points in it
      vtkSmartPointer<vtkPoints> line_points = vtkSmartPointer<vtkPoints>::New();
      int num_verts                          = polydata_in->GetNumberOfPoints( );
      line_points->SetNumberOfPoints( idx.size() );
      cout << "point cloud of size " << idx.size() << " from mesh of # verts " << num_verts << endl;

      vtkSmartPointer<vtkPolyDataNormals>  normals = vtkSmartPointer<vtkPolyDataNormals>::New();
      normals->SetInput( polydata_in );
      normals->SetAutoOrientNormals(true);
      normals->SetNonManifoldTraversal(true);
      normals->Update();

      //Create a polydata to store everything in
      vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
      polyData = normals->GetOutput();


      int   k = 0;
      double  p_normal[3]; // to store normal vector
      double* q_normal;    // to store normal vector
      vtkDataArray* normal_vectors = polyData-> GetPointData()->GetNormals();


      for( std::set<int>::const_iterator  iter = idx.begin();  iter != idx.end(); ++iter ) {
          int pt_id = (int) (*iter);
          polyData->GetPoint( pt_id, pt );
          q_normal = normal_vectors->GetTuple3(pt_id);
          shift_by_normal( pt, q_normal );
          line_points->InsertPoint( k, pt );
          k++;
      }


      //add the points to the dataset
      polyData->SetPoints(line_points);

      vtkSmartPointer<vtkPointSource> source = vtkSmartPointer<vtkPointSource>::New();
      // source->SetInput( polyData );
      source->SetNumberOfPoints( idx.size() );
      source->SetRadius(5.0);
      source->Update();

      //Create a polydata to store everything in
      vtkSmartPointer<vtkPolyData> polyDataOut = vtkSmartPointer<vtkPolyData>::New();

      polyDataOut = source->GetOutput();
      polyDataOut->SetPoints( line_points );

      return polyDataOut;

  }

void getClosestMeshIndicesToFloatPoints( const vector<vector<float> >& initPoints3D,
                           vtkPolyData* inputMesh, vtkIntArray* initialPoints ) {
    vtkPoints* verts = inputMesh->GetPoints();
    double thispt[3];
    unsigned int iNumMeshVerts  = verts->GetNumberOfPoints();
    unsigned int iNumSeedPoints = initPoints3D.size();

    for( unsigned int k = 0; k < iNumSeedPoints; k++ ) {
        float xcur = (initPoints3D[k])[0];
        float ycur = (initPoints3D[k])[1];
        float zcur = (initPoints3D[k])[2];
        float fMinDist = 1e20;
        unsigned int iMinIdx = 0;
        for( unsigned int i = 0; i < iNumMeshVerts; i++ ) {
          verts->GetPoint( i, thispt );
          float dist = sqrt( pow(xcur - thispt[0],2) + pow(ycur - thispt[1],2) + pow(zcur - thispt[2],2) );
          if( dist < fMinDist ) {
              fMinDist = dist;
              iMinIdx = i;
          }
        }
        Lout() <<" "<<iMinIdx<<" ...";
        initialPoints->InsertNextValue( iMinIdx );
      }
      LENDL
}


void PrintPolydataArrayInfo( vtkPolyData* outputMesh )
{

    int n_arrays = outputMesh->GetPointData()->GetNumberOfArrays();
    vtkPointData*  point_data = outputMesh->GetPointData();
    int k = 0;
    Lout() << "printing names and min/max of named arrays in 'outputmesh'  " << endl;
    while( k < n_arrays ) {
      vtkFloatArray* farray_k =  vtkFloatArray::SafeDownCast( point_data->GetArray( k ) );
      vtkIntArray* iarray_k =  vtkIntArray::SafeDownCast( point_data->GetArray( k ) );
      k++;
      if( !farray_k && !iarray_k )
        continue;
      if( farray_k ) {
          const char*  ptr_name = farray_k->GetName() ;
          if( !ptr_name )
            continue;
          string k_name( ptr_name );
          cout << "f_array:  " << k_name << ", " ;
          double ptx[2];
          farray_k->GetRange(ptx);
          cout << ptx[0] << "," << ptx[1] << ","  << endl;
      } else if( iarray_k ) {
          const char*  ptr_name = iarray_k->GetName() ;
          if( !ptr_name )
            continue;
          string k_name( ptr_name );
          LOUT "i_array:  " << k_name << ", " ;
          double ptx[2];
          iarray_k->GetRange( ptx );
          LOUT ptx[0] << "," << ptx[1] << ","  LENDL
      }

    }

}


}

