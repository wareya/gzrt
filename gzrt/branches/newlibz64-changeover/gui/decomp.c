/*****************************
* Zelda 64 Decompressor Menu *
*****************************/
#include <gzrt.h>
#include <z64.h>
#include <stdio.h>

#define WU32(x, w)          \
{                           \
	(x)[0] = (w)>>24; 		\
	(x)[1] = (w)>>16& 0xFF; \
	(x)[2] = (w)>>8 & 0xFF; \
	(x)[3] = (w) & 0xFF;    \
}

#define WRITE_ENTRY(p, id, i, w) \
	WU32(&p[id * 16 + i * 4], w)

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
	unsigned char * ftable;
	
	/* Does the parent exist? */
	if( g_list_find( parents, c ) )
		
		/* Yep... */
		return;
		
	/* Check filesize */
	if( c->c->filesize >= 40 * 1024 * 1024 )
	{
		gzrt_notice( "Error", "This ROM is already decompressed!" );
		return;
	}
		
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
	while( (result = gtk_dialog_run( GTK_DIALOG(dialog) )) )
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
		case GTK_RESPONSE_REJECT:
		 gtk_widget_destroy(dialog);
		 parents = g_list_remove( parents, c );
		 goto skiploop;
		break;
		
		/* Default */
		default:
		 parents = g_list_remove( parents, c );
		 gtk_widget_destroy(dialog);
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
	window = gtk_window_new( GTK_WINDOW_POPUP );
    gtk_widget_set_size_request( window, 400, 30 );
	gtk_window_set_title( GTK_WINDOW(window), " " );
	gtk_window_set_position( GTK_WINDOW(window), GTK_WIN_POS_CENTER_ALWAYS );
	gtk_window_set_keep_above( GTK_WINDOW(window), TRUE );
	gtk_window_set_modal( GTK_WINDOW(window), TRUE );
	
	/* Create progress bar */
	pbar = gtk_progress_bar_new();
	gtk_container_add( GTK_CONTAINER(window), pbar );
	gtk_widget_show_all( window );
	pbarset( pbar, 0.0, "%.2f%%", 0.0 );
	
	/* Create scratch buffer */
	buffer = gzrt_malloc( 8 * 1024 * 1024 );
	
	/* Create a new FS table */
	ftable = gzrt_malloc(ZFSEnd(c->z->fs) - ZFSStart(c->z->fs));
	
	/* Begin decompression */
	for( i = 0; i < z64fs_entries(c->z->fs); i++ )
	{
		/* Does this file exist? */
		if( !ZFileExists(c->z->fs, i) )
		{
			WRITE_ENTRY( ftable, i, 0, ZFileVirtStart(c->z->fs, i) );
			WRITE_ENTRY( ftable, i, 1, ZFileVirtEnd(c->z->fs, i)   );
			WRITE_ENTRY( ftable, i, 2, 0xFFFFFFFF 			   );
			WRITE_ENTRY( ftable, i, 3, 0xFFFFFFFF 			   );
			continue;
		}
		
		/* Read the file */
		z64fs_read_file( c->z, i, buffer );
		
		/* Write it */
		fwrite( buffer, ZFileVirtSize(c->z->fs, i), 1, out );
		
		/* Update new file table */
		WRITE_ENTRY( ftable, i, 0, ZFileVirtStart(c->z->fs, i) );
		WRITE_ENTRY( ftable, i, 1, ZFileVirtEnd(c->z->fs, i)   );
		WRITE_ENTRY( ftable, i, 2, ZFileVirtStart(c->z->fs, i) );
		WRITE_ENTRY( ftable, i, 3, 0 					   );
		
		/* Update progress bar */
		if( !((i + 1) % (z64fs_entries(c->z->fs) / 32)) )
			pbarset( pbar, (double)i / z64fs_entries(c->z->fs), "%.2f%%", (double)i / z64fs_entries(c->z->fs) * 100.0 );
	}
	
	/* Write new file table */
	int tmp = ftell(out);
	fseek( out, ZFSStart(c->z->fs), SEEK_SET );
	fwrite( ftable, 1, ZFSEnd(c->z->fs) - ZFSStart(c->z->fs), out );
	fseek( out, tmp, SEEK_SET );
	
	/* Set 100% */
	pbarset( pbar, 1.0, "Padding file..." );
	
	/* Write padding */
	for( i = ftell( out ); i < 64 * 1024 * 1024; i++ )
		fputc( 0x00, out );
	
	/* Fix CRCs */
	pbarset( pbar, 1.0, "Fixing CRCs..." );
	/* n64rom_crc_quick( out ); */
	
	/* Finished */
	gtk_widget_destroy( window );
	fclose( out );
	gzrt_free( buffer );
	gzrt_free( ftable );
	
	/* Remove it from parents list */
	parents = g_list_remove( parents, c );
	
	/* Show message */
	gzrt_notice( "Notice", "ROM decompression complete." );
}

