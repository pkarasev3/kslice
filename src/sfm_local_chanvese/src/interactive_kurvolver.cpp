#include "interactive_kurvolver.h"
#include "sfm_local_chanvese_mex.h"
#include <iostream>
using std::endl;
using std::cout;


 //

void interactive_edgebased(double *img, double *phi, double* U_integral, double *label, long *dims,
                    LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
                    int iter, double rad, double lambda, int display, double ImgMin, double ImgMax )
{
    double *F;
    double scale[1];
    scale[0] = 0.0;
    //initialize datastructures and statistics
    en_lrbac_init(Lz,img,phi,dims,rad);
    for(int i=0;i<iter;i++){
      // compute force
      F = en_edgebased_compute(Lz,phi,img,dims, scale,lambda,rad, ImgMin, ImgMax);

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

void interactive_edgebased_ext(double *img, double *phi, double* U_integral, double *label, long *dims,
                    LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,LL* Lchanged,
                    int iter, double rad, double lambda, int display, double ImgMin, double ImgMax, double* normvec, double* pointonplane,float distweight)
{
    double *F;
    double scale[1];
    scale[0] = 0.0;
    //initialize datastructures and statistics
    en_lrbac_init(Lz,img,phi,dims,rad);
    for(int i=0;i<iter;i++){
      // compute force
      F = en_edgebased_compute(Lz,phi,img,dims, scale,lambda,rad, ImgMin, ImgMax);

      /** TODO: currently uses approximation for input observer. port the full double-loop version from
          matlab here. Tricky because this fast C sfls code overwrites global/file scoped variables. */
      // apply controller, modify F in-place
      apply_control_function_ext( Lz, phi, F, U_integral, img, iter, dims,normvec, pointonplane,distweight );

      //perform iteration
      ls_iteration_ext(F,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in,Lchanged);

      //update statistics
      en_lrbac_update(img, dims, Lin2out, Lout2in, rad);

    }
    if( display > 0 )
      cout << "done sfls iters: " << iter << endl;

    en_lrbac_destroy();
}


void interactive_rbchanvese(double *img, double *phi, double* U_integral, double *label, long *dims,
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

  //en_lrbac_destroy();
}

void interactive_rbchanvese_ext(double *img, double *phi, double* U_integral, double *label, long *dims,
                            LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,LL*Lchanged,
                          int iter, double rad, double lambda, int display, double* normvec, double* pointonplane,float distweight)
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
    apply_control_function_ext( Lz, phi, F, U_integral, img, iter, dims,normvec, pointonplane,distweight );

    //perform iteration
    ls_iteration_ext(F,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in,Lchanged);

    //update statistics
    en_lrbac_update(img, dims, Lin2out, Lout2in, rad);

  }
  if( display > 0 )
    cout << "done sfls iters: " << iter << endl;

  en_lrbac_destroy();
}


void interactive_chanvese(double *img, double *phi, double* U_integral, double *label, long *dims,
                          LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,
                          int iter, double rad, double lambda, int display)
{
    double *F;
    double scale[1]; scale[0] = 0;
    int countdown;

    //initialize datastructures and statistics
    en_chanvese_init(img,phi,dims);
    for(int i=0;i<iter;i++){
      //compute force
      F = en_chanvese_compute(Lz,phi,img,dims,scale,lambda);

      // apply controller, modify F in-place
      apply_control_function( Lz, phi, F, U_integral, img, iter, dims );
      //perform iteration
      ls_iteration(F,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in);
      //update statistics
      en_chanvese_update(img, dims, Lin2out, Lout2in);

      //display stuff (maybe)
      if(display){
          if ( (i % display)==0) {
              std::cout<<"This is iteration # "<<i<<std::endl;
          }
      }
    }
}

void interactive_chanvese_ext(double *img, double *phi, double* U_integral, double *label, long *dims,
                          LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,LL* Lchanged,
                          int iter, double lambda, int display, double* normvec, double* pointonplane,float distWeight)
{
    double *F;
    double scale[1]; scale[0] = 0;

    //initialize datastructures and statistics
    en_chanvese_init(img,phi,dims);
    for(int i=0;i<iter;i++){
      //compute force
      F = en_chanvese_compute(Lz,phi,img,dims,scale,lambda);
      // apply controller, modify F in-place
      apply_control_function_ext( Lz, phi, F, U_integral, img, iter, dims,normvec, pointonplane,distWeight );
      //perform iteration
      ls_iteration_ext(F,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in,Lchanged);
      //update statistics
      en_chanvese_update(img, dims, Lin2out, Lout2in);

      //display stuff (maybe)
      if(display){
          if ( (i % display)==0) {
              std::cout<<"This is iteration # "<<i<<std::endl;
          }
      }
    }
}

