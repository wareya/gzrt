/*******************
* ZFDB - Messaging *
*******************/
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "zfdb.h"
#include "color.h"

/* Logo */
#define APP_LOGO																					             \
TEXT_BLUE " _______ _______"  TEXT_GREEN " ______  ______   "									            "\n" \
TEXT_BLUE "(_______|_______|" TEXT_GREEN  "______)(____  \\  " 								           	    "\n" \
TEXT_BLUE "   __    _____  "  TEXT_GREEN " _     _ ____)  ) "  TEXT_NORM TEXT_BOLD "Author:" TEXT_BOLD_OFF " ZZT32 [zzt256@gmail.com]" "\n" \
TEXT_BLUE "  / /   |  ___) "  TEXT_GREEN "| |   | |  __  (  "  TEXT_NORM TEXT_BOLD "Date:" TEXT_BOLD_OFF " " __DATE__ " " __TIME__     "\n" \
TEXT_BLUE " / /____| |     "  TEXT_GREEN "| |__/ /| |__)  ) "										    	"\n" \
TEXT_BLUE "(_______)_|     "  TEXT_GREEN "|_____/ |______/  "									            "\n" \
TEXT_NORM

/* Debug message list */
#ifndef FINAL
struct DebugMesg
{
	char * mesg;
	struct timeval tv;
	struct DebugMesg * next;
};
static struct DebugMesg dmesgbase;
#endif

/* Show logo */
void showLogo ( void )
{
	MESSAGE( "%s\n", APP_LOGO );
}

/* Quit with error message and code */
void quitError ( int code, char * fmt, ... )
{
	va_list ap;
	
	/* Handle args */
	va_start( ap, fmt );
	
	/* Print message to stderr */
	vfprintf( stderr, fmt, ap );
	
	/* Done w/ args */
	va_end( ap );
	
	/* Dump log */
#ifndef FINAL
	MESSAGE( "Dumping log...\n" );
	kdump();
#endif
	
	/* Return error */
	exit( code );
}

 /* Append a debug message */
int kprintf ( char * fmt, ... )
{
#ifndef FINAL
 	va_list ap;
	static struct DebugMesg * last;
 	struct DebugMesg * ptr = &dmesgbase;
	
	/* Last already set? */
	if( last )
		ptr = last;
 	
 	/* Find end of list */
 	while( ptr->next )
 		ptr = ptr->next;
 	
 	/* New node? */
 	if( ptr->mesg )
 	{
 		/* Allocate memory */
 		ptr->next = calloc( sizeof(struct DebugMesg), 1 );
 		
 		/* Walk */
 		ptr = ptr->next;
 	}
	
	/* Store latest */
	if( last != ptr )
		last = ptr;
 	
 	/* Prepare message */
 	va_start( ap, fmt );
 	ptr->mesg = sqlite3_vmprintf( fmt, ap );
 	va_end( ap );
	
	/* Store time */
	gettimeofday( &ptr->tv, NULL );
 	
 	/* Return length */
 	return strlen(ptr->mesg);
#else
	return -1;
#endif
}

/* Dump an SQL query to same place as above */
int dumpQuery ( char * q )
{
#ifndef FINAL
	kprintf( "SQL query: " TEXT_CYAN "%s" TEXT_NORM, q );
	
	return 0;
#else
	return 0;
#endif
}
 
/* Dump the debug text */
void kdump ( void )
{
#ifndef FINAL
	struct DebugMesg * ptr = &dmesgbase;

	while( ptr && ptr->mesg )
	{
		 printf( "[" TEXT_RED "%10.4f" TEXT_NORM "] %s\n", 
		 timeDiff(&ptr->tv, &start), ptr->mesg );
		 ptr = ptr->next;
	}
#endif
}
