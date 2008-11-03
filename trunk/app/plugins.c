/*********************
* GZRT Plugin System *
*********************/
#include <gzrt.h>
#include <dirent.h>
#include <string.h>
#include <app/settings.h>

/* Constants */
#define PLUGINS_DIR			"plugins"
#define PLUGIN_META_NAME	"gzrt_plugin_info"
#ifdef WIN32
# define PLUGIN_EXT ".dll"
#else
# define PLUGIN_EXT ".so"
#endif

/* Connect data to a widget */
#define HOOKUP( component, widget, name )                       \
    g_object_set_data_full( G_OBJECT(component), name,          \
    gtk_widget_ref(widget), (GDestroyNotify)gtk_widget_unref )

/* Lookup connected data */
#define LOOKUP( component, name )   \
    g_object_get_data( G_OBJECT(component), name )

/* Inherited functions struct */
static const struct Functions 
functions =
{
	/* Memory management */
	(void*)gzrt_malloc, 
	(void*)gzrt_calloc, 
	(void*)gzrt_free, 
	(void*)gzrt_mem_use,
	
	/* Debugging / error handling */
	NULL, 
	(void*)gzrt_gui_debug_add, 
	(void*)gzrt_notice, 
	NULL,
	
	/* Plugin cleanup */
	(void*)gzrt_plugin_cleanup, 
	NULL
};

/* Plugin list */
typedef struct
{
	/* Dynamic library context */
	GModule * dl;
	
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
static PLUGINS   plugins;				/* List of plugins          */
static PLUGINS * selected = &plugins;	/* Selected plugin			*/
static int	     total;					/* Total number of plugins	*/

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
			item = gtk_image_menu_item_new_with_mnemonic( p->meta->long_name );
			
			/* Add it to menu */
			gtk_container_add( GTK_CONTAINER(menu), item );
			
			/* Set handler */
			if( p->meta->menu_bar )
				g_signal_connect_swapped( G_OBJECT(item), "activate", G_CALLBACK(p->meta->menu_bar), NULL );
			
			/* Call the functions init function (if applicable) */
			if( !init && p->meta->init )
				p->meta->init( &functions );
			
			/* Seek to next */
			p = p->next;
		}
	
	/* Create menu seperator 
	sep = gtk_separator_menu_item_new();
	gtk_container_add( GTK_CONTAINER(menu), sep );*/
	
	/* Create plugin preferences entry 
	item = gtk_menu_item_new_with_mnemonic( "Preferences" );
	g_signal_connect_swapped( G_OBJECT(item), "activate", G_CALLBACK(gzrt_plugins_preferences), NULL );
	gtk_container_add( GTK_CONTAINER(menu), item );*/
	
	/* Initialized */
	init++;
	
	/* Show all */
	gtk_widget_show_all( menu );
	
	/* No plugins? */
	gtk_widget_set_sensitive( menu_head, (plugins.dl ? TRUE : FALSE) );
	
	/* Return final product */
	return menu_head;
}

/* Return a list of plugins */
GList *
gzrt_get_plugins ( void )
{
	GList * list = NULL;
	PLUGINS * p = &plugins;
	
	while( p && p->dl )
	{
		list = g_list_append( list, p->fn + 8 );
		p = p->next;
	}
	
	return list;
}

/* Return the short name of the default plugin */
const char *
gzrt_plugin_default_get_short ( void )
{
	if( selected->meta )
		return selected->meta->short_name;
	else
		return NULL;
}

/* Return the long name of the default plugin */
const char *
gzrt_plugin_default_get_long ( void )
{
	if( selected->meta )
		return selected->meta->long_name;
	else
		return NULL;
}

/* Set default plugin */
void gzrt_set_default_plugin ( GtkWidget * window )
{
	GtkWidget * select = LOOKUP( window, "select-box" );
	PLUGINS   * p = &plugins;
    char * text = gtk_combo_box_get_active_text( GTK_COMBO_BOX(select) );
	
	while( p )
	{
		if( !strcmp( p->meta->long_name, text ) )
		{
			selected = p;
			GZRTD_MESG( "Default plugin set to \"%s\".", text );
			gtk_widget_destroy( window );
			return;
		}
		
		p = p->next;
	}
	
}

/* Set a default plugin from filename */
void gzrt_plugin_set_default ( char * name )
{
	PLUGINS * p = &plugins;
	
	while( p && p->dl )
	{
		if( strstr( p->fn, name ) )
		{
			selected = p;
			break;
		}
		p = p->next;
	}
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
	if( !(p = g_module_open(name, G_MODULE_BIND_LOCAL)) )
		return NULL;
	
	/* Check if there is plugin metadata */
	if( !g_module_symbol(p, PLUGIN_META_NAME, &t) )
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
		
		/* Proper prefix */
		if( !strstr(ent->d_name, PLUGIN_EXT) )
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
		if( !(data = load_plugin( buffer )) ) {
			GZRTD_MESG( "Error: %s", g_module_error() );
			continue;
		}
		else
			GZRTD_MESG( "Loaded plugin \"%s\".", data->long_name );
		
		/* Default? */
		if( GZRTConfig.default_plugin )
		if( !strcmp(ent->d_name, GZRTConfig.default_plugin) )
		{
			PLUGINS * t = &plugins;
			
			while( t->next )
				t = t->next;
			
			selected = t;
			
			GZRTD_MESG( "Plugin set as default." );
		}
	}
	
	/* Close directory */
	closedir(handle);
	
	/* Did we load any plugins? */
	if( !plugins.dl )
		GZRTD_MESG( "No plugins found." );
}

/* Call a plugin using the default */
void gzrt_call_plugin ( void * RomSpec, void * file )
{
	struct PluginTransac  * transaction;
	struct PluginFileSpec * filedata = file;
	GList * result;
	
	/* Quick check */
	if( !total )
		return;
	
	/* Check for duplicate instance of this plugin & file */
	if( (result = g_list_find_custom( selected->files, 
		filedata->filename, (GCompareFunc)strcmp )) )
	{
		GZRTD_MESG( "File \"%s\" is already open.", filedata->filename );
		gzrt_free( filedata->file );
		gzrt_free( filedata       );
		return;
	}
	
	/* Store the name in the list */
	selected->files = g_list_append( selected->files, filedata->filename );
	
	/* Prepare transaction information */
	transaction			= gzrt_calloc( sizeof(struct PluginTransac) );
	transaction->file	= filedata;
	transaction->plugin = selected;
	
	/* Call the plugin */
	selected->meta->action( &functions, RomSpec, transaction );
}

/* Plugin cleanup */
void gzrt_plugin_cleanup ( struct PluginTransac * t )
{
	/* Remove the file from the file list */
	((PLUGINS*)t->plugin)->files = g_list_remove( ((PLUGINS*)t->plugin)->files, t->file->filename );
	
	/* Debug */
	GZRTD_MESG( "File \"%s\" closed.", t->file->filename );
	
	/* Free file data */
	gzrt_free( t->file->file );
	gzrt_free( t->file       );
	gzrt_free( t             );
}

/* Return the name of the current plugin */
char * gzrt_plugin_curname ( void )
{
	return selected->meta->short_name;
}

