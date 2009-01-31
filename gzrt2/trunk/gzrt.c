/*********************
* GNU Zelda ROM Tool *
*********************/
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include "gzrt.h"
#include "ipc.h"
#include "plugin.h"
#include "msg.h"
#include "gui.h"
#include "db.h"

/* Program configuration */
struct gzrtConf conf;

/* Initialization */
static void gzrt_init ( int * argc, char *** argv )
{
    /* Set the program time of day */
    gettimeofday( &conf.gzrt_start, NULL );
    
    /* Set PID */
    conf.gzrt_pid = getpid();
    
    DEBUG( "Initializing..." );
    
    /* Signal handlers */
    gzrt_signal_init();
    DEBUG( "Signal handlers set up." );
    
    /* Initialize GTK */
    gtk_init( argc, argv );
    DEBUG( "Initialized GTK." );
    
    /* Initialize IPC */
    gzrt_ipc_init();
    DEBUG( "Initialized IPC." );
    
    /* Start the plugin manager */
    gzrt_plugin_manager_init();
    DEBUG( "Initialized plugin manager." );
    
    /* Initialize database interface */
    if( gzrt_db_init() )
        DEBUG( "Database interface started successfully." );
    else
        DEBUG( "Problem while starting database interface." );
    
    /* Create the GUI */
    gzrt_gui_init();
    DEBUG( "Initialized the GUI." );
    
    DEBUG( "Program started." );
}

/* Cleanup */
static void gzrt_deinit ( void )
{
    int i, l = g_list_length(conf.procs);
    
    DEBUG( "Quitting..." );
    
    /* Kill clients */
    gzrt_ipc_kill_clients();
    
    /* Free list */
    g_list_free(conf.procs);
    
    /* Stop db manager */
    gzrt_db_deinit();
}

/* Quit with code */
void gzrt_exit ( int code )
{
    /* Set return code */
    conf.gzrt_return = code;
    
    /* Quit from GTK */
    gtk_main_quit();
}

/* Record a new process */
void gzrt_process_log ( pid_t pid )
{
    conf.procs = g_list_append( conf.procs, GINT_TO_POINTER(pid) );
    
    DEBUG( "Logged new process %i.", pid );
}

/* Remove a process */
void gzrt_process_remove ( pid_t pid )
{
    conf.procs = g_list_remove( conf.procs, GINT_TO_POINTER(pid) );
    
    DEBUG( "Removed process %i.", pid );
}

/* Main */
int main ( int argc, char ** argv )
{
    /* Initialize the application */
    gzrt_init( &argc, &argv );
    
    /* Sleep in gtk_main() */
    gtk_main();
    
    /* Deinitialize */
    gzrt_deinit( );
    
    /* Return predetermined value */
    return conf.gzrt_return;
}