/*
void interactive_customspeed(double* speedimg, double *img, double *phi, double* U_integral, double *label, long *dims,
                             LL *Lz, LL *Ln1, LL *Lp1, LL *Ln2, LL *Lp2, LL *Lin2out, LL *Lout2in,LL* Lchanged,
                             int iter, double rad, double lambda, int display, double* normvec, double* pointonplane,float distweight)

{
    double *F;
    double scale[1]; scale[0] = 0;
    int countdown;

    //initialize datastructures and statistics
    en_chanvese_init(img,phi,dims);
    std::cout<<clock()<<std::endl;
    for(int i=0;i<iter;i++){
        //compute force
        F=en_custom_compute(Lz,speedimg,phi,dims,scale,lambda);
        // apply controller, modify F in-place
        apply_control_function_ext( Lz, phi, F, U_integral, img, iter, dims,normvec, pointonplane,distweight );
        //perform iteration
        ls_iteration_ext(F,phi,label,dims,Lz,Ln1,Lp1,Ln2,Lp2,Lin2out,Lout2in,Lchanged);
        //update statistics
        en_chanvese_update(img, dims, Lin2out, Lout2in);

        //display stuff (maybe)
        if(display){
            if ( (i % display)==0) {
                std::cout<<"This is iteration # "<<i<<std::endl;
            }
        }
    }
    std::cout<<clock()<<std::endl;

}
*/


void apply_control_function(LL *Lz,double *phi, double* F,
                            double* U_integral, double *img, int iter, long* dims)
{ // apply user's time-integrated edits inside the updates
  int x,y,z,idx,n;
  double I,dpx,dpy,dpz;
  ll_init(Lz);
  n=0;
  double gamma        = 1.0 / 50.0; //1.0 / iter;

  while(Lz->curr != NULL){          //loop through list
    x = Lz->curr->x;
    y = Lz->curr->y;
    z = Lz->curr->z;
    idx          = Lz->curr->idx;
    I            = img[idx];
    double U     = U_integral[idx];
    double err   = ( 3.0 * tanh(U / 3.0) - phi[idx] );

    //kappa not used!?
    //double kappa = en_kappa_norm_pt(Lz->curr,phi,dims,&dpx,&dpy,&dpz);
    double f     = gamma * abs(U) * (F[n] - err);

    F[n]         = F[n] - f;
    ll_step(Lz);
    n++;       //next point
  }


}

void apply_control_function_ext(LL *Lz,double *phi, double* F,
                                double* U_integral, double *img, int iter, long* dims,
                                double* normal,double* poP,float distweight )
{ // apply user's time-integrated edits inside the updates
  int x,y,z,idx,n;
  double I;
  ll_init(Lz);
  n=0;
  double gamma        = 10.0 / 50.0; //10.0 / iter;
  double diff[3]={0,0,0};
  double distance=0;
  while(Lz->curr != NULL){
    distance=0;
    //loop through list
    x = Lz->curr->x;
    y = Lz->curr->y;
    z = Lz->curr->z;

    diff[0]=x-poP[0];
    diff[1]=y-poP[1];
    diff[2]=z-poP[2];

    for (int i=0;i<3;i++)
    {
        distance+=diff[i]*normal[i];
    }
    // weight the update by distance from view-plane
    // to prevent updates far from where user is looking/editing
    double mult= exp(-distweight*abs(distance));

    idx          = Lz->curr->idx;
    I            = img[idx];
    double U     = U_integral[idx];
    double err   = ( 3.0 * tanh(U / 3.0) - phi[idx] );

    //kappa not used!?
    //double kappa = en_kappa_norm_pt(Lz->curr,phi,dims,&dpx,&dpy,&dpz);
    double f     = gamma * abs(U) * (F[n] - err);

    F[n]         = (F[n] - f)*mult;
    ll_step(Lz);
    n++;       //next point
  }


}
