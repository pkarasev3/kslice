#include "KSegmentor3D.h"
#include "llist.h"
//#include "sfm_local_chanvese_mex.h"
#include "interactive_kurvolver.h"
#include "vtkImageData.h"
//#ifndef WIN32
//	#include <omp.h>
//    #include <initializer_list>
//#endif
#include <string>
#include <sstream>
#include "vtkImageGaussianSmooth.h"
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <assert.h>


using std::string;

KSegmentor3D::KSegmentor3D(vtkImageData* image, vtkImageData* label, vtkImageData* UIVol,
                           bool contInit, int currSlice, int numIts, float distWeight, double lambdaPenalty, int brushRad, int currLabel, double *imgSpacing)
{
  m_EnergyName = GetSupportedEnergyNames()[1];
  this->InitializeVariables(image,label, UIVol, contInit, currSlice, numIts, distWeight, lambdaPenalty, brushRad, currLabel, imgSpacing);

// TODO: re-integrate this, check that label value is handled correctly from slicer
// "set U based on input label", e.g. when input is large complex label that user wants to edit slightly
//  if(contInit) {
//    std::cout<<"Initializing user input using label data"<<std::endl;
//    this->initializeUserInputImageWithContour();
//  }

  this->CreateLLs(this->LL3D);
  this->CreateLLs(this->LL2D);
  //LL* Lztmp = this->LL3D.Lz;
  //assert(Lztmp != NULL);

  this->segEngine=new energy3c(this->rad); //initialized once, radius does not get to change

  //initialize pointers 2D
  this->imgSlice    = NULL;
  this->maskSlice   = NULL;
  this->U_I_slice   = NULL;
  this->labelSlice  = NULL;
  this->phiSlice    = NULL;
  this->m_IJK_orient="IJ";
  this->prevSlice   =-1;
  this->currSlice   =-1;
  this->prevMode    = "None"; // have not done 2D or 3D
  this->last2DOrient= "None"; // null init for orientation
  this->firstPassInit = true; // have not done initializeData() yet

}

//void KSegmentor3D::accumulateCurrentUserInput( double value,const unsigned int element,
//                                               double weight /*=1.0 default */)
//{
//  double Umax  = 1.0;
//  Umax         = this->GetUmax(); assert(Umax>0);
//  double Ustep = weight * (Umax)/2.0;
//  if( fabs(Ustep) < 0.01 ) {
//    /*cout <<"whoa something is *messed up*, check Umax " << endl;*/ assert(1); }

//  double user_input      = -Ustep * ( value > 0.5 ) +
//      Ustep * ( value <= 0.5 );

//  this->ptrU_t_Image[element] = user_input;

//  if( ptrIntegral_Image[element] < -Umax ) {
//    ptrIntegral_Image[element] = -Umax;
//  } else if( ptrIntegral_Image[element] > Umax ) {
//    ptrIntegral_Image[element] = Umax;
//  }

//  this->OnUserPaintsLabel(); // Ivan: consider OnUserPaintsLabel the "on label changed" entry point
//}

//void KSegmentor3D::integrateUserInput()
//{
//  ptrIntegral_Image = static_cast<double*>(this->U_Integral_image->GetScalarPointer());
//  ptrU_t_Image      = static_cast<double*>(this->U_t_image->GetScalarPointer());

//  int pos=0;
//  int Nelements=this->m_UpdateVector.size();
//  cout << " Integrating:  KSegmentor3D::integrateUserInput(), N = "
//       << Nelements << endl;
//  for (int element=0;element<Nelements;element++)
//  {
//    pos=this->m_UpdateVector[element];
//    this->ptrIntegral_Image[pos] += this->ptrU_t_Image[pos];
//    this->ptrU_t_Image[pos]       = 0;
//  }
//}


