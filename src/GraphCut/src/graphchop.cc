/* interface for the algorithm described in  GCoptimization.h. */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "jm.h"
#include "graphchop.h"
#include "GCoptimization.h"

typedef GCoptimizationGeneralGraph GeneralGraph;

#define sub2ind(r,c,d) (dims[0]*dims[1]*(d) + dims[0]*(c) + (r))

/* pre-calculated edge families */

/*-- 2D --*/
int N4[13][3] = {{ 0, 1, 0 },
              { -1,0, 0 }};
int N8[13][3] = {{ 0, 1, 0 },
              { -1, 1, 0 },
              { -1, 0, 0 },
              { -1, -1, 0 }};
int N16[13][3]= {{ 0, 1, 0 },
              {-1, 2, 0 },
              {-1, 1, 0 },
              {-2, 1, 0 },
              {-1, 0, 0 },
              {-2,-1, 0 },
              {-1,-1, 0 },
              {-1,-2, 0 }};

/*-- 3D --*/
int N6[13][3] = {{ 1, 0, 0},
              { 0, 1, 0},
              { 0, 0, 1}};
int N18[13][3]= {{ 1, 0, 0},
                 { 1, 1, 0},
                 { 0, 1, 0},
                 {-1, 1, 0},
                 { 0, 0, 1},
                 { 1, 0, 1},
                 { 0, 1, 1},
                 {-1, 0, 1},
                 { 0,-1, 1}};
int N26[13][3]= {{ 1, 0, 0},
                 { 0, 1, 0},
                 { 1, 1, 0},
                 {-1, 1, 0}, /* 4 */
                 { 0, 0, 1},
                 { 1, 0, 1},
                 { 1, 1, 1},
                 { 0, 1, 1},
                 {-1, 1, 1}, /* 9 */
                 {-1, 0, 1},
                 {-1,-1, 1},
                 { 0,-1, 1}, /* 12 */
                 { 1,-1, 1}};


/* undirected neighbor edges */
static uint n_edges(int e[3], uint dims[3], uint **P, uint **Q)
{
    uint cnt = (dims[0]-abs(e[0]))*(dims[1]-abs(e[1]))*(dims[2]-abs(e[2]));
    MALLOC_SET(uint, *P, cnt);
    MALLOC_SET(uint, *Q, cnt);

    /*- internal -*/
    const int R = e[0], C = e[1], D = e[2];
    int r_min = (R < 0)? -R : 0;
    int r_max = dims[0] - ((R < 0)? 0 : R);
    int c_min = (C < 0)? -C : 0;
    int c_max = dims[1] - ((C < 0)? 0 : C);
    int d_min = (D < 0)? -D : 0;
    int d_max = dims[2] - ((D < 0)? 0 : D);
    for (int d = d_min, i =0; d < d_max; d++) {
        for (int r = r_min; r < r_max; r++) {
            for (int c = c_min; c < c_max; c++, i++) {
                (*P)[i] = sub2ind(r, c, d) + 1;
                (*Q)[i] = sub2ind(r+R, c+C, d+D) + 1;
            }
        }
    }

    return cnt;
}



/* build graph */
static GeneralGraph *graph(uint dims[3], uint N, uint label_cnt,
                           t_weight_fn R_fn, n_weight_fn B_fn, double *V,
                           double **R_w)
{
    uint node_cnt = dims[0]*dims[1]*dims[2];
    GeneralGraph *g = new GeneralGraph(node_cnt, label_cnt);


    /*-- terminal nodes --*/
    MALLOC_SET(double, *R_w, node_cnt*label_cnt);
    for (uint lbl = 0; lbl < label_cnt; lbl++) {
        double *w_lbl = R_fn(lbl);
        for (uint d = 0; d < dims[2]; d++) {
            for (uint c = 0; c < dims[1]; c++) {
                for (uint r = 0; r < dims[0]; r++) {
                    uint i = sub2ind(r,c,d);
                    (*R_w)[i*label_cnt + lbl] = w_lbl[i];
                }
            }
        }
    }
    g->setDataCost(*R_w);


    /*-- V --*/
    g->setSmoothCost(V);


    /*-- neighboring system */
    int fam[13][3];  /* HACK */
    if (dims[2] > 1) {
        /* 3D */
        switch (N) {
        case 6:  memcpy(fam, N6, sizeof(fam)); break;
        case 18: memcpy(fam, N18, sizeof(fam)); break;
        case 26: memcpy(fam, N26, sizeof(fam)); break;
        default: assert(0 && "invalid 3D neighborhood");
        }
    } else {
        /* 2D */
        switch (N) {
        case 4:  memcpy(fam, N4, sizeof(fam)); break;
        case 8:  memcpy(fam, N8, sizeof(fam)); break;
        case 16: memcpy(fam, N16, sizeof(fam)); break;
        default: assert(0 && "invalid 2D neighborhood");
        }
    }

    for (uint k = 0; k < N/2; k++) {
        uint *n_P, *n_Q;
        uint n_cnt = n_edges(fam[k], dims, &n_P, &n_Q);
        double e[3];
        for (int i = 0; i < 3; i++)
            e[i] = fam[k][i];
        double *n_w = B_fn(e, n_P, n_Q, n_cnt);
        /* set edge weights */
        for (int i = 0; i < n_cnt; i++) {
            uint self = n_P[i] - 1;
            uint other = n_Q[i] - 1;
            g->setNeighbors(self, other, n_w[i]);
        }
        JM_FREE(n_P); JM_FREE(n_Q);
    }
    return g;
}


uint *graphchop(uint dims[3], uint N, uint label_cnt,
                t_weight_fn R_fn, n_weight_fn B_fn, double *V)
{
    /*-- build graph --*/
    double *R_w;
    GeneralGraph *g = graph(dims, N, label_cnt, R_fn, B_fn, V, &R_w);
    g->readyToOptimise();

    /*-- determine min-cut/max-flow --*/
    g->expansion(); /* run to convergence */

    /*-- form label map --*/
    uint node_cnt = dims[0]*dims[1]*dims[2];
    MALLOC_INIT(uint, map, node_cnt);
    for (uint i = 0; i < node_cnt; i++)
        map[i] = g->whatLabel(i) + 1;

    delete g;
    JM_FREE(R_w);

    return map;
}
