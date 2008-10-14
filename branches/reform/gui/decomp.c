/*****************************
* Zelda 64 Decompressor Menu *
*****************************/
#include <gzrt.h>
#include <z64.h>
#include <stdio.h>

static GList * parents;

/* Set progress bar text */
static void pbarset ( GtkWidget * p, double percent, char * fmt, ... )
{
	va_list	 ap;
	char	 buffer[128];
	
	va_start( ap, fmt );
	vsnprintf( buffer, sizeof(buffer), fmt, ap );
	va_end( ap );
	
	gtk_progress_bar_set_fraction( GTK_PROGRESS_BAR(p), percent );
	gtk_progress_bar_set_text( GTK_PROGRESS_BAR(p), buffer );
	
	while( gtk_events_pending() )
		gtk_main_iteration();
}
	
/* Show extractor window */
void gzrt_wdecomp_show ( MAINWIN * c )
{
	FILE          * h;
	GtkWidget     * window;
	GtkWidget     * pbar;
	GtkWidget     * dialog;
	int		        result;
	unsigned char * buffer;
	unsigned      * ftable;
	
	/* Does the parent exist? */
	if( g_list_find( parents, c ) )
		
		/* Yep... */
		return;
		
	/* Add parent */
	parents = g_list_append( parents, c );
		
	/* Create file saving dialog */
	dialog = gtk_file_chooser_dialog_new
	( 
		"Choose a destination", NULL,
		GTK_FILE_CHOOSER_ACTION_SAVE, 
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN,   GTK_RESPONSE_ACCEPT, 
		NULL
	);
	gtk_widget_show_all( dialog );
	
	/* Run the dialog and fetch the result */
	while( result = gtk_dialog_run( GTK_DIALOG(dialog) ) )
	switch( result )
	{
		/* A file has been chosen */
		case GTK_RESPONSE_ACCEPT:
		{
			char * n = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(dialog) );
			
			/* Check that file doesn't exist */
			if( (h = fopen(n, "rb")) )
			{
				gzrt_notice("Error", "File already exists.");
				fclose( h );
				continue;
			}
				
			/* Check that we can write to it */
			if( !(h = fopen(n, "wb")) )
			{
				gzrt_notice("Error", "Can't write to that file!");
				continue;
			}
			
			/* Destroy window */
			gtk_widget_destroy( dialog );
			
			/* Looks good, gogogo */
			goto decompress_rom;
		}
		break;
		
		/* Cancel */
		case GTK_RESPONSE_CANCEL:
		 gtk_widget_destroy(dialog);
		 parents = g_list_remove( parents, c );
		 goto skiploop;
		break;
		
		/* Default */
		default:
		 parents = g_list_remove( parents, c );
		 goto skiploop;
	}
	
	/* Out of loop */
skiploop: ;
	return;
	
	/* Decompress a ROM */
decompress_rom: ;
	FILE * out = h;
	int    i;
	
	/* Create window */
	window = gtk_window_new( GTK_WINDOW_POPUP);
    gtk_widget_set_size_request( window, 400, 30 );
	gtk_window_set_title( GTK_WINDOW(window), " " );
	gtk_window_set_position( GTK_WINDOW(window), GTK_WIN_POS_CENTER_ALWAYS );
	gtk_window_set_keep_above( GTK_WINDOW(window), TRUE );
	
	/* Create progress bar */
	pbar = gtk_progress_bar_new();
	gtk_container_add( GTK_CONTAINER(window), pbar );
	gtk_widget_show_all( window );
	pbarset( pbar, 0.0, "%%.2f%", 0.0 );
	
	/* Create scratch buffer */
	buffer = malloc( 8 * 1024 * 1024 );
	
	/* Create a new FS table */
	ftable = malloc(ZFSEnd(c->z) - ZFSStart(c->z));
	
	/* Begin decompression */
	for( i = 0; i < z64fs_entries(c->z); i++ )
	{
		Z64FSEntry * cur;
		
		/* Does this file exist? */
		if( !ZFileExists(c->z, i) )
			continue;
		
		/* Get current file entry */
		cur = z64fs_file(c->z, i);
		
		/* Read the file */
		z64fs_read_file( c->z, i, buffer );
		
		/* Write it */
		fwrite( buffer, ZFileVirtSize(c->z, i), 1, out );
		
		/* Update progress bar */
		if( !((i + 1) % (z64fs_entries(c->z) / 32)) )
			pbarset( pbar, (double)i / z64fs_entries(c->z), "%.2f%%", (double)i / z64fs_entries(c->z) * 100.0 );
	}
	
	/* Set 100% */
	pbarset( pbar, 1.0, "Padding file..." );
	
	/* Write padding */
	for( i = ftell( h ); i < 64 * 1024 * 1024; i++ )
		fputc( 0x00, h );
	
	/* Finished */
	gtk_widget_destroy( window );
	fclose( out );
	free( buffer );
	
	/* Remove it from parents list */
	parents = g_list_remove( parents, c );
	
	/* Show message */
	gzrt_notice( "Notice", "ROM decompression complete." );
}