void KSegmentor3D::initializeData()
{
  currSlice=0;
  prevSlice=-1; //this will never be true

  if( firstPassInit ) {
      std::cout<<"Initializing data in c++"<<std::endl;
      //imageVol->GetScalarRange( imgRange );
      //labelVol->GetScalarRange( labelRange );
      //if( abs(labelRange[1]) < 1e-3 )
      //{ // empty label; so set the proper range
      //  labelRange[1] = this->currLabel;
      //}
      //this->imgRange=imgRange;
      int imgType=imageVol->GetScalarType();
      vrcl::convertImage( imgType,imageVol->GetScalarPointer(),img, dimx, dimy, dimz);
      firstPassInit = false;
  } else {
      // callgrind claims that this range calc takes a big chunk o time
  }

  int labType=labelVol->GetScalarType();
  vrcl::convertLabel(labType,labelVol->GetScalarPointer(), mask, dimx, dimy, dimz);

}


void KSegmentor3D::OnUserPaintsLabel() {
  this->prevSlice = -1;
}

// function template convertDoubleSlice definition

namespace {
class UeffectException : public std::exception
{
public:
    const char* what() {
        return "User input caused unexpected behavior internally!";
    }
};
double UmaxConst = 5.0;
std::vector<double> cached_phi;

bool check_U_behavior(const std::vector<float>& phi0, float* phi1, short* U )
{    /** ensure that phi does not change sign when
              U is "sufficiently large" */
    bool isGood = true;
    int sz=phi0.size();
    double Umin = 1e99; double Umax = -1e99;
    cout <<  "\033[01;45m\033]" << "checking U effect... " << "\033[00m\033]" << endl;
    for(int i=0;i<sz;i++) {
        double Uij = U[i];
        if(Uij<Umin) {Umin = Uij;}
        if(Uij>Umax) {Umax = Uij;}
        double phi0_ij = phi0[i];
        double phi1_ij = phi1[i];
        bool changedSign = false;
        if( fabs(Uij) >= UmaxConst ) {
            changedSign = ( phi0_ij * phi1_ij < 0 ) &&
                    ( (fabs(phi0_ij)>1e-6)&&(fabs(phi1_ij)>1e-6) );
            if( changedSign ) {
                isGood = false;
                break;
            }
        }
    }
    cout << "Umin=" << Umin << ", Umax = " << Umax << endl;
    return isGood;
}


}

