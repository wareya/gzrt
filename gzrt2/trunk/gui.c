/********************************
* GZRT Graphical User Interface *
********************************/
#include <stdio.h>
#include <gtk/gtk.h>
#include "gzrt.h"
#include "gui.h"

/* Set the window icon sets */
static void gzrt_gui_icons_init ( void )
{
	int i;
	GList * list = NULL;
	static const char * icons[] =
	{
		GZRT_GUI_RES_PATH "/ico16x16.png",
		GZRT_GUI_RES_PATH "/ico32x32.png",
		GZRT_GUI_RES_PATH "/ico64x64.png",
		NULL
	};
	
	/* Create GList of icons */
	for( i = 0; icons[i]; i++ )
	{
		/* Load icons */
		GdkPixbuf * g = gdk_pixbuf_new_from_file( icons[i], NULL );
		
		/* Append to list */
		list = g_list_append( list, g );
	}
	
	/* Set default icon */
	gtk_window_set_default_icon_list( list );
}

/* Create the start screen */
static void gzrt_gui_create_start_screen ( void )
{
	GtkWidget * window;
	GtkWidget * image;
	GtkWidget * vbox;
	GtkWidget * hbox;
	GtkWidget * button1;
	GtkWidget * button2;
	GtkWidget * button3;
	GtkWidget * button4;
	
	/* Create window */
	window  = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	image   = gtk_image_new_from_file( GZRT_GUI_RES_LOGO );
	vbox    = gtk_vbox_new( FALSE, 8 );
	hbox    = gtk_hbox_new( TRUE, 8 );
	button1 = gtk_button_new_with_mnemonic( "_Open ROM" );
	button2 = gtk_button_new_with_mnemonic( "_Quickstart" );
	button3 = gtk_button_new_with_mnemonic( "_About" );
	button4 = gtk_button_new_with_mnemonic( "_Exit" );
	gtk_widget_set_size_request( button1, -1, 35 );
	gtk_window_set_title( GTK_WINDOW(window), GZRT_GUI_WINDOW_TITLE );
	gtk_window_set_position( GTK_WINDOW(window), GTK_WIN_POS_CENTER_ALWAYS );
	gtk_window_set_resizable( GTK_WINDOW(window), FALSE );
	gtk_window_set_deletable( GTK_WINDOW(window), FALSE );
	gtk_container_set_border_width( GTK_CONTAINER(window), 10 );
	gtk_container_add( GTK_CONTAINER(window), vbox );
	gtk_box_pack_start( GTK_BOX(vbox), image, FALSE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(vbox), hbox, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(hbox), button1, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(hbox), button2, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(hbox), button3, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(hbox), button4, TRUE, TRUE, 0 );
	gtk_widget_show_all( window );
	
	/* Set up signals */
	g_signal_connect_swapped( G_OBJECT(button4), "clicked", G_CALLBACK(gzrt_exit), (void*)0 );
}

/* Create the GUI */
void gzrt_gui_init ( void )
{
	/* Initialize icons */
	gzrt_gui_icons_init();
	
	/* Show start screen */
	gzrt_gui_create_start_screen();
}
