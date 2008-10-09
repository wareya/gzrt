/*********************
* GZRT Plugin System *
*********************/
#include <gzrt.h>
#include <dirent.h>
#include <dlfcn.h>

/* Constants */
#define PLUGINS_DIR			"plugins"
#define PLUGIN_META_NAME	"gzrt_plugin_info"

/* Inherited functions struct */
static const struct Functions 
functions =
{
	/* Malloc */
	gzrt_malloc,
	
	/* Calloc */
	gzrt_calloc,
	
	/* Free */
	gzrt_free,
	
	/* Memory usage */
	gzrt_mem_use
};

/* Plugin list */
typedef struct
{
	/* Dynamic library context */
	void * dl;
	
	/* Metadata */
	struct PluginMeta * meta;
	
	/* Filename */
	char * fn;
	
	/* List of file open */
	GList * files;
	
	/* Next plugin */
	void * next;
}
PLUGINS;

/* List */
static PLUGINS  plugins;	/* List of plugins          */
static int	    total;		/* Total number of plugins	*/

/* Get amount of plugins */
int gzrt_plugins_count ( void )
{
	return total;
}

/* Generate plugins menu item */
GtkWidget * gzrt_plugins_menu ( void )
{
	GtkWidget * menu_head;
	GtkWidget * menu;
	GtkWidget * item;
	PLUGINS   * p = &plugins;
	static int  init;
	
	/* Create title object */
	menu_head = gtk_menu_item_new_with_mnemonic( "_Plugins" );
	
	/* Create menu object */
	menu = gtk_menu_new();
	gtk_menu_item_set_submenu( GTK_MENU_ITEM(menu_head), menu );
	
	/* Create each respective plugin's entry */
	if( plugins.dl )
		while( p )
		{
			/* Create menu entry */
			item = gtk_image_menu_item_new_with_mnemonic( p->meta->short_name );
			gtk_widget_show( item );
			
			/* Add it to menu */
			gtk_container_add( GTK_CONTAINER(menu), item );
			
			/* Set handler */
			g_signal_connect_swapped( G_OBJECT(item), "activate", G_CALLBACK(p->meta->menu_bar), NULL );
			
			/* Call the functions init function (if applicable) */
			if( !init && p->meta->init )
				p->meta->init( &functions );
			
			/* Seek to next */
			p = p->next;
		}
	
	/* Initialized */
	init++;
	
	/* Return final product */
	return menu_head;
}

/* Add a plugin to list */
static void add_plugin ( char * name, void * p, void * meta )
{
	PLUGINS * j = &plugins;                                     
	
	/* Discover end of list */
	while( j->next )
		j = j->next;
	
	/* Is this instance used? */
	if( !j->dl )
	{
		/* Nope */
		j->dl = p;
		j->fn = strdup( name );
	}
	else
	{
		/* Yes, new space */
		j->next = calloc( sizeof(PLUGINS), 1 );
		j = j->next;
		
		/* Store */
		j->dl = p;
		j->fn = strdup( name );
	}
	
	/* Store metadata */
	j->meta = meta;
	
	/* Increase total */
	total++;
}

/* Load a plugin + append to list */
static struct PluginMeta * 
load_plugin ( char * name )
{
	void * p, * t;
	
	/* Open */
	if( !(p = dlopen(name, RTLD_LOCAL | RTLD_NOW)) )
		return NULL;
	
	/* Check if there is plugin metadata */
	if( !(t = dlsym( p, PLUGIN_META_NAME )) )
		return NULL;
	
	/* Loaded - append to list */
	add_plugin( name, p, t );
	
	/* Success */
	return t;
}

/* Load plugins */
void gzrt_load_plugins ( void )
{
	DIR					* handle;
    struct dirent		* ent;
	char				  buffer[512];
	struct PluginMeta	* data;
	
	/* Status message */
	GZRTD_MESG( "Loading plugins from dir \"%s\"...", PLUGINS_DIR );
	
	/* Open directory */
	if( !(handle = opendir(PLUGINS_DIR)) )
	{
		GZRTD_MESG( "Failed to open directory for reading." );
		return;
	}
	
	/* Loop through each file */
	while( (ent = readdir(handle)) != NULL )
	{
		DIR * tmp;
		
		/* No "." or ".." */
		if( !strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..") )
			continue;
		
		/* Concatenate */
		sprintf( buffer, "%s" GZRT_SLASH "%s", PLUGINS_DIR, ent->d_name );
		
		/* Is it a directory? */
		if( (tmp = opendir(buffer)) )
		{
			closedir(tmp);
			continue;
		}
		
		/* Load it */
		if( !(data = load_plugin( buffer )) )
			GZRTD_MESG( "Error: %s", dlerror() );
		else
			GZRTD_MESG( "Loaded plugin \"%s\".", data->long_name );
	}
	
	/* Did we load any plugins? */
	if( !plugins.dl )
		GZRTD_MESG( "No plugins found." );
}

/* Call plugin */
void gzrt_call_plugin ( void * file )
{
	struct PluginFileSpec * F = file;
	GList * result;
	GZRTD_MESG( "Plugin action requested.", PLUGINS_DIR );
	
	/* Check the plugin total */
	if( total == 1 )
	{
		/* Only one */
		GZRTD_MESG( "Only one plugin present - \"%s\".", plugins.meta->short_name );
		
		/* Check that this plugin does not have this file open already */
		if( !(result = g_list_find_custom( plugins.files, F->filename, strcmp )) )
		{
			/* Append it */
			plugins.files = g_list_append( plugins.files, strdup(F->filename) );
			
			/* Call handler */
			plugins.meta->action( file );
		}
		else
		{
			/* It's already open */
			GZRTD_MESG( "File \"%s\" already open.", F->filename );
			plugin_cleanup( &functions, F );
		}
	}
	else 
	{
		GtkWidget * radio;
		GtkWidget * window;
		GList	  * group;
		
		return;
	}
	
	/* Done */
	GZRTD_MESG( "Plugin request serviced." );
}
