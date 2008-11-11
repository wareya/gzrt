/*******************
* GZRT GUI Globals *
*******************/
#include <gzrt.h>
#include <app/settings.h>

/* GUI quit handler */
void gzrt_gui_quit ( void )
{
	gtk_main_quit();
	exit( 0 );
}

/* GUI init */
void gzrt_gui_init ( int argc, char **argv )
{
	/* Set up icons */
	N64Rom *ctx = NULL;
	GList *list = NULL;
	static const char *icons[] =
	{
		GZRT_GUI_ICON_1, GZRT_GUI_ICON_2,
		GZRT_GUI_ICON_3, GZRT_GUI_ICON_4,
		NULL
	};
	int i;
	
	for( i = 0; icons[i]; i++ )
	{
		GdkPixbuf *g = gdk_pixbuf_new_from_file(icons[i], NULL);
		list = g_list_append( list, g );
	}
	
	/* Set default icon */
	gtk_window_set_default_icon_list( list );
	
	/* Show debug window */
	#ifdef GZRT_DEBUG
	 gzrt_gui_debug_create();
	#endif
	
	/* Load configuration */
	gzrt_config_load();
	
	/* Load plugins */
	gzrt_load_plugins();
	
	/* Find a file to open */
	int opened = FALSE;
	
	/* Is the default file set? */
	if( GZRTConfig.default_rom )
	{
		/* Yep, try it */
		if( (ctx = n64rom_load(GZRTConfig.default_rom)) )
		{
			
			/* Goes */
			if( !gzrt_wmain_create_new( ctx ) )
				
				/* File selection */
				gzrt_wfilesel_show();
			
			else
				
				opened = TRUE;
		}
		else 
			gzrt_notice( "Notice!", "There was a default ROM specified, but it wasn't found.\nPlease fix this!" );
	}
	
	/* Try command line arguments also */
	if( argc > 1 )
		for( i = 1; i < argc; i++ )
		{
			if( !(ctx = n64rom_load( argv[i] )) )
			{
				char buffer[128];
				sprintf(buffer, "Failed to open \"%s\".", argv[i]);
				gzrt_notice( "Error", buffer );
			}
			else if( gzrt_wmain_create_new( ctx ) )
					opened = TRUE;
		}
	
	/* Show splash? */
	if( !opened )
		gzrt_wsplash_init( (void*)gzrt_wfilesel_show );
}

/*
** Creates elements for padding a window with a 12px border
** Returns an hbox for which to place more elements
*/

/* Create a new instance, attach and return */
PADDING * gzrt_wpadding_create ( GtkWidget *window )
{
	/* Allocate memory */
	PADDING * w = malloc( sizeof(PADDING) );
	
	/* Create boxes */
	w->hbox = gtk_hbox_new( FALSE, 12 );
	w->vbox = gtk_vbox_new( FALSE, 12 );
	w->main = gtk_hbox_new( FALSE, 0  );
	
	/* Pack */
	gtk_box_pack_start( GTK_BOX(w->hbox), w->vbox, TRUE, TRUE, 12 );
	gtk_box_pack_start( GTK_BOX(w->vbox), w->main, TRUE, TRUE, 12 );
	
	/* Window attach */
	gtk_container_add( GTK_CONTAINER(window), w->hbox );
	
	/* Return struct */
	return w;
}

/* Close an already existing instance of it */
void gzrt_wpadding_close ( PADDING * h )
{
	free( h );
}

/* Create a standard window with 12px border */
WINDOW * gzrt_window_create ( char *title, int w, int h, int flags )
{
	WINDOW * win = malloc( sizeof(WINDOW) );
	
	/* Create window */
	win->window = gtk_window_new( GTK_WINDOW_TOPLEVEL  );
	gtk_window_set_title( GTK_WINDOW(win->window), title );
	gtk_window_set_position( GTK_WINDOW(win->window), GTK_WIN_POS_CENTER );
	gtk_window_set_destroy_with_parent( GTK_WINDOW(win->window), TRUE);
	
	/* Set size */
	if( w && h )
		gtk_widget_set_size_request( GTK_WIDGET(win->window), w, h );
	
	/* Create padding */
	win->hbox = gtk_hbox_new( FALSE, 12 );
	win->vbox = gtk_vbox_new( FALSE, 12 );
	win->main = gtk_hbox_new( FALSE, 0  );
	
	/* Pack 'em */
	gtk_box_pack_start( GTK_BOX(win->hbox), win->vbox, TRUE, TRUE, 12 );
	gtk_box_pack_start( GTK_BOX(win->vbox), win->main, TRUE, TRUE, 12 );
	
	/* Attach to window */
	gtk_container_add( GTK_CONTAINER(win->window), win->hbox );
	
	/* On destroy, free memory 
	g_signal_connect_swapped( G_OBJECT(win->window), "destroy", G_CALLBACK(gzrt_window_close), (gpointer)win );*/
	
	/* Return window */
	return win;
}

/* Attach a widget */
void gzrt_window_attach ( WINDOW *w, GtkWidget *o )
{
	gtk_box_pack_start( GTK_BOX(w->main), o, TRUE, TRUE, 0 );
}

/* Show a window */
void gzrt_window_show ( WINDOW *w )
{
	gtk_widget_show_all( w->window );
}

/* Hide a window */
void gzrt_window_hide ( WINDOW *w )
{
	gtk_widget_hide( w->window );
}

/* Close */
void gzrt_window_close ( WINDOW *w )
{
	gtk_widget_destroy( w->window );
	free( w );
}

