/**********************
* GUI Padding Element *
**********************/
#include <gzrt.h>

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
