#ifndef __LSOPS3C_H
#define __LSOPS3C_H

#include "llist.h"
#include "sparse3c.h"
#include <vector>

void ls_iteration(double *F, float *phi, short *label, long* dims,
                  LL* Lz, LL* Ln1, LL* Lp1, LL *Ln2, LL *Lp2, 
                  LL *Lin2out, LL* Lout2in);

void ls_iteration_ext(double *F, float *phi, short *label, long* dims,
                  LL* Lz, LL* Ln1, LL* Lp1, LL *Ln2, LL *Lp2,
                  LL *Lin2out, LL* Lout2in,LL* Lchanged);

void ls_mask2phi3c(short *mask, float *phi, short *label, long* dims,
                   LL* Lz, LL* Ln1, LL* Ln2, LL* Lp1, LL* Lp2);

void ls_mask2phi3c_ext(short *mask, float *phi, short *label, long* dims,
                   LL* Lz, LL* Ln1, LL* Ln2, LL* Lp1, LL* Lp2,LL* Lchanged);

void ls_mask2phi3c_update(const std::vector< unsigned int >& updateVector,
                          const std::vector< std::vector<unsigned int> >& coordVector,
                          short *mask, float *phi, short *label, long* dims,
                          LL *Lz, LL *Ln1, LL *Ln2, LL *Lp1, LL *Lp2,LL* Lchanged);

double ls_min_hood_onlevel(int idx, long x, long y, long z, long *dims, 
                           float *phi,short *label, double level);

double ls_max_hood_onlevel(int idx, long x, long y, long z, long *dims, 
                           float *phi,short *label, double level);


#endif
