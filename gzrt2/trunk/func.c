/****************************
* GZRT Supporting Functions *
****************************/
#include <stdio.h>
#include "gzrt.h"

double timeDiff ( struct timeval * a, struct timeval * b )
{
    return
        (((double)a->tv_usec + (double)a->tv_sec * 1000000.0) -
        ((double)b->tv_usec + (double)b->tv_sec * 1000000.0))
        / 1000000.0;
}

