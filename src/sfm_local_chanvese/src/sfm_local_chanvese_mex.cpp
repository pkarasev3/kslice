/*********************************************************************
 * sfm_local_chanvese_mex.cpp
 *
 * This file performs the statistical computations needed for running 
 * the localized chan-vese active contour segmentation energy using the 
 * Sparse Field method presented by Whitaker.
 * 
 * written by: Shawn Lankton (4/17/2009) - www.shawnlankton.com
 *
 ********************************************************************/
#include "sfm_local_chanvese_mex.h"
#include <iostream>
#include <fstream>

using std::endl;
using std::cout;
using std::string;

//[phi C L] = ls_sparse(img,mask,iterations,display,lam,rad,dthresh);

void lrbac_vessel_yz(double *img, double *phi, double *label, long *dims,
                     LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
                     int iter, double rad, double lambda, double dthresh, short *plhs,
                     int display){
  double *F;
  double scale[1]; scale[0]=0;
  int countdown;

  //initialize datastructures and statistics
  en_lrbac_init(Lz,img,phi,dims,rad);
  for(int i=0;i<iter;i++){
    //compute force
    F = en_lrbac_vessel_yz_compute(Lz,phi,img,dims, scale,lambda,rad,dthresh);
    //perform iteration
    ls_iteration(F,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in);
    //update statistics
    en_lrbac_vessel_yz_update(img, dims, Lin2out, Lout2in, rad,dthresh);

  }
  //destroy old datastructures
  en_lrbac_destroy();
}

void lrbac_vessel_cv(double *img, double *phi, double *label, long *dims,
                     LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
                     int iter, double rad, double lambda, double dthresh, short *plhs,
                     int display){
  double *F;
  double scale[1]; scale[0]=0;
  int countdown;

  //initialize datastructures and statistics
  en_lrbac_init(Lz,img,phi,dims,rad);
  for(int i=0;i<iter;i++){
    //compute force
    F = en_lrbac_vessel_cv_compute(Lz,phi,img,dims, scale,lambda,rad,dthresh);
    //perform iteration
    ls_iteration(F,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in);
    //update statistics
    en_lrbac_vessel_cv_update(img, dims, Lin2out, Lout2in, rad,dthresh);

    //display stuff (maybe)
//    if(display) show_countdown(iter,i,&countdown,plhs);
  }
  //destroy old datastructures
  en_lrbac_destroy();
}

void lrbac_chanvese(double *img, double *phi, double *label, long *dims,
                    LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
                    int iter, double rad, double lambda,  short *plhs,int display){
  double *F;
  double scale[1]; scale[0]=0;
  int countdown;
  //initialize datastructures and statistics
  en_lrbac_init(Lz,img,phi,dims,rad);
  double Fmax;
  for(int i=0;i<iter;i++){
    //compute force

    F = en_lrbac_compute(Lz,phi,img,dims, scale,lambda,rad);
    //perform iteration
    ls_iteration(F,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in);
    //update statistics
    en_lrbac_update(img, dims, Lin2out, Lout2in, rad);

    //display stuff (maybe)

  }
  if( display > 0 )
    std::cout << "done sfls iters: " << iter << endl;
  //destroy old datastructures
  en_lrbac_destroy();
}

void chanvese(double *img, double *phi, double *label, long *dims,
              LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
              int iter,double lambda,  short *plhs,int display){
  double *F;
  double scale[1]; scale[0] = 0;
  int countdown;

  //initialize datastructures and statistics
  en_chanvese_init(img,phi,dims);
  for(int i=0;i<iter;i++){
    //compute force
    F = en_chanvese_compute(Lz,phi,img,dims,scale,lambda);
    //perform iteration
    ls_iteration(F,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in);
    //update statistics
    en_chanvese_update(img, dims, Lin2out, Lout2in);

    //display stuff (maybe)
    if(display){
        if ( (i % display)==0) {
            std::cout<<"This is iteration # "<<i<<std::endl;
        }
    }
  }
}