void KSegmentor3D::Update2D(bool reInitFromMask)
{
    //set the orientation
    int dim0=0;
    int dim1=0;
    int dim2=0;

    int radShuffle[3]; //will be use to store re-shuffled radii, according to orientation

    vrcl::Orient sliceView = vrcl::SLICE_IJ;
    if( m_IJK_orient == "IJ" ) {
        dim0 = mdims[0];
        dim1 = mdims[1];
        dim2 = mdims[2];
        radShuffle[0]=this->segEngine->GetRadius()[0];
        radShuffle[1]=this->segEngine->GetRadius()[1];
        radShuffle[2]=this->segEngine->GetRadius()[2];
        sliceView = vrcl::SLICE_IJ;
    }else if( m_IJK_orient == "JK" ) {
        dim0 = mdims[1];
        dim1 = mdims[2];
        dim2 = mdims[0];
        radShuffle[0]=this->segEngine->GetRadius()[1];
        radShuffle[1]=this->segEngine->GetRadius()[2];
        radShuffle[2]=this->segEngine->GetRadius()[0];
        sliceView = vrcl::SLICE_JK;
    }else if( m_IJK_orient == "IK" ) {
        dim0 = mdims[0];
        dim1 = mdims[2];
        dim2 = mdims[1];
        radShuffle[0]=this->segEngine->GetRadius()[0];
        radShuffle[1]=this->segEngine->GetRadius()[2];
        radShuffle[2]=this->segEngine->GetRadius()[1];
        sliceView = vrcl::SLICE_IK;
    }
    std::cout<<"orientation is: "<<m_IJK_orient<<std::endl;

    std::vector<long> dimsSlice(5); //this is never used, should be removed
    dimsSlice[0] = dim0;
    dimsSlice[1] = dim1;
    dimsSlice[2] = 1;
    dimsSlice[3] = dimsSlice[0]*dimsSlice[1];
    dimsSlice[4] = dimsSlice[0]*dimsSlice[1]*dimsSlice[2];

    cout << "orient=" << m_IJK_orient << ", prevslice=" << prevSlice << ", " << "currslice= " << currSlice << endl;
    bool sameSlice=(prevSlice == this->currSlice);

    if( sameSlice==1 && !reInitFromMask ) {
        cout <<  "\033[01;32m\033]" << "using cached phi " << "\033[00m\033]" << endl;
        ll_init(LL2D.Lz);
        ll_init(LL2D.Ln1);
        ll_init(LL2D.Ln2);
        ll_init(LL2D.Lp1);
        ll_init(LL2D.Lp2);
    }else {
        //delete in case anything is left over
        delete [] this->imgSlice;
        delete [] this->labelSlice;
        delete [] this->maskSlice;
        delete [] this->U_I_slice;
        delete [] this->phiSlice;

        //allocate new slices, with correct dimensions
        this->imgSlice          = new double[ dim0 * dim1 ];
        this->maskSlice         = new short[ dim0 * dim1 ];
        this->U_I_slice         = new short[ dim0 * dim1 ];
        this->labelSlice        = new short[ dim0 * dim1 ];
        this->phiSlice          = new float[ dim0 * dim1 ];

        //copy images based on type
        int imgType=imageVol->GetScalarType();
        vrcl::convertSliceToDouble(imgType, imageVol->GetScalarPointer(), imgSlice  , dim0, dim1, dim2, currSlice, sliceView );

        //copy labels based on type
        int labType=labelVol->GetScalarType();
        vrcl::convertSliceToShort(labType, labelVol->GetScalarPointer(), maskSlice  , dim0, dim1, dim2, currSlice, sliceView );

        //set up the new level set
        this->CreateLLs(LL2D);
        ls_mask2phi3c(maskSlice,phiSlice,labelSlice,&(dimsSlice[0]), LL2D.Lz,LL2D.Ln1,LL2D.Ln2,LL2D.Lp1,LL2D.Lp2);
        cout <<  "\033[01;42m\033]"<< "2Dfirst time on slice! " << "\033[00m\033]" << endl;
    }
    prevSlice=currSlice;

    /** moved this out of "is cache" portion; always reslice U to maintain sync, whew what a bug-hunt (This does not make sense)
    //user input is always double */
    int uiType=U_Integral_image->GetScalarType();
    vrcl::convertSliceToShort(uiType, U_Integral_image->GetScalarPointer(), this->U_I_slice  , dim0, dim1, dim2, currSlice, sliceView );


    // save the phi before levelset, to verify expected behavior
    cache_phi.resize(dim0*dim1);
    std::memcpy( &(cache_phi[0]),&(phiSlice[0]),sizeof(float)*dim0 * dim1 );

    //run the active contour, ** modify phiSlice in-place! (IVAN: yes?) **
    // Why does the U_I_slice keep seeming like zeros when clicking??

    bool reInit;
    if(prevMode=="2D" && last2DOrient==m_IJK_orient && sameSlice==1){
        reInit=0;
    }else{
        reInit=1;
        prevMode="2D";   //keep track for next call
        last2DOrient=m_IJK_orient;
    }

    std::cout<<"running "<<iter<<" iterations"<<std::endl;

    interactive_rbchanvese(segEngine, imgSlice,phiSlice,U_I_slice,labelSlice,&(dimsSlice[0]),
                           LL2D.Lz,LL2D.Ln1,LL2D.Lp1,LL2D.Ln2,LL2D.Lp2,LL2D.Lin2out,LL2D.Lout2in,
                           iter,lambda*0.5,display, reInit, radShuffle);

    bool isGood = check_U_behavior(cache_phi, phiSlice, U_I_slice);
    if( !isGood ) { std::cout << "BAD!" << std::endl; }

    //threshold phi, set label to appropriate values
    int labType=labelVol->GetScalarType();
    vrcl::convertFloatToSlice( labType, labelVol->GetScalarPointer(), phiSlice, dim0, dim1, dim2, currSlice, sliceView, currLabel );

    //clean up, but is m_UpdateVector, m_CoordinatesVector necessary??
    cout <<  "Lz size: "       << LL2D.Lz->length << endl;

    //whats the point of these two variable? awesomeness (please clarify)
    m_UpdateVector.clear(); // should hook this up for user input accum
    m_CoordinatesVector.clear();
}

