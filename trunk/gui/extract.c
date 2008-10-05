/************************
* ROM Extraction Window *
************************/
#include <gzrt.h>

/* Structure */
static struct _gzrt_wextract gzrt_wextract;

/* Pointer shorthand */
static struct _gzrt_wextract *w = &gzrt_wextract;

/* Last window */
static MAINWIN *last = NULL;

/* Initialize the window */
void gzrt_wextract_init ( void )
{
	/* Create a new window */
	w->window = gtk_window_new(  GTK_WINDOW_TOPLEVEL                                       );
	/* gtk_widget_set_size_request( GTK_WINDOW(w->window), GZRT_WEXTRACT_W,   GZRT_WEXTRACT_H ); */
	gtk_window_set_title(        GTK_WINDOW(w->window), GZRT_WEXTRACT_T                    );
	gtk_window_set_position(     GTK_WINDOW(w->window), GTK_WIN_POS_CENTER                 );
	
	/* Create border vbox */
	w->wvbox = gtk_vbox_new( FALSE, 12 );
	
	/* Create border hbox */
	w->whbox = gtk_hbox_new( FALSE, 12 );
	
	/* Pack 'em */
	gtk_box_pack_start( GTK_BOX(w->wvbox),       w->whbox, TRUE, TRUE, 12 );
	gtk_container_add( GTK_CONTAINER(w->window), w->wvbox );
	
	/* Create vbox for elements */
	w->vbox = gtk_vbox_new( FALSE, 4 );
	
	/* Create a frame for options */
	GtkWidget *h = gtk_hbox_new( FALSE, 12 ), *v = gtk_vbox_new( FALSE, 12 );
	w->frame = gtk_frame_new( "Options" );
	gtk_container_add( GTK_CONTAINER(w->frame), h );
	
	/* Check boxes */
	w->b_strip_prefixes = gtk_check_button_new_with_label( GZRT_WEXTRACT_C_1 );
	w->b_scene_table = gtk_check_button_new_with_label( GZRT_WEXTRACT_C_2 );
	w->b_actor_table = gtk_check_button_new_with_label( GZRT_WEXTRACT_C_3 );
	
	/* Pack */
	GtkWidget *vv = gtk_vbox_new( FALSE, 4 );
	gtk_box_pack_start( GTK_BOX(w->whbox), w->vbox,  TRUE, TRUE,  10 );
	gtk_box_pack_start( GTK_BOX(w->vbox),  w->frame, TRUE, TRUE, 0  );
	gtk_box_pack_start( GTK_BOX(h),        v,        TRUE, TRUE,  12  );
	gtk_box_pack_start( GTK_BOX(v),        vv,        TRUE, TRUE,  12  );
	gtk_box_pack_start( GTK_BOX(vv), w->b_strip_prefixes, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(vv), w->b_scene_table, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(vv), w->b_actor_table, TRUE, TRUE, 0 );
	
	#ifdef GZRT_DEBUG
	w->dontwrite = gtk_check_button_new_with_label( GZRT_WEXTRACT_CD1 );
	gtk_box_pack_start( GTK_BOX(vv), w->dontwrite, TRUE, TRUE, 0 );
	#endif
	
	GtkWidget *l = gtk_label_new( "Output directory:" );
	GtkWidget *a = gtk_alignment_new( 0.0, 1.0, 0.0, 1.0 );
	gtk_container_add( GTK_CONTAINER(a), l );
	gtk_box_pack_start( GTK_BOX(vv), a, TRUE, TRUE, 0 );
	w->entry = gtk_entry_new();
	w->browse = gtk_button_new_with_label( "Browse..." );
	GtkWidget *hh = gtk_hbox_new( FALSE, 4 );
	gtk_box_pack_start( GTK_BOX(hh), w->entry, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(hh), w->browse, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(vv), hh, TRUE, TRUE, 0 );
	
	#ifdef GZRT_DEBUG
	a = gtk_alignment_new( 0.0, 1.0, 0.0, 1.0 );
	l = gtk_label_new( "Filename format:" );
	gtk_container_add( GTK_CONTAINER(a), l );
	w->fmt = gtk_entry_new();
	gtk_entry_set_text( GTK_ENTRY(w->fmt), GZRT_WEXTRACT_FMT_NT);
	gtk_box_pack_start( GTK_BOX(vv), a, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(vv), w->fmt, TRUE, TRUE, 0 );
	#endif
	
	w->button = gtk_button_new_with_label( GZRT_WEXTRACT_OK );
	a = gtk_alignment_new( 0.5, 1.0, 0.25, 1.0 );
	gtk_container_add( GTK_CONTAINER(a), w->button ); 
	gtk_box_pack_start( GTK_BOX(w->vbox), a, TRUE, TRUE, 0  );
	
	/* Show everything 
	gtk_widget_show_all( w->window ); */
	
	/* Extract */
	g_signal_connect_swapped( G_OBJECT(w->button), "clicked", gzrt_wextract_begin, NULL );
	
	/* On destroy, regenerate */
	g_signal_connect_swapped( G_OBJECT(w->window), "destroy", gzrt_wextract_regen, NULL );
	
	/* Debug */
	GZRTD_MESG( "Extractor window created." );
}

