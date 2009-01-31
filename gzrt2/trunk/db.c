/**********************************
* GZRT Database Manager (SQLite3) *
**********************************/
#include <stdio.h>
#include "sqlite3.h"
#include "gzrt.h"
#include "msg.h"
#include "db.h"

/* Settings table specification */
#define TABLE_NAME	"gzrt_settings"
#define TABLE_SPEC					\
"CREATE TABLE " TABLE_NAME "	\n"	\
"(								\n"	\
"   property TEXT,  			\n"	\
"	value    TEXT               \n"	\
");                             \n"

/* A few database variables */
sqlite3 * gzrt_db;
char    * gzrt_dbname = ".gzrt.sqlite3";
int       gzrt_db_loaded;

/* Open the database */
static gboolean
gzrt_db_open ( void )
{
	int ret;
	
	/* Open database */
	ret = sqlite3_open( gzrt_dbname, &gzrt_db );
	
	/* Check value */
	if( ret )
	{
		/* Show error */
		gzrt_message( MSG_WARNING, "Warning", "Unable to open database:\n%s",
		sqlite3_errmsg(gzrt_db) );
		
		return FALSE;
	}
	
	/* Looks good */
	DEBUG( "\"%s\" opened by Sqlite successfully.", gzrt_dbname );
	return TRUE;
}

/* Close database */
static gboolean
gzrt_db_close ( void )
{
	int ret;
	
	/* Close the database */
	ret = sqlite3_close( gzrt_db );
	
	/* Check value */
	if( ret )
	{
		/* Show error */
		gzrt_message( MSG_WARNING, "Warning", "Unable to close database:\n%s",
		sqlite3_errmsg(gzrt_db) );
		
		return FALSE;
	}
	
	/* Looks good */
	DEBUG( "\"%s\" closed successfully.", gzrt_dbname );
}

/* Initialize database interface */
int gzrt_db_init ( void )
{
	int ret;
	
	/* Open database */
	ret = gzrt_db_open();
	
	/* Successful? */
	if( ret )
		gzrt_db_loaded = TRUE;
	
	return ret;
}

/* Deinitialize database interface */
int gzrt_db_deinit ( void )
{
	/* Not loaded? */
	if( !gzrt_db_loaded )
		return FALSE;
	
	/* Close database */
	return gzrt_db_close();
}