//#ifndef WIN32
#if 0 
void KSegmentor3D::CalcViewPlaneParams( )
{
    /** should this use physical spacing instead of slice steps? */
    if( m_IJK_orient == "IJ" ) {
        m_PlaneNormalVector = {0.0,0.0,1.0};
        m_PlaneCenter       = {0.0,0.0,(double)currSlice};
    }else if( m_IJK_orient == "JK" ) {
        m_PlaneNormalVector = {1.0,0.0,0.0};
        m_PlaneCenter       = {(double)currSlice,0.0,0.0};
    }else if( m_IJK_orient == "IK" ) {
        m_PlaneNormalVector = {0.0,1.0,0.0};
        m_PlaneCenter       = {0.0,(double)currSlice,0.0};
    }else {
        cout << "Bad, invalid orientation!?" << endl;
    }
}
#else
void KSegmentor3D::CalcViewPlaneParams( )
{
    /** should this use physical spacing instead of slice steps? */
	m_PlaneNormalVector = std::vector<double>(3,0.0);
	m_PlaneCenter       = std::vector<double>(3,0.0);
    if( m_IJK_orient == "IJ" ) {
        m_PlaneNormalVector[2]=1.0; // = {0.0,0.0,1.0};
        m_PlaneCenter[2]      =(double)currSlice;
    }else if( m_IJK_orient == "JK" ) {
        m_PlaneNormalVector[0]=1.0;
        m_PlaneCenter[0]=(double)currSlice;
    }else if( m_IJK_orient == "IK" ) {
        m_PlaneNormalVector[1]=1.0;
        m_PlaneCenter[1]=(double)currSlice;
    }else {
        cout << "Bad, invalid orientation!?" << endl;
    }
}
#endif

