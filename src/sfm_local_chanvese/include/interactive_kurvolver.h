#pragma once



#include "sfm_local_chanvese_mex.h"

void interactive_chanvese(double *img, double *phi, double* U_integral, double *label, long *dims,
                    LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
                    int iter, double rad, double lambda, int display);


void apply_control_function(LL *Lz,double *phi, double* F,
                            double* U_integral,double *img, int iter, long* dims);
