/*******************
* GZRT: Open a ROM *
*******************/
#include <stdio.h>
#include <gtk/gtk.h>
#include "gzrt.h"
#include "file.h"
#include "msg.h"
#include "gui.h"

/* Open a non-zelda ROM */
GzrtFile * gzrt_gui_open_rom ( int what )
{
	int v;
	
	GzrtFile * ret = NULL;
	
	GtkWidget * dialog = gtk_file_chooser_dialog_new
	( 
		"Load ROM", NULL,
		GTK_FILE_CHOOSER_ACTION_OPEN, 
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, 
		NULL
	);
	
diagrun: ;
	/* Check outcome */
	switch( gtk_dialog_run(GTK_DIALOG(dialog)) )
	{
		/* Chose? */
		case GTK_RESPONSE_ACCEPT:
	    
		 /* Try to load the file */
		 if( !(ret = gzrt_file_open(what, gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(dialog) ))) )
		 {
			gzrt_message( MSG_ERROR, "Error", "An error occured while trying to load your ROM." );
		 }
		 else
		 {
			 /* Worked */
			 gtk_widget_hide( dialog );
			 
			 /* Notice */
			 gzrt_message( 0, "Notice", "Loaded \"%s\" successfully.", ret->name );
			 
			 goto outloop;
		 }
		
		break;
		
		/* NO! */
		case GTK_RESPONSE_CANCEL: case GTK_RESPONSE_DELETE_EVENT:
		
		 gtk_widget_destroy( dialog );
		 goto outloop;
		
		break;
	}
	
	/* Loop again! */
	goto diagrun;
	
	/* Out of loop */
outloop:
	return ret;
}
