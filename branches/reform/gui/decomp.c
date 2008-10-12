/*****************************
* Zelda 64 Decompressor Menu *
*****************************/
#include <gzrt.h>

/* Array of windows (for each open ROM) */
static struct _gzrt_wdecompress 
	gzrt_wdecompress[GZRT_WMAIN_MAX];

/* MWI interface for the decompressor */
static MWI gzrt_wdecompress_mwi;

/* Create a new zdec window */
void gzrt_wdecompress_create ( MAINWIN *mw )
{
	char buffer[128];
	
	/* Is this ROM already decompressed? */
	if( mw->c->filesize > 48 * 1024 * 1024 )
	{
		gzrt_werror_show( "Error", "This ROM is already decompressed!", 0 );
		return;
	}
	
	/* Clear memory we're using */
	struct _gzrt_wdecompress * w = &gzrt_wdecompress[mw->id];
	memset( w, sizeof(struct _gzrt_wdecompress), 0 );
	
	/* Disable creation of any more menus */
	gzrt_wmain_disable_item( mw, "decompress_rom1" );
	
	/* Store main window */
	w->mw = mw;
	
	/* Initialize internal window */
	w->win = gzrt_window_create( GZRT_WDECOMPRESS_T, 0, 0, 0 );
	
	/* Layout elements */
	w->vbox_outer = gtk_vbox_new( FALSE, 4 );
	w->vbox_inner = gtk_vbox_new( FALSE, 4 );
	
	/* Create frame with settings */
	w->frame = gtk_frame_new( "Options" );
	
	/* Create settings */
	GtkWidget *a = gtk_alignment_new( 0.0, 1.0, 0.0, 1.0 );
	w->label = gtk_label_new( "Output file:" );
	gtk_container_add( GTK_CONTAINER(a), w->label );
	w->hbox = gtk_hbox_new( FALSE, 4 );
	w->entry = gtk_entry_new();
	w->button = gtk_button_new_with_label( "Browse..." );
	
	/* Final OK button */
	w->ok_button = gtk_button_new_with_label( "Decompress" );
	
	/* Pack settings */
	gtk_box_pack_start( GTK_BOX(w->hbox), w->entry, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(w->hbox), w->button, TRUE, TRUE, 0 );
	
	/* Padding */
	w->pad1 = gtk_hbox_new( FALSE, 12 );
	w->pad2 = gtk_vbox_new( FALSE, 12 );
	gtk_box_pack_start( GTK_BOX(w->pad1), w->pad2, TRUE, TRUE, 6 );
	gtk_box_pack_start( GTK_BOX(w->pad2), w->vbox_inner, TRUE, TRUE, 6 );
	
	/* Pack objects */
	gtk_box_pack_start( GTK_BOX(w->vbox_outer), w->frame, TRUE, TRUE, 0 );
	gtk_container_add( GTK_CONTAINER(w->frame), w->pad1 );
	gtk_box_pack_start( GTK_BOX(w->vbox_inner), a, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(w->vbox_inner), w->hbox, TRUE, TRUE, 0 );
	
	/* Button (+ alignment) */
	a = gtk_alignment_new( 0.5, 1.0, 0.125, 1.0 );
	gtk_container_add( GTK_CONTAINER(a), w->ok_button );
	gtk_box_pack_start( GTK_BOX(w->vbox_outer), a, TRUE, TRUE, 0 );
	
	/* Attach to window */
	gzrt_window_attach( w->win, w->vbox_outer );
	
	/* On destroyed, free resources */
	g_signal_connect_swapped( G_OBJECT(w->win->window), "destroy", G_CALLBACK(gzrt_wdecompress_closed), (gpointer)w );
	g_signal_connect_swapped( G_OBJECT(w->ok_button), "clicked", G_CALLBACK(gzrt_wdecompress_routine), (gpointer)w );
	
	/* Status window */
	gzrt_wmain_status_addmsg( mw, "Decompressing ROM" );
	
	/* Display */
	gzrt_window_show( w->win );
	
	/* Debug */
	GZRTD_MESG( "Created new ZDEC window, root #%u.", mw->id + 1 );
}

/* Close a ZDEC window */
void gzrt_wdecompress_close ( struct _gzrt_wdecompress * i )
{
	gtk_widget_destroy( i->win->window );
	gzrt_wmain_status_rmmsg( i->mw );
}

/* Close a ZDEC window */
void gzrt_wdecompress_closed ( struct _gzrt_wdecompress * i )
{
	gzrt_free( i->win );
	
	/* Reenable menu */
	gzrt_wmain_enable_item( i->mw, "decompress_rom1" );
	gzrt_wmain_status_rmmsg( i->mw );
	
	/* Debug */
	GZRTD_MESG( "ZDEC window for root #%u destroyed.", i->mw->id + 1 );
}

