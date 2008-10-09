/**************************
* Display an error window *
**************************/
#include <gzrt.h>

/* Structure */
struct _gzrt_werror gzrt_werror;

/* Shorthand pointer variable */
static struct _gzrt_werror  * w = &gzrt_werror;

/* Display an error window */
void gzrt_werror_show ( char *t, char *s, ERRLEVEL errlevel )
{	
	/* Create new window */
	w->win = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	
	/* Debug */
	GZRTD_MESG("New notice box: \"%s\", \"%s\", %u.", t, s, errlevel );
	
	/* Set attributes */
	gtk_window_set_position(     GTK_WINDOW(w->win), GTK_WIN_POS_CENTER         );
	gtk_window_set_default_size( GTK_WINDOW(w->win), GZRT_ERROR_W, GZRT_ERROR_H );
	gtk_window_set_title(        GTK_WINDOW(w->win), t                          );
	
	/* Set icon */
	gtk_window_set_icon_from_file( GTK_WINDOW(w->win), GZRT_ERROR_ICO, NULL );
	
	/* Padding */
	PADDING * p = gzrt_wpadding_create( w->win );
	
	/* Create vbox for widget alignment */
	w->vbx = gtk_vbox_new( FALSE, 2 );
	
	/* Create label for displaying text */
	w->label = gtk_label_new( s );
	gtk_label_set_justify( GTK_LABEL(w->label), GTK_JUSTIFY_CENTER );
	
	/* Create new button */
	w->okb = gtk_button_new_with_label( GZRT_ERROR_OKB );
	
	/* Alignment */
	GtkWidget *a = gtk_alignment_new( 0.5, 1.0, 0.125, 1.0 );
	gtk_container_add( GTK_CONTAINER(a), w->okb );
	
	/* Pack into box */
	gtk_box_pack_start( GTK_BOX(w->vbx), w->label, TRUE,  TRUE,  2 );
	gtk_box_pack_start( GTK_BOX(w->vbx), a, FALSE, FALSE, 2 );
	
	/* Pack box into window */
	gtk_box_pack_start( GTK_BOX(p->main), w->vbx, TRUE, TRUE, 0 );
	gzrt_wpadding_close( p );
	
	/* Show all */
	gtk_widget_show_all( GTK_WIDGET(w->win) );
	gtk_window_present(  GTK_WINDOW(w->win) );
	
	/* Signals */
	if( errlevel == GZRT_ERR_CRITICAL )
	{
		g_signal_connect( G_OBJECT(w->okb), "clicked", G_CALLBACK( gzrt_gui_quit ), NULL );
		GZRTD_MESG_CONSOLE( "Fatal error encountered." );
		gtk_main();
	}
	else
		g_signal_connect( G_OBJECT(w->okb), "clicked", G_CALLBACK( gzrt_werror_destroy ), NULL );
}

/* Destroy any existing error window */
void gzrt_werror_destroy ( void )
{
	if( w->win )
	{
		gtk_widget_destroy( w->win );
		w->win = NULL;
	}
}
