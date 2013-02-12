#include "KSegmentor3D.h"
#include "llist.h"
#include "sfm_local_chanvese_mex.h"
#include "interactive_kurvolver.h"
#include "vtkImageData.h"
#include "KSandbox.h"
#include <omp.h>
#include <string>
#include <sstream>
#include <opencv2/highgui/highgui.hpp>
#include "vtkImageGaussianSmooth.h"


#include <ctime>

using std::string;
using cv::Mat;

//these global variables are no good, need to fix later
extern double ain, aout, auser; // means
extern double *pdfin, *pdfout, *pdfuser;
extern long numdims;
extern double engEval;
extern bool UseInitContour;
extern double *Ain, *Aout, *Sin, *Sout; //local means

namespace vrcl
{

KSegmentor3D* KSegmentor3D::CreateSegmentor(vtkImageData *image, vtkImageData *label, bool contInit)
{
  KSegmentor3D* seg3DPointer = new KSegmentor3D;
  seg3DPointer->m_EnergyName = GetSupportedEnergyNames()[1];
  seg3DPointer->InitializeVariables(seg3DPointer,image,label, contInit);

  if(contInit)
  { /** an input label was provided; assume intent is for U initially strong then */
    std::cout<<"Initializing user input using label data"<<std::endl;
    seg3DPointer->initializeUserInputImageWithContour();
  }



  seg3DPointer->initializeData();
  seg3DPointer->CreateLLs(seg3DPointer->LL3D);

  seg3DPointer->intializeLevelSet3D();
  return seg3DPointer;
}

void KSegmentor3D::accumulateUserInputInUserInputImages( double value,const unsigned int element)
{
  double Umax            = 1.0;   // It is bizarre that having this at 10.0 works,
  // technically it shouldn't because we're using inside the  tanh() function
  // comparing it with \phi() which is between -3 and +3 . If we can't get
  // values between -3 and +3 the smoothness breaks down.

  // Umax = this->GetUmax(); // this is initialized to bullshit, TODO fix it
  Umax = 1.0/3.0;
  if( fabs(Umax) < 0.01 ) { cout <<"whoa something is F'd, check Umax " << endl; exit(1); }

  double user_input      = -Umax * ( value > 0.5 ) + Umax * ( value <= 0.5 );

  //Changed accumulation! (+=) instead of (=)
  //this->ptrU_t_Image[element]+=user_input; // I think this leads to crazily high/disparate values ...
  // at least, smoothness needs to be enforced somewhere else ...
  this->ptrU_t_Image[element] = user_input;

  // integrate/updateVector are broken, this is a workaround
  //ptrIntegral_Image[element] = user_input + ptrIntegral_Image[element];
  double Ulimit = 3.0;
  if( ptrIntegral_Image[element] < -Ulimit ) {
    ptrIntegral_Image[element] = -Ulimit;
  } else if( ptrIntegral_Image[element] > Ulimit ) {
    ptrIntegral_Image[element] = Ulimit;
  }


}

void KSegmentor3D::integrateUserInputInUserInputImage()
{
  ptrIntegral_Image = static_cast<double*>(this->U_Integral_image->GetScalarPointer());
  ptrU_t_Image      = static_cast<double*>(this->U_t_image->GetScalarPointer());

  int pos=0;
  int Nelements=this->m_UpdateVector.size(); // compiler may not optimize this out, b/c technically m_UpdateVector could change size in the loop
  cout << " Integrating:  KSegmentor3D::integrateUserInputInUserInputImage(), N= "
       << Nelements << endl; // TODO: This is broken in 2D case !?
  for (int element=0;element<Nelements;element++)
  {
    pos=this->m_UpdateVector[element];

    this->ptrIntegral_Image[pos] += this->ptrU_t_Image[pos];
    this->ptrU_t_Image[pos]= 0; // this->ptrU_t_Image[pos]*0.5;
  }
}


void KSegmentor3D::UpdateArraysAfterTransform()
{
  this->mdims=U_t_image->GetDimensions();

  //Set dimensions
  dimz = (int)mdims[2];
  dimy = (int)mdims[1];
  dimx = (int)mdims[0];

  dims[2] = dimz;
  dims[1] = dimy;
  dims[0] = dimx;

  dims[3] = dims[0]*dims[1];
  dims[4] = dims[0]*dims[1]*dims[2];

  this->U_Integral_image->GetSpacing( m_Spacing_mm );

  cout << "num dims = " << numdims << "; updated KSegmentor3D with dims[0,1,2] = "
       << dimx << "," << dimy << "," << dimz << endl;

  this->rad = 3.0 / std::max( m_Spacing_mm[0],m_Spacing_mm[1] ); // about 3mm in physical units
  this->rad = std::min(7.0,this->rad); // force non-huge radius if the spacing is retarded
  this->rad = std::max(3.0, this->rad); // force non-tiny radius if the spacing is retarded

  this->initializeData();

  this->CreateLLs(LL3D);

  this->intializeLevelSet3D();
}


void KSegmentor3D::initializeData()
{
  imageVol->GetScalarRange( imgRange );
  labelVol->GetScalarRange( labelRange );
  if( abs(labelRange[1]) < 1e-3 )
  { // empty label; so set the proper range
    labelRange[1] = KViewerOptions::getDefaultDrawLabelMaxVal();
  }
  //assert( 0 != imgRange[1] ); // what the, all black ?? impossible !

  this->imgRange=imgRange;
  ptrCurrImage = static_cast<unsigned short*>(imageVol->GetScalarPointer());
  ptrCurrLabel = static_cast<unsigned short*>(labelVol->GetScalarPointer());

  long elemNum=0;
  for (int k=0; k<=dimx-1; k++) {
    for (int j=0; j<=dimy-1; j++)  {
      for (int i=0; i<=dimz-1; i++,elemNum++) {
        this->mask[elemNum]=(double) ( 0 < ptrCurrLabel[elemNum] );
        this->img[elemNum] = (double) ptrCurrImage[elemNum];
      }
    }
  }
}



void KSegmentor3D::Update2D()
{
  this->integrateUserInputInUserInputImage();
  this->CreateLLs(LL2D);

  ptrCurrImage        = static_cast<unsigned short*>(imageVol->GetScalarPointer());
  ptrCurrLabel        = static_cast<unsigned short*>(labelVol->GetScalarPointer());
  ptrIntegral_Image = static_cast<double*>(this->U_Integral_image->GetScalarPointer());

  double* imgSlice          = new double[  mdims[0]*mdims[1] ];
  double* maskSlice       = new double[ mdims[0]*mdims[1] ];
  double* U_I_slice        = new double[ mdims[0]*mdims[1] ];
  double* phiSlice          = new double[ mdims[0]*mdims[1] ];
  double* labelSlice       = new double[ mdims[0]*mdims[1] ];

  unsigned int element3D;
  long elemNum=0;
  long maskThresh=0.5*labelRange[1];
  double maxU = -1e99; double minU = 1e99;
  for (int j=0; j<=mdims[1]-1; j++)  {
    for (int i=0; i<=mdims[0]-1; i++) {
      // indexing definition:  ptr[k*mdims[1]*mdims[0] +j*mdims[0]+i];
      element3D=this->currSlice*mdims[1]*mdims[0] +j*mdims[0]+i;
      imgSlice[elemNum]        = (double) ptrCurrImage[element3D];
      maskSlice[elemNum]       = (double) (  maskThresh < ptrCurrLabel[element3D] );

      U_I_slice[elemNum] = (double) ptrIntegral_Image[element3D];
      if ( maxU < U_I_slice[elemNum] ) { //PKDebug
        maxU = U_I_slice[elemNum]; cout << "maxU = " << maxU << endl;
      } else if ( minU > U_I_slice[elemNum] ) { //PKDebug
        minU = U_I_slice[elemNum]; cout << "minU = " << minU << endl;
      }
      elemNum++;
    }
  }


  bool writePhi=false;
  if ( writePhi ){
    //IKDebug
    std::ofstream maskRec("maskRec.txt",std::ios_base::out);
    for(int i=0;i< elemNum; i++){
      maskRec<<maskSlice[i]<<' ';
    }
    maskRec.close();
  }





  std::vector<long> dimsSlice(5);
  dimsSlice[0] = mdims[0];
  dimsSlice[1] = mdims[1];
  dimsSlice[2] = 1;
  dimsSlice[3] = dimsSlice[0]*dimsSlice[1];
  dimsSlice[4] = dimsSlice[0]*dimsSlice[1]*dimsSlice[2];


  ls_mask2phi3c(maskSlice,phiSlice,labelSlice,&(dimsSlice[0]),
                LL2D.Lz,LL2D.Ln1,LL2D.Ln2,LL2D.Lp1,LL2D.Lp2);

  if ( writePhi ){
    //IKDebug
    std::ofstream phiSliceRec("phiSlice.txt",std::ios_base::out);
    for(int i=0;i< elemNum; i++){
      phiSliceRec<<phiSlice[i]<<' ';
    }
    phiSliceRec.close();
  }


  interactive_rbchanvese(imgSlice,phiSlice,U_I_slice,labelSlice,&(dimsSlice[0]),
                         LL2D.Lz,LL2D.Ln1,LL2D.Lp1,LL2D.Ln2,LL2D.Lp2,LL2D.Lin2out,LL2D.Lout2in,
                         iter,rad,lambda*0.5,display);

  //threshold phi to find segmentation label, assign it to appropriate range of label!
  elemNum=0;
  for (int j=0; j<=mdims[1]-1; j++)  {
    for (int i=0; i<=mdims[0]-1; i++) {
      double phi_val = phiSlice[elemNum];
      double phi_out = (-phi_val + 3.0) / 6.0 ;/*6*/; // shift and scale from [-3,3] to [0,1]

      element3D=this->currSlice*mdims[1]*mdims[0] +j*mdims[0]+i;

      unsigned short value_IK= ( (unsigned short) 0 >= phi_val )*labelRange[1];   // IK: if phi=0, make label 0, else it's 1
      unsigned short value_PK= ( (unsigned short) ( ( (phi_out > 0.95)
                                                      + (phi_out > 0.8) + (phi_out > 0.65)
                                                      + (phi_out > 0.5) ) * labelRange[1] / 4.0 ) );
      ptrCurrLabel[element3D]= value_IK;
      //if( (value_PK == 0) && !( 0 == value_IK ) ) {
      if(value_IK != value_PK){
        //cout << "IK!=PK;  IK = " << value_IK << ", PK = " << value_PK << endl;
        //cout << "crap, extraneous loss of levelset! " << endl; assert(0);
      }
      elemNum++;
    }
  }

  cout <<  ", KSegmentor3D.cpp :   Lz size: "       << LL2D.Lz->length << endl;

  if ( writePhi ){
    std::ofstream phiRec("phiRec.txt",std::ios_base::out);
    for(int i=0;i< elemNum; i++){
      phiRec<<ptrCurrLabel[element3D=this->currSlice*mdims[1]*mdims[0] + i]<<' ';
    }
    phiRec.close();
  }

  bool bSavePNG = false;
  if( bSavePNG ) {
    std::stringstream ss;
    ss << "U_integral_ " << std::setw(3) << std::setfill('0') << currSlice << ".png";
    saveMatToPNG( U_I_slice, ss.str() );
  }
  delete [] imgSlice;
  delete [] labelSlice;
  delete [] maskSlice;
  delete [] phiSlice;
  delete [] U_I_slice;
  m_UpdateVector.clear();
  m_CoordinatesVector.clear();
}

void KSegmentor3D::Update3D()
{
  bool bSmoothU = false;
  if( bSmoothU ) {
    cout << "smoothing integral image 3D " << endl;
    vtkSmartPointer<vtkImageGaussianSmooth> imageSmoother = vtkSmartPointer<vtkImageGaussianSmooth>::New();
    imageSmoother->SetDimensionality(3);
    imageSmoother->SetStandardDeviations(1.0,1.0,1.0);
    imageSmoother->SetInput(this->U_Integral_image);
    imageSmoother->Update();
    this->U_Integral_image->DeepCopy(imageSmoother->GetOutput());
  }

  /** PK:
                  Large commented blocks around Lchanged, lsmask2phi3c_ext:
                   april 2012 we wrote these codes that mostly work, except that the lists are really *sets* and will accumulate
                   repeat entries as written. Ran out of time and commented out; not clear if this is a bottle-neck or not. (10 jan 2013)
          */

  cout << "integrating mask 3D " << endl;
  this->integrateUserInputInUserInputImage();

  cout << "updating mask 3D " << endl;
  this->UpdateMask(true);

  this->CreateLLs(LL3D); // TODO: fix the caching so that only uniques are kept

  ll_init(LL3D.Lchanged);
  //
  // TODO: need this?
  //        std::set<unsigned int> idx_used;
  //        int Nelements=this->m_UpdateVector.size();
  //        for (int element=0;element<Nelements;element++)
  //        {
  //            unsigned int idx=this->m_UpdateVector[element];
  //            int found       = std::count( idx_used.begin(), idx_used.end(), idx );
  //            if( 0 < found ) {
  //                ll_pushnew(Lchanged,m_CoordinatesVector[element][0],m_CoordinatesVector[element][1],
  //                                                           m_CoordinatesVector[element][2],idx);
  //                idx_used.insert(idx);
  //            }
  //        }
  cout << "m_UpdateVector Size BEGIN: " << m_UpdateVector.size()
       <<  ", Lchanged size BEGIN: " << LL3D.Lchanged->length
        <<  ", Lz size BEGIN: "       << LL3D.Lz->length << endl;

  ptrCurrImage = static_cast<unsigned short*>(imageVol->GetScalarPointer());
  ptrCurrLabel = static_cast<unsigned short*>(labelVol->GetScalarPointer());
  ptrIntegral_Image = static_cast<double*>(this->U_Integral_image->GetScalarPointer());
  double u_in, u_out;
  // Crap, the linked lists will accumulate zillions of repeating entries ...
  // Must flush them and keep unique entries, or something (?)
  //if(this->m_UpdateVector.size()!=0)
  //        ls_mask2phi3c_update(this->m_UpdateVector,
  //                            this->m_CoordinatesVector,mask,phi,label,dims,
  //                            LL3D.Lz,LL3D.Ln1,LL3D.Ln2,LL3D.Lp1,LL3D.Lp2,LL3D.Lchanged);

  ls_mask2phi3c(mask,phi,label,dims,LL3D.Lz,LL3D.Ln1,LL3D.Ln2,LL3D.Lp1,LL3D.Lp2);
  //   ls_mask2phi3c_ext(mask,phi,label,dims,LL3D.Lz,LL3D.Ln1,LL3D.Ln2,LL3D.Lp1,LL3D.Lp2,LL3D.Lchanged);
  cout << "m_UpdateVector Size: " << m_UpdateVector.size()
       <<" m_CoordinatesVector Size: "<<this->m_CoordinatesVector.size()
      <<  ", Lchanged size: " << LL3D.Lchanged->length
       <<  ", Lz size: "       << LL3D.Lz->length << endl;

  if( 0 == m_EnergyName.compare("ChanVese") ) {
    interactive_chanvese_ext(img,phi,ptrIntegral_Image,label,dims,
                             LL3D.Lz,LL3D.Ln1,LL3D.Lp1,LL3D.Ln2,LL3D.Lp2,LL3D.Lin2out,LL3D.Lout2in,LL3D.Lchanged,
                             iter,0.5*lambda,display,this->m_PlaneNormalVector,
                             this->m_PlaneCenter,this->m_DistWeight);
    bool bDisplayChanVeseCost = false;
    if( bDisplayChanVeseCost ) {
      double cv_cost = this->evalChanVeseCost(u_in,u_out);
      cout << "chan vese cost = " << cv_cost << endl;
    }
  }
  else if( 0 == m_EnergyName.compare("LocalCV") )
    interactive_rbchanvese_ext(img,phi,ptrIntegral_Image,label,dims,
                               LL3D.Lz,LL3D.Ln1,LL3D.Lp1,LL3D.Ln2,LL3D.Lp2,LL3D.Lin2out,LL3D.Lout2in,LL3D.Lchanged,
                               iter,rad,lambda,display,this->m_PlaneNormalVector,
                               this->m_PlaneCenter,this->m_DistWeight);
  else
    cout << "Error, unsupported energy name! " << m_EnergyName << endl;


  this->evalChanVeseCost(u_in,u_out);
  double cutoff_thresh = (u_in - u_out)*this->m_ThreshWeight;
  cout << "uin, uout, cutoff=" << u_in << ", " << u_out << ", " << cutoff_thresh << endl;

  double mult=labelRange[1] / 4.0;

  double phi_val = 0;
  double phi_out = 0;
  double outputVal=0;

  // Caution: Lchanged can contain duplicate entries !!!
  ll_init(LL3D.Lchanged);
  // while(Lchanged->curr != NULL)
  double changeInLabel = 0;
  int Nelements = this->dimx * this->dimy * this->dimz;
  for (int idx=0;idx<Nelements;idx++) {
    phi_val = phi[idx];
    if( (u_out < u_in) && ( img[idx] < (u_out+2*cutoff_thresh) ) )
    { // force it to be out
      phi_val = 3.0;
    }
    else if( (u_out < u_in) && ( img[idx] < (u_out+cutoff_thresh) ) )
    { // force it to be out
      phi_val = std::min( phi_val, 0.0 );
    }
    phi_out = (-phi_val + 3.0) / 6.0;
    outputVal=  (unsigned short) ( ( (phi_out > 0.95) +
                                     (phi_out > 0.8) +
                                     (phi_out > 0.65) +
                                     (phi_out > 0.5) )
                                   *mult);
    changeInLabel += fabs( (outputVal - ptrCurrLabel[idx]) > 1e-3 );
    ptrCurrLabel[idx] =outputVal;
    mask[idx]         =(double) ( 0 < outputVal ); // d'oh, *update the mask!*
  }
  cout << "m_UpdateVector Size: " << m_UpdateVector.size()
       <<  ", Lchanged size: " << LL3D.Lchanged->length
        <<  ", |change| in label: " << changeInLabel << endl;

  double spc[3];
  this->U_Integral_image->GetSpacing(spc);

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
  delete [] this->seg;
  delete [] this->iList;
  delete [] this->jList;


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

}





//////////////////////////////////////////////////////////// Gulag

#if 0
if (m_CustomSpeedImgPointer!=NULL)
{
  /* interactive_customspeed(this->m_CustomSpeedImgPointer,img,phi,ptrIntegral_Image,label,dims,
                            Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in,Lchanged,
                            iter,rad,lambda,display,this->m_PlaneNormalVector,this->m_PlaneCenter,this->m_DistWeight);*/
}
else if( m_bUseEdgeBased ) {
  interactive_edgebased_ext(img,phi,ptrIntegral_Image,label,dims,
                            Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in,Lchanged,
                            iter,rad,0.5*lambda,display,m_SatRange[0],m_SatRange[1],
                            this->m_PlaneNormalVector,this->m_PlaneCenter,this->m_DistWeight);
}

/*vtkMetaImageWriter* labelWriter=  vtkMetaImageWriter::New();
labelWriter->SetInput(createVTKImageFromPointer<double>(this->ptrIntegral_Image,this->U_Integral_image->GetDimensions(), spc) );
labelWriter->SetFileName( "0-Integral.mhd");
labelWriter->Write();

labelWriter->SetInput(this->U_Integral_image );
labelWriter->SetFileName( "0-IntegralImage.mhd");
labelWriter->Write();

labelWriter->SetInput(createVTKImageFromPointer<double>(this->ptrU_t_Image,this->U_Integral_image->GetDimensions(), spc) );
labelWriter->SetFileName( "0-U_t.mhd");
labelWriter->Write();


labelWriter->SetInput(this->U_t_image);
labelWriter->SetFileName( "0-U_t-image.mhd");
labelWriter->Write();*/


#endif
