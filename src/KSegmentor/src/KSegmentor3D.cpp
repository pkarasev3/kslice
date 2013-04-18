#include "KSegmentor3D.h"
#include "llist.h"
#include "sfm_local_chanvese_mex.h"
#include "interactive_kurvolver.h"
#include "vtkImageData.h"
#include <omp.h>
#include <string>
#include <sstream>
#include "vtkImageGaussianSmooth.h"
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <assert.h>


using std::string;

KSegmentor3D::KSegmentor3D(vtkImageData* image, vtkImageData* label, vtkImageData* UIVol,
                           bool contInit, int currSlice, int numIts, float distWeight, int brushRad, int currLabel)
{
  m_EnergyName = GetSupportedEnergyNames()[1];
  this->InitializeVariables(image,label, UIVol, contInit, currSlice, numIts, distWeight, brushRad, currLabel);

// TODO: re-integrate this, check that label value is handled correctly from slicer
// "set U based on input label", e.g. when input is large complex label that user wants to edit slightly
//  if(contInit) {
//    std::cout<<"Initializing user input using label data"<<std::endl;
//    this->initializeUserInputImageWithContour();
//  }

  this->CreateLLs(this->LL3D);
  this->CreateLLs(this->LL2D);
  LL* Lztmp = this->LL3D.Lz;
  assert(Lztmp != NULL);

  //initialize pointers 2D
  this->imgSlice    = NULL;
  this->maskSlice   = NULL;
  this->U_I_slice   = NULL;
  this->labelSlice  = NULL;
  this->phiSlice    = NULL;

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

void KSegmentor3D::integrateUserInput()
{
  ptrIntegral_Image = static_cast<double*>(this->U_Integral_image->GetScalarPointer());
  ptrU_t_Image      = static_cast<double*>(this->U_t_image->GetScalarPointer());

  int pos=0;
  int Nelements=this->m_UpdateVector.size();
  cout << " Integrating:  KSegmentor3D::integrateUserInput(), N = "
       << Nelements << endl;
  for (int element=0;element<Nelements;element++)
  {
    pos=this->m_UpdateVector[element];
    this->ptrIntegral_Image[pos] += this->ptrU_t_Image[pos];
    this->ptrU_t_Image[pos]       = 0;
  }
}


void KSegmentor3D::initializeData()
{
  currSlice=0;
  prevSlice=-1; //this will never be true
  imageVol->GetScalarRange( imgRange );
  labelVol->GetScalarRange( labelRange );
  if( abs(labelRange[1]) < 1e-3 )
  { // empty label; so set the proper range
    labelRange[1] = this->currLabel;
  }

  this->imgRange=imgRange;
  //ptrCurrImage = static_cast< short*>(imageVol->GetScalarPointer());
  //ptrCurrLabel = static_cast< short*>(labelVol->GetScalarPointer());


  int imgType=imageVol->GetScalarType();
  switch(imgType)
  {
  case 0:     //#define VTK_VOID            0
      assert(0);
      break;
  case 1:    //#define VTK_BIT             1
      vrcl::convertImage( (bool *) imageVol->GetScalarPointer(), this->img, dimx, dimy, dimz);
      break;
  case 2:    //#define VTK_CHAR            2
      vrcl::convertImage( (char *) imageVol->GetScalarPointer(), this->img, dimx, dimy, dimz);
      break;
  case 3:    //#define VTK_UNSIGNED_CHAR   3
      vrcl::convertImage( (unsigned char *)imageVol->GetScalarPointer(), this->img, dimx, dimy, dimz);
      break;
  case 4:    //#define VTK_SHORT           4
      vrcl::convertImage( (short *)imageVol->GetScalarPointer(), this->img, dimx, dimy, dimz);
      break;
  case 5:    //#define VTK_UNSIGNED_SHORT  5
      vrcl::convertImage( (unsigned short *)imageVol->GetScalarPointer(), this->img, dimx, dimy, dimz);
      break;
  case 6:    //#define VTK_INT             6
      vrcl::convertImage( (int *)imageVol->GetScalarPointer(), this->img, dimx, dimy, dimz);
      break;
  case 7:    //#define VTK_UNSIGNED_INT    7
      vrcl::convertImage( (unsigned int *)imageVol->GetScalarPointer(), this->img, dimx, dimy, dimz);
      break;
  case 8:    //#define VTK_LONG            8
      vrcl::convertImage( (long *)imageVol->GetScalarPointer(), this->img, dimx, dimy, dimz);
      break;
  case 9:    //#define VTK_UNSIGNED_LONG   9
      vrcl::convertImage( (unsigned long *)imageVol->GetScalarPointer(), this->img, dimx, dimy, dimz);
      break;
  case 10:    //#define VTK_FLOAT          10
      vrcl::convertImage( (float *)imageVol->GetScalarPointer(), this->img, dimx, dimy, dimz);
      break;
  case 11:    //#define VTK_DOUBLE         11
      vrcl::convertImage( (double *)imageVol->GetScalarPointer(), this->img, dimx, dimy, dimz);
      break;
  }

  int labType=labelVol->GetScalarType();
  switch(labType)
  {
  case 0:     //#define VTK_VOID            0
      assert(0);
      break;
  case 1:    //#define VTK_BIT             1
      vrcl::convertLabel( (bool *) labelVol->GetScalarPointer(), this->mask, dimx, dimy, dimz);
      break;
  case 2:    //#define VTK_CHAR            2
      vrcl::convertLabel( (char *) labelVol->GetScalarPointer(), this->mask, dimx, dimy, dimz);
      break;
  case 3:    //#define VTK_UNSIGNED_CHAR   3
      vrcl::convertLabel( (unsigned char *)labelVol->GetScalarPointer(), this->mask, dimx, dimy, dimz);
      break;
  case 4:    //#define VTK_SHORT           4
      vrcl::convertLabel( (short *)labelVol->GetScalarPointer(), this->mask, dimx, dimy, dimz);
      break;
  case 5:    //#define VTK_UNSIGNED_SHORT  5
      vrcl::convertLabel( (unsigned short *)labelVol->GetScalarPointer(), this->mask, dimx, dimy, dimz);
      break;
  case 6:    //#define VTK_INT             6
      vrcl::convertLabel( (int *)labelVol->GetScalarPointer(), this->mask, dimx, dimy, dimz);
      break;
  case 7:    //#define VTK_UNSIGNED_INT    7
      vrcl::convertLabel( (unsigned int *)labelVol->GetScalarPointer(), this->mask, dimx, dimy, dimz);
      break;
  case 8:    //#define VTK_LONG            8
      vrcl::convertLabel( (long *)labelVol->GetScalarPointer(), this->mask, dimx, dimy, dimz);
      break;
  case 9:    //#define VTK_UNSIGNED_LONG   9
      vrcl::convertLabel( (unsigned long *)labelVol->GetScalarPointer(), this->mask, dimx, dimy, dimz);
      break;
  case 10:    //#define VTK_FLOAT          10
      vrcl::convertLabel( (float *)labelVol->GetScalarPointer(), this->mask, dimx, dimy, dimz);
      break;
  case 11:    //#define VTK_DOUBLE         11
      vrcl::convertLabel( (double *)labelVol->GetScalarPointer(), this->mask, dimx, dimy, dimz);
      break;
  }

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
bool check_U_behavior(const std::vector<double>& phi0, double* phi1, double* U )
{    /** ensure that phi does not change sign when
              U is "sufficiently large" */
    bool isGood = true;
    int sz=phi0.size();
    cout <<  "\033[01;45m\033]" << "checking U effect... " << "\033[00m\033]" << endl;
    for(int i=0;i<sz;i++) {
        double Uij = U[i];
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
    return isGood;
}


}

void KSegmentor3D::Update2D(bool reInitFromMask)
{
    //set the orientation
    int dim0=0; int dim1=0; int dim2=0;
    vrcl::Orient sliceView = vrcl::SLICE_IJ;
    if( m_IJK_orient == "IJ" ) {
        dim0 = mdims[0];
        dim1 = mdims[1];
        sliceView = vrcl::SLICE_IJ; dim2=mdims[2];
    }else if( m_IJK_orient == "JK" ) {
        dim0 = mdims[1];
        dim1 = mdims[2]; dim2=mdims[0];
        sliceView = vrcl::SLICE_JK;
    }else if( m_IJK_orient == "IK" ) {
        dim0 = mdims[0];
        dim1 = mdims[2]; dim2=mdims[1];
        sliceView = vrcl::SLICE_IK;
    }

    std::vector<long> dimsSlice(5);
    dimsSlice[0] = dim0;
    dimsSlice[1] = dim1;
    dimsSlice[2] = 1;
    dimsSlice[3] = dimsSlice[0]*dimsSlice[1];
    dimsSlice[4] = dimsSlice[0]*dimsSlice[1]*dimsSlice[2];

    cout << "orient=" << m_IJK_orient << ", prevslice=" << prevSlice << ", " << "currslice= " << currSlice << endl;

    if( (prevSlice == this->currSlice) && !reInitFromMask ) {
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
        this->maskSlice         = new double[ dim0 * dim1 ];
        this->U_I_slice         = new double[ dim0 * dim1 ];
        this->labelSlice        = new double[ dim0 * dim1 ];
        this->phiSlice          = new double[ dim0 * dim1 ];

        //user input is always double
        ptrIntegral_Image   = static_cast<double*>(this->U_Integral_image->GetScalarPointer());
        vrcl::convertSliceToDouble( ptrIntegral_Image, this->U_I_slice  , dim0, dim1, dim2, currSlice, sliceView );

        //copy images based on type
        int imgType=imageVol->GetScalarType();
        vrcl::convertSliceToDouble(imgType, (bool *)imageVol->GetScalarPointer(), imgSlice  , dim0, dim1, dim2, currSlice, sliceView );

        //copy labels based on type
        int labType=labelVol->GetScalarType();
        vrcl::convertSliceToDouble(labType,(double *)labelVol->GetScalarPointer(), maskSlice  , dim0, dim1, dim2, currSlice, sliceView );

        //set up the new level set
        this->CreateLLs(LL2D);
        ls_mask2phi3c(maskSlice,phiSlice,labelSlice,&(dimsSlice[0]), LL2D.Lz,LL2D.Ln1,LL2D.Ln2,LL2D.Lp1,LL2D.Lp2);
        cout <<  "\033[01;42m\033]"<< "2Dfirst time on slice! " << "\033[00m\033]" << endl;
    }
    prevSlice=currSlice;

    // save the phi before levelset, to verify expected behavior
    cache_phi.resize(dim0*dim1);
    std::memcpy( &(cache_phi[0]),&(phiSlice[0]),sizeof(double)*dim0 * dim1 );

    //run the active contour, ** modify phiSlice in-place! (IVAN: yes?) **
    // Why does the U_I_slice keep seeming like zeros when clicking??
    interactive_rbchanvese(imgSlice,phiSlice,U_I_slice,labelSlice,&(dimsSlice[0]),
                           LL2D.Lz,LL2D.Ln1,LL2D.Lp1,LL2D.Ln2,LL2D.Lp2,LL2D.Lin2out,LL2D.Lout2in,
                           iter,rad,lambda*0.5,display);

    bool isGood = check_U_behavior(cache_phi, phiSlice, U_I_slice);
    if( !isGood ) { std::cout << "BAD!" << std::endl; }

    //threshold phi, set label to appropriate values
    int labType=labelVol->GetScalarType();
    vrcl::convertDoubleToSlice( labType, labelVol->GetScalarPointer(), phiSlice,
                                dim0, dim1, dim2, currSlice, sliceView, currLabel );

    //clean up, but is m_UpdateVector, m_CoordinatesVector necessary??
    cout <<  "Lz size: "       << LL2D.Lz->length << endl;

    //whats the point of these two variable? awesomeness
    m_UpdateVector.clear(); // should hook this up for user input accum
    m_CoordinatesVector.clear();
}


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

    interactive_rbchanvese(  img, phi, ptrIntegral_Image, label, dims,
                             LL3D.Lz, LL3D.Ln1, LL3D.Lp1, LL3D.Ln2, LL3D.Lp2, LL3D.Lin2out, LL3D.Lout2in,
                             iter,rad,lambda*0.5,display);

    //threshold the level set to update the mask
    int Nelements = this->dimx * this->dimy * this->dimz;
    int labType=labelVol->GetScalarType();
    switch(labType)
    {
    case 0:     //#define VTK_VOID            0
        assert(0);
        break;
    case 1:    //#define VTK_BIT             1
        vrcl::setLabel3D((bool *) labelVol->GetScalarPointer(),  phi, Nelements, this->currLabel);
        break;
    case 2:    //#define VTK_CHAR            2
        vrcl::setLabel3D((char *) labelVol->GetScalarPointer(),  phi, Nelements, this->currLabel);
        break;
    case 3:    //#define VTK_UNSIGNED_CHAR   3
        vrcl::setLabel3D((unsigned char *) labelVol->GetScalarPointer(),  phi, Nelements, this->currLabel);
        break;
    case 4:    //#define VTK_SHORT           4
        vrcl::setLabel3D((short *) labelVol->GetScalarPointer(),  phi, Nelements, this->currLabel);
        break;
    case 5:    //#define VTK_UNSIGNED_SHORT  5
        vrcl::setLabel3D((unsigned short *) labelVol->GetScalarPointer(),  phi, Nelements, this->currLabel);
        break;
    case 6:    //#define VTK_INT             6
        vrcl::setLabel3D((int *) labelVol->GetScalarPointer(),  phi, Nelements, this->currLabel);
        break;
    case 7:    //#define VTK_UNSIGNED_INT    7
        vrcl::setLabel3D((unsigned int *) labelVol->GetScalarPointer(),  phi, Nelements, this->currLabel);
        break;
    case 8:    //#define VTK_LONG            8
        vrcl::setLabel3D((long *) labelVol->GetScalarPointer(),  phi, Nelements, this->currLabel);
        break;
    case 9:    //#define VTK_UNSIGNED_LONG   9
        vrcl::setLabel3D((unsigned long *) labelVol->GetScalarPointer(),  phi, Nelements, this->currLabel);
        break;
    case 10:    //#define VTK_FLOAT          10
        vrcl::setLabel3D((float *) labelVol->GetScalarPointer(),  phi, Nelements, this->currLabel);
        break;
    case 11:    //#define VTK_DOUBLE         11
        vrcl::setLabel3D((double *) labelVol->GetScalarPointer(),  phi, Nelements, this->currLabel);
        break;
    }

    cout <<  "dims are:" << dims[0] << "    " << dims[1] << "      " << dims[2] << endl;
    cout <<  "Lz size: "       << LL3D.Lz->length << endl;

    //whats the point of these two variables?
    // after PK cleans up, these will be clarified
    m_UpdateVector.clear();
    m_CoordinatesVector.clear();
}



KSegmentor3D::~KSegmentor3D(){

  //delete [] this->mdims;//Causes trouble! Haveto find out why!!
  delete [] this->img;
  delete [] this->mask;
  delete [] this->imgRange;
  delete [] this->labelRange;
  delete [] this->phi;
  delete [] this->label;


  delete [] this->imgSlice;
  delete [] this->maskSlice;
  delete [] this->U_I_slice;
  delete [] this->labelSlice;

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
