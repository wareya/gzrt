#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <glib.h>

/*
	Operations:
	 read	[-flags] buffername context start end
	 write	[-flags] buffername context
	 dump	[-flags] buffername
	 open	[-flags] context filename
	 close	[-flags] context
	 
	 READ OPERATIONS:
	  read		[-flags] buffername context [location]
	  ] Create/reformat a buffer containing the contents
	  ] of [location]
	  append	[-flags] buffername context [location]
	  ] Append the data at location to buffer
*/

#define ARG_MAX	32

typedef gboolean bool;

typedef bool (*CommandHandler)(const char ** argv, int argc);

struct Command
{
	const char   * name;
	CommandHandler handler;
};

struct Buffer
{
	GList * parts;
};

struct BufferChunk
{
	guint8	* data;
	guint32   size;
	guint32	  a_start;
	guint32	  a_end;
};

struct Context
{
	char * name;
	FILE * handle;
};

/* Function declarations */
static bool func_read ( const char ** argv, int argc );
static bool func_open ( const char ** argv, int argc );

static GList * contexts;
static GList * buffers;

static const struct Command
commands[] =
{
	{ "read", func_read },
	{ "open", func_open }
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

static int
find_context ( struct Context * a, char * name )
{
	return strcmp( a->name, name );
}

static bool
func_open ( const char ** argv, int argc )
{
	extern char *strdup(const char *s1);
	struct Context * new;
	int i;
	
	/* Skip past any arguments... */
	for( i = 1; argv[i] && argv[i][0] == '-'; i++ );
		
	/* No arguments? */
	if( !argv[i] )
	{
		printf("No filename given.\n" );
		return FALSE;
	}
	
	/* Does it exist already? */
	if( g_list_find_custom( contexts, argv[1], find_context ) )
	{
		printf( "Context \"%s\" already exists. Close it first.\n", 
		argv[1] );
	}
	
	/* Store it */
	new			=  malloc( sizeof(struct Context) );
	new->name   = strdup( argv[2] );
	new->handle = fopen( argv[2], "rb" );
	contexts    = g_list_append( contexts, new );
	
	printf( "Opened \"%s\" successfully as context \"%s\".\n", argv[2], argv[1] );
	
	return TRUE;
}

static bool
execute_command ( const char * string )
{
	const char * args[ARG_MAX];
	char *  seek = (char*)string;
	int     i, argc;
	
	/* Skip any preceeding spaces */
	while( *seek == ' ' )
		seek++;
		
	/* Nothing? */
	if( !*seek )
		return FALSE;
	
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
		memcpy( (void*)args[i], buffer, arglen + 1 );
		
		/* Seek ahead */
		seek += arglen;
		while( *seek && *seek == ' ' )
			seek++;
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
	int itr = 0;
	
	do 
	{
		/* Handle command */
		if( itr && buffer[0] )
		{
			 struct mallinfo info;
			 execute_command( buffer );
			/* printf( "Return code: %i\tItr: %i\tCmd: %s\n", r, itr, buffer ); */
			 info = mallinfo();
			 printf( "Mem use: %u\n", info.uordblks );
		}
		
		/* Show the prompt */
		printf( "(%s) ", name );
		
		/* Update iteration count */
		itr++;
	}
	while( read_input(buffer, sizeof(buffer), stdin) );
	
	return 0;
}

int main ( int argc, char ** argv )
{
	command_loop();
	
	return EXIT_SUCCESS;
}

