#pragma once


#include "sparse3c.h"
#include "lsops3c.h"
//#include "sfm_local_chanvese_mex.h"

void interactive_rbchanvese(energy3c *segEngine, double *img, float *phi, short *U_integral, short *label, long *dims,
                          LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
                          int iter, double lambda, int display, bool firstPass);

void interactive_rbchanvese_ext(energy3c* segEngine, double *img, float *phi, short *U_integral, short *label, long *dims,
                            LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in, LL*Lchanged,
                          int iter, double lambda, int display, double* normvec, double* pointonplane, float distweight);

void interactive_chanvese(energy3c *segEngine, double *img, float *phi, short *U_integral, short *label, long *dims,
                    LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
                    int iter, double rad, double lambda, int display);

void interactive_chanvese_ext(energy3c *segEngine, double *img, float *phi, short *U_integral, short *label, long *dims,
                          LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in, LL* Lchanged,
                          int iter, double lambda, int display, double* normvec, double* pointonplane, float distweight=0);

/*void interactive_customspeed(double* speedimg, double *img, float *phi, short *U_integral, short *label, long *dims,
                          LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,LL* Lchanged,
                          int iter, double rad, double lambda, int display, double* normvec, double* pointonplane,float distweight=0);*/

void interactive_edgebased(double *img, float *phi, short *U_integral, short *label, long *dims,
                    LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
                    int iter, double rad, double lambda, int display,
                    double ImgMin = -1e9, double ImgMax = 1e9);

void interactive_edgebased_ext(double *img, float *phi, short *U_integral, short *label, long *dims,
                    LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,LL* Lchanged,
                    int iter, int *rad, double lambda, int display, double ImgMin, double ImgMax, double* normvec, double* pointonplane,float distweight);

void apply_control_function(LL *Lz, float *phi, float *F,
                            short *U_integral, double *img, int iter, long* dims);

void apply_control_function_ext(LL *Lz, float *phi, float *F,
                            short *U_integral, double *img, int iter, long* dims, double* normal, double* poP, float distWeight=0 );
