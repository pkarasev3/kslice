#ifndef LEVELSETCURVE
#define LEVELSETCURVE

#include "ImageEnergy.h"
#include "MeanEnergy.h"
#include "EdgeContrastEnergy.h"

struct AdjData {
	int myIdx;
	vector<int> myNeighbs;
};

struct MeshData {
	vector<AdjData> adjimm;
};


class LevelSetCurve
{
public:
	LevelSetCurve( const cv::Mat& img, const cv::Mat& phi_init, const std::string& windowname );
	~LevelSetCurve( );
	double EvolveSparseField( int iterations, IplImage* output = NULL );
	double EvolveNumeric( int iterations, IplImage* output = NULL );
  void getPhi( cv::Mat& dst );
  
private:
	ImageEnergy* energy;
	IplImage* myimg; // shared with the energy? eep.
	CvMat* phi; // the levelset function
	vector<int> labels; // labels of which layer a point is in
	MeshData* meshdata; // data structure for looking up neighbors
	MeshData* GetMeshData(CvMat* input);
	string windowname;
	void ClearLists();
	void SelfUnion( vector<int>& vec );
	void DropIdx( const vector<int>& L_zp, const vector<int>& L_zn, vector<int>& L_z );
	void AppendIdx( const vector<int>& src, vector<int>& dst );
	
	vector<int>* neigh;
	vector<int> L0;
	vector<int> Lp1;
	vector<int> Ln1;
	vector<int> Lp2;
	vector<int> Ln2;


};


#endif
