/*****************
* GNU Hex Editor *
*****************/
#ifndef __GHE_H__
#define __GHE_H__

#include <stdio.h>
#include <sys/time.h>

/* Local includes */
#include "file.h"

/* Constants */
#define T_PROMPT_NAME	"ghe"
#define T_PROMPT_FMT	"(%s) "
#define	TRUE			1
#define FALSE			0

/* Types */
typedef int bool;
typedef unsigned char ubyte;

/* An operation handler */
typedef bool (*OpHandle)( int, char ** );

/* Functions - other */
extern char * strdup ( char * );

/* Functions - ghe.c */
extern int ghe_print ( char *, ... );

/* Functions - debug.c */
extern int ghe_debug ( char *, ... );
extern int ghe_debug_summary ( int, char ** );

/* Functions - file.c */
extern GheFile * ghe_file_open ( char * path );
extern GheBuffer * ghe_buffer_new ( void );
extern int ghe_buffer_append ( GheBuffer *, const ubyte *, int );

/* Functions - func.c */
extern int read_input ( char *, size_t, FILE * );
extern double ghe_runtime ( void );
extern double ghe_timesince ( struct timeval * );

/* Functions - ops.c */
extern int ghe_register_handler ( char *, OpHandle );
extern int ghe_hash_table_init ( void );
extern int ghe_op_lookup ( char *, OpHandle * );
extern int ghe_op_call ( OpHandle, char * );
extern int ghe_op_open ( int argc, char ** argv );

/* Global variables */
extern struct timeval ghe_start;

#endif /* __GHE_H__ */
