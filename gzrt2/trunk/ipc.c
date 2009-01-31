/************************************
* GZRT - Interprocess communication *
************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "gzrt.h"
#include "msg.h"
#include "ipc.h"

/* Shared variables */
static int socket_fd = -1;
static GList * clients;
static struct sockaddr_un local, remote;

/* GZRT IPC daemon */
static void gzrt_ipc_main ( void )
{
    int commsock;
    static unsigned char buffer[512];
    struct gzrtIPCident * packet;
    struct gzrtClient * tmp;
    int len;
    
    /* Create the socket */
    local.sun_family = AF_UNIX;
    strcpy( local.sun_path, ".gzrt.socket" );
    socket_fd = socket( AF_UNIX, SOCK_STREAM, 0 );
    unlink( local.sun_path );
    len = strlen(local.sun_path) + sizeof(local.sun_family);
    bind( socket_fd, (struct sockaddr *)&local, len );
    
    /* Listen for clients */
    listen( socket_fd, 5 ); 
    
    DEBUG( "Now listening for clients." );
    
listen_loop:
    
    /* Accept a new connection */
    commsock = accept( socket_fd, (struct sockaddr*)&remote, &len );
    
    /* Recieve identification packet */
    recv( commsock, buffer, sizeof(struct gzrtIPCident), 0 );
    
    /* Identify packet */
    packet = (struct gzrtIPCident*)buffer;
    
    /* Check magic */
    if( packet->magic != GZRT_IPC_MAGIC )
    {
        /* Close the connection */
        close( commsock );
    }
    
    /* Add a new client */
    tmp = malloc( sizeof(struct gzrtClient) );
    tmp->pid = packet->pid;
    tmp->fd = commsock;
    clients = g_list_append( clients, tmp );
    
    DEBUG( "New client -> pid %i", tmp->pid );
    
    /* Loop again */
    goto listen_loop;
}

/* Initialize IPC */
void gzrt_ipc_init ( void )
{
    /* Create new thread for listening */
    pthread_create( &conf.ipc_thread, NULL, (void*)gzrt_ipc_main, NULL );
    
    DEBUG( "Created IPC thread." );
}

/* Terminate all clients */
void gzrt_ipc_kill_clients ( void )
{
    int i, l = g_list_length(clients);
    
    /* For now, just sending SIGTERM */
    for( i = 0; i < l; i++ )
    {
        int v;
        struct gzrtClient * c = g_list_nth(clients, i)->data;
        
        close( c->fd );
        
        if( !(v = kill( c->pid, SIGTERM )) )
            DEBUG( "Sent SIGTERM to %i...", c->pid );
        else
            DEBUG( "Error while sending SIGTERM to %i.", c->pid );
    }
    
    DEBUG( "All clients severed." );
}