/* Decompress a ROM */
void gzrt_wdecompress_routine ( struct _gzrt_wdecompress * i )
{
	char *out = (char*)gtk_entry_get_text( GTK_ENTRY(i->entry) );
	char *file_table = gzrt_malloc( z64fs_num_entries( i->mw->z ) * 16 );
	char *data; /* Pointer in ROM */
	char *temp; /* For decompression */
	char *use;  /* Points to one of the above */
	FILE *handle;
	int d, fpos = 0;
	clock_t s = clock();
	int update_counter = 0;
	
	/* Allocate memory */
	if( !(temp = gzrt_malloc(GZRT_WDECOMPRESS_MAX) ) )
		gzrt_werror_show( "Error", "Out of memory.", 1 );
	
	/* Verify resources provided */
	if( !can_write(out) )
	{
		gzrt_werror_show( "Error", "Cannot write to chosen file.", 0 );
		gzrt_free( file_table );
		return;
	}
	else
		/* Create a file handle */
		handle = fopen( out, "wb" );
		
	/* Create progress bar */
	PROGRESS *bar = gzrt_wpbar_new();
	gzrt_wpbar_show( bar );
	
	/* Set filesystem seeker */
	i->mw->z->pos = 0;
		
	/* Loop through each and every file */
	for( d = 0; d < z64fs_num_entries( i->mw->z ); d++ )
	{
		/* Read next file table entries */
		z64fs_read_next( i->mw->z );
		
		/* Set data pointer */
		data = i->mw->c->data + ZF2START( i->mw->z );
		
		/* Is the file decompressed? */
		if( z64fs_is_compressed( i->mw->z ) )
		{
			/* Yes, we need to decompress it */
			yaz0_decode( data + 16, temp, Z1FSIZE(i->mw->z) );
			use = temp;
		}
		else
			use = data;
		
		/* Write file data */
		fwrite( data, Z1FSIZE(i->mw->z), 1, handle );
		
		/* Update file table */
		WU32( file_table + d * 16, fpos 						);
		WU32( file_table + d * 16 + 4, fpos + Z1FSIZE(i->mw->z) );
		WU32( file_table + d * 16 + 8, fpos 					);
		WU32( file_table + d * 16 + 12, 0 						);
		
		/* Update file pos */
		fpos += Z1FSIZE(i->mw->z);
		
		/* Update 'update counter' (progress bar redraw) */
		update_counter += Z1FSIZE(i->mw->z);
		
		/* Update progress bar? */
		if( update_counter >= GZRT_WDECOMPRESS_UPDATE_INVERVAL )
		{
			update_counter -= GZRT_WDECOMPRESS_UPDATE_INVERVAL;
			gzrt_wpbar_set( bar, (double)d / (double)z64fs_num_entries( i->mw->z ) );
			gzrt_wpbar_text( bar, "%.2f/%.2f", (double)fpos / 1024.0 / 1024.0, 64.0 );
			
			/* Update window */
			while ( gtk_events_pending() )
				gtk_main_iteration();
		}
		
		/* Debug */
		GZRTD_MESG( "Concatenated file %u of %u at $%08X.", d + 1, z64fs_num_entries( i->mw->z ), fpos );
	}
	
	/* Write file table */
	fseek( handle, i->mw->z->rstart, SEEK_SET );
	fwrite( file_table, z64fs_num_entries( i->mw->z ) * 16, 1, handle );
	
	/* Debug */
	GZRTD_MESG( "Wrote new file table at 0x%08X (%u bytes).", i->mw->z->rstart, z64fs_num_entries( i->mw->z ) * 16 );
	
	/* Write some filler */
	temp = gzrt_calloc( 64 * 1024 * 1024 - fpos );
	fwrite( temp, 64 * 1024 * 1024 - fpos, 1, handle );
	fclose( handle );
	GZRTD_MESG( "Wrote %u bytes of filler.", 64 * 1024 * 1024 - fpos );
	
	/* Close progress bar */
	gzrt_wpbar_close( bar );
	
	/* Destroy window */
	gzrt_wdecompress_close( i );
	
	/* Done! */
	GZRTD_MESG( "Finished decompressing ROM. Time: %.4f seconds (%.2f files a second).",
	(double)(clock() - s) / (double)CLOCKS_PER_SEC, 
	(double)z64fs_num_entries( i->mw->z ) / ((double)(clock() - s) / (double)CLOCKS_PER_SEC));
	return;
}
