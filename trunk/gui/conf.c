/****************************
* GZRT Configuration Window *
****************************/
#include <gzrt.h>
#include <app/settings.h>

static GList     * parents;
static GtkWidget * rom;
static GtkWidget * plugin;
static GtkWidget * win;

/* Local functions */
static void use_current   ( MAINWIN * c );
static void find_other    ( MAINWIN * c );
static void save_settings ( MAINWIN * c );
static void destroy       ( MAINWIN * c );
static void destroyed     ( MAINWIN * c );

/* Generation functions */
static GtkWidget * frame_app_settings ( MAINWIN * c );
static GtkWidget * frame_file_handler ( MAINWIN * c );

/* Config tabs */
typedef GtkWidget * (*GENFUNC)( MAINWIN * c );
typedef struct
{
	const char * name;
	GENFUNC      gen;
}
SPEC;
static const SPEC
tabs[] = 
{
	{ "Application defaults", frame_app_settings },
	{ "File handlers",        frame_file_handler }
};

/* Create the window */
void gzrt_wconf_show ( MAINWIN * c )
{
	GtkWidget * window;
	GtkWidget * vbox;
	GtkWidget * align;
	GtkWidget * hbox;
	GtkWidget * button;
	GtkWidget * note;
	gint        i;
	
	/* Does this parent already exist? */
	if( g_list_find( parents, c ) )
		
		/* Yeah */
		return;
	
	/* Add parent */
	parents = g_list_append( parents, c );
	
	/* Create window */
	win = window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	gtk_window_set_title( GTK_WINDOW(window), "Configuration" );
	gtk_window_set_position( GTK_WINDOW(window), GTK_WIN_POS_CENTER );
	gtk_window_set_modal( GTK_WINDOW(window), TRUE );
	gtk_container_set_border_width( GTK_CONTAINER(window), 12 );
	g_signal_connect_swapped( G_OBJECT(window), "destroy", G_CALLBACK(destroyed), c );
	
	/* Create vbox */
	vbox = gtk_vbox_new( FALSE, 12 );
	gtk_container_add( GTK_CONTAINER(window), vbox );
	
	/* Generate notebook */
	note = gtk_notebook_new();
	for( i = 0; i < sizeof(tabs)/sizeof(SPEC); i++ )
	{
		gtk_notebook_append_page
		( 
			GTK_NOTEBOOK(note), 
			tabs[i].gen(c), 
			gtk_label_new(tabs[i].name)
		);
	}
	gtk_box_pack_start( GTK_BOX(vbox), note, TRUE, TRUE, 0 );
	
	/* Create buttons */
	align = gtk_alignment_new( 0.0f, 0.5f, 0.25f, 1.0f );
	gtk_box_pack_start( GTK_BOX(vbox), align, FALSE, TRUE, 0 );
	hbox = gtk_hbox_new( TRUE, 8 );
	gtk_container_add( GTK_CONTAINER(align), hbox );
	button = gtk_button_new_with_label( "Close" );
	gtk_box_pack_start( GTK_BOX(hbox), button, TRUE, TRUE, 0 );
	g_signal_connect_swapped( G_OBJECT(button), "clicked", G_CALLBACK(destroy), c );
	button = gtk_button_new_with_label( "Apply" );
	gtk_box_pack_start( GTK_BOX(hbox), button, TRUE, TRUE, 0 );
	g_signal_connect_swapped( G_OBJECT(button), "clicked", G_CALLBACK(save_settings), c );
	
	/* Show everything */
	gtk_widget_show_all( window );
}

