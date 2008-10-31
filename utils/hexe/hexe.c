#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#define ARG_MAX	32

typedef enum { FALSE, TRUE } bool;

typedef bool (*CommandHandler)(const char ** argv, int argc);

struct Command
{
	const char   * name;
	CommandHandler handler;
};

/* Function declarations */
static bool func_read ( const char ** argv, int argc );

static const struct Command
commands[] =
{
	{ "read", func_read }
};

const char * name = "hexe";

static int
read_input ( char * buffer, size_t len, FILE * in )
{
	int c, i = 0;
	
	while( (c = fgetc(in)) != EOF && c != '\n' && i < len - 1 )
		buffer[i++] = c;
	
	buffer[i] = '\0';
	
	return( i ? TRUE : EOF );
}

static void
free_args ( const char ** args )
{
	while( *args )
		free( (void*)*(args++) );
}

static bool
func_read ( const char ** argv, int argc )
{
	while( *argv )
		printf( "%i:%s\n", argc, *argv++ );
	return TRUE;
}

static bool
execute_command ( const char * string )
{
	const char * args[ARG_MAX];
	char *  seek = (char*)string;
	int     len = strlen( string );
	int     i, argc;
	
	/* Loop through the arguments */
	for( i = 0; i < ARG_MAX; i++ )
	{
		char buffer[128];
		int arglen, k;
		
		/* Read next arg */
		for( k = 0, arglen = 0; seek[k] && seek[k] != ' ' && seek[k] != '\n'; k++ )
		{
			buffer[k] = seek[k]; 
			arglen++;
		}
		buffer[k] = '\0';
		
		/* End? */
		if( !buffer[0] )
			break;
		
		/* Allocate memory for it */
		args[i] = malloc( arglen );
		
		/* Store it */
		memcpy( (void*)args[i], buffer, arglen );
		
		/* Seek ahead */
		seek += arglen;
	}
	
	/* Set the last pointer to NULL */
	args[i] = NULL;
	argc = i;
	
	/* Find the command */
	for( i = 0; i < sizeof(commands) / sizeof(struct Command); i++ )
		if( !strcmp(args[0], commands[i].name) )
		{
			/* Call it */
			bool ret = FALSE;
			if( commands[i].handler )
			  ret = commands[i].handler( args, argc );
			
			/* Free argument list */
			free_args( args );
			
			/* Return code */
			return ret;
		}
	
	/* Unable to find it */
	printf( "Command not found \"%s\".\n", args[0] );
	free_args( args );
	return FALSE;
}

static int
command_loop ( void )
{
	char buffer[512];
	static int itr;
	
	do 
	{
	
		/* Handle command */
		if( itr && buffer[0] )
		{
			int r = execute_command( buffer );
			/* printf( "Return code: %i\tItr: %i\tCmd: %s\n", r, itr, buffer ); */
			 struct mallinfo info;
			 info = mallinfo();
			 printf( "Mem use: %u\n", info.uordblks );
		}
		
		/* Show the prompt */
		printf( "(%s) ", name );
		
		/* Update iteration count */
		itr++;
	}
	while( read_input(buffer, sizeof(buffer), stdin) );
}

int main ( int argc, char ** argv )
{
	command_loop();
	
	return EXIT_SUCCESS;
}

