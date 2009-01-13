/*****************************
* Zelda 64 Function Database *
*****************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include "zfdb.h"
#include "sqlite3.h"

/* Global variables */
struct timeval start;

/* File information */
u32    fsize;
char * fname;
u8   * fbuff;


/*
 **
**  Control flow functions
 */

/* Initialization */
void zfdbInit ( void )
{
	/* Set app start time */
	gettimeofday( &start, NULL );
	
	/* Logo */
	showLogo();
	
	/* */ kprintf( "Application begin..." );
	
	/* Open the database */
	dbOpen();
	
	/* */ kprintf( "Database \"%s\" opened successfully.", dbname );
	
	/* Check that the tables exist */
	if( !dbTableExists( dbtablename ) )
	{
		/* */ kprintf( "Table \"%s\" does not exist, creating" );
		
		/* Create the table */
		dbCreateTable();
		
		/* */ kprintf( "Table created successfully." );
	}
	else
	{
		/* */ kprintf( "Table \"%s\" present.", dbtablename );
	}
}

/* Deinitialization */
void zfdbDeinit ( void )
{
	/* Close database */
	dbClose();
	
	/* */ kprintf( "Database \"%s\" closed successfully.", dbname );
	/* */ kprintf( "Ending application..." );
}

/* Load a file */
void loadFile ( char * name )
{
	FILE * h;
	
	/* Preserve filename */
	fname = strdup(name);
	
	/* Open file for reading */
	if( !(h = fopen(name, "rb")) )
		quitError( -1, "Unable to open file \"%s\" for reading:\n%s\n",
		fname, strerror(errno) );
	
	/* Get filesize */
	fseek( h, 0, SEEK_END );
	fsize = ftell(h);
	
	/* Alloc memory */
	fbuff = malloc(fsize);
	
	/* Read */
	fseek( h, 0, SEEK_SET );
	fread( fbuff, 1, fsize, h );
	fclose( h );
	
	/* */ kprintf( "Read %.2fKbytes from file \"%s\".", (double)fsize / 1024.0, fname );
}


/*
 ** ~|
  ** ~|~
 **** ~|~~
  ** ~|~ 
 ** ~|
*/


int main ( int argc, char ** argv )
{
	zfdbInit();
	
	loadFile(argv[1]);
	
	MESSAGE( "Loaded file \"%s\" successfully.\n", argv[1] );
	MESSAGE( "Beginning import...\n" );
	
	dbBeginTransaction();
	mips_process_file();
	dbCommit();
	
	MESSAGE( "Import complete! Changes commited to database.\n" );
	
	zfdbDeinit();
	
	return 0;
}