/* Show extractor window */
void gzrt_wextract_show ( MAINWIN *h )
{
	last = h;
	
	/* Set main window status */
	gzrt_wmain_status_addmsg( h, "Extracting ROM contents" );
	GZRTD_MESG( "Extractor window shown; fed WINMAIN * 0x%08X (#%u).", h, h->id );
	gtk_widget_show_all( w->window );
}

/* Regenerate window */
void gzrt_wextract_regen ( void )
{
	gzrt_wmain_status_rmmsg( last );
	GZRTD_MESG( "Extractor window destroyed - recreating." );
	gzrt_wextract_init();
}

/* Extraction procedure */
void gzrt_wextract_begin ( void )
{
	char *output = (char*)gtk_entry_get_text( GTK_ENTRY(w->entry) );
	char *temp = malloc( GZRT_WEXTRACT_MAX );
	char *use;
	char *data;
	char  dest_path[512]; 
	FILE *h;
	clock_t s = clock();
	int type;
	struct _gzrt_wextract_stats stats = 
		{ 0, last->z, last->t };
	
	/* Formatting options */
	#ifdef GZRE_DEBUG
	 char *name_fmt = (char*)gtk_entry_get_text( GTK_ENTRY(w->fmt) );
	#else
	 char *name_fmt = ( last->t ? GZRT_WEXTRACT_FMT : GZRT_WEXTRACT_FMT_NT );
	#endif
		
	/* No dir specified? */
	if( !strlen(output) )
	{
		gzrt_werror_show( "Error", "No output directory specified.", 0 );
		free( data );
		free( temp );
		return;
	}
	
	/* Make output directory */
	if( !is_dir( output ) )
	{
		if( MKDIR( output ) == -1 )
		{
			gzrt_werror_show( "Error", "Unable to create output folder", 0 );
			free( data );
			free( temp );
			return;
		}
		else
			GZRTD_MESG( "Created dir \"%s\".", output );
	}
	
	/* Set up progress bar */
	last->pbar = gzrt_wpbar_new();
	gzrt_wpbar_show( last->pbar );
	
	/* Set filesystem seeker */
	last->z->pos = 0;
	
	/* Extraction loop begin */
	GZRTD_MESG( "Extraction begun. Destination: \"%s\". Format: \"%s\".", output, name_fmt );
	for( int i = 0; i < z64fs_num_entries( last->z ); i++ )
	{
		/* Read next entry */
		z64fs_read_next( last->z );
		
		/* Set correct pointer */
		data = last->c->data + ZF2START( last->z );
		
		/* Is it compressed? */
		if( z64fs_is_compressed( last->z ) )
		{
			/* Yes, we need to decompress it */
			yaz0_decode( data + 16, temp, Z1FSIZE(last->z) );
			use = temp;
		}
		else
			use = data;
		
		/* Detect filetype */
		type = 
		( 
			stats.nt ? 
			 
			 /* Using name table */
			 zd_detect_name_main( stats.nt->cur ) :
			 
			 /* Using data analysis */
			 zd_detect_main( use, Z1FSIZE( last->z ))
		);
		
		/* Destination pathname */
		char buffer[64];
		gzrt_wextract_soffsetf( buffer, &stats, name_fmt );
		snprintf( dest_path, sizeof(dest_path), "%s" GZRT_SLASH "%s.%s", 
			output, buffer, string_filetype( type ) );
		
		/*
		**
		*/
		/* (DEBUG) write? */
		#ifdef GZRT_DEBUG
		 if( !gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(w->dontwrite) ) )
		 {
		#endif
		
			/* Do it faggot */
			if( !(h = fopen( dest_path, "wb" )) )
			{
				GZRTD_MESG( "Could not open \"%s\".", dest_path  );
				gzrt_werror_show( "Error", "Unable to write output file", 1 );
			}
			fwrite( use, Z1FSIZE(last->z), 1, h );
			fclose( h );
	
		/* (DEBUG) write? */
		#ifdef GZRT_DEBUG
		 }
		#endif
		/*
		**
		*/
		
		/* Statistics update */
		stats.files_extracted++;
		
		/* Notice! */
		#ifdef GZRT_DEBUG
		 if( z64fs_is_compressed( last->z ) )
			 GZRTD_MESG( "Extracted compressed from %08X - %08X to \"%s\".",
			 ZF2START(last->z), ZF2END(last->z), dest_path );
		 else
			 GZRTD_MESG( "Extracted decompressed from %08X - %08X to \"%s\".",
			 ZF2START(last->z), ZF2END(last->z), dest_path );
		#endif
		
		/* Update progress bar display */
		if( !((i + 1) % GZRT_WEXTRACT_UPD) )
		{
			gzrt_wpbar_set( last->pbar, (double)i / (double)z64fs_num_entries( last->z ));
			gzrt_wpbar_text( last->pbar, "%u / %u", i, z64fs_num_entries( last->z ) );
			while (gtk_events_pending ())
				gtk_main_iteration ();
		}
		
		/* Read next name table entry */
		if( last->t )
			z64nt_read_next( last->t );
	}
	
	/* Notice */
	GZRTD_MESG( "Extraction finished. Time taken: %.4f seconds.",
		(double)(clock() - s) / (double)CLOCKS_PER_SEC );
	
	/* Close progress bar */
	gzrt_wpbar_close( last->pbar );
	
	/* Free resources */
	free( temp );
	
	/* Close extractor window */
	gtk_widget_destroy( w->window );
}

