/*****************************
* GZRT About Window (credit) *
*****************************/
#include <gzrt.h>

static struct _gzrt_wabout gzrt_wabout;

/* Pointer shorthand */
static struct _gzrt_wabout * w = &gzrt_wabout;

/* Initialize the window */
void gzrt_wabout_init ( void )
{
	PADDING *p;
	GtkWidget *a = gtk_alignment_new( 0.5, 0.5, 0.25, 0.25 );
	
	/* Create a new window */
	w->window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	
	/* Set window attrs */
	gtk_widget_set_size_request( w->window, GZRT_WABOUT_W, GZRT_WABOUT_H );
	gtk_window_set_title( GTK_WINDOW(w->window), GZRT_WABOUT_TITLE );
	gtk_window_set_position( GTK_WINDOW(w->window), GTK_WIN_POS_CENTER );
	
	/* Create padding */
	p = gzrt_wpadding_create( w->window );
	
	/* Create vbox */
	w->vbox = gtk_vbox_new( FALSE, 0 );
	
	/* Connect box to inner padding */
	gtk_box_pack_start( GTK_BOX(p->main), w->vbox, TRUE, TRUE, 0 );
	
	/* Close padding handle (we don't need it) */
	gzrt_wpadding_close( p );
	
	/* Create label */
	w->label = gtk_label_new( GZRT_WABOUT_MSG );
	gtk_label_set_justify( GTK_LABEL(w->label), GTK_JUSTIFY_CENTER );
	
	/* Create button */
	w->button = gtk_button_new_with_label( "Ok" );
	
	/* Pack! */
	gtk_container_add( GTK_CONTAINER(a), w->button );
	gtk_box_pack_start( GTK_BOX(w->vbox), w->label,  TRUE,  TRUE,  0 );
	gtk_box_pack_start( GTK_BOX(w->vbox), a, FALSE, FALSE, 0 );
	
	/* When deleted, recreate */
	g_signal_connect( G_OBJECT(w->window), "destroy", G_CALLBACK(gzrt_wabout_init), NULL );
	g_signal_connect( G_OBJECT(w->button), "clicked", G_CALLBACK(gzrt_wabout_hide), NULL );
}

/* Show the window */
void gzrt_wabout_show ( void )
{
	gtk_widget_show_all( w->window );
}

/* Hide the window */
void gzrt_wabout_hide ( void )
{
	gtk_widget_hide( w->window );
}
