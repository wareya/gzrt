/************************
* GZRT - Plugin manager *
************************/
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <poll.h>
#include <stdlib.h>
#include <glib.h> 
#include "gzrt.h"
#include "msg.h"
#include "file.h"
#include "plugin.h"

typedef const char cchr;

#define GZRT_PLUGINS_CONF   GZRT_PLUGIN_PATH "/plugins.ini"


/* ----------------------------------------------
   Plugin descriptor
   ---------------------------------------------- */

/* An identification packet */
struct gzrtPluginIdent
{
	int   magic;
	pid_t pid;
};

/* List of recognized plugins */
static GList * plugins;		/* Type is `GzrtPlugin` 		*/

/* List of plugin instances */
static GList * instances;		/* Type is `GzrtPluginInstance` */
static double inst_last_change;	/* Last time new plugin added   */


/* ----------------------------------------------
   Plugin communication
   ---------------------------------------------- */

/* Handle a packet from a plugin */
static void gzrt_plugin_manager_handle ( GzrtPluginInstance * P )
{
	static GzrtPacket packet;
	int ret;
	
	/* Read a packet in from the file descriptor */
	ret = read( P->fd[1], &packet, sizeof(packet) );
	
	/* Check return value */
	if( ret == -1 )
	{
		/* This plugin is not communicating properly - kill it */
		gzrt_plugin_manager_kill_plugin( P );
		
		return;
	}
	
	/* Check magic... */
	if( packet.magic != GZRT_PLUGIN_MAGIC )
	{
		/* This plugin is not identifying itself properly - kill */
		gzrt_plugin_manager_kill_plugin( P );
		
		return;
	}
	
	/* Notice */
	DEBUG( "Recieved packet from %i; data segment is %i bytes in length.", P->pid, packet.length );
}

/* Send some data to a plugin */
static void gzrt_plugin_manager_send_data ( GzrtPluginInstance * P, void * data, int size )
{
	static GzrtPacket packet;
	
	/* Set data... */
	packet.magic = GZRT_PLUGIN_MAGIC;
	packet.origin = getpid();
	gettimeofday( &packet.time, NULL );
	packet.length = size;
	
	/* Write packet header */
	write( P->fd[1], &packet, sizeof(packet) );
	
	/* Write remaining data */
	write( P->fd[1], data, size );
	
	/* Notice */
	DEBUG( "Send packet to %i; data segment is %u bytes.", P->pid, size );
}

/* Initialize the plugin communication module */
static void gzrt_plugin_manager_main ( void )
{
	static double last_update;
	static int watchcount;
	static struct pollfd * watchlist;;
	static int pollret = -1;
	static int status;
	static pid_t child;
	int i;
	
listen_loop: ;
	
	/* Rebuild? */
	if( last_update != inst_last_change )
	{
		int len = g_list_length(instances);
		
		DEBUG( "Rebuilding FD watch index." );
		
		/* Store new time */
		last_update = inst_last_change;
		
		/* Free old */
		if( watchlist )
          free( watchlist );
		
		/* Store len */
		watchcount = len;
		
		/* Yes, rebuild fd watch list */
		watchlist = calloc( sizeof(struct pollfd), len );
		
		/* Fill it */
		for( i = 0; i < len; i++ )
		{
			/* Get current plugin instance */
			GzrtPluginInstance * cur = g_list_nth( instances, i )->data;
			
			/* Store FD & flags */
			watchlist[i].fd = cur->fd[1];
			watchlist[i].events = POLLIN;
		}
	}
	
	/* Is there something to watch? */
	if( last_update && watchlist )
	{
		/* Yeah */
		pollret = poll( watchlist, watchcount, 100 );
	}
	else
	{
		/* Nope, just sleep */
		g_usleep( G_USEC_PER_SEC / 10 );
	}
	
	/* Check the return value of poll */
	if( pollret )
	{
		/* `pollret` file descriptors are ready for access */
		
		/* Handle each */
		for( i = 0; i < watchcount; i++ )
		{
			/* Check flags... */
			if( (watchlist[i].revents & POLLIN) )
				
				/* Handle it */
				gzrt_plugin_manager_handle( g_list_nth(instances, i)->data );
		}
	}
	
	/* Check the status of child processes */
	while( (child = waitpid( -1, &status, WNOHANG )) )
	{
		/* Error? */
		if( child == -1 )
			break; /* Handle this later... */
		
		/* Did the child quit? */
		if( WIFEXITED(status) || WIFSIGNALED(status) )
		{
			/* Yeah, remove references from it */
			gzrt_plugin_manager_unref_by_pid( child );
			
			DEBUG( "Lost child %i.", child );
		}
	}
    
    /* Loop again */
    goto listen_loop;
}


