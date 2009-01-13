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

/* Import a file into the database */
void importFile ( char * name )
{
    /* Load the file */
    loadFile( name );
    
    MESSAGE( "Loaded file \"%s\" successfully.\nBeginning import...\n", name );
    
    dbBeginTransaction();
    mips_process_file();
    dbCommit();
    
    MESSAGE( "Import complete! Changes commited to database.\n" );
}

/* Import a text file */
void importText ( char * name )
{
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
        u32 addr = sqlite3_column_int( statement, 0 );
        char * ret = sqlite3_column_text( statement, 1 );
        char * name = sqlite3_column_text( statement, 2 );
        char * args = sqlite3_column_text( statement, 3 );
        
        fprintf( o, "extern %s %s %s; /* 0x%08X */\n", ret, name, args, addr );
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
    
    zfdbInit();
    
    /* No args? */
    if( argc == 1 )
        showUsage(argc, argv);
    
    /* Check arguments */
    if( argc == 2 )
    {
        fname = argv[1];
    }
    else
    {
        /* Scan through arguments */
        for( i = 1; i < argc; i++ )
        {
            if( argv[i][0] == '-' ) switch( argv[i][1] )
            {
                case 'i':
                 config.action = ACTION_IMPORT_DUMP;
                break;
                
                case 't':
                 config.action = ACTION_IMPORT_TEXT;
                break;
                
                case 'd':
                 config.flags |= FLAG_SHOW_DEBUG;
                break;
                
                case 'h':
                 config.action = ACTION_DUMP_HEADER;
                break;
            }
            else
                
                fname = argv[i];
        }
    }
    
    /* Check action */
    switch( config.action )
    {
        case ACTION_IMPORT_DUMP:
         importFile( fname );
        break;
        
        case ACTION_IMPORT_TEXT:
         importText( fname );
        break;
        
        case ACTION_DUMP_HEADER:
         dumpHeader( fname );
        break;
        
        default:
         importFile( fname );
    }
    
    zfdbDeinit();
    
    /* Show debug messages? */
    if( config.flags & FLAG_SHOW_DEBUG )
        kdump();
    
    return 0;
}