void user_chanvese(double *img, double *phi,double penaltyAlpha, double *seed, double *label, long *dims,
              LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
              int iter,double lambda,  short *plhs,int display){
  double *F;
  double scale[1]; scale[0] = 0;
  int countdown;

  //initialize datastructures and statistics
  en_user_chanvese_init(img,phi,dims,seed);
  for(int i=0;i<iter;i++){
    //compute force
    F = en_user_chanvese_compute(Lz,phi,img,penaltyAlpha,dims,scale,lambda);
    //perform iteration
    ls_iteration(F,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in);
    //update statistics
    en_chanvese_update(img, dims, Lin2out, Lout2in); //stays same as for regular chan-vese

    //display stuff (maybe)
    if(display){
        if ( (i % display)==0) {
            std::cout<<"This is iteration # "<<i<<std::endl;
        }
    }

  }
}

void lrbac_user_chanvese(double *img, double *phi,double penaltyAlpha, double *seed, double *label, long *dims,
                    LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
                    int iter, double rad, double lambda,  short *plhs,int display){
  double *F;
  double scale[1]; scale[0]=0;
  int countdown;

  //initialize datastructures and statistics
  en_lrbac_user_init(Lz,img,phi,dims,rad,seed);
  for(int i=0;i<iter;i++){
    //compute force
    F = en_lrbac_user_compute(Lz,phi,img,penaltyAlpha, dims, scale,lambda,rad);
    //perform iteration
    ls_iteration(F,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in);
    //update statistics
    en_lrbac_update(img, dims, Lin2out, Lout2in, rad);

    //display stuff (maybe)
//    if(display==1) show_countdown(iter,i,&countdown,plhs);
    std::cout<<"This is iteration # "<<i<<std::endl;
    if(display){
        if ( (i % display)==0) {
            std::cout<<"This is iteration # "<<i<<std::endl;
        }
    }

//    if (i==20){
//    std::ofstream myfile_mean_in("mean_in.txt");
//    std::ofstream myfile_mean_out("mean_out.txt");


//    for(int i=0; i<NUMEL;i++){
//        myfile_mean_in <<Sin[i]/Ain[i]<<std::endl;
//        myfile_mean_out <<Sout[i]/Aout[i]<<std::endl;
//    }

//    myfile_mean_in.close();
//    myfile_mean_out.close();
//    }

  }


  //destroy old datastructures
  en_lrbac_destroy();
  en_user_bhattacharyya_destroy();  
}



void meanvar(double *img, double *phi, double *label, long *dims,
             LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
             int iter,double lambda,  short *plhs,int display){
  double *F;
  double scale[1]; scale[0] = 0;
  int countdown;

  //initialize datastructures and statistics
  en_meanvar_init(img,phi,dims);
  for(int i=0;i<iter;i++){
    //compute force
    F = en_meanvar_compute(Lz,phi,img,dims,scale,lambda);
    //perform iteration
    ls_iteration(F,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in);
    //update statistics
    en_meanvar_update(img, dims, Lin2out, Lout2in);

    //display stuff (maybe)
//    if(display) show_countdown(iter,i,&countdown,plhs);
  }
}

void bhattacharyya(double *img, double *phi, double *label, long *dims,
                   LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
                   int iter,double lambda,  short *plhs,int display){
  double *F;
  double scale[1]; scale[0] = 0;
  int countdown;

  //std::cout<<"Entered in Bhattacharyya"<<std::endl;


  //initialize datastructures and statistics
  en_bhattacharyya_init(img,phi,dims);
  for(int i=0;i<iter;i++){
    //compute force
    F = en_bhattacharyya_compute(Lz,phi,img,dims,scale,lambda);
    //perform iteration
    ls_iteration(F,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in);
    //update statistics
    en_bhattacharyya_update(img, dims, Lin2out, Lout2in);

  }
  en_bhattacharyya_destroy();
}



