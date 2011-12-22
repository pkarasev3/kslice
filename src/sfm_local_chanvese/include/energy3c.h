#ifndef __ENERGY3C_H
#define __ENERGY3C_H

#include <stdlib.h>
#include "llist.h"
#include "sparse3c.h"

// functions to minimize edge-based (kslice-CT) energy
double *en_edgebased_compute(LL *Lz,double *phi, double *img, long *dims, double *scale, double lam, double rad, double ImgMin, double ImgMax);

// functions to minimize lrbac (vessel, yezzi) energy
double *en_lrbac_vessel_yz_compute(LL *Lz,double *phi, double *img, long *dims, double *scale, double lam, double rad, double dthresh);
void en_lrbac_vessel_yz_init_point(double* img, double* phi, int idx, int x, int y, int z, long *dims, double rad, double dthresh);
void en_lrbac_vessel_yz_update(double* img, long *dims, LL *Lin2out, LL *Lout2in, double rad, double dthresh);

// functions to minimize lrbac (vessel, chanvese) energy
double *en_lrbac_vessel_cv_compute(LL *Lz,double *phi, double *img, long *dims, double *scale, double lam, double rad, double dthresh);
void en_lrbac_vessel_cv_init_point(double* img, double* phi, int idx, int x, int y, int z, long *dims, double rad, double dthresh);
void en_lrbac_vessel_cv_update(double* img, long *dims, LL *Lin2out, LL *Lout2in, double rad, double dthresh);

// functions to minimize lrbac (chanvese) energy
double *en_lrbac_compute(LL *Lz,double *phi, double *img, long *dims, double *scale, double lam, double rad);
void en_lrbac_init(LL *Lz, double *img,double *phi, long *dims, double rad);
void en_lrbac_init_point(double* img, double* phi, int idx, int x, int y, int z, long *dims, double rad);
double *en_lrbac_gball(double rad);
void en_lrbac_destroy();
void en_lrbac_update(double* img, long *dims, LL *Lin2out, LL *Lout2in, double rad);


// functions to minimize bhattacharyya energy
double *en_bhattacharyya_compute(LL *Lz,double *phi, double *img, long *dims, double *scale, double lam);
void en_bhattacharyya_init(double *img, double *phi, long *dims);
void en_bhattacharyya_update(double* img, long *dims, LL *Lin2out, LL *Lout2in);
void en_bhattacharyya_destroy();


// functions to minimize user bhattacharyya energy
void en_user_bhattacharyya_init(double* img, double* phi, long *dims,double* seed);
double *en_user_bhattacharyya_compute(LL *Lz, double *phi, double *img, double penaltyAlpha, long *dims, double *scale, double lam);
void en_user_bhattacharyya_destroy();

// function to minimize user chan vese
double *en_user_chanvese_compute(LL *Lz, double *phi, double *img,double penaltyAlpha, long *dims, double *scale, double lam);
void en_user_chanvese_init(double* img, double* phi, long *dims, double* seed);

//functions to minimize user local global constrained chan-vese
double *en_lrbac_user_compute(LL *Lz,double *phi, double *img,double penaltyAlpha, long *dims, double *scale, double lam, double rad);
void en_lrbac_user_init(LL *Lz,double *img,double *phi, long *dims, double rad, double* seed);

// functions to minimize mean&variance energy
double *en_meanvar_compute(LL *Lz,double *phi, double *img, long *dims, double *scale, double lam);
void en_meanvar_init(double *img, double *phi, long *dims);
void en_meanvar_update(double* img, long *dims, LL *Lin2out, LL *Lout2in);

// functions to minimize chan-vese energy
double *en_chanvese_compute(LL *Lz,double *phi, double *img, long *dims, double *scale, double lam);
void en_chanvese_init(double *img, double *phi, long *dims);
void en_chanvese_update(double* img, long *dims, LL *Lin2out, LL *Lout2in);


double *en_custom_compute(LL* Lz, double* speedimg,double *phi,  long *dims,double *scale, double lam);

// functions to minimize RGB vector valued chan-vese energy
double *en_chanvese_rgb_compute(LL *Lz,double *phi, double *img, long *dims, double *scale, double lam);
void en_chanvese_rgb_init(double *img, double *phi, long *dims);
void en_chanvese_rgb_update(double* img, long *dims, LL *Lin2out, LL *Lout2in);

// functions to minimize constrained yezzi energy
double *en_yezzi_compute(LL *Lz,double *phi, double *img,long *dims,double *scale, double lam);
void en_yezzi_init(LL* Lz, double *img, double *phi, long *dims);
void en_yezzi_update(double* img, long *dims, LL *Lin2out, LL *Lout2in);

// function to grow contour (uses en_null_update)
double *en_grow_compute(LL *Lz,double *img, double* phi, long *dims, double lam, double dthresh);

// function to shrink contour w/o collapsing (uses en_null_update)
double *en_shrink_compute(LL *Lz, double* img, double* phi, long *dims,double rad, double lam, double *scale);

// function to smooth contour with curvature (uses en_null_update)
double *en_kappa_compute(LL *Lz,double *phi,long *dims);

// clears update lists for energies that don't require updates
void en_null_update(double* img, long *dims, LL *Lin2out, LL *Lout2in);

// returns curvature (kappa) at the list point p (x,y,z)
double en_kappa_pt(PT* p, double *phi, long *dims);

// returns curvature (kappa) at p (x,y,z) and sets dx and dy values for the norm
double en_kappa_norm_pt(PT* p, double *phi, long *dims, double *dx, double *dy, double *dz);

//display histogram function
void draw_histogram(double *pdf,char* pdfInFileName, double a, int display);
void display_slice(double *img,int *dims, int sliceNum, char *name, double *imgRange );

//compute min/max in image to build histogram correctly
void findMinMax(double *img,long *dims);
void smoothHist(double *pdfRough, double *pdfSmooth );

//int round(double r);
#endif
