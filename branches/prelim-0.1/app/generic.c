/*****************************
* Generic Functions for GZRT *
*****************************/
#include <gzrt.h>

void trim ( char *s )
{
    int l;
    
    for( l = strlen(s) - 1; s[l] == ' '; l-- );
    s[l + 1] = 0;
}

int is_dir ( char *n )
{
    DIR *h;
    
    if( !(h = opendir(n)) )
        return 0;
    else {
        closedir( h );
        return 1;
    }
}

int can_write ( char *n )
{
    FILE *h;
    
    if( !(h = fopen(n, "rb+")) )
        return 0;
    else {
        fclose(h);
        return 1;
    }
}

/* String compare; case insensitive (Windows) */
int stringcmp ( char *s, char *e )
{
    while( tolower(*s) == tolower(*e) )
    {
        if( !*s || !*e )
            break;
        
        s++;
        e++;
    }
    
    return tolower(*s) - tolower(*e);
}


double time_since_start ( void )
{
    struct timeval tv;
    gettimeofday( &tv, NULL );
    double x, y;
    
    x = (double)tv.tv_sec * 1000000.0 + (double)tv.tv_usec;
    y = (double)program_start.tv_sec * 1000000.0 + (double)program_start.tv_usec;
    
    return (x - y) / 1000000.0;
}
