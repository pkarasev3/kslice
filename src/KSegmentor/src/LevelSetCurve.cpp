#include "LevelSetCurve.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/legacy/compat.hpp>
#include <iostream>

using namespace cv;
using std::vector;
using std::valarray;
using std::cout;
using std::endl;
using std::string;

LevelSetCurve::LevelSetCurve(  const Mat& img, const Mat& phi_init, 
                               const string& windowname )
{
  vector<int> params(0);
  IplImage* img_ = &( IplImage(img));
  CvMat*    phi_ = &( CvMat(phi_init) );
  this->energy = new MeanEnergy(img_, params );
  //this->energy = new EdgeContrastEnergy(img, params );
  
	this->phi = cvCloneMat( phi_ ); 
	this->myimg = cvCloneImage( img_ );
	meshdata = GetMeshData( phi );
	this->windowname = windowname;
	labels = vector<int>( phi->height * phi->width );
}

void LevelSetCurve::getPhi( cv::Mat& dst )
{
  dst = cv::Mat( cvCloneMat( phi ) );
  
}

LevelSetCurve::~LevelSetCurve( )
{
	cvReleaseMat( &phi );
	cvReleaseImage( &myimg );
	delete meshdata;
}

MeshData* LevelSetCurve::GetMeshData( CvMat* mat ) {
	MeshData* data = new MeshData();
	int w = mat->width;
	int h = mat->height;
	data->adjimm = vector<AdjData>(mat->height * mat->width);
	int row,col;
	int k = 0;
	for( int i = 0; i < h; i++ ) {
		for( int j = 0; j < w; j++ ) {
			row = i;
			col = j;
			int up = row-1;
			int down = row+1;
			int left = col-1;
			int right = col+1;

			if( up >= 0 ) {
				data->adjimm[k].myNeighbs.push_back(k-w);
			}
			if( down < h ) {
				data->adjimm[k].myNeighbs.push_back(k+w);
			}
			if( left >= 0 ) {
				data->adjimm[k].myNeighbs.push_back(k-1);
			}
			if( right < w ) {
				data->adjimm[k].myNeighbs.push_back(k+1);
			}
			k++;
		}
		
	}
	return data;

}

void LevelSetCurve::ClearLists() {
	L0 =vector<int>(0); 
	Lp1=vector<int>(0); 
	Ln1=vector<int>(0);
	Lp2=vector<int>(0); 
	Ln2=vector<int>(0); 
}

