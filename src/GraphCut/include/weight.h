#ifndef __WEIGHT_H
#define __WEIGHT_H

#include "graphchop.h"

typedef double *(t_weight_fn)(uint label);
typedef double *(n_weight_fn)(double *e, uint *P, uint *Q, uint cnt);

#endif
