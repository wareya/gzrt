/*****************************
* GZRT About Window (credit) *
*****************************/
#include <gzrt.h>
#include <gui/about.h>

/* Does this window exist already? */
static int status;

/* Show the window */
void gzrt_wabout_show ( void )
{
	GtkWidget * window;
	GtkWidget * align;
	GtkWidget * vbox;
	GtkWidget * label;
	GtkWidget * button;
	
	/* Already? */
	if( status )
		return;
	
	/* Create */
	window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	align = gtk_alignment_new( 0.5, 0.5, 0.25, 0.25 );
	
	/* Set window attributes */
	gtk_window_set_position( GTK_WINDOW(window), GTK_WIN_POS_CENTER );
	gtk_container_set_border_width( GTK_CONTAINER(window), 12 );
	gtk_window_set_title( GTK_WINDOW(window), "About" );
	
	/* Create vbox */
	vbox = gtk_vbox_new( FALSE, 8 );
	gtk_container_add( GTK_CONTAINER(window), vbox );
	
	/* Create label */
	label = gtk_label_new( GZRT_WABOUT_MSG );
	gtk_label_set_justify( GTK_LABEL(label), GTK_JUSTIFY_CENTER );
	
	/* Create button */
	button = gtk_button_new_with_label( "Ok" );
	
	/* Pack! */
	gtk_container_add( GTK_CONTAINER(align), button );
	gtk_box_pack_start( GTK_BOX(vbox), label,  TRUE,  TRUE,  0 );
	gtk_box_pack_start( GTK_BOX(vbox), align, FALSE, FALSE, 0 );
	
	/* When deleted, recreate */
	g_signal_connect( G_OBJECT(window), "destroy", G_CALLBACK(gzrt_wabout_closed), NULL );
	g_signal_connect_swapped( G_OBJECT(button), "clicked", G_CALLBACK(gzrt_wabout_close), window );
	
	/* SHow it */
	gtk_widget_show_all( window );
	
	/* Set status */
	status = TRUE;
}

/* Window has been closed */
void gzrt_wabout_closed ( void )
{
	status = FALSE;
}

/* Close the window */
void gzrt_wabout_close ( GtkWidget * w )
{
	status = FALSE;
	gtk_widget_destroy( w );
}
