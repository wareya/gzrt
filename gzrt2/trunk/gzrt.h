/*********************
* GNU Zelda ROM Tool *
*********************/
#ifndef __GZRT_H__
#define __GZRT_H__

#include <stdio.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>

/* Program version */
#define GZRT_VERSION	"v0.4"

/* Application flags */
#define GZRT_F_PLUGINS_LOADED	(1 << 1)
#define GZRT_F_PLUGINS_ERROR	(1 << 2)

/* Configuration structer */
struct gzrtConf
{
	/* Generic */
	struct timeval gzrt_start;	/* Program start time        */
	int            gzrt_return;	/* Return value on return    */
	pid_t          gzrt_pid;    /* GZRT's process ID         */
	int            gzrt_flags;	/* Application flags (see ^) */
	GList        * procs;		/* A list of processes open  */
	
	/* IPC */
	pthread_t ipc_thread;	/* Thread */
	
	/* Plugin manager */
	pthread_t pm_thread;	/* Thread					*/
	double    pm_load_perc;	/* For loading progress bar */
};

/* Global variables */
extern struct gzrtConf conf;

/* Functions -- gzrt.c */
extern void gzrt_exit ( int );
extern void gzrt_process_log ( pid_t );
extern void gzrt_process_remove ( pid_t );

/* Functions -- func.c */
extern double timeDiff ( struct timeval *, struct timeval * );

#endif /* !__GZRT_H__ */
