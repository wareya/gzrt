/******************************
* ZFDB - Database Interaction *
******************************/
#include <stdio.h>
#include <stdlib.h>
#include "zfdb.h"

/* Constants */
#define DB_TABLE_NAME   "z64_functions"
#define DB_TABLE_FMT                        \
"CREATE TABLE z64_functions             \n" \
"(                                      \n" \
"   address     INTEGER PRIMARY KEY,    \n" \
"   name        TEXT,                   \n" \
"   arguments   TEXT,                   \n" \
"   return      TEXT,                   \n" \
"   library     TEXT,                   \n" \
"   description TEXT                    \n" \
");                                     \n"

/* Database handle */
sqlite3 * db;

/* Database name */
char * dbname = "ZFDB.db";
char * dbtablename = DB_TABLE_NAME;

/* Open the database */
int dbOpen ( void )
{
    int ret;
    
    /* Any errors? */
    if( (ret = sqlite3_open( dbname, &db )) )
        
        /* Yep, quit with error... */
        quitError( -1, "Error while opening database:\n%s\n", sqlite3_errmsg(db) );
    
    /* Success */
    return ret;
}

/* Close the database */
int dbClose ( void )
{
    int ret;
    
    /* Any errors? */
    if( (ret = sqlite3_close( db )) )
    
        /* Yep, quit with error... */
        quitError( -1, "Error while closing database:\n%s\n", sqlite3_errmsg(db) );
    
    /* Success */
    return ret;
}

/* Check if table is open */
void dbCheckOpen ( void )
{
    if( !db )
        quitError( -1, "Error, DB not open.\n" );
}

/* Check if a table exists */
bool dbTableExists ( char * name )
{
    char * q;
    sqlite3_stmt * statement;
    int v, rows = 0;
    
    /* Die if somehow db not open */
    dbCheckOpen();
    
    /* Prepare query */
    q = sqlite3_mprintf( "SELECT * FROM sqlite_master "
    "WHERE name == \"%s\";", name );
    /* */ dumpQuery(q);
    v = sqlite3_prepare( db, q, strlen(q), &statement, NULL );
    
    /* Check query status */
    if( v != SQLITE_OK )
        quitError( -1, "Error while preparing SQL statement.\n" );
    
    /* Step through and get results */
    while( sqlite3_step( statement ) == SQLITE_ROW )
        rows++;
    
    /* Finalize statement */
    sqlite3_finalize( statement );
    
    /* Return */
    return (rows ? TRUE : FALSE);
}

/* Create our table */
int dbCreateTable ( void )
{
    int v;
    
    v = sqlite3_exec( db, DB_TABLE_FMT, NULL, NULL, NULL );
    
    /* Check return */
    if( v )
    {
        quitError( -1, "SQL error while creating database:\n%s\n",
        sqlite3_errmsg(db) );
    }
    
    return v;
}

/* Begin transaction */
int dbBeginTransaction ( void )
{
    int v;
    
    v = sqlite3_exec( db, "BEGIN TRANSACTION;", NULL, NULL, NULL );
    
    if( v )
    {
        quitError( -1, "SQL error while beginning transaction:\n%s\n",
        sqlite3_errmsg(db) );
    }
    
    return v;
}

/* Commit */
int dbCommit ( void )
{
    int v;
    
    v = sqlite3_exec( db, "COMMIT;", NULL, NULL, NULL );
    
    if( v )
    {
        quitError( -1, "SQL error while commiting changes:\n%s\n",
        sqlite3_errmsg(db) );
    }
    
    return v;
}

/* Insert a function */
int dbInsertFunction ( u32 address, char * name, char * args, char * ret,
    char * lib, char * desc )
{
    char * q;
    int v;
    
    /* Prepare address */
    address &= 0x00FFFFFF;
    
    /* Create query */
    q = sqlite3_mprintf( "INSERT INTO z64_functions VALUES ( %d, '%s', '%s', "
    "'%s', '%s', '%s' );", address, name, args, ret, lib, desc );
    dumpQuery( q );
    
    /* Insert it */
    v = sqlite3_exec( db, q, NULL, NULL, NULL );
    
    /* Check return */
    if( v )
    {
        quitError( -1, "SQL error while inserting function reference:\n%s\n",
        sqlite3_errmsg(db) );
    }
    
    return v;
}

/* Check if a function has already been noted */
bool dbFunctionExists ( u32 addr )
{
    char * q;
    sqlite3_stmt * statement;
    int v, rows = 0;
    
    /* Die if somehow db not open */
    dbCheckOpen();
    
    /* Prepare query */
    q = sqlite3_mprintf( "SELECT * FROM %s "
    "WHERE address == %d;", dbtablename, addr );
    /* */ dumpQuery(q);
    v = sqlite3_prepare( db, q, strlen(q), &statement, NULL );
    
    /* Check query status */
    if( v != SQLITE_OK )
        quitError( -1, "Error while preparing SQL statement.\n" );
    
    /* Step through and get results */
    while( sqlite3_step( statement ) == SQLITE_ROW )
        rows++;
    
    /* Finalize statement */
    sqlite3_finalize( statement );
    
    /* Return */
    return (rows ? TRUE : FALSE);
}
