/*************
* Operations *
*************/
#include <ghe.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Operation handler chain */
struct OpList
{
	struct OpList * next;
	char          * name;
	OpHandle        call;
};

/* Operation handlers struct */
typedef struct
{
	char   * name;
	OpHandle call;
}
OpSpec;

/* Operation spec */
static const OpSpec opspec[] =
{
	{ "open",		(OpHandle)ghe_op_open 		},
	{ "close",		NULL 						},
	{ "dump",		NULL 						},
	{ "read",		NULL 						},
	{ "write",		NULL 						},
	{ "dmesg",      (OpHandle)ghe_debug_summary },
	{ "meminfo",    NULL                        }
};

/* Size of hash table */
#define HASH_TABLE_SIZE 128

/* Hash table */
static struct OpList * hashtable[HASH_TABLE_SIZE];

/* Return an approximate hash for a string */
static unsigned
hash ( char * str )
{
	unsigned ret;
	
	for( ret = 0; *str != '\0'; str++ )
		ret = *str + 31 * ret;
	
	return ret % HASH_TABLE_SIZE;
}

/* Insert an operator handle */
int ghe_register_handler ( char * name, OpHandle h )
{
	unsigned ha = hash(name);
	struct OpList * p, * t;
	
	/* Create the entry */
	p = malloc( sizeof(struct OpList) );
	p->next = NULL;
	p->name = strdup(name);
	p->call = h;
	
	/* Check the status of entry... */
	if( hashtable[ha] )
	{
		t = hashtable[ha];
		
		/* This entry already exists - find end of list */
		while( t->next )
			t = t->next;
		
		/* Set next entry */
		p->next = t;
	}
	else
		hashtable[ha] = p;
	
	/* Done */
	return TRUE;
}

/* Generate the hash table for operations */
int ghe_hash_table_init ( void )
{
	int i, m = sizeof(opspec) / sizeof(OpSpec);
	
	for( i = 0; i < m; i++ )
	{
		/* Add it to the table */
		ghe_register_handler( opspec[i].name, opspec[i].call );
	}
	
	/* Done */
	return TRUE;
}

/* Lookup a value on the hash table */
int ghe_op_lookup ( char * name, OpHandle * dest )
{
	unsigned n = hash(name);
	struct OpList * p;
	
	/* Check input */
	if( !*name || !name )
		return FALSE;
	
	/* Set retfunc to NULL */
	*dest = NULL;
	
	/* Unset? */
	if( !hashtable[n] )
	{
		ghe_debug( "Function not found \"%s\"", name, name );
		return FALSE;	/* Does not exist */
	}
	
	/* Look through the list */
	p = hashtable[n];
	while( p )
	{
		/* Compare string */
		if( !strcmp(hashtable[n]->name, name) )
		{
			/* Yep, it's the only one */
			*dest = hashtable[n]->call;
			return TRUE;
		}
		
		/* Go to next */
		p = p->next;
	}
	
	/* Not found, apparently */
	ghe_debug( "Function not found \"%s\"", name );
	return FALSE;
}

/* Call a handler */
int ghe_op_call ( OpHandle func, char * commandline )
{
	char ** argv;
	int argc = 1, i, inspace = FALSE;
	int len, ret;
	char * ptr;
	
	/* Skip past spaces */
	while( isspace(*commandline) )
		commandline++;
	
	/* Check for null */
	if( !func )
	{
		char * t = strstr(commandline, " ");
		int l = (!t ? strlen(commandline) : t - commandline);
		ghe_debug( "No registered handler for operation \"%.*s\".", l, commandline );
		return FALSE;
	}
	
	/* Count the amount of arguments */
	for( i = 0; commandline[i]; i++ )
	{
		if( !inspace && isspace(commandline[i]) )
		{
			argc++;
			inspace = TRUE;
		}
		else if( inspace && !isspace(commandline[i]) )
			inspace = FALSE;
	}
	
	/* Allocate memory */
	argv = malloc( sizeof(char*) * (argc + 1) );
	ptr = commandline;
	i = 0;
	
	/* Null pointer */
	argv[argc] = NULL;
	
	/* Load up args */
	do
	{
      char * n = strstr(ptr, " ");
	  
	  /* Found? */
	  if( !n )
	  {
		  len = strlen(ptr);
		  argv[i] = malloc( len + 1 );
		  memcpy( argv[i], ptr, len );
		  argv[i][len] = '\0';
		  break;
	  }
	  
      len = n - ptr;
      argv[i] = malloc( len + 1 );
	  memcpy( argv[i], ptr, len );
	  argv[i][len] = '\0';
      ptr += len + 1;
	  
	  i++;
	}
	while( i < argc );
	
	/* Call the function */
	ghe_debug( "Calling \"%s\" 0x%08X.", argv[0], (int)func );
	ret = func( argc, argv );
	
	/* Anything unusual about the return value? */
	if( ret != TRUE )
		ghe_debug( "* Return abnormal: %i.", ret );
	
	/* Free arguments */
	for( i = 0; argv[i]; i++ )
		free( argv[i] );
	free( argv );
	
	return ret;
}

/* Open a file */
int ghe_op_open ( int argc, char ** argv )
{
	static GheFile * c;
	
	c = ghe_file_open( argv[1], NULL );
	
	if( c )
		ghe_print( "Opened file \"%s\" successfully.\n", argv[1] );
	else {
		ghe_print( "Unable to open file \"%s\"!\n", argv[1] );
		return FALSE;
	}
	
	return TRUE;
}
