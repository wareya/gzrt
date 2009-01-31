/****************************
* GZRT Supporting Functions *
****************************/
#include <stdio.h>
#include <string.h>
#include "gzrt.h"

double timeDiff ( struct timeval * a, struct timeval * b )
{
    return
        (((double)a->tv_usec + (double)a->tv_sec * 1000000.0) -
        ((double)b->tv_usec + (double)b->tv_sec * 1000000.0))
        / 1000000.0;
}

double timeSinceStart ( void )
{
    struct timeval tmp;
    
    gettimeofday( &tmp, NULL );
    
    return timeDiff( &tmp, &conf.gzrt_start );
}

char * filename ( char * s )
{
    int l = strlen( s );
    
    /* Find path indicator or end */
    for( --l; l >= 0 && s[l] != '/'; --l );
    
    /* End? */
    if( s[l] == '/' )
        l++;
    
    return &s[l];
}
