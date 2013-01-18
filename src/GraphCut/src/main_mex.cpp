#include <stdio.h>
#include <string.h>
#include "jm.h"
#include "graphchop.h"
#include "mex.h"

int main()
{
}
static bool usage(const char *msg)
{
    mexPrintf("error: %s\n"
              "\n"
              "usage: \n"
              " map = graphchop(dims, N, label_cnt, R, B, V)\n"
              "   map - labeling\n"
              "   dims=[r c d]\n"
              "   N from {4,8,26} in 2D or {6,18,26} in 3D\n"
              "   label_cnt - number of labels to use\n"
              "   R - regional term function (given label)\n"
              "   B - shared boundary term function\n"
              "   V - relative label-to-label penalties\n",
              msg);
    return false;
}


/* 1 iff scalar */
static int is_scalar(const mxArray *m)
{
    const int dims_cnt = mxGetNumberOfDimensions(m);
    const mwSize *dims = mxGetDimensions(m);
    return dims_cnt == 2 && dims[0] == 1 && dims[1] == 1;
}


static bool check_usage(int nlhs, mxArray *plhs[],
                        int nrhs, const mxArray *prhs[])
{

    /* number of parameters */
    if (nrhs != 6)
        return usage("wrong number of inputs");
    /* number of returns */
    if (nlhs != 1)
        return usage("wrong number of outputs");


    /* dims */
    int dims_cnt = mxGetNumberOfDimensions(prhs[0]);
    const mwSize *dims = mxGetDimensions(prhs[0]);
    if (dims_cnt != 2 || !(dims[0] == 1 && (dims[1] == 2 || dims[1] == 3)))
        return usage("dims must be row vector: [r c] or [r c d]");

    const double *dim = mxGetPr(prhs[0]);
    int is_3D = (dims[1] == 3) && dim[2] > 1;
    if (!(dim[0] > 1 && dim[1] > 1 && (!is_3D || dim[2] > 1)))
        return usage("dim [r c] or [r c d] for r,c at least 2, d positive");

    /* N */
    if (!is_scalar(prhs[1]))
        return usage("N must be a scalar");
    uint N = (uint)mxGetScalar(prhs[1]);
    if (is_3D && !(N == 6 || N == 18 || N == 26))
        return usage("only neighborhoods of 6,18,26 supported in 3D");
    else if (!is_3D && !(N == 4 || N == 8 || N == 16))
        return usage("only neighborhoods of 4,8,16 supported in 2D");


    /* label_cnt */
    if (!is_scalar(prhs[2]))
        return usage("label_cnt must be scalar");
    if (mxGetScalar(prhs[2]) <= 1)
        return usage("must have at least two labels");

    /* R */
    if (mxGetClassID(prhs[3]) != mxFUNCTION_CLASS)
        return usage("R must be a function handle");

    /* B */
    if (mxGetClassID(prhs[4]) != mxFUNCTION_CLASS)
        return usage("B must be a function handle");

    /* V */
    if (!mxIsDouble(prhs[5]))
        return usage("V must be of type double");
    if (mxGetNumberOfDimensions(prhs[5]) != 2)
        return usage("V must be a matrix of n-by-n");
    const mwSize *V_dims = mxGetDimensions(prhs[5]);
    if (V_dims[0] != V_dims[1])
        return usage("V must be of size label_cnt x label_cnt");

    return true; /* passed inspection */
}



static mxArray *m_R_fn;
static double *V;
static mxArray **m_R_w;
static mxArray *m_label;
static double *label;
static double *R_fn(uint lbl)
{
    *label = lbl + 1;
    mxArray *prhs[] = { m_R_fn, m_label };
    mexCallMATLAB(1, &m_R_w[lbl], 2, prhs, "feval");
    ASSERT(mxIsDouble(m_R_w[lbl]));
    return mxGetPr(m_R_w[lbl]);
}


static mxArray *m_B_fn;
static mxArray *m_e;
static mxArray *m_B_w;
static double *B_fn(double *e, uint *P, uint *Q, uint cnt)
{
    mxArray *m_P = mxCreateNumericMatrix(1, cnt, mxUINT32_CLASS, mxREAL);
    mxArray *m_Q = mxCreateNumericMatrix(1, cnt, mxUINT32_CLASS, mxREAL);
    memcpy(mxGetData(m_e), e, 3*sizeof(*e)); /* OPT: setdata? */
    memcpy(mxGetData(m_P), P, cnt*sizeof(*P));
    memcpy(mxGetData(m_Q), Q, cnt*sizeof(*Q));
    mxArray *prhs[] = { m_B_fn, m_e, m_P, m_Q };
    mexCallMATLAB(1, &m_B_w, 4, prhs, "feval");
    ASSERT(mxIsDouble(m_B_w));
    mxDestroyArray(m_P);
    mxDestroyArray(m_Q);
    return mxGetPr(m_B_w);
}



void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{
    /*-- input --*/
    const mwSize *dim = mxGetDimensions(prhs[0]);
    int is_3D = (dim[1] == 3);
    double *d_dims = mxGetPr(prhs[0]);
	
	mexSetTrapFlag(1); /* bail to Matlab prompt on mexCallMATLAB error control MATLAB's response to errors in mexCallMATLAB, 1=>On error, control returns to your MEX-file

    /*-- check command arguments --*/
    if (check_usage(nlhs, plhs, nrhs, prhs) == 0) //check that 
        return;



    uint dims[3] = { d_dims[0], d_dims[1], (is_3D? d_dims[2] : 1) };
    mwSize dims_m[3] = { d_dims[0], d_dims[1], (is_3D? d_dims[2] : 1) };
    uint N = mxGetScalar(prhs[1]);
    uint label_cnt = mxGetScalar(prhs[2]);
    m_R_fn = (mxArray *)prhs[3];
    m_B_fn = (mxArray *)prhs[4];
    V = mxGetPr(prhs[5]);


    /*-- weight calculation call backs --*/
    m_label = mxCreateDoubleScalar(-1);
    label = mxGetPr(m_label);
    MALLOC_SET(mxArray *, m_R_w, label_cnt);


    /* compute neighbor weights */
    m_e = mxCreateNumericMatrix(3, 1, mxDOUBLE_CLASS, mxREAL);


    /*-- graph cut --*/
    uint *map = graphchop(dims, N, label_cnt, R_fn, B_fn, V);

    /*-- output --*/
    plhs[0] = mxCreateNumericArray(3, dims_m, mxUINT32_CLASS, mxREAL);
    memcpy(mxGetData(plhs[0]), map, dims[0]*dims[1]*dims[2]*sizeof(*map));

    /*-- clean up --*/
    for (uint i = 0; i < label_cnt; i++)
        mxDestroyArray(m_R_w[i]);
    JM_FREE(m_R_w);
    mxDestroyArray(m_B_w);
    mxDestroyArray(m_label);
    mxDestroyArray(m_e);
    JM_FREE(map);
}