/* ----------------------------------------------
   INI Loader Descriptor
   ---------------------------------------------- */

static struct ConfEntry
{
    char * name;
    GType  type;
}
entries[] =
{
    { "name",        G_TYPE_STRING },
    { "author",      G_TYPE_STRING },
    { "group",       G_TYPE_STRING },
    { "version",     G_TYPE_STRING },
    { "description", G_TYPE_STRING },
    { NULL }
};

/* Load all the plugins */
static gboolean
gzrt_plugin_load ( void )
{
    struct gzrtPlugin * plugin;
    GKeyFile * plugconf;
    GError * error = NULL;
    char ** groups;
    int i, j, sanetized = 0, failed = 0;
    
    /* Open key file */
    plugconf = g_key_file_new();
    
    /* Load the key file */
    if( !g_key_file_load_from_file( plugconf, GZRT_PLUGINS_CONF, 0, &error ) )
    {
        /* Show error */
        gzrt_message( MSG_WARNING, "Warning", "Unable to parse \"%s\":\n%s",
        GZRT_PLUGINS_CONF, error->message );
        return FALSE;
    }
    
    /* Enumerate groups */
    groups = g_key_file_get_groups( plugconf, NULL );
    
    /* Loop through groups */
    for( i = 0; groups[i]; i++ )
    {
        struct stat tmp;
        char buff[128];
        
        /* Check that the file exists */
        snprintf( buff, sizeof(buff), GZRT_PLUGIN_PATH "/%s", groups[i] );
        if( stat(buff, &tmp) )
        {
            gzrt_message( MSG_WARNING, "Warning", "Unable to open plugin file "
            "\"%s\":\n%s", buff, strerror(errno) );
            failed++;
            continue;
        }
        
        /* Create a new plugin */
        plugin = malloc( sizeof(struct gzrtPlugin) );
        plugin->meta = malloc( sizeof(struct gzrtPluginMeta) );
        
        /* Load values */
        for( j = 0; entries[j].name; j++ )
        {
            cchr * val;
            
            /* Get value */
            val = g_key_file_get_value( plugconf, groups[i], entries[j].name, &error );
            
            /* Store it */
            ((char**)plugin->meta)[j] = strdup(val);
        }
        
        /* Store filename */
        plugin->filename = g_strdup_printf( "%s/%s", GZRT_PLUGIN_PATH, groups[i] );
        
        /* Append plugin */
        plugins = g_list_append( plugins, plugin );
        
        /* Update count */
        sanetized++;
    }
    
    /* Close key file -- we're done with it */
    g_key_file_free( plugconf );
    
    /* Failed? */
    if( failed )
    {
        gzrt_message( MSG_WARNING, "Warning", "There was an error loading one "
        "or more plugins - please fix this." );
    }
    
    DEBUG( "%i plugins processed.", sanetized );
    
    return (sanetized ? TRUE : FALSE);
}

/* Initialize the plugin manager */
void gzrt_plugin_manager_init ( void )
{
    if( gzrt_plugin_load() )
    {
        /* Create thread */
        pthread_create( &conf.pm_thread, NULL, (void*)gzrt_plugin_manager_main, NULL );
        
        DEBUG( "Plugin manager thread started." );
    }
    
    return;
}

/* Start a plugin */
void gzrt_plugin_manager_start_plugin ( GzrtPlugin * P )
{
	/* Record instance */
	GzrtPluginInstance * self;
	self->plugin = P;
	
	/* Allocate memory */
	self = calloc( sizeof(GzrtPluginInstance), 1);
	
	/* Set up a socket pair */
	socketpair(AF_UNIX, SOCK_STREAM, 0, self->fd);
	
	DEBUG( "%s\n", P->filename );
	
	/* Fork a new process */
	if( !(self->pid = fork()) )
	{
		/* Close standard output */
		close( 1 );
		
		/* Duplicate created file descriptor */
		dup( self->fd[0] );
		
		/* Close original */
		close( self->fd[0] );
		
		/* Call application */
		execl( P->filename, P->filename, NULL );
		
		/* It should never reach this point */
		exit( -1 );
	}
	
	/* Record new instance */
	instances = g_list_append( instances, self );
	
	/* Record last change */
	inst_last_change = timeSinceStart();
}