/* Create file handler frame */
static GtkWidget *
frame_file_handler ( MAINWIN * c )
{
	static const char *
	names[] = 
	{
		"zdata",  "zactor",
		"zobj",   "zmap",
		"zscene", "zasm"
	};
	
	GtkWidget * label;
	GtkWidget * entry;
	GtkWidget * table;
	GtkWidget * align;
	GtkWidget * button;
	GtkWidget * hbox;
	
	char buffer[512];
	
	int i;
	                   
	align = gtk_alignment_new( 0.5f, 0.5f, 1.0f, 1.0f );
	gtk_alignment_set_padding( GTK_ALIGNMENT(align), 8, 8, 12, 12 );
	
	/* Create table */
	table = gtk_table_new( sizeof(names)/sizeof(char*), 2, FALSE );
	gtk_table_set_row_spacings( GTK_TABLE(table), 8 );
	gtk_table_set_col_spacings( GTK_TABLE(table), 8 );
	gtk_container_add( GTK_CONTAINER(align), table );
	
	/* Generate */
	for( i = 0; i < sizeof(names)/sizeof(char*); i++ )
	{
		/* Create label */
		snprintf( buffer, sizeof(buffer), "<span font_desc=\"Courier\">%s</span> handler:", names[i] );
		label = gtk_label_new( buffer );
		gtk_label_set_use_markup( GTK_LABEL(label), TRUE );
		gtk_misc_set_alignment( GTK_MISC(label), 0.0f, 0.5f );
		gtk_table_attach( GTK_TABLE(table), label, 0, 1, i, i + 1, GTK_FILL, 0, 0, 0 );
		
		/* Create hbox */
		hbox = gtk_hbox_new( FALSE, 4 );
		entry = gtk_entry_new();
		button = gtk_button_new_with_label( "Browse..." );
		gtk_box_pack_start( GTK_BOX(hbox), entry, TRUE, TRUE, 0 );
		gtk_box_pack_start( GTK_BOX(hbox), button, FALSE, FALSE, 0 );     
		gtk_table_attach( GTK_TABLE(table), hbox, 1, 2, i, i + 1, GTK_FILL | GTK_EXPAND, 0, 0, 0 );
	}
	
	/* Return the alignment */
	return align;
}

/* Create the application settings frame */
static GtkWidget * 
frame_app_settings ( MAINWIN * c )
{
	GtkWidget * vbox;
	GtkWidget * frame;
	GtkWidget * align;
	GtkWidget * hbox;
	GtkWidget * button;
	GtkWidget * label;
	GtkWidget * entry;
	GtkWidget * combo;
	GtkWidget * table;
	GList     * plugins;
	gint        i;
	
	/* Create content alignment */
	align = gtk_alignment_new( 0.5f, 0.5f, 1.0f, 1.0f );
	gtk_alignment_set_padding( GTK_ALIGNMENT(align), 8, 8, 12, 12 );
	
	/* Create table */
	table = gtk_table_new( 2, 2, FALSE );
	gtk_table_set_row_spacings( GTK_TABLE(table), 8 );
	gtk_table_set_col_spacings( GTK_TABLE(table), 8 );
	gtk_container_add( GTK_CONTAINER(align), table );
	
	/* Default ROM */
	label = gtk_label_new( "Default ROM:" );
	gtk_misc_set_alignment( GTK_MISC(label), 0.0f, 0.5f );
	gtk_table_attach( GTK_TABLE(table), label, 0, 1, 0, 1, GTK_FILL, 0, 0, 0 );
	hbox = gtk_hbox_new( FALSE, 4 );
	gtk_table_attach( GTK_TABLE(table), hbox, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, 0, 0, 0 );
	rom = entry = gtk_entry_new();
	gtk_box_pack_start( GTK_BOX(hbox), entry, TRUE, TRUE, 0 );
	button = gtk_button_new_with_label( "Current" );
	gtk_box_pack_start( GTK_BOX(hbox), button, FALSE, FALSE, 0 );
	g_signal_connect_swapped( G_OBJECT(button), "clicked", G_CALLBACK(use_current), c );
	button = gtk_button_new_with_label( "Other..." );
	gtk_box_pack_start( GTK_BOX(hbox), button, FALSE, FALSE, 0 );
	g_signal_connect_swapped( G_OBJECT(button), "clicked", G_CALLBACK(find_other), c );
	
	/* Is there a ROM default already? */
	if( GZRTConfig.default_rom )
		gtk_entry_set_text( GTK_ENTRY(rom), GZRTConfig.default_rom );
	
	/* Default plugin */
	label = gtk_label_new( "Default plugin:" );
	gtk_misc_set_alignment( GTK_MISC(label), 0.0f, 0.5f );
	gtk_table_attach( GTK_TABLE(table), label, 0, 1, 1, 2, GTK_FILL, 0, 0, 0 );
	plugin = combo = gtk_combo_box_entry_new_text();
	gtk_table_attach( GTK_TABLE(table), combo, 1, 2, 1, 2, GTK_FILL, 0, 0, 0 );
	
	/* Get list of plugins */
	plugins = gzrt_get_plugins();
	
	/* No plugins? */
	if( !plugins )
		gtk_widget_set_sensitive( plugin, FALSE );
	else
	for( i = 0; i < g_list_length(plugins); i++ )
	{
		char * nom = g_list_nth(plugins, i)->data;
		
		gtk_combo_box_append_text( GTK_COMBO_BOX(combo), nom );
		
		/* Is there a default already set? */
		if( GZRTConfig.default_plugin && !strcmp(GZRTConfig.default_plugin, nom) )
			gtk_combo_box_set_active( GTK_COMBO_BOX(combo), i );
	}
	g_list_free( plugins );
	
	return align;
}