void user_bhattacharyya(double *img, double *phi, double penaltyAlpha, double *seed, double *label, long *dims,
                   LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
                   int iter,double lambda,  short *plhs,int display){
  double *F;
  double scale[1]; scale[0] = 0;
  int countdown;

  std::cout<<"The display is set to: "<<display<<std::endl;
//  if(display==1){
//        for (int i=0;i<dims[2];i++){
//          display_slice(img,dims,i,"image");
//          display_slice(phi,dims,i,"label");
//        }
//  }


  std::cout<<"Entered in User Bhattacharyya"<<std::endl;

  //initialize datastructures and statistics
  en_user_bhattacharyya_init(img,phi,dims,seed);

  for(int i=0;i<iter;i++){
    //compute force
    F = en_user_bhattacharyya_compute(Lz,phi,img, penaltyAlpha,dims,scale,lambda); //working here
    //perform iteration
    ls_iteration(F,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in);
    //update statistics
    en_bhattacharyya_update(img, dims, Lin2out, Lout2in); //nothing changes in the updates from bhattacharyya

  }
  std::cout<<"Completed Segementation"<<std::endl;
  en_user_bhattacharyya_destroy();
}

void yezzi(double *img, double *phi, double *label, long *dims,
           LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
           int iter,double lambda,  short *plhs,int display){
  double *F;
  double scale[1]; scale[0]=0;
  int countdown;

  //initialize datastructures and statistics
  en_yezzi_init(Lz,img,phi,dims);
  for(int i=0;i<iter;i++){
    //compute force
    F = en_yezzi_compute(Lz,phi,img,dims,scale,lambda);
    //perform iteration
    ls_iteration(F,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in);
    //update statistics
    en_yezzi_update(img, dims, Lin2out, Lout2in);

    //display stuff (maybe)
    //    if(display) show_countdown(iter,i,&countdown,plhs);
  }
}

void grow(double *img, double *phi, double *label, long *dims,
           LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
          int iter,double lambda,  short *plhs,int display){
  double *F;
  int countdown;

  for(int i=0;i<iter;i++){
    //compute force
    F = en_grow_compute(Lz, img, phi,dims,lambda, 800);
    //perform iteration
    ls_iteration(F,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in);
    //update statistics
    en_null_update(img, dims, Lin2out, Lout2in);

    //display stuff (maybe)
//    if(display) show_countdown(iter,i,&countdown,plhs);
  }
}

void shrink(double *img, double *phi, double *label, long *dims,
            LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
            int iter,double rad,double lambda, short *plhs,int display){
  double *F;
  int countdown;
  double scale[1]; scale[0] = 0;

  en_lrbac_init(Lz,img,phi,dims,rad);
  for(int i=0;i<iter;i++){
    //compute force
    F = en_shrink_compute(Lz,img,phi,dims,rad,lambda,scale);
    //perform iteration
    ls_iteration(F,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in);
    //update statistics
    en_lrbac_update(img, dims, Lin2out, Lout2in, rad);

    //display stuff (maybe)
//    if(display) show_countdown(iter,i,&countdown,plhs);
  }
  //destroy old datastructures
  en_lrbac_destroy();
}

void kappa(double *img, double *phi, double *label, long *dims,
           LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
           int iter,double lambda,  short *plhs,int display){
  double *F;
  int countdown;

  for(int i=0;i<iter;i++){
    //compute force
    F = en_kappa_compute(Lz,phi,dims);
    //perform iteration
    ls_iteration(F,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in);
    //update statistics
    en_null_update(img, dims, Lin2out, Lout2in);

    //display stuff (maybe)
//    if(display) show_countdown(iter,i,&countdown,plhs);
  }
}

void prep_C_output(LL *Lz,long *dims,double *phi, short **iList, short **jList, long &lengthZLS){
  int n = 0;
  ll_init(Lz);

  if(Lz==NULL){
    iList=NULL;
    jList=NULL;
    lengthZLS=0;
  }else{
    *iList=(short*) malloc(Lz->length*sizeof(short));
    *jList=(short*) malloc(Lz->length*sizeof(short));
    lengthZLS= (Lz->length);
  }

  while(Lz->curr != NULL){
    (*iList)[n] =(short)(Lz->curr->x);
    (*jList)[n] =(short)(Lz->curr->y);
    ll_step(Lz);
    n++;
  }
  return;
}

