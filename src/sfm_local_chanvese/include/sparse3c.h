#ifndef __SPARSE3C_H
#define __SPARSE3C_H

#include <math.h>
#include "llist.h"
#include "lsops3c.h"
#include "energy3c.h"

//void show_countdown(int iter,int i, int *countdown, mxArray **ppphi);

void prep_C_output(LL *Lz,long *dims,double *phi, short **iList, short **jList, long &lengthZLS);

// "Soft Plaque Detection and Vessel Segmentation"
// - Lankton, et. al.
void lrbac_vessel_yz(double *img, double *phi, double *label, long *dims,
                  LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
                  int iter, double rad, double lambda, double dthresh, short *plhs,
                  int display);

// "Soft Plaque Detection and Vessel Segmentation"
// - Lankton, et. al.
void lrbac_vessel_cv(double *img, double *phi, double *label, long *dims,
                  LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
                  int iter, double rad, double lambda, double dthresh, short *plhs,
                  int display);

// "Localizing Region-based Active Contours"
// - Lankton, et. al.
void lrbac_chanvese(double *img, double *phi, double *label, long *dims,
                    LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
                    int iter, double rad, double lambda,  short *plhs,int display = 0);

// "Image Segmentation Using Active Contours Driven by the Bhattacharyya
// Gradient Flow" - Michailovich, et al.
void bhattacharyya(double *img, double *phi, double *label, long *dims,
                   LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
                   int iter,double lambda,  short *plhs,int display);

// "Active Contours Without Edges" - Chan and Vese
void chanvese(double *img, double *phi, double *label, long *dims,
              LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
              int iter,double lambda,  short *plhs,int display);

// "Active Contours Without Edges" - Chan and Vese
//  img is assumed to be [ r0 g0 b0 r1 g1 b1 ... ]
void chanvese_rgb(double *img, double *phi, double *label, long *dims,
              LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
              int iter,double lambda,  short *plhs,int display);

// "A Variational Framework for Active and Adaptive Segmentation of Vector
// Valued Images" - Rousson and Deriche
void meanvar(double *img, double *phi, double *label, long *dims,
             LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
             int iter,double lambda,  short *plhs,int display);

// "A Fully Global Approach to Image Segmentation via Coupled Curve Evolution
// Equations" - Yezzi, et. al.
void yezzi(double *img, double *phi, double *label, long *dims,
           LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
           int iter,double lambda,  short *plhs,int display);

void grow(double *img, double *phi, double *label, long *dims,
           LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
           int iter,double lambda,  short *plhs,int display);

// "Soft Plaque Detection and Vessel Segmentation:
// - Lankton et. al.
void shrink(double *img, double *phi, double *label, long *dims,
           LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
            int iter,double rad,double lambda, short *plhs,int display);

void kappa(double *img, double *phi, double *label, long *dims,
           LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
           int iter,double lambda,  short *plhs,int display);


void user_bhattacharyya(double *img, double *phi, double penaltyAlpha, double *seed, double *label, long *dims,
                   LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
                   int iter,double lambda,  short *plhs,int display);

void user_chanvese(double *img, double *phi,double penaltyAlpha, double *seed, double *label, long *dims,
              LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
              int iter,double lambda,  short *plhs,int display);

void lrbac_user_chanvese(double *img, double *phi,double penaltyAlpha, double *seed, double *label, long *dims,
                    LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
                    int iter, double rad, double lambda,  short *plhs,int display);
#endif
