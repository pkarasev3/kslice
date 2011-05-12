#include "meshcurvolver_test_common.h"
#include <cmath>

using std::vector;

vtkPoints* createSyntheticBumps( int Npts )
{
   vtkPoints* points = vtkPoints::New();
    float x,y,z;

    for(int i=0; i<Npts; i++)
    {
        x = vtkMath::Random(-6.0,6.0);
        y = vtkMath::Random(-6.0,6.0);
        z = cos( x*vtkMath::Pi() / 4.0 ) * cos( y*vtkMath::Pi() / 4.0 );
        points->InsertNextPoint(x,y,z);
    }

    return points;
}

vtkPoints* createSyntheticDonut( int Npts )
{
   vtkPoints* points = vtkPoints::New();
    float x,y,z,u,v;
    float R_ = 1.0;
    float r = 0.5;
    for(int i=0; i<Npts; i++)
    {
        u = vtkMath::Random( -vtkMath::Pi(),vtkMath::Pi() );
        v = vtkMath::Random( -vtkMath::Pi(),vtkMath::Pi() );
        double R = R_ + vtkMath::Random( -0.1, 0.1 );
        x = (R + r * cos(v) ) * cos( u );
        y = (R + r * cos(v) ) * sin( u );
        z = r * sin( v );
        points->InsertNextPoint(x,y,z);
    }

    return points;
}

vtkPoints* createSyntheticStarBlob( int Npts )
{
      vtkPoints* points = vtkPoints::New();
    float x,y,z,u,v;
    double R_ = 2.0;
    double r  = 1.0;
    for(int i=0; i<Npts; i++)
    {
        u = vtkMath::Random( -vtkMath::Pi(),vtkMath::Pi() );
        v = vtkMath::Random( -vtkMath::Pi(),vtkMath::Pi() );
        double R = R_ + vtkMath::Random( -0.1, 0.1 );
        x = (R + r * cos(v) ) * cos( u );
        y = (R + r * cos(v) ) * sin( u );
        z = r * sin( v );
        points->InsertNextPoint(x,y,z);
    }

    return points;
}



void getSyntheticSeedPoints( vector< vector<float> >&  pts3D ) {
    pts3D.clear();
    vector<float> p1(3); 	vector<float> p2(3);
    vector<float> p3(3);	vector<float> p4(3);
    p1[0] = 1;  p1[1] = 1;  p1[2] = 0;
    p2[0] = 1;  p2[1] = 20; p2[2] = 0;
    p3[0] = 20; p3[1] = 20;  p3[2] = 0;
    p4[0] = 20; p4[1] = 1; p4[2] = 0;
    pts3D.push_back( p1 );
    pts3D.push_back( p2 );
    pts3D.push_back( p3 );
    pts3D.push_back( p4 );
}

void getSyntheticSeedPoints2( vector< vector<float> >&  pts3D ) {
    pts3D.clear();
    vector<float> p1(3); 	vector<float> p2(3);
    vector<float> p3(3);	vector<float> p4(3);
    p1[0] = 1;  p1[1] = 1;  p1[2] = 0;
    p2[0] = 1;  p2[1] = 5;  p2[2] = 10;
    p3[0] = 5; p3[1] =  10; p3[2] = 10;
    p4[0] = 10; p4[1] = 20; p4[2] = 0;
    pts3D.push_back( p2 );
    pts3D.push_back( p1 );
    pts3D.push_back( p3 );
    pts3D.push_back( p4 );
}
