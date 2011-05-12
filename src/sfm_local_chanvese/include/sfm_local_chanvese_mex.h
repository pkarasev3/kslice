#ifndef SFM_LOCAL_CHANVESE_MEX_H
#define SFM_LOCAL_CHANVESE_MEX_H

#include "sparse3c.h"

//------------------------------
// IMPLEMENTED ENERGIES
//-----------------------------
//lrbac_vessel_yz(img,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in,iter,rad,lambda,dthresh,plhs,display);
//lrbac_vessel_cv(img,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in,iter,rad,lambda,dthresh,plhs,display);
//lrbac_chanvese( img,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in,iter,rad,lambda,plhs,display);
//bhattacharyya(  img,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in,iter,lambda,plhs,display);
//chanvese(       img,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in,iter,lambda,plhs,display);
//meanvar(        img,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in,iter,lambda,plhs,display);
//yezzi(          img,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in,iter,lambda,plhs,display);
//grow(           img,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in,iter,lambda,plhs,display);
//shrink(         img,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in,iter,rad,lambda,plhs,display);
//kappa(          img,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in,iter,lambda,plhs,display);
//user_bhattacharyya(img,phi,penaltyAlpha,seed,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in,iter,lambda,plhs,display);
//user_chanvese(       img,phi,penaltyAlpha,seed,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in,iter,lambda,plhs,display);
//lrbac_user_chanvese( img,phi,penaltyAlpha,seed,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in,iter,rad,lambda,plhs,display);


//[phi C L] = ls_sparse(img,mask,iterations,display,lam,rad,dthresh);

#endif // SFM_LOCAL_CHANVESE_MEX_H