void KSegmentor3D::Update3D(bool reInitFromMask)
{

    if( !reInitFromMask ) {// do this only if re-making the level set function
        cout <<  "\033[01;33m\033]" << "3D, using cached phi " << "\033[00m\033]" << endl;
        ll_init(LL3D.Lz);
        ll_init(LL3D.Ln1);
        ll_init(LL3D.Ln2);
        ll_init(LL3D.Lp1);
        ll_init(LL3D.Lp2);
        ll_init(LL3D.Lout2in); //ensure that Lout2in, Lin2out dont need to be intialized!!
        ll_init(LL3D.Lin2out);
    }else{
        this->initializeData();
        this->CreateLLs(LL3D);
        ls_mask2phi3c(mask,phi,label,dims,LL3D.Lz,LL3D.Ln1,LL3D.Ln2,LL3D.Lp1,LL3D.Lp2);
    }

    //supposedly, U_Integral_image->GetScalarRange(Urange) is a time consuming operation
    //double Urange[2]; // check range; is it getting set right from python?
    //U_Integral_image->GetScalarRange(Urange);
    //std::cout<< "Update3D:  Umin=" << Urange[0] << ", Umax=" << Urange[1] << std::endl;

    ptrIntegral_Image = static_cast<short*>( U_Integral_image->GetScalarPointer() );

    cout << "m_EnergyName = " << m_EnergyName << endl;

    bool reInit;
    if(prevMode=="3DLocCV"){
        reInit=0;
    }else{
        reInit=1;
        prevMode="3DLocCV";   //keep track for next call
    }


    if( 0 == m_EnergyName.compare("ChanVese") )
    {
        cout << " run basic chan-vese on it "<< endl;
        CalcViewPlaneParams();
        assert(m_PlaneNormalVector.size()==3);
        interactive_chanvese(segEngine, img,phi,ptrIntegral_Image,label,dims,
                               LL3D.Lz,LL3D.Ln1,LL3D.Lp1,LL3D.Ln2,LL3D.Lp2,LL3D.Lin2out,LL3D.Lout2in,
                               iter,0.5*lambda,display);
        bool bDisplayChanVeseCost = false;
        if( bDisplayChanVeseCost )
        {
            double u0,u1;
            double cv_cost = this->evalChanVeseCost(u0,u1);
            cout << "chan vese cost = " << cv_cost << endl;
        }
        prevMode="3DChanVese";
    }
    else if( 0 == m_EnergyName.compare("ChanVeseLimited") )
    {
        cout << " run limited basic chan-vese on it "<< endl;
        CalcViewPlaneParams();
        assert(m_PlaneNormalVector.size()==3);
        interactive_chanvese_ext(segEngine, img,phi,ptrIntegral_Image,label,dims,
                               LL3D.Lz,LL3D.Ln1,LL3D.Lp1,LL3D.Ln2,LL3D.Lp2,LL3D.Lin2out,LL3D.Lout2in,LL3D.Lchanged,
                               iter,0.5*lambda,display,&(m_PlaneNormalVector[0]),
                               &(m_PlaneCenter[0]),this->m_DistWeight);
        bool bDisplayChanVeseCost = false;
        if( bDisplayChanVeseCost )
        {
            double u0,u1;
            double cv_cost = this->evalChanVeseCost(u0,u1);
            cout << "chan vese cost = " << cv_cost << endl;
        }
        prevMode="3DChanVese";
    }
    else if (0== m_EnergyName.compare("LocalCVLimited"))
    {
        cout << " run local global chan-vese on it, limiting around current plane "<< endl;
        CalcViewPlaneParams();
        assert(m_PlaneNormalVector.size()==3);
        interactive_rbchanvese_ext(segEngine, img,phi,ptrIntegral_Image,label,dims,
                               LL3D.Lz,LL3D.Ln1,LL3D.Lp1,LL3D.Ln2,LL3D.Lp2,LL3D.Lin2out,LL3D.Lout2in,LL3D.Lchanged,
                               iter,0.5*lambda,display,&(m_PlaneNormalVector[0]),
                               &(m_PlaneCenter[0]),this->m_DistWeight, reInit, this->segEngine->GetRadius() );
        prevMode="3DLocCV";
    }
    else if( 0 == m_EnergyName.compare("LocalCV") )
    {
        cout <<" run localized func " << endl;
        interactive_rbchanvese(    /* TODO: compute this energy!*/
                                 segEngine, img, phi, ptrIntegral_Image, label, dims,
                                 LL3D.Lz, LL3D.Ln1, LL3D.Lp1, LL3D.Ln2, LL3D.Lp2, LL3D.Lin2out, LL3D.Lout2in,
                                 iter,lambda*0.5,display, reInit, this->segEngine->GetRadius() );
        prevMode="3DLocCV";
    }
    else
    {
      cout << "Error, unsupported energy name! " << m_EnergyName << endl;
    }

    //threshold the level set to update the mask
    int Nelements = this->dimx * this->dimy * this->dimz;
    int labType=labelVol->GetScalarType();
    vrcl::setLabel3D(labType, labelVol->GetScalarPointer(),  phi, Nelements, this->currLabel);

    cout <<  "dims are:" << dims[0] << "    " << dims[1] << "      " << dims[2] << endl;
    cout <<  "Lz size: "       << LL3D.Lz->length << endl;

    //whats the point of these two variables?
    // after PK cleans up, these will be clarified
    m_UpdateVector.clear();
    m_CoordinatesVector.clear();
}



