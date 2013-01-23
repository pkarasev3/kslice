#include "KViewerOptions.h"

#ifndef WIN32
#include <sys/types.h>
#include <unistd.h>
#endif


void KViewerOptions::KViewerOptions(){

}

void KViewerOptions::~KViewerOptions(){

}

void KViewerOptions::SetBrushRad(int rad){
    brushRad=rad;
}


int KViewerOptions::GetBrushRad(){
    return brushRad;
}

void KViewerOptions::SetNumIters(int iters){
    numIters=iters;
}

int KViewerOptions::GetNumIters(){
    return numIters;
}