/* Close the window */
static void
destroy ( MAINWIN * c )
{
	parents = g_list_remove( parents, c );
	gtk_widget_destroy( win );
}
static void
destroyed ( MAINWIN * c )
{
	parents = g_list_remove( parents, c );
}

/* Use current ROM */
static void
use_current ( MAINWIN * c )
{
	gtk_entry_set_text( GTK_ENTRY(rom), (gchar*)c->c->filename );
}

/* Find another ROM */
static void
find_other ( MAINWIN * c )
{
	gint	result;
	char  * filename;
	
	/* Create file saving dialog */
	GtkWidget * dialog = gtk_file_chooser_dialog_new
	( 
		"Choose a new ROM", NULL,
		GTK_FILE_CHOOSER_ACTION_SAVE, 
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN,   GTK_RESPONSE_ACCEPT, 
		NULL
	);
	gtk_widget_show_all( dialog );
	
	/* Run the dialog and fetch the result */
	while( (result = gtk_dialog_run( GTK_DIALOG(dialog) )) )
	switch( result )
	{
		/* A file has been chosen */
		case GTK_RESPONSE_ACCEPT:
		{
			filename = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(dialog) );;
			N64Rom * h;
			
			/* Can we open it? */
			if( !(h = n64rom_load( filename )) )
			{
				char buffer[512];
				
				/* Nope... */
				sprintf( buffer, "Unable to load this ROM. Error:\n%s", n64rom_error() );
				gzrt_notice( "Error", buffer );
				continue;
			}
			else
				n64rom_close( h );	

			/* Set the filename */
			gtk_entry_set_text( GTK_ENTRY(rom), filename );
			
			/* Destroy window */
			gtk_widget_destroy( dialog );
			
			/* Done */
			return;
		}
		break;
		
		/* Cancel */
		case GTK_RESPONSE_REJECT:
		 gtk_widget_destroy(dialog);
		 return;
		break;
		
		/* Default */
		default:
		 gtk_widget_destroy(dialog);
		 return;
	}
}

/* Save the settings */
static void
save_settings ( MAINWIN * c )
{
	const char * nom = gtk_entry_get_text( GTK_ENTRY(rom) );
	char * nom2 = gtk_combo_box_get_active_text( GTK_COMBO_BOX(plugin) );
	
	if( strlen(nom) > 0 ) {
		free( GZRTConfig.default_rom );
		GZRTConfig.default_rom = strdup(nom);
	}
	if( strlen(nom2) > 0 ) {
		free( GZRTConfig.default_plugin );
		GZRTConfig.default_plugin = strdup(nom2);
		gzrt_plugin_set_default( nom2 );
	}
	
	gzrt_config_save();
	
	gzrt_notice("Notice", "Settings saved successfully!");
	
	destroy(c);
}