KSegmentor3D::~KSegmentor3D(){
  std::cout<<"deallocating kslice 3d"<<std::endl;
  //delete [] this->mdims;//Causes trouble! Haveto find out why!!
  delete [] this->img;
  delete [] this->mask;
  //delete [] this->imgRange;
  //delete [] this->labelRange;
  delete [] this->phi;
  delete [] this->label;

  delete [] this->mdims;
  delete [] this->imgSlice;
  delete [] this->maskSlice;
  delete [] this->U_I_slice;
  delete [] this->labelSlice;
  delete [] this->phiSlice;

  LL *Lz, *Ln1, *Ln2, *Lp1, *Lp2;
  LL *Lin2out, *Lout2in,*Lchanged;


  Lz=LL3D.Lz;
  Ln1=LL3D.Ln1;
  Ln2=LL3D.Ln2;
  Lp1=LL3D.Lp1;
  Lp2=LL3D.Lp2;
  Lin2out=LL3D.Lin2out;
  Lout2in=LL3D.Lout2in;
  Lchanged=LL3D.Lchanged;

  ll_destroy(Lz);
  ll_destroy(Ln1);
  ll_destroy(Ln2);
  ll_destroy(Lp1);
  ll_destroy(Lp2);
  ll_destroy(Lin2out);
  ll_destroy(Lout2in);
  ll_destroy(Lchanged);

  Lz=LL2D.Lz;
  Ln1=LL2D.Ln1;
  Ln2=LL2D.Ln2;
  Lp1=LL2D.Lp1;
  Lp2=LL2D.Lp2;
  Lin2out=LL2D.Lin2out;
  Lout2in=LL2D.Lout2in;
  Lchanged=LL2D.Lchanged;

  ll_destroy(Lz);
  ll_destroy(Ln1);
  ll_destroy(Ln2);
  ll_destroy(Lp1);
  ll_destroy(Lp2);
  ll_destroy(Lin2out);
  ll_destroy(Lout2in);
  ll_destroy(Lchanged);

  delete segEngine;
}