/* Name format string parser */
int gzrt_wextract_soffsetf ( char *dest, struct _gzrt_wextract_stats * stats, char *fmt )
{
	/*
	** Formatting modifiers
	**
	** %l	ID relative to files extracted
	** %L	ID relative to file table
	** %n	Filename
	** %o1	First virtual address
	** %o2	Second virtual address
	** %o3	Third virtual address
	** %o4	Fourth virtual address
	*/
	
	int i, d = 0;
	
	for( i = 0; fmt[i] <= '~' && fmt[i] >= ' '; i++ )
	{
		if( fmt[i] == '%' )
		{
			switch( fmt[i + 1] )
			{
				/* ID relative to files extracted */
				case 'l':
				 d += sprintf( &dest[d], "%04u", stats->files_extracted );
				break;
				
				/* ID relative to filesystem */
				case 'L':
				 d += sprintf( &dest[d], "%04u", stats->fs->pos / 16 );
				break;
				
				/* Addresses */
				case 'o':
				 d += sprintf( &dest[d], "%08X", 
					 U32(stats->fs->fs_table + (stats->fs->pos - 16) + (((fmt[i + 2] - '0') - 1) * 4) ));
				 i++;
				break;
				
				/* Filename */
				case 'n':
				 d += sprintf( &dest[d], "%s", ( stats->nt ? (char*)stats->nt->cur : "" ) );
				break;
				
				/* Unknown */
				default:
				 GZRTD_MESG( "Unknown format specifier \"%c\".", fmt[i + 1] );
			}
			i++;
		}
		else
			dest[d++] = fmt[i];
	}
	
	dest[d] = 0;
	
	/* Resturn length */
	return d;
}
