/*****************************
* Zelda 64 Function Database *
*****************************/
#include <glib.h>
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
#include "color.h"

/* Global variables */
struct timeval start;

/* Application config */
struct Config config;

/* File information */
u32    fsize;
char * fname;
u8   * fbuff;

/*
 **
**  Other important functions
 */

/* Check for a valid address */
bool validAddress ( u32 addr )
{
    addr &= 0x00FFFFFF;
    
    if( addr < 0x00400000 && addr > 0x00000400 )
        return TRUE;
    else
        return FALSE;
}


/*
 **
**  Control flow functions
 */

static const GOptionEntry args_normal[] =
{
	{ "action",   'A', 0,                                                   G_OPTION_ARG_STRING,   &config.a_action, "Action to take",         NULL },
	{ "database", 'D', G_OPTION_FLAG_OPTIONAL_ARG | G_OPTION_FLAG_FILENAME, G_OPTION_ARG_FILENAME, &config.a_db,     "Database to work with",  NULL },
	{ "file",     'F', G_OPTION_FLAG_OPTIONAL_ARG | G_OPTION_FLAG_FILENAME, G_OPTION_ARG_FILENAME, &config.a_file,   "File to work with",      NULL },
	{ "debug",    'g', G_OPTION_FLAG_OPTIONAL_ARG,                          G_OPTION_ARG_NONE,     &config.a_debug,  "Display debug messages", NULL },
	{ NULL }
};

/* Initialization */
static void zfdbInit ( int * argc, char *** argv )
{
	GOptionContext * context;
	GError * error;
	
	/* Command line arguments */
	context = g_option_context_new( "Interact with an N64 function database" );
	g_option_context_add_main_entries( context, args_normal, NULL );
	g_option_context_parse( context, argc, argv, &error );
	
    /* Set app start time */
    gettimeofday( &start, NULL );
    
    /* Logo */
    showLogo();
    
    /* */ kprintf( "Application begin..." );
	
	/*
	 * Parse arguments
	 */
	if( !config.a_action )	/* No action */
	{
		MESSAGE( "Please choose an action.\n" );
		exit( -1 );
	}
		
	if( config.a_db )		/* Specific DB name */
		dbname = (char*)config.a_db;
	
	/* Debug text */
	if( config.a_debug )
		config.flags |= FLAG_SHOW_DEBUG;
	
	/* Handle action */
	if( !strcmp(config.a_action, "import") )
		config.action = ACTION_IMPORT_DUMP;
	else
	if( !strcmp(config.a_action, "import-text") )
		config.action = ACTION_IMPORT_TEXT;
	else
	if( !strcmp(config.a_action, "dump-header") )
		config.action = ACTION_DUMP_HEADER;
	else
	exit( -1 );
	
    
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
static void zfdbDeinit ( void )
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

/* Import a file into the database */
void importFile ( void )
{
	char * name = config.a_file;
	
    /* Load the file */
    loadFile( name );
    
    MESSAGE( "Loaded file \"%s\" successfully.\nBeginning import...\n", name );
    
    dbBeginTransaction();
    mips_process_file();
    dbCommit();
    
    MESSAGE( "Import complete! Changes commited to database.\n" );
}

/* Import a text file */
void importText ( void )
{
	char * name = config.a_file;
    FILE * fh;
    char func[256];
    u32 addr;
    
    if( !(fh = fopen(name, "r")) )
        quitError( -1, "Unable to open \"%s\" for reading:\n%s\n",
        strerror(errno) );
    
    MESSAGE( "Importing text file \"%s\"...\n", name );
    
    while( fscanf(fh, "%x,%s", &addr, func) == 2 )
    {
        /* Check address */
        if( !validAddress(addr) )
            continue;
        
        dbInsertFunction( addr, func, "( int, ... )", "int", "N64 OS",
        "Provided as a part of the Nintendo 64 operating system." );
    }
    
    fclose( fh );
    
    MESSAGE( "Done!\n" );
}

/* Dump a header file */
void dumpHeader ( char * out )
{
    char * q;
    sqlite3_stmt * statement;
    int v;
    FILE * o = fopen( out, "wb" );
    
    /* Prepare query */
    q = sqlite3_mprintf( "SELECT address, return, name, arguments "
    "FROM %s;", dbtablename );
    dumpQuery(q);
    
    v = sqlite3_prepare( db, q, strlen(q), &statement, NULL );
    
    /* Check query status */
    if( v != SQLITE_OK )
    {
        quitError( -1, "Error while preparing SQL statement:\n%s\n",
        sqlite3_errmsg(db) );
    }
    
    /* Step through and get the results */
    while( sqlite3_step( statement) == SQLITE_ROW )
    {
        /* u32 addr = sqlite3_column_int( statement, 0 ); */
        char * ret  = (char*)sqlite3_column_text( statement, 1 );
        char * name = (char*)sqlite3_column_text( statement, 2 );
        char * args = (char*)sqlite3_column_text( statement, 3 );
        
        if( (config.flags & FLAG_NO_UNKNOWNS) && strstr(name, "FNC_") )
            continue;
        
        fprintf( o, "extern %s %s %s;\n", ret, name, args );
    }
    
    /* Finalize statement */
    sqlite3_finalize( statement );
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
    char * fname;
    int i;
    
	/* Initialize the program */
    zfdbInit( &argc, &argv );
    
    /* Check action */
    switch( config.action )
    {
        case ACTION_IMPORT_DUMP:
         importFile( );
        break;
        
        case ACTION_IMPORT_TEXT:
         importText( );
        break;
        
        case ACTION_DUMP_HEADER:
         dumpHeader( config.a_file );
        break;
        
        default:
         importFile( );
    }
    
    zfdbDeinit();
    
    /* Show debug messages? */
    if( config.flags & FLAG_SHOW_DEBUG )
        kdump();
    
    return 0;
}
