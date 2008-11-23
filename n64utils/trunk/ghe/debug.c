/***********************
* Debugging facilities *
***********************/
#include <ghe.h>
#include <sys/time.h>
#include <stdarg.h>
#include <stdlib.h>

/* A message entry */
struct DebugMesg
{
	struct DebugMesg * prev;
	struct DebugMesg * next;
	char * message;
	struct timeval tv;
};

/* */
static struct DebugMesg * messages;
static struct DebugMesg * latest;

/* Append a message */
int ghe_debug ( char * fmt, ... )
{
	char buffer[1024];
	va_list ap;
	struct DebugMesg * t;
	int len;
	
	/* Prepare buffer */
	va_start( ap, fmt );
	len = vsnprintf( buffer, sizeof(buffer), fmt, ap );
	va_end( ap );
	
	/* Create a new entry */
	t = malloc( sizeof(struct DebugMesg) );
	
	/* Set the values */
	t->prev = (!messages ? NULL : latest );
	t->next = NULL;
	t->message = strdup(buffer);
	gettimeofday( &t->tv, NULL );
	
	/* Append this entry to list */
	if( !messages && !latest )
	{
		/* Nothing here yet */
		messages = latest = t;
	}
	else
	{
		/* Append it */
		latest->next = t;
		latest = t;
	}
	
	/* Okay */
	return len;
}

/* Dump last ten debug messages */
int ghe_debug_summary ( int argc, char ** argv )
{
	int i;
	struct DebugMesg * p = latest;
	int amount = (argc == 1 ? 10 : atoi(argv[1]));
	
	/* Notify */
	if( argc > 1 )
		ghe_print( "Showing last %u entries.\n", amount );
	
	/* Skip back ten entries */
	for( i = 0; p->prev && i < amount; i++ )
		p = p->prev;
	
	for( ; i > 0 && p; i-- )
	{
		ghe_print( "[%8.2f] %s\n", ghe_timesince(&p->tv), p->message );
		p = p->next;
	}
	
	return TRUE;
}

