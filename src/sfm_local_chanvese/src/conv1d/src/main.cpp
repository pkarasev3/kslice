// testing 2D convolution

#include <cstdio>
#include "convolution.h"
#include "Timer.h"

int main()
{
    float in[5] = {3, 4, 5, 0, 0};
    float out[5];
    float k[2] = {2,1};
    int i;
    Timer t;

    t.start();
    bool success = convolve1D(in, out, 5, k, 2);
    t.stop();

    if(success) printf("SUCCESS, Elapsed Time: %5.3f us\n", t.getElapsedTimeInMicroSec());
    else        printf("FAILED\n");

    printf("INPUT\n");
    for(i=0; i < 5; ++i)
    {
        printf("%5.2f, ", in[i]);
    }
    printf("\n\n");

    printf("KERNEL\n");
    for(i=0; i < 2; ++i)
    {
        printf("%5.2f, ", k[i]);
    }
    printf("\n\n");

    printf("OUTPUT\n");
    for(i=0; i < 5; ++i)
    {
        printf("%5.2f, ", out[i]);
    }
    printf("\n");

    return 0;
}
