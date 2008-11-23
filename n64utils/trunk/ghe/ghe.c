/*****************
* GNU Hex Editor *
*****************/
#include <ghe.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* Program start time */
struct timeval ghe_start;

/* GHE initialization */
int ghe_init ( int argc, char ** argv )
{
	/* Get start time */
	gettimeofday( &ghe_start, NULL );
	
	/* Create the hash table for functions */
	ghe_hash_table_init();
	
	/* Log message */
	ghe_debug( "Created hash table successfuly." );
	
	return TRUE;
}

/* Print a message to terminal */
int ghe_print ( char * fmt, ... )
{
	char buffer[1024];
	int len;
	va_list ap;
	
	/* Prepare buffer */
	va_start( ap, fmt );
	len = vsnprintf( buffer, sizeof(buffer), fmt, ap );
	va_end( ap );
	
	/* Print it */
	printf( "%s", buffer );
	fflush( stdout );
	
	/* Return length */
	return len;
}


int main ( int argc, char ** argv )
{
	static char buffer[1024];
	int iterations = 0;
	
	/* Initialize the application */
	ghe_init( argc, argv );
	
	do 
	{
	  /* Lookup function? */
	  if( iterations )
	  {
		  char buff[64];
		  OpHandle t;
		  bool retval;
		  
		  if( sscanf( buffer, "%s", buff ) == -1 )
			  goto nolookup;
		  retval = ghe_op_lookup( buff, &t );
		  
		  /* Call it */
		  if( retval )
			  ghe_op_call( t, buffer );
		  else
			  ghe_print( "Command not found \"%s\".\n", buff );
		  
	  	  nolookup: ;
	  }
		  
	  ghe_print( T_PROMPT_FMT, T_PROMPT_NAME );
	  
	  iterations++;
	} 
	while( (read_input( buffer, sizeof(buffer), stdin )) != EOF );
	
	return 0;
}
