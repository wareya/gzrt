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
	gzrt_malloc, gzrt_calloc, gzrt_free, gzrt_mem_use,
	
	/* Debugging / error handling */
	NULL, gzrt_gui_debug_add, gzrt_notice, NULL,
	
	/* Plugin cleanup */
	gzrt_plugin_cleanup, NULL
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
	GtkWidget * sep;
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
	
	/* Create menu seperator */
	sep = gtk_separator_menu_item_new();
	gtk_container_add( GTK_CONTAINER(menu), sep );
	
	/* Create plugin preferences entry */
	item = gtk_menu_item_new_with_mnemonic( "Preferences" );
	g_signal_connect_swapped( G_OBJECT(item), "activate", G_CALLBACK(gzrt_plugins_preferences), NULL );
	gtk_container_add( GTK_CONTAINER(menu), item );
	
	/* Initialized */
	init++;
	
	/* Show all */
	gtk_widget_show_all( menu );
	
	/* Return final product */
	return menu_head;
}

/* Preferences menu */
GtkWidget * gzrt_plugins_preferences ( int action )
{
	GtkWidget * window;
	GtkWidget * frame;
	GtkWidget * label;
	GtkWidget * hbox;
	GtkWidget * vbox;
	GtkWidget * align;
	GtkWidget * select;
	GtkWidget * ok;
	PLUGINS   * p = &plugins;
	static int  init;
	
	/* Are we just cleaning up? */
	if( action ) {
		init = 0;
		return NULL;
	}
	
	/* Already init'd */
	if( init )
		return NULL;
	
	/* Create window */
	window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	gtk_window_set_title( GTK_WINDOW(window), "Plugin preferences" );
	gtk_container_set_border_width( GTK_CONTAINER(window), 12 );
	
	/* Create vertical organizer */
	vbox = gtk_vbox_new( FALSE, 8 );
	
	/* Create "default plugin" frame */
	frame = gtk_frame_new( NULL );
	label = gtk_label_new("<b>Default plugin</b>");
	gtk_label_set_use_markup( GTK_LABEL(label), TRUE );
	gtk_frame_set_label_widget( GTK_FRAME(frame), label );
	
	/* Create frame alignment */
	align = gtk_alignment_new( 0.5f, 0.5f, 1.0f, 1.0f );
	gtk_alignment_set_padding( GTK_ALIGNMENT(align), 8, 8, 12, 12 );
	gtk_container_add( GTK_CONTAINER(frame), align );
	
	/* Create horizontal box for contents */
	hbox = gtk_hbox_new( FALSE, 8 );
	gtk_container_add( GTK_CONTAINER(align), hbox );
	
	/* Create combo box */
	select = gtk_combo_box_entry_new_text();
	
	/* Populate */
	while( p )
	{
		/* Append */
		gtk_combo_box_append_text( GTK_COMBO_BOX(select), p->meta->long_name );
		
		/* Next */
		p = p->next;
	}
	
	/* Create confirm button */
	ok = gtk_button_new_with_label( "Apply" );
	
	/* Pack everything */
	gtk_box_pack_start( GTK_BOX(hbox), select, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(hbox), ok,     TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(vbox), frame,  TRUE, TRUE, 0 );
	gtk_container_add( GTK_CONTAINER(window), vbox );
	
	/* Signals */
	g_signal_connect_swapped( G_OBJECT(ok), "clicked", G_CALLBACK(gzrt_set_default_plugin), (gpointer)window );
	g_signal_connect_swapped( G_OBJECT(window), "destroy", G_CALLBACK(gzrt_plugins_preferences), (gpointer)1 );
	
	/* Show everything */
	gtk_widget_show_all( window );
	
	/* Hookup data */
	HOOKUP( window, select, "select-box" );
	
	/* Set init status */
	init = 1;
	
	/* Return it */
	return window;
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

/* Call plugin */
void __gzrt_call_plugin ( void * file )
{
	struct PluginFileSpec * F = file;
	struct PluginTransac  * T = gzrt_calloc( sizeof(struct PluginTransac) );
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
			/* It's already open 
			GZRTD_MESG( "File \"%s\" already open.", F->filename );
			plugin_cleanup( &functions, F );*/
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

/* Call a plugin using the default */
void gzrt_call_plugin ( void * file )
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
	selected->meta->action( transaction );
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