/** за трусость под огнем, расстрел

//void KSegmentor3D::Update3DUnknown(bool reInitFromMask)
//{
////    bool bSmoothU = false;
////    if( bSmoothU ) {
////        cout << "smoothing integral image 3D " << endl;
////        vtkSmartPointer<vtkImageGaussianSmooth> imageSmoother = vtkSmartPointer<vtkImageGaussianSmooth>::New();
////        imageSmoother->SetDimensionality(3);
////        imageSmoother->SetStandardDeviations(1.0,1.0,1.0);
////        imageSmoother->SetInput(this->U_Integral_image);
////        imageSmoother->Update();
////        this->U_Integral_image->DeepCopy(imageSmoother->GetOutput());
////    }


////    cout << "integrating mask 3D " << endl;
////    //this->integrateUserInput();   //done in python

//  // The bug is as follows: the level-set evolution modifies Lchanged indices,
//  // but afterwards they are not appearing in the list of modified coordinates!
//  // Thus, the next time around the mask doesn't get to update them!

//  cout << "updating mask 3D " << endl;
//  //this->UpdateMask(true);

//  this->CreateLLs(LL3D); // TODO: fix the caching so that only uniques are kept

//  // The bug is as follows: the level-set evolution modifies Lchanged indices,
//  // but afterwards they are not appearing in the list of modified coordinates!
//  // Thus, the next time around the mask doesn't get to update them!

//  ll_init(LL3D.Lchanged);

//  //        int Nelements=this->m_UpdateVector.size();
//  //        for (int element=0;element<Nelements;element++)
//  //        {
//  //            unsigned int idx=this->m_UpdateVector[element];
//  //            int found       = std::count( idx_used.begin(), idx_used.end(), idx );
//  //            if( 0 < found ) {
//  //                ll_pushnew(Lchanged,m_CoordinatesVector[element][0],m_CoordinatesVector[element][1],
//  //                                                           m_CoordinatesVector[element][2],idx);
//  //                idx_used.insert(idx);
//  //            }
//  //        }
//  cout << "m_UpdateVector Size BEGIN: " << m_UpdateVector.size()
//       << ", Lchanged size BEGIN: "     << LL3D.Lchanged->length
//       << ", Lz size BEGIN: "           << LL3D.Lz->length << endl;

//  ptrCurrImage = static_cast< short*>(imageVol->GetScalarPointer());
//  ptrCurrLabel = static_cast< short*>(labelVol->GetScalarPointer());
//  ptrIntegral_Image = static_cast<double*>(this->U_Integral_image->GetScalarPointer());
//  double u_in, u_out;

//  // Crap, the linked lists will accumulate zillions of repeating entries ...
//  // Must flush them and keep unique entries, or something (?)
//  //if(this->m_UpdateVector.size()!=0)
//  //        ls_mask2phi3c_update(this->m_UpdateVector,
//  //                            this->m_CoordinatesVector,mask,phi,label,dims,
//  //                            LL3D.Lz,LL3D.Ln1,LL3D.Ln2,LL3D.Lp1,LL3D.Lp2,LL3D.Lchanged);

//  ls_mask2phi3c(mask,phi,label,dims,LL3D.Lz,LL3D.Ln1,LL3D.Ln2,LL3D.Lp1,LL3D.Lp2);
//  //ls_mask2phi3c_ext(mask,phi,label,dims,LL3D.Lz,LL3D.Ln1,LL3D.Ln2,LL3D.Lp1,LL3D.Lp2,LL3D.Lchanged);
//  cout << "m_UpdateVector Size: "      << m_UpdateVector.size()
//       << " m_CoordinatesVector Size: "<< this->m_CoordinatesVector.size()
//       << ", Lchanged size: "          << LL3D.Lchanged->length
//       << ", Lz size: "                << LL3D.Lz->length << endl;

//  if( 0 == m_EnergyName.compare("ChanVese") ) {
//    interactive_chanvese_ext(img,phi,ptrIntegral_Image,label,dims,
//                             LL3D.Lz,LL3D.Ln1,LL3D.Lp1,LL3D.Ln2,LL3D.Lp2,LL3D.Lin2out,LL3D.Lout2in,LL3D.Lchanged,
//                             iter,0.5*lambda,display,this->m_PlaneNormalVector,
//                             this->m_PlaneCenter,this->m_DistWeight);
//    bool bDisplayChanVeseCost = false;
//    if( bDisplayChanVeseCost ) {
//      double cv_cost = this->evalChanVeseCost(u_in,u_out);
//      cout << "chan vese cost = " << cv_cost << endl;
//    }
//  }
//  else if( 0 == m_EnergyName.compare("LocalCV") )
////    interactive_rbchanvese_ext(img,phi, ptrIntegral_Image, label, dims,
////                               LL3D.Lz, LL3D.Ln1, LL3D.Lp1, LL3D.Ln2, LL3D.Lp2, LL3D.Lin2out, LL3D.Lout2in, LL3D.Lchanged,
////                               iter,rad, lambda, display, this->m_PlaneNormalVector,
////                               this->m_PlaneCenter, this->m_DistWeight);
//    interactive_rbchanvese(    img, phi, ptrIntegral_Image, label, dims,
//                               LL3D.Lz, LL3D.Ln1, LL3D.Lp1, LL3D.Ln2, LL3D.Lp2, LL3D.Lin2out, LL3D.Lout2in,
//                               iter,rad,lambda*0.5,display);
//    bool bDisplayChanVeseCost = false;
//    if( bDisplayChanVeseCost ) {
//        double cv_cost = this->evalChanVeseCost(u_in,u_out);
//        cout << "chan vese cost = " << cv_cost << endl;
//    }
//  else
//    cout << "Error, unsupported energy name! " << m_EnergyName << endl;


//  this->evalChanVeseCost(u_in,u_out);
//  double cutoff_thresh = (u_in - u_out)*this->m_ThreshWeight;
//  cout << "uin, uout, cutoff=" << u_in << ", " << u_out << ", " << cutoff_thresh << endl;

//  double mult=labelRange[1] / 4.0;

//  double phi_val = 0;
//  double phi_out = 0;
//  double outputVal=0;

//  // Caution: Lchanged can contain duplicate entries !!!
//  ll_init(LL3D.Lchanged);
//  // while(Lchanged->curr != NULL)
//  double changeInLabel = 0;
//  int Nelements = this->dimx * this->dimy * this->dimz;
//  for (int idx=0;idx<Nelements;idx++)
//  {

//    // traverse through all modified Lz *and their neighbors!*
//    // neighbors are where phi_out is something between 0 and 1
//    //          x0 = Lchanged->curr->x;
//    //          y0 = Lchanged->curr->y;
//    //          z0 = Lchanged->curr->z;
//    //          for (int dx = -2; dx < 2; dx ++ ) {
//    //              x = x0 + dx;
//    //              for (int dy = -2; dy < 2; dy ++ ) {
//    //                  y = y0 + dy;
//    //                  for (int dz = -2; dz < 2; dz ++ ) {
//    //                      z = z0 + dz;
//    //                      if( 0 < ( (x<0)+(y<0)+(z<0)+(x>=mdims[0])+(y>=mdims[1])+(z>=mdims[2]) ) )
//    //                      { // the neighbor is out of bounds !
//    //                          continue;
//    //                      }
//    //    idx = (z)*mdims[1]*mdims[0] +(y)*mdims[0]+(x);
//    phi_val = phi[idx];
//    if( (u_out < u_in) && ( img[idx] < (u_out+2*cutoff_thresh) ) )
//    { // force it to be out
//      phi_val = 3.0;
//    }
//    else if( (u_out < u_in) && ( img[idx] < (u_out+cutoff_thresh) ) )
//    { // force it to be out
//      phi_val = std::min( phi_val, 0.0 );
//    }
//    phi_out = (-phi_val + 3.0) / 6.0;
//    outputVal=  (unsigned short) ( ( (phi_out > 0.95) +
//                                     (phi_out > 0.8) +
//                                     (phi_out > 0.65) +
//                                     (phi_out > 0.5) )
//                                   *mult);
//    changeInLabel += fabs( double(outputVal - ptrCurrLabel[idx]) ) ;
//    ptrCurrLabel[idx] =outputVal;
//    mask[idx]         =(double) ( 0 < ptrCurrLabel[idx] ); // d'oh, *update the mask!*

//    //                  }
//    //              }
//    //          }
//    //          ll_step(Lchanged);
//  }
//  cout << "m_UpdateVector Size: " << m_UpdateVector.size()
//       <<  ", Lchanged size: " << LL3D.Lchanged->length
//        <<  ", |change| in label: " << changeInLabel << endl;

//  double spc[3];
//  this->U_Integral_image->GetSpacing(spc);

//  m_UpdateVector.clear();
//  m_CoordinatesVector.clear();
//}

//PKDelete
//void KSegmentor3D::UpdateArraysAfterTransform()
//{
//  this->mdims=U_t_image->GetDimensions();

//  //Set dimensions
//  dimz = (int)mdims[2];
//  dimy = (int)mdims[1];
//  dimx = (int)mdims[0];

//  dims[2] = dimz;
//  dims[1] = dimy;
//  dims[0] = dimx;

//  dims[3] = dims[0]*dims[1];
//  dims[4] = dims[0]*dims[1]*dims[2];

//  this->U_Integral_image->GetSpacing( m_Spacing_mm );

//  cout <<  "; updated KSegmentor3D with dims[0,1,2] = "
//        << dimx << "," << dimy << "," << dimz << endl;

//  this->rad = 3.0 / std::max( m_Spacing_mm[0],m_Spacing_mm[1] ); // about 3mm in physical units
//  this->rad = std::min(7.0,this->rad); // force non-huge radius if the spacing is retarded
//  this->rad = std::max(3.0, this->rad); // force non-tiny radius if the spacing is retarded

//  this->initializeData();

//  this->CreateLLs(LL3D);

//  this->intializeLevelSet3D();
//}
*/
