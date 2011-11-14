#include "interactive_kurvolver.h"
#include "sfm_local_chanvese_mex.h"
#include <iostream>
using std::endl;
using std::cout;

void interactive_chanvese(double *img, double *phi, double* U_integral, double *label, long *dims,
                          LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
                          int iter, double rad, double lambda, int display)
{
  double *F;
  double scale[1];
  scale[0] = 0.0;
  //initialize datastructures and statistics
  en_lrbac_init(Lz,img,phi,dims,rad);
  for(int i=0;i<iter;i++){
    // compute force
    F = en_lrbac_compute(Lz,phi,img,dims, scale,lambda,rad);

    /** TODO: currently uses approximation for input observer. port the full double-loop version from
        matlab here. Tricky because this fast C sfls code overwrites global/file scoped variables. */
    // apply controller, modify F in-place
    apply_control_function( Lz, phi, F, U_integral, img, iter, dims );

    //perform iteration
    ls_iteration(F,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in);

    //update statistics
    en_lrbac_update(img, dims, Lin2out, Lout2in, rad);

  }
  if( display > 0 )
    cout << "done sfls iters: " << iter << endl;

  en_lrbac_destroy();
}

void apply_control_function(LL *Lz,double *phi, double* F,
                            double* U_integral, double *img, int iter, long* dims )
{ // apply user's time-integrated edits inside the updates
  int x,y,z,idx,n;
  double I,dpx,dpy,dpz;
  ll_init(Lz);
  n=0;
  double gamma        = 1.0 / iter;

  while(Lz->curr != NULL){          //loop through list
    x = Lz->curr->x;
    y = Lz->curr->y;
    z = Lz->curr->z;
    idx          = Lz->curr->idx;
    I            = img[idx];
    double U     = U_integral[idx];
    double err   = ( 3.0 * tanh(U / 1.5) - phi[idx] );

    //kappa not used!?
    double kappa = en_kappa_norm_pt(Lz->curr,phi,dims,&dpx,&dpy,&dpz);
    double f     = gamma * abs(U) * (F[n] - err);

    F[n]         = F[n] - f;
    ll_step(Lz);
    n++;       //next point
  }


}
