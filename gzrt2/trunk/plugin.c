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
#include <stdlib.h>
#include <glib.h> 
#include "gzrt.h"
#include "msg.h"
#include "ipc.h"
#include "plugin.h"

typedef const char cchr;

#define GZRT_PLUGINS_CONF   GZRT_PLUGIN_PATH "/plugins.ini"


/* ----------------------------------------------
   Plugin descriptor
   ---------------------------------------------- */
   
/* Plugin meta information */
struct gzrtPluginMeta
{
    cchr * name;
    cchr * author;
    cchr * group;
    cchr * version;
    cchr * desc;
};

/* Plugin descriptors */
struct gzrtPlugin
{
    /* Generic information */
    char * filename;
    
    /* Plugin meta information */
    struct gzrtPluginMeta * meta;
    
    /* Process ID */
    pid_t pid;
};

/* List of active plugins */
static GList * plugins;



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


/*
 * Note: Plugin handler configuration/details stored in the global
 * GZRT configuration struct (see gzrt.h)
 */

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
        plugin->filename = strdup(groups[i]);
        
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

/* Plugin manager main event loop */
static void gzrt_plugin_manager_main ( void )
{
loop:
    sleep( 1 );
    goto loop;
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
