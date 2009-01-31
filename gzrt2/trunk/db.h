/**********************************
* GZRT Database Manager (SQLite3) *
**********************************/
#ifndef __GZRT_DB_H__
#define __GZRT_DB_H__

#include "sqlite3.h"

/* Global variables */
extern sqlite3 * gzrt_db;
extern char    * gzrt_dbname;
extern int       gzrt_db_loaded;

/* Functions -- db.c */
extern int gzrt_db_init ( void );
extern int gzrt_db_deinit ( void );

#endif /* __GZRT_DB_H__ */

