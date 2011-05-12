#ifndef __GRAPHCHOP_H
#define __GRAPHCHOP_H

#include "jm.h"
#include "weight.h"

struct point_t {
    uint r, c, d;
};

#ifdef __cplusplus
extern "C" {
#endif

uint *graphchop(uint dims[3], uint N, uint label_cnt,
                t_weight_fn R_fn, n_weight_fn B_fn, double *V);

#ifdef __cplusplus
}
#endif


#endif
