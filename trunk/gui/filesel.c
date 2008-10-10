/*****************************
* File Selection GUI Element *
*****************************/
#include <gzrt.h>

/* Structure */
static struct _gzrt_wfilesel gzrt_wfilesel;

/* Shorthand */
static struct _gzrt_wfilesel * w = &gzrt_wfilesel;

/* Vars */
static int status;

/* Open a file select box */
void gzrt_wfilesel_show ( void )
{
	/* Set up window */
	w->fswin = gtk_file_selection_new( GZRT_FILE_SELECT_TITLE );
	gtk_file_selection_set_filename( GTK_FILE_SELECTION(w->fswin), GZRT_FILE_DEFAULT_NAME );
	gtk_widget_show( w->fswin );
	
	/* Signal - OK button */
	g_signal_connect
	(
		G_OBJECT( GTK_FILE_SELECTION(w->fswin)->ok_button ), 
		"clicked", G_CALLBACK( gzrt_wfilesel_check ), (gpointer)w->fswin 
	);
	
	/* Signal - Cancel button button */
	g_signal_connect
	( 
		G_OBJECT( GTK_FILE_SELECTION(w->fswin)->cancel_button ), 
		"clicked", G_CALLBACK( gzrt_wfilesel_cancel ), (gpointer)w->fswin 
	);
	
	/* Signal - Closed */
	g_signal_connect
	(
		G_OBJECT( w->fswin ), "destroy", 
		G_CALLBACK( gzrt_wfilesel_cancel ), (gpointer)w->fswin 
	);
}

/* File selected. Perform checks */
void gzrt_wfilesel_check ( GtkWidget *wd, GtkFileSelection *fs )
{
	char *filename = (char*)gtk_file_selection_get_filename( fs );
	N64ROM *ctx;
	
	/* Debug */
	GZRTD_MESG( "Chose file \"%s\".", filename );
	
	/* Initialize ROM context */
	if( !(ctx = n64rom_load(filename)) )
		gzrt_notice( "Error occured!", n64rom_error() );
	else {
		status++;
		if( gzrt_wmain_create_new( ctx ) )
		{
		gtk_widget_destroy( w->fswin );}
	}
}

/* Window closed - which action? */
void gzrt_wfilesel_cancel ( void )
{
	if( !status && !window_amount )
		gzrt_gui_quit();
	else {
		if( status )
			status--;
		gtk_widget_destroy( w->fswin );
	}
}