/* Get plugins */
GList * gzrt_plugin_manager_get_plugins ( void )
{
	return plugins;
}

/* Get plugins */
GList * gzrt_plugin_manager_get_instances ( void )
{
	return instances;
}

/* Kill all plugin instances */
void gzrt_plugin_manager_kill_all ( void )
{
	int i, l = g_list_length( instances );
	
	for( i = 0; i < l; i++ )
	{
		GzrtPluginInstance * p = g_list_nth( instances, i )->data;
		
		kill( p->pid, SIGTERM );
		
		DEBUG( "Sent SIGTERM to %i.", p->pid );
	}
}

/* Kill a plugin instance */
void gzrt_plugin_manager_kill_plugin ( GzrtPluginInstance * P )
{
	close( P->fd[0] );
	close( P->fd[1] );
	kill( P->pid, SIGTERM );
	free( P );
	
	/* Remove reference */
	instances = g_list_remove( instances, P );
	
	/* Record last change */
	inst_last_change = timeSinceStart();
}

/* Kill by PID */
void gzrt_plugin_manager_kill_plugin_by_pid ( pid_t pid )
{
	int i, l = g_list_length(instances);
	GzrtPluginInstance * P = NULL;
	
	for( i = 0; i < l; i++ )
	{
		P = g_list_nth( instances, i )->data;
		
		if( P->pid == pid )
		{
			/* Close sockets */
			close( P->fd[0] );
			close( P->fd[1] );
			
			/* Terminate process */
			kill( P->pid, SIGTERM );
		}
	}
	
	/* Remove reference */
	if( P != NULL )
	{
		instances = g_list_remove( instances, P );
	}
	
	free( P );
	
	/* Record last change */
	inst_last_change = timeSinceStart();
}

/* Unreference plugin by pid */
void gzrt_plugin_manager_unref_by_pid ( pid_t pid )
{
	int i, l = g_list_length(instances);
	GzrtPluginInstance * P = NULL;
	
	for( i = 0; i < l; i++ )
	{
		P = g_list_nth( instances, i )->data;
		
		if( P->pid == pid )
		{
			/* Close sockets */
			close( P->fd[0] );
			close( P->fd[1] );
		}
	}
	
	/* Remove reference */
	if( P != NULL )
	{
		instances = g_list_remove( instances, P );
	}
	
	free( P );
	
	/* Record last change */
	inst_last_change = timeSinceStart();
}


/* --------------------------------------
   Plugin request handlers
   -------------------------------------- */

/* A ROM enumeration */
typedef struct
{
	int amount;
	int lengths[];
	/* path strings placed here */
}
RGzrtEnumRom;

/* A ROM read request */
typedef struct
{
	int length;
	unsigned char data[];
}
RGzrtRomRead;

/* Enumerate ROMs open */
static void gzrt_p_enum_roms ( GzrtPluginInstance * P )
{
	GzrtFile * cur;
	GList * files;
	RGzrtEnumRom * final;
	int total = 0, len, tlen, i, amount;
	void * target;
	
	/* Get current list */
	files = gzrt_files_open();
	
	/* Count total length of strings */
	amount = g_list_length( files );
	for( i = 0; i < amount; i++ )
	{
		cur = g_list_nth( files, i )->data;
		len += strlen(cur->path) + 1;
	}
	
	/* Length aquired, prepare final */
	tlen = sizeof(RGzrtEnumRom) + len * sizeof(int) + len;
	final = malloc( tlen );
	
	/* Write amount */
	final->amount = amount;
	
	/* Write lengths */
	for( i = 0; i < amount; i++ )
	{
		cur = g_list_nth( files, i )->data;
		
		final->lengths[i] = strlen( cur->path ) + 1;
	}
	
	/* String target */
	target = final + sizeof(int) + len * sizeof(int);
	
	/* Write strings */
	for( i = 0; i < amount; i++ )
	{
		cur = g_list_nth( files, i )->data;
		
		strcpy( target, cur->path );
		
		final += final->lengths[i];
	}
	
	/* Send it to plugin */
	gzrt_plugin_manager_send_data( P, final, tlen );
}

/* Read some stuff from a ROM */
static int gzrt_p_rom_read ( char * path, int offset, int len )
{
}
	
