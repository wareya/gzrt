/**********************
* Assisting functions *
**********************/
#include <ghe.h>
#include <sys/time.h>
#include <stdio.h>

int read_input ( char * buffer, size_t len, FILE * in )
{
	int c, i = 0;
	
	while( (c = fgetc(in)) != EOF && c != '\n' && i < len - 1 )
		buffer[i++] = c;
	
	buffer[i] = '\0';
	
	return( c == EOF ? EOF : TRUE );
}


double ghe_runtime ( void )
{
	struct timeval tv;
	double x, y;
	gettimeofday( &tv, NULL );
	
	x = (double)tv.tv_sec * 1000000.0 + (double)tv.tv_usec;
	y = (double)ghe_start.tv_sec * 1000000.0 + (double)ghe_start.tv_usec;
	
	return (x - y) / 1000000.0;
}

double ghe_timesince ( struct timeval * tv )
{
	double x, y;
	
	x = (double)tv->tv_sec * 1000000.0 + (double)tv->tv_usec;
	y = (double)ghe_start.tv_sec * 1000000.0 + (double)ghe_start.tv_usec;
	
	return (x - y) / 1000000.0;
}
