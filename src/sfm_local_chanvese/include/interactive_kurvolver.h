#pragma once



#include "sfm_local_chanvese_mex.h"

void interactive_rbchanvese(double *img, double *phi, double* U_integral, double *label, long *dims,
                          LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
                          int iter, double rad, double lambda, int display);

void interactive_rbchanvese_ext(double *img, double *phi, double* U_integral, double *label, long *dims,
                            LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,LL*Lchanged,
                          int iter, double rad, double lambda, int display, double* normvec, double* pointonplane,float distweight);

void interactive_chanvese(double *img, double *phi, double* U_integral, double *label, long *dims,
                    LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
                    int iter, double rad, double lambda, int display);

void interactive_chanvese_ext(double *img, double *phi, double* U_integral, double *label, long *dims,
                          LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,LL* Lchanged,
                          int iter, double lambda, int display, double* normvec, double* pointonplane,float distweight=0);

/*void interactive_customspeed(double* speedimg, double *img, double *phi, double* U_integral, double *label, long *dims,
                          LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,LL* Lchanged,
                          int iter, double rad, double lambda, int display, double* normvec, double* pointonplane,float distweight=0);*/

void interactive_edgebased(double *img, double *phi, double* U_integral, double *label, long *dims,
                    LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
                    int iter, double rad, double lambda, int display,
                    double ImgMin = -1e9, double ImgMax = 1e9);

void interactive_edgebased_ext(double *img, double *phi, double* U_integral, double *label, long *dims,
                    LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,LL* Lchanged,
                    int iter, double rad, double lambda, int display, double ImgMin, double ImgMax, double* normvec, double* pointonplane,float distweight);

void apply_control_function(LL *Lz,double *phi, double* F,
                            double* U_integral,double *img, int iter, long* dims);

void apply_control_function_ext(LL *Lz,double *phi, double* F,
                            double* U_integral, double *img, int iter, long* dims, double* normal,double* poP,float distWeight=0 );
