/***********************
* GZRT Signal Handlers *
***********************/
#include <stdio.h>
#include <signal.h>
#include "gzrt.h"
#include "msg.h"

/* CTRL-C handler */
static void gzrt_sigint ( int s )
{
    DEBUG( "CTRL-C caught." );
    
    /* Can we quit? */
    if( !(conf.gzrt_flags & GZRT_F_CANT_QUIT) )
    {
        conf.gzrt_return = -1;
        gtk_main_quit();
    }
}

/* TERM handler */
static void gzrt_sigterm ( int s )
{
    DEBUG( "SIGTERM caught." );
    
    /* Quit */
    conf.gzrt_return = -1;
    gtk_main_quit();
}
    

/* Initialize signal handlers */
void gzrt_signal_init ( void )
{
    signal( SIGINT,  gzrt_sigint  );
    signal( SIGTERM, gzrt_sigterm );
}