double LevelSetCurve::EvolveSparseField( int iterations, IplImage* output )
{
	double delta = 0.0;
	// the big function.
	// use 'sparse field' evolution as in Whitaker et al 1998

	// step 1. build L0, the zero levelset.
	// definition: point is on L0 if it is positive but has 
	// at least one negative neigbhor
	CvMat* placed = cvCloneMat( phi );
	cvZero(placed);
	int w = output->width;
	int h = output->height;

	ClearLists();
	
	for( int i =0; i < w*h; i++ ) {
		if( phi->data.fl[i] < 0 )
			labels[i] = -3;
		else
			labels[i] = 3;
	}
	cvCopy( myimg, output );

	int numel = this->meshdata->adjimm.size();
	for( int i = 0; i < numel; i++ ) {
		if( phi->data.fl[i] < 0 )
			continue;
		bool bHasNegNeigh = false;
		neigh = &(meshdata->adjimm[i].myNeighbs);
		int numneigh = neigh->size();
		for( int j = 0; j < numneigh; j++ ) {
			if( phi->data.fl[ (*neigh)[j] ] < 0 )
				bHasNegNeigh = true;
		}
		if( bHasNegNeigh ) {
			L0.push_back(i);
			int col = i % output->width;
			int row = i / output->width;
			cvSet2D( output, row, col, cvScalar(0) );
			placed->data.fl[i] = 1.0;
			phi->data.fl[i] = 0.001;
			labels[i] = 0;
		}

	}

	// step 2. build Lp1, Ln1 the one-away layers from L0
	for( int i = 0; i < L0.size(); i++ ) {
		int idx = L0[i];
		neigh = &(meshdata->adjimm[idx].myNeighbs);
		int numneigh = neigh->size();
		for( int j = 0; j < numneigh; j++ ) {
			int neighidx = (*neigh)[j];
			if( phi->data.fl[ neighidx ] < 0 && placed->data.fl[neighidx] < 0.5 ) {
				Ln1.push_back( neighidx);
				placed->data.fl[neighidx] = 1.0;
				int col = neighidx % output->width;
				int row = neighidx / output->width;
				cvSet2D( output, row, col, cvScalar(255) );
				phi->data.fl[neighidx] = -1.0;
				labels[i] = -1;
			}
			if( phi->data.fl[ neighidx ] >= 0 && placed->data.fl[neighidx] < 0.5 ) {
				Lp1.push_back( neighidx );
				placed->data.fl[neighidx] = 1.0;
				int col = neighidx % output->width;
				int row = neighidx / output->width;
				cvSet2D( output, row, col, cvScalar(255) );
				phi->data.fl[neighidx] = 1.0;
				labels[i] = 1;
			}
		}
	}

	// step 3. build Lp2, Ln2 the two-away layers from L0
	for( int i = 0; i < Lp1.size(); i++ ) {
		int idx = Lp1[i];
		neigh = &(meshdata->adjimm[idx].myNeighbs);
		int numneigh = neigh->size();
		for( int j = 0; j < numneigh; j++ ) {
			int neighidx = (*neigh)[j];
			if( phi->data.fl[ neighidx ] > 0 && placed->data.fl[neighidx] < 0.5 ) {
				Lp2.push_back( neighidx);
				placed->data.fl[neighidx] = 1.0;
				int col = neighidx % output->width;
				int row = neighidx / output->width;
				cvSet2D( output, row, col, cvScalar(100) );
				phi->data.fl[neighidx] = 2.0;
				labels[i] = 2;
			}
			/*if( phi->data.fl[ neighidx ] < 0 ) {
				int impossible = 1;
			}*/
		}
	}
	for( int i = 0; i < Ln1.size(); i++ ) {
		int idx = Ln1[i];
		neigh = &(meshdata->adjimm[idx].myNeighbs);
		int numneigh = neigh->size();
		for( int j = 0; j < numneigh; j++ ) {
			int neighidx = (*neigh)[j];
			if( phi->data.fl[ neighidx ] < 0 && placed->data.fl[neighidx] < 0.5 ) {
				Ln2.push_back( neighidx);
				placed->data.fl[neighidx] = 1.0;
				int col = neighidx % output->width;
				int row = neighidx / output->width;
				cvSet2D( output, row, col, cvScalar(100) );
				phi->data.fl[neighidx] = -2.0;
				labels[i] = -2;
			}
			/*if( phi->data.fl[ neighidx ] > 0 ) {
				int impossible = 1;
			}*/
		}
	}

	for( int i = 0; i < w*h; i++ ) {
		int col = i % w;
		int row = i / w;
		if( phi->data.fl[i] > 0 && placed->data.fl[i] < 0.5 ) {
			phi->data.fl[i] = 3.0;
			placed->data.fl[i] = 1.0;
			cvSet2D( output, row, col, cvScalar(255) );
		}
		else if ( phi->data.fl[i] < 0 && placed->data.fl[i] < 0.5 ) {
			phi->data.fl[i] = -3.0;
			placed->data.fl[i] = 1.0;
			cvSet2D( output, row, col, cvScalar(0) );
		}
	}

	vector<int> S0(0); // entering zero LS
	vector<int> Sp(0); // entering +1 level
	vector<int> Sn(0); // entering -1 level
	vector<int> Sp1(0); // entering +2 level
	vector<int> Sn1(0); // entering -2 level
	vector<int> Sp2(0); // to be dropped entirely
	vector<int> Sn2(0); // to be dropped entirely
	valarray<double> vals(4);

	double prev_energy = 0.0;
        std::stringstream ss;
	for( int ii = 0; ii < iterations; ii++ ) {
    
                ss << ii << " .. ";
    
		S0.clear();
		Sp.clear(); 
		Sn.clear(); 
		Sp1.clear();
		Sn1.clear();
		Sp2.clear();
		Sn2.clear();

				
		double current_energy = this->energy->Evaluate( L0, phi );
		double delta = abs( current_energy - prev_energy );
		prev_energy = current_energy;
		
                /*if( delta < 1000 && ii > 100 )
			return delta;*/

		// 1. for each active grid point x_i:
		// a. compute local geometry of level set
		// b. compute net change of u_x from internal and external forces
		// (encapsulated in getforce() function )

		

		valarray<double> F = energy->GetSpeed( L0, phi );
		
		double dDirection = 1.0;
		double cfl = 0.35;
		for( int i = 0; i < F.size(); i++ ) phi->data.fl[ L0[i] ] += dDirection*cfl * F[i];
		
		// 2. find where zero layer exceeds [-0.5,0.5]
		
		for( int i = 0; i < L0.size(); i++ ) {
			int idx = L0[i];
			if( phi->data.fl[idx] >= 0.5 )
				Sp.push_back( idx );
			if( phi->data.fl[idx] <= -0.5 )
				Sn.push_back( idx );
		}



		// 3. visit grid points in layers L_p1, L_n1 and update grid point
		// values by adding/subtracting one unit from next inner layer.
		// if multiple neighbors, use the closest one:
		
		

		for( int i = 0; i < Lp1.size(); i++ ) {
			int idx = Lp1[i];
			neigh = &(meshdata->adjimm[ idx ].myNeighbs);
			//valarray<double> vals(neigh->size()); // get neighbors of this point in Lp1
			for( int k = 0; k < neigh->size(); k++ ) { 
				vals[k] = phi->data.fl[ (*neigh)[k] ];
			}
			for( int k = neigh->size(); k < vals.size(); k++ ) {
				vals[k] = vals[k-1];
			}
			bool bHasL0neigh = false;
			for( int k = 0; k < neigh->size(); k++ ) {
				int neighidx = (*neigh)[k]; // if this is not in L0 don't use it for the max()
				if( labels[neighidx] == 0 ) // found it in L0
					bHasL0neigh = true;
			}
			if( !bHasL0neigh ) { // i have no zero neighbors, // therefore i am no longer eligble for Lp1 membership
				Sp1.push_back( idx );
			}
			else {
				for( int k = 0; k < neigh->size(); k++ ) {
					int neighidx = (*neigh)[k]; // if this is not in L0 don't use it for the max()
					if( labels[neighidx] != 0 )
						vals[k] = 1.0;
				}
				double oldval = phi->data.fl[idx];
				double newval = vals.min() + 1.0;
				phi->data.fl[idx] = newval;
				// if this new value is outside allowed range, move it to appropriate S layer
				if( newval <= 0.5 )
					S0.push_back(idx);
				else if( newval > 1.5 )
					Sp1.push_back(idx);
			}		
		}
		for( int i = 0; i < Ln1.size(); i++ ) {
			int idx = Ln1[i];
			neigh = &(meshdata->adjimm[ idx ].myNeighbs);
			//valarray<double> vals(neigh->size());
			for( int k = 0; k < neigh->size(); k++ ) { 
				vals[k] = phi->data.fl[ (*neigh)[k] ];
			}
			for( int k = neigh->size(); k < vals.size(); k++ ) {
				vals[k] = vals[k-1];
			}
			bool bHasL0neigh = false;
			for( int k = 0; k < neigh->size(); k++ ) {
				int neighidx = (*neigh)[k]; // if this is not in L0 don't use it for the max()
				if( labels[neighidx] == 0 ) // found it in L0
					bHasL0neigh = true;
			}
			if( !bHasL0neigh ) { // i have no zero neighbors, 
				// therefore i am no longer eligble for Ln1 membership
				Sn1.push_back( idx );
			}
			else {
				for( int k = 0; k < neigh->size(); k++ ) {
					int neighidx = (*neigh)[k]; // if this is not in L0 don't use it for the max()
					if( labels[neighidx] != 0 ) // found it in L0
						vals[k] = -1.0;
				}
				double oldval = phi->data.fl[idx];
				double newval = vals.max() - 1.0;
				phi->data.fl[idx] = newval;
				// if this new value is outside allowed range, move it to appropriate S layer
				if( newval >= -0.5 )
					S0.push_back(idx);
				else if( newval < -1.5 )
					Sn1.push_back(idx);

			}
		}

		
		for( int i = 0; i < Lp2.size(); i++ ) {
			int idx = Lp2[i];
			neigh = &(meshdata->adjimm[ idx ].myNeighbs);
			//valarray<double> vals(neigh->size()); // get neighbors of this point in Lp1
			
			for( int k = 0; k < neigh->size(); k++ ) { 
				vals[k] = phi->data.fl[ (*neigh)[k] ];
			}
			for( int k = neigh->size(); k < vals.size(); k++ ) {
				vals[k] = vals[k-1];
			}
			bool bHasLp1neigh = false;
			for( int k = 0; k < neigh->size(); k++ ) {
				int neighidx = (*neigh)[k]; // if this is not in L0 don't use it for the max()
				if( labels[neighidx] == 1 ) // found it in L0
					bHasLp1neigh = true;
			}
			if( !bHasLp1neigh ) { // i have no Lp1 neighbors therefore i am no longer eligble for Lp2 membership
				Sp2.push_back( idx );
			}
			else {
				for( int k = 0; k < neigh->size(); k++ ) {
					int neighidx = (*neigh)[k]; // 
					if( labels[neighidx] != 1 ) 
						vals[k] = 5.0;
				}
				double oldval = phi->data.fl[idx];
				double newval = vals.min() + 1.0;
				phi->data.fl[idx] = newval;
				// if this new value is outside allowed range, move it to appropriate S layer
				if( newval <= 1.5 )
					Sp.push_back(idx);
				else if( newval > 2.5 )
					Sp2.push_back(idx);
			}		
		}
		for( int i = 0; i < Ln2.size(); i++ ) {
			int idx = Ln2[i];
			neigh = &(meshdata->adjimm[ idx ].myNeighbs);
			//valarray<double> vals(neigh->size());
			for( int k = 0; k < neigh->size(); k++ ) { 
				vals[k] = phi->data.fl[ (*neigh)[k] ];
			}
			for( int k = neigh->size(); k < vals.size(); k++ ) {
				vals[k] = vals[k-1];
			}
			bool bHasLn1neigh = false;
			for( int k = 0; k < neigh->size(); k++ ) {
				int neighidx = (*neigh)[k]; // if this is not in Ln1 don't use it for the max()
				if( labels[neighidx] == -1 ) 
					bHasLn1neigh = true; // found it in Ln1
					
			}
			if( !bHasLn1neigh ) { // i have no Ln1 neighbors therefore i am no longer eligble for Ln2 membership
				Sn2.push_back( idx );
			}
			else {
				for( int k = 0; k < neigh->size(); k++ ) {
					int neighidx = (*neigh)[k]; // if this is not in Ln1 don't use it for the max()
					if( labels[neighidx] != -1 ) // did not find it in Ln1
						vals[k] = -5.0;
				}
				double oldval = phi->data.fl[idx];
				double newval = vals.max() - 1.0;
				phi->data.fl[idx] = newval;
				if( newval >= -1.5 )
					Sn.push_back(idx);
				else if( newval < -2.5 )
					Sn2.push_back(idx);
			}
		}
		
		// erase from L0 the points that just got stored
		
		DropIdx( Sp, Sn, L0 );
		AppendIdx( S0, L0);

		DropIdx( Sp, Sp2, Lp2 );
		AppendIdx( Sp1, Lp2 );
		DropIdx( Sn, Sn2, Ln2 );
		AppendIdx( Sn1, Ln2 );

		DropIdx( S0, Sp1, Lp1 );
		AppendIdx( Sp, Lp1 );
		DropIdx( S0, Sn1, Ln1 );
		AppendIdx( Sn, Ln1 );

			// update labels
		for( int i = 0; i < L0.size(); i++ ) {
			labels[ L0[i] ] = 0;
		}
		for( int i = 0; i < Lp1.size(); i++ ) {
			labels[ Lp1[i] ] = 1;
		}
		for( int i = 0; i < Ln1.size(); i++ ) {
			labels[ Ln1[i] ] = -1;
		}
		for( int i = 0; i < Lp2.size(); i++ ) {
			labels[ Lp2[i] ] = 2;
		}
		for( int i = 0; i < Ln2.size(); i++ ) {
			labels[ Ln2[i] ] = -2;
		}
		for( int i = 0; i < Sp2.size(); i++ ) {
			labels[ Sp2[i] ] = 3;
		}
		for( int i = 0; i < Sn2.size(); i++ ) {
			labels[ Sn2[i] ] = -3;
		}

		// grab new points for outermost layers from neighbors of Lp1, Ln1
		for( int i = 0; i < Lp1.size(); i++ ) {
			int idx = Lp1[i];
			neigh = &(meshdata->adjimm[idx].myNeighbs);
			int numneigh = neigh->size();
			for( int j = 0; j < numneigh; j++ ) {
				int neighidx = (*neigh)[j]; // if its not in Lp1 or L0 grab it!
				if( labels[neighidx] == 3 ) {
					Lp2.push_back( neighidx);
					labels[neighidx] = 2;
					//double val = phi->data.fl[idx]+1;
					//phi->data.fl[ neighidx ] = val;
				}
			}
		}
		for( int i = 0; i < Ln1.size(); i++ ) {
			int idx = Ln1[i];
			neigh = &(meshdata->adjimm[idx].myNeighbs);
			int numneigh = neigh->size();
			for( int j = 0; j < numneigh; j++ ) {
				int neighidx = (*neigh)[j]; // if its not in Ln1 or L0 grab it!
				if( labels[neighidx] == -3 ) {
					Ln2.push_back( neighidx);
					//double val = phi->data.fl[idx]-1;
					//phi->data.fl[ neighidx ] = val;
					labels[neighidx] = -2;
				}
			}
		}

		// discard redundant verts
		SelfUnion( Ln2 );
		SelfUnion( Lp2 );
		SelfUnion( Ln1 );
		SelfUnion( Lp1 );
		SelfUnion( L0 );

		if( !output ) // don't make an output for intermediate display
		{
		}
		else
		{
      int multidisp = 1;
      cvCopy( myimg, output );
      for( int i = 0; i < L0.size(); i++ ) {
        int idx = L0[i];
        cvSet2D( output, idx / w, idx%w, cvScalar(255) );
      }
      if (multidisp) {
        for( int i = 0; i < Lp1.size(); i++ ) {
          int idx = Lp1[i];
          cvSet2D( output, idx / w, idx%w, cvScalar(0) );
        }
        for( int i = 0; i < Ln1.size(); i++ ) {
          int idx = Ln1[i];
          cvSet2D( output, idx / w, idx%w, cvScalar(0) );
        }
        for( int i = 0; i < Lp2.size(); i++ ) {
          int idx = Lp2[i];
          cvSet2D( output, idx / w, idx%w, cvScalar(255) );
        }
        for( int i = 0; i < Ln2.size(); i++ ) {
          int idx = Ln2[i];
          cvSet2D( output, idx / w, idx%w, cvScalar(255) );
        }
      }
//      cvShowImage( windowname.c_str(), output );
//      int c  =  cv::waitKey(5);
		}
	
	}
 
	cvReleaseMat( &placed );
	return delta;
}

void LevelSetCurve::SelfUnion( vector<int>& vec )
{
	sort( vec.begin(), vec.end() );
	vec.erase( unique(vec.begin(), vec.end()),vec.end() );
	int breakhere = 1;
}

void LevelSetCurve::AppendIdx( const vector<int>& src, vector<int>& dst ) {
	for( int i = 0; i < src.size(); i++ )
		dst.push_back( src[i] );
	SelfUnion( dst );
}

void LevelSetCurve::DropIdx( const vector<int>& L_zp, const vector<int>& L_zn, vector<int>& L_z )
{
	vector<int> L_z_(0);
	for( int i = 0; i < L_z.size(); i++ ) {
		int num1 = count( L_zp.begin(), L_zp.end(), L_z[i] );
		int num2 = count( L_zn.begin(), L_zn.end(), L_z[i] );
		if( num1 + num2 == 0 )
			L_z_.push_back( L_z[i] );
	}
	L_z = L_z_;

}


