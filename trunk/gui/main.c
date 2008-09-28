/*******************
* GZRT Main Window *
*******************/
#include <gzrt.h>
#include <stdarg.h>

/*
** No global struct here, since there can be several main windows
** containing ROM information and the like.
** SEE: main.h for limit
*/

MAINWIN *wmain_instances[GZRT_WMAIN_MAX];
int	window_amount = 0, close_called = 0, ram_use = 0;

PROGRESS *d;

/* Create a new window */
int gzrt_wmain_create_new ( N64ROM * rc )
{
	/* Not allocated? */
	if( !wmain_instances[0] )
		for( int i = 0; i < GZRT_WMAIN_MAX; i++ )
			wmain_instances[i] = calloc( sizeof(MAINWIN), 1 );
	
	/* Is this ROM already loaded? */
	for( int i = 0; i < window_amount; i++ )
		if( !FILENAME_CMP( rc->filename, wmain_instances[i]->c->filename ) )
		{
			gzrt_werror_show( "Notice", "This ROM is already loaded.", 0 );
			GZRTD_MESG( "ROM \"%s\" is already loaded.\n", rc->filename );
			n64rom_close( rc );
			return FALSE;
		}
	
	/* Too many windows? */
	if( window_amount >= GZRT_WMAIN_MAX )
	{
		GZRTD_MESG( "Cannot create more windows; limit reached (%u).", GZRT_WMAIN_MAX );
		return FALSE;
	}
	
	/* Keep menus in check */
	if( window_amount + 1 == GZRT_WMAIN_MAX )
	{
		int i;
		
		/* Disable all 'Open's */
		for( i = 0; i < window_amount; i++ )
			gzrt_wmain_disable_item( wmain_instances[i], "open1" );
		
		/* Debug */
		GZRTD_MESG( "Main window instances full; 'Open' disabled." );
		GZRTD_MESG( "%u", i );
	}
	else
	{
		/* Make sure all 'Open's are enabled */
		for( int i = 0; i < window_amount; i++ )
			gzrt_wmain_enable_item( wmain_instances[i], "open1" );
	}
	
	/* Set ID */
	wmain_instances[window_amount]->id = window_amount;
	
	/* Store ROM context */
	wmain_instances[window_amount]->c = rc;
	
	/* Debug */
	GZRTD_MESG( "Creating window %u/%u (%08X).", window_amount + 1, GZRT_WMAIN_MAX, wmain_instances[window_amount + 1] );
	GZRTD_MESG( "ROM storage: %.2f MB.", (float)(ram_use += rc->filesize) / 1024.0 / 1024.0 );
	GZRTD_MESG( "Loading ROM \"%s\".", rc->filename );
	
	/* Load ROM */
	d = gzrt_wpbar_new();
	gzrt_wpbar_set( d , 0);
	gzrt_wpbar_show( d );
	if( !(n64rom_read( rc, pbu )) )
		gzrt_werror_show( "Error occured", n64rom_error(), 1 );
	GZRTD_MESG( "Loaded ROM successfully." );
	gzrt_wpbar_close( d );
	
	/* Byteswap the ROM if required */
	if( rc->endian != N64_BIG )
	{
		/* Swap */
		d = gzrt_wpbar_new();
		if( rc->endian == N64_LITTLE )
			n64rom_swap( rc, 32, 0, 32, 1 );
		else if( rc->endian == N64_V64 )
			n64rom_swap( rc, 16, 0, 32, 1 );
		gzrt_wpbar_show( d );
		
		/* Write */
		fseek( rc->handle, 0, SEEK_SET );
		for( int i = 0; i < rc->filesize; i += rc->filesize / 32 )
		{
			fwrite( rc->data + i, rc->filesize / 32, 1, rc->handle );
			gzrt_wpbar_set( d , (double)i / (double)rc->filesize );
		}
		gzrt_wpbar_close( d );
		
		/* Notice */
		gzrt_werror_show( "Notice", "ROM was automatically byteswapped.", 0 );
	}
	
	/* Identify Zelda filesystem elements */
	if( !(wmain_instances[window_amount]->z = z64fs_init( rc->filename )) )
	{
		GZRTD_MESG( "Could not find filesystem!" );
		gzrt_werror_show( "Error", "Unable to find filesystem in ROM.", 0 );
		n64rom_close( rc );
		return FALSE;
	}
	
	/* Identify Zelda 64 name table elements */
	if( !(wmain_instances[window_amount]->t = z64nt_init( rc->filename )) )
		GZRTD_MESG( "No name table in this ROM." );
	
	/* Information */
	GZRTD_MESG( "Name table p:  $%08X", wmain_instances[window_amount]->t );
	GZRTD_MESG( "File table p:  $%08X", wmain_instances[window_amount]->z );
	GZRTD_MESG( "ROM context p: $%08X", rc								  );
	
	/* Fill struct with GTK elements & update count */
	gzrt_wmain_fill( wmain_instances[window_amount] );
	
	/* Set message */
	gzrt_wmain_status_addmsg( wmain_instances[window_amount], "Ready" );
	
	/* Update counter */
	window_amount++;
	
	/* We did it! */
	return TRUE;
}

/* Close a preexisting window */
void gzrt_wmain_close ( MAINWIN *w )
{
	void *tmp; int d, i = w->id;
	GZRTD_MESG( "Closing window #%u...", i + 1 );
	
	/* Is this the last window? If so, we can just quit */
	if( !(window_amount - 1) )
		gzrt_gui_quit();
		
	/* Update memory usage */
	ram_use -= w->c->filesize;
		
	/* Free associated ROM context */
	GZRTD_MESG( "Freeing ROM context...", i + 1 );
	n64rom_close( w->c );
	
	/* Destroy window widget */
	if( w->window )
	{
		GZRTD_MESG( "Disconnecting destroy signal handler from window...", i + 1 );
		g_signal_handler_disconnect( wmain_instances[i]->window,  (int)wmain_instances[i]->hid );
		gtk_widget_destroy( wmain_instances[i]->window );
	}
	
	/* Store original memory location */
	tmp = wmain_instances[i];
	
	/* Shift down the rest of the windows */
	for( d = 0; d < window_amount - i; d++ )
	{
		wmain_instances[i + d] = wmain_instances[i + d + 1];
		if( wmain_instances[i + d]->id )
			wmain_instances[i + d]->id--;
	}
	
	/* Restore original */
	wmain_instances[i + d + 1] = tmp;
	
	/* Update counter */
	window_amount--;
	
	/* Debug */
	GZRTD_MESG( "Deleted window #%u, shifted rest down %u positions.", i, window_amount - i );
	GZRTD_MESG( "ROM storage: %.2f", (float)ram_use / 1024.0 / 1024.0 );
	GZRTD_MESG( "Windows status: %u/%u", window_amount, GZRT_WMAIN_MAX );
}

/* Closed */
void gzrt_wmain_closed ( MAINWIN *w )
{
	w->window = NULL;
	gzrt_wmain_close( w );
}

/* Focus changed */
void gzrt_wmain_focus ( MAINWIN *w )
{
	GZRTD_MESG( "Window #%u.", w->id + 1 );
}

/* Update progress bar */
void pbu ( int a, int b )
{
	gzrt_wpbar_text(d, "%.2f%%", (double)a / (double)b * 100.0 );
	gzrt_wpbar_set( d, (double)a / (double)b );
	gzrt_wpbar_show( d );
	while( gtk_events_pending() )
		gtk_main_iteration();
}



/* Create new instance */
void gzrt_wmain_fill ( MAINWIN *c )
{
	/* GTK Elements */
	GtkWidget *Main_Window;
	GtkWidget *Main_vbox;
	GtkWidget *Menu;
	GtkWidget *File_menu;
	GtkWidget *File_menu_menu;
	GtkWidget *open1;
	GtkWidget *reload1;
	GtkWidget *close1;
	GtkWidget *image5;
	GtkWidget *quit1;
	GtkWidget *Operations_menu;
	GtkWidget *Operations_menu_menu;
	GtkWidget *extract_files1;
	GtkWidget *decompress_rom1;
	GtkWidget *file_search1;
	GtkWidget *separator1;
	GtkWidget *byteswap_rom1;
	GtkWidget *fix_crc1;
	GtkWidget *Help_menu;
	GtkWidget *Help_menu_menu;
	GtkWidget *about1;
	GtkWidget *Main_window_padding;
	GtkWidget *Main_window_seperator;
	GtkWidget *hpaned1;
	GtkWidget *Side_frame_seperator;
	GtkWidget *Frame_1_alignment;
	GtkWidget *ROM_Info;
	GtkWidget *Frame_1_label_alignment;
	GtkWidget *Frame_1_label_seperator;
	GtkWidget *f1l1;
	GtkWidget *f1l2;
	GtkWidget *f1l3;
	GtkWidget *f1l4;
	GtkWidget *f1l5;
	GtkWidget *f1l6;
	GtkWidget *Frame_1_label;
	GtkWidget *Frame_2_alignment;
	GtkWidget *Filesystem_info;
	GtkWidget *Frame_2_label_alignment;
	GtkWidget *Frame_2_label_seperator;
	GtkWidget *f2l1;
	GtkWidget *f2l2;
	GtkWidget *f2l3;
	GtkWidget *f2l4;
	GtkWidget *f2l5;
	GtkWidget *f2l6;
	GtkWidget *Frame_2_label;
	GtkWidget *alignment12;
	GtkWidget *Right_pane;
	GtkWidget *Column_view_scroller;
	GtkWidget *treeview1;
	GtkWidget *Action_buttons_organizer;
	GtkWidget *Action_buttons_seperator;
	GtkWidget *Extract_button;
	GtkWidget *alignment10;
	GtkWidget *hbox6;
	GtkWidget *image4;
	GtkWidget *label16;
	GtkWidget *View_button;
	GtkWidget *alignment9;
	GtkWidget *hbox5;
	GtkWidget *image3;
	GtkWidget *label15;
	GtkWidget *Replace_button;
	GtkWidget *alignment8;
	GtkWidget *hbox4;
	GtkWidget *image2;
	GtkWidget *label14;
	GtkWidget *Disassemble_button;
	GtkWidget *alignment7;
	GtkWidget *hbox3;
	GtkWidget *image1;
	GtkWidget *label13;
	GtkWidget *App_status;
	GtkAccelGroup *accel_group;
	
	/* GTK Elements - scroll window */
	GtkWidget       *liststore;
	GtkTreeIter      iter;
    GtkCellRenderer *renderer;

	
	/* GTK Elements - debug */
	#ifdef GZRT_DEBUG
	 GtkWidget *wai1;
	#endif
	
	/* Debug */
	GZRTD_MESG( "Filling struct wmain_instances[%u] (0x%08X).",
		c->id, &wmain_instances[ c->id ] );
	
	/* Variables */
	char buffer[256];
	
	/* ~~ */
	
	
	/* Load the ROM 
	d = gzrt_wpbar_new();
	gzrt_wpbar_set( d , 0);
	gzrt_wpbar_show( d );
	if( !(n64rom_read( c->c, pbu )) )
		gzrt_werror_show( "Error occured", n64rom_error(), 1 );
	GZRTD_MESG( "Loaded ROM successfully. %08X", U32(c->c->data) );
	gzrt_wpbar_close( d );*/
	
	/* ~~ */
	
	/* Set up window */
	accel_group = gtk_accel_group_new ();
	snprintf( buffer, sizeof(buffer), "%s%s: %s - \"%.24s\"",
		GZRT_WMAIN_NAME,
	#ifdef GZRT_DEBUG
	 "*DEBUG*",
	#else
	 "",
	#endif
		c->c->filename, c->c->header + 0x20 );
	Main_Window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (Main_Window), buffer );
	gtk_window_set_default_size (GTK_WINDOW (Main_Window), 640, 480);

	Main_vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (Main_vbox);
	gtk_container_add (GTK_CONTAINER (Main_Window), Main_vbox);

	Menu = gtk_menu_bar_new ();
	gtk_widget_show (Menu);
	gtk_box_pack_start (GTK_BOX (Main_vbox), Menu, FALSE, FALSE, 0);

	File_menu = gtk_menu_item_new_with_mnemonic (_("_File"));
	gtk_widget_show (File_menu);
	gtk_container_add (GTK_CONTAINER (Menu), File_menu);

	File_menu_menu = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (File_menu), File_menu_menu);

	open1 = gtk_image_menu_item_new_from_stock ("gtk-open", accel_group);
	gtk_widget_show (open1);
	gtk_container_add (GTK_CONTAINER (File_menu_menu), open1);
	
	reload1 = gtk_menu_item_new_with_mnemonic (_("_Reload"));
	gtk_widget_show (reload1);
	gtk_container_add (GTK_CONTAINER (File_menu_menu), reload1);

	close1 = gtk_image_menu_item_new_with_mnemonic (_("_Close"));
	gtk_widget_show (close1);
	gtk_container_add (GTK_CONTAINER (File_menu_menu), close1);

	image5 = gtk_image_new_from_stock ("gtk-close", GTK_ICON_SIZE_MENU);
	gtk_widget_show (image5);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (close1), image5);

	quit1 = gtk_image_menu_item_new_from_stock ("gtk-quit", accel_group);
	gtk_widget_show (quit1);
	gtk_container_add (GTK_CONTAINER (File_menu_menu), quit1);

	Operations_menu = gtk_menu_item_new_with_mnemonic (_("_Operations"));
	gtk_widget_show (Operations_menu);
	gtk_container_add (GTK_CONTAINER (Menu), Operations_menu);

	Operations_menu_menu = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (Operations_menu), Operations_menu_menu);

	extract_files1 = gtk_menu_item_new_with_mnemonic (_("_Extract Files"));
	gtk_widget_show (extract_files1);
	gtk_container_add (GTK_CONTAINER (Operations_menu_menu), extract_files1);

	decompress_rom1 = gtk_menu_item_new_with_mnemonic (_("_Decompress ROM"));
	gtk_widget_show (decompress_rom1);
	gtk_container_add (GTK_CONTAINER (Operations_menu_menu), decompress_rom1);

	file_search1 = gtk_menu_item_new_with_mnemonic (_("_File search"));
	gtk_widget_show (file_search1);
	gtk_container_add (GTK_CONTAINER (Operations_menu_menu), file_search1);

	separator1 = gtk_separator_menu_item_new ();
	gtk_widget_show (separator1);
	gtk_container_add (GTK_CONTAINER (Operations_menu_menu), separator1);
	gtk_widget_set_sensitive (separator1, FALSE);

	byteswap_rom1 = gtk_menu_item_new_with_mnemonic (_("_Byteswap ROM"));
	gtk_widget_show (byteswap_rom1);
	gtk_container_add (GTK_CONTAINER (Operations_menu_menu), byteswap_rom1);

	fix_crc1 = gtk_menu_item_new_with_mnemonic (_("Fix _CRC"));
	gtk_widget_show (fix_crc1);
	gtk_container_add (GTK_CONTAINER (Operations_menu_menu), fix_crc1);

	Help_menu = gtk_menu_item_new_with_mnemonic (_("_Help"));
	gtk_widget_show (Help_menu);
	gtk_container_add (GTK_CONTAINER (Menu), Help_menu);

	Help_menu_menu = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (Help_menu), Help_menu_menu);

	about1 = gtk_menu_item_new_with_mnemonic (_("_About"));
	gtk_widget_show (about1);
	gtk_container_add (GTK_CONTAINER (Help_menu_menu), about1);
	
	#ifdef GZRT_DEBUG
	 wai1 = gtk_menu_item_new_with_mnemonic (_("_Who am I?"));
	 gtk_widget_show (wai1);
	 gtk_container_add (GTK_CONTAINER (Help_menu_menu), wai1);
	#endif

	Main_window_padding = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (Main_window_padding);
	gtk_box_pack_start (GTK_BOX (Main_vbox), Main_window_padding, TRUE, TRUE, 0);
	gtk_alignment_set_padding (GTK_ALIGNMENT (Main_window_padding), 12, 12, 12, 12);

	Main_window_seperator = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (Main_window_seperator);
	gtk_container_add (GTK_CONTAINER (Main_window_padding), Main_window_seperator);

	hpaned1 = gtk_hpaned_new ();
	gtk_widget_show (hpaned1);
	gtk_box_pack_start (GTK_BOX (Main_window_seperator), hpaned1, TRUE, TRUE, 0);

	Side_frame_seperator = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (Side_frame_seperator);
	gtk_paned_pack1 (GTK_PANED (hpaned1), Side_frame_seperator, FALSE, TRUE);

	Frame_1_alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (Frame_1_alignment);
	gtk_box_pack_start (GTK_BOX (Side_frame_seperator), Frame_1_alignment, TRUE, TRUE, 0);
	gtk_alignment_set_padding (GTK_ALIGNMENT (Frame_1_alignment), 4, 4, 0, 4);

	ROM_Info = gtk_frame_new (NULL);
	gtk_widget_show (ROM_Info);
	gtk_container_add (GTK_CONTAINER (Frame_1_alignment), ROM_Info);

	Frame_1_label_alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (Frame_1_label_alignment);
	gtk_container_add (GTK_CONTAINER (ROM_Info), Frame_1_label_alignment);
	gtk_alignment_set_padding (GTK_ALIGNMENT (Frame_1_label_alignment), 8, 8, 12, 12);

	Frame_1_label_seperator = gtk_vbox_new (FALSE, 2);
	gtk_widget_show (Frame_1_label_seperator);
	gtk_container_add (GTK_CONTAINER (Frame_1_label_alignment), Frame_1_label_seperator);

	/* Set ROM information */
	snprintf( buffer, sizeof(buffer), "Name: %.24s", c->c->header + 0x20 );
	f1l1 = gtk_label_new ( buffer );
	gtk_widget_show (f1l1);
	gtk_box_pack_start (GTK_BOX (Frame_1_label_seperator), f1l1, TRUE, TRUE, 0);
	gtk_misc_set_alignment (GTK_MISC (f1l1), 0, 0.5);
    /* | */
	snprintf( buffer, sizeof(buffer), "Code: %.4s", c->c->header + 0x3B );
	f1l2 = gtk_label_new ( buffer );
	gtk_widget_show (f1l2);
	gtk_box_pack_start (GTK_BOX (Frame_1_label_seperator), f1l2, TRUE, TRUE, 0);
	gtk_misc_set_alignment (GTK_MISC (f1l2), 0, 0.5);
	/* | */
	snprintf( buffer, sizeof(buffer), "Size: %.2fMB (%.2fMBits)", (float)c->c->filesize / 1024.0 / 1024.0, (float)c->c->filesize / 1024.0 / 1024.0 * 8.0 );
	f1l3 = gtk_label_new ( buffer );
	gtk_widget_show (f1l3);
	gtk_box_pack_start (GTK_BOX (Frame_1_label_seperator), f1l3, TRUE, TRUE, 0);
	gtk_misc_set_alignment (GTK_MISC (f1l3), 0, 0.5);
	/* | */
	snprintf( buffer, sizeof(buffer), "CRC 1: <span font_desc=\"Courier\">0x%08X</span>", U32(c->c->header + 0x10) );
	f1l4 = gtk_label_new ( "" );
	gtk_label_set_markup( GTK_LABEL(f1l4), buffer );
	gtk_widget_show (f1l4);
	gtk_box_pack_start (GTK_BOX (Frame_1_label_seperator), f1l4, TRUE, TRUE, 0);
	gtk_misc_set_alignment (GTK_MISC (f1l4), 0, 0.5);
	/* | */
	snprintf( buffer, sizeof(buffer), "CRC 2: <span font_desc=\"Courier\">0x%08X</span>", U32(c->c->header + 0x14) );
	f1l5 = gtk_label_new ( "" );
	gtk_label_set_markup( GTK_LABEL(f1l5), buffer );
	gtk_widget_show (f1l5);
	gtk_box_pack_start (GTK_BOX (Frame_1_label_seperator), f1l5, TRUE, TRUE, 0);
	gtk_misc_set_alignment (GTK_MISC (f1l5), 0, 0.5);
	/* | */
	snprintf( buffer, sizeof(buffer), "Entry point: <span font_desc=\"Courier\">0x%08X</span>", U32(c->c->header + 0x08) );
	f1l6 = gtk_label_new ( "" );
	gtk_label_set_markup( GTK_LABEL(f1l6), buffer );
	gtk_widget_show (f1l6);
	gtk_box_pack_start (GTK_BOX (Frame_1_label_seperator), f1l6, TRUE, TRUE, 0);
	gtk_misc_set_alignment (GTK_MISC (f1l6), 0, 0.5);
	/* \_________________ */
	
	/* Set filesystem information */
	Frame_1_label = gtk_label_new (_("<b>ROM Info</b>"));
	gtk_widget_show (Frame_1_label);
	gtk_frame_set_label_widget (GTK_FRAME (ROM_Info), Frame_1_label);
	gtk_label_set_use_markup (GTK_LABEL (Frame_1_label), TRUE);
	/* | */
	Frame_2_alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (Frame_2_alignment);
	gtk_box_pack_start (GTK_BOX (Side_frame_seperator), Frame_2_alignment, TRUE, TRUE, 0);
	gtk_alignment_set_padding (GTK_ALIGNMENT (Frame_2_alignment), 4, 4, 0, 4);
	/* | */
	Filesystem_info = gtk_frame_new (NULL);
	gtk_widget_show (Filesystem_info);
	gtk_container_add (GTK_CONTAINER (Frame_2_alignment), Filesystem_info);
	/* | */
	Frame_2_label_alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (Frame_2_label_alignment);
	gtk_container_add (GTK_CONTAINER (Filesystem_info), Frame_2_label_alignment);
	gtk_alignment_set_padding (GTK_ALIGNMENT (Frame_2_label_alignment), 8, 8, 12, 13);
	/* | */
	Frame_2_label_seperator = gtk_vbox_new (FALSE, 2);
	gtk_widget_show (Frame_2_label_seperator);
	gtk_container_add (GTK_CONTAINER (Frame_2_label_alignment), Frame_2_label_seperator);
	
	
	#define MONO(x) "<span font_desc=\"Courier\">", x, "</span>"
	
	snprintf( buffer, sizeof(buffer), "Filesystem start: %s0x%08X%s", MONO(c->z->rstart) );
	f2l1 = gtk_label_new ( "" );
	gtk_label_set_markup( GTK_LABEL(f2l1), buffer );
	gtk_widget_show (f2l1);
	gtk_box_pack_start (GTK_BOX (Frame_2_label_seperator), f2l1, TRUE, TRUE, 0);
	gtk_misc_set_alignment (GTK_MISC (f2l1), 0, 0.5);

	snprintf( buffer, sizeof(buffer), "Filesystem end: %s0x%08X%s", MONO(c->z->rend) );
	f2l2 = gtk_label_new ( "" );
	gtk_label_set_markup( GTK_LABEL(f2l2), buffer );
	gtk_widget_show (f2l2);
	gtk_box_pack_start (GTK_BOX (Frame_2_label_seperator), f2l2, TRUE, TRUE, 0);
	gtk_misc_set_alignment (GTK_MISC (f2l2), 0, 0.5);

	snprintf( buffer, sizeof(buffer), "File count: %u", z64fs_num_entries( c->z ) );
	f2l3 = gtk_label_new ( buffer );
	gtk_widget_show (f2l3);
	gtk_box_pack_start (GTK_BOX (Frame_2_label_seperator), f2l3, TRUE, TRUE, 0);
	gtk_misc_set_alignment (GTK_MISC (f2l3), 0, 0.5);

	snprintf( buffer, sizeof(buffer), "Total size: %.2fMB", (float)z64fs_calc_size_decompressed( c->z ) / 1024.0 / 1024.0 );
	f2l4 = gtk_label_new ( buffer );
	gtk_widget_show (f2l4);
	gtk_box_pack_start (GTK_BOX (Frame_2_label_seperator), f2l4, TRUE, TRUE, 0);
	gtk_misc_set_alignment (GTK_MISC (f2l4), 0, 0.5);

	/* Name table */
	if( c->t )
	{
		snprintf( buffer, sizeof(buffer), "Name table start: %s0x%08X%s", MONO(__NAME_TABLE_START) );
		f2l5 = gtk_label_new ( "" );
		gtk_label_set_markup( GTK_LABEL(f2l5), buffer );
		gtk_widget_show (f2l5);
		gtk_box_pack_start (GTK_BOX (Frame_2_label_seperator), f2l5, TRUE, TRUE, 0);
		gtk_misc_set_alignment (GTK_MISC (f2l5), 0, 0.50);
		
		snprintf( buffer, sizeof(buffer), "Name table end: %s0x%08X%s", MONO(__NAME_TABLE_END) );
		f2l6 = gtk_label_new ( "" );
		gtk_label_set_markup( GTK_LABEL(f2l6), buffer );
		gtk_widget_show (f2l6);
		gtk_box_pack_start (GTK_BOX (Frame_2_label_seperator), f2l6, TRUE, TRUE, 0);
		gtk_misc_set_alignment (GTK_MISC (f2l6), 0, 0.50);
	}
	else
	{
		f2l5 = gtk_label_new ( "Name table: no" );
		gtk_widget_show (f2l5);
		gtk_box_pack_start (GTK_BOX (Frame_2_label_seperator), f2l5, TRUE, TRUE, 0);
		gtk_misc_set_alignment (GTK_MISC (f2l5), 0, 0.50);
	}
	
	Frame_2_label = gtk_label_new (_("<b>Filesystem Info</b>"));
	gtk_widget_show (Frame_2_label);
	gtk_frame_set_label_widget (GTK_FRAME (Filesystem_info), Frame_2_label);
	gtk_label_set_use_markup (GTK_LABEL (Frame_2_label), TRUE);

	alignment12 = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignment12);
	gtk_paned_pack2 (GTK_PANED (hpaned1), alignment12, TRUE, TRUE);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignment12), 0, 0, 4, 0);

	Right_pane = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (Right_pane);
	gtk_container_add (GTK_CONTAINER (alignment12), Right_pane);

	Column_view_scroller = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (Column_view_scroller);
	gtk_box_pack_start (GTK_BOX (Right_pane), Column_view_scroller, TRUE, TRUE, 0);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (Column_view_scroller), GTK_SHADOW_IN);

	treeview1 = gtk_tree_view_new ();
	gtk_widget_show (treeview1);
	gtk_container_add (GTK_CONTAINER (Column_view_scroller), treeview1);
	
	/* ~~ 
	
	if( !(c->z) )*/
		goto after_tree;
	
	/* Set up columns */
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes( GTK_TREE_VIEW(treeview1), -1, "ID", renderer, "text", 0, NULL );
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes( GTK_TREE_VIEW(treeview1), -1, "VStart", renderer, "text", 1, NULL );
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes( GTK_TREE_VIEW(treeview1), -1, "VEnd", renderer, "text", 2, NULL );
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes( GTK_TREE_VIEW(treeview1), -1, "Start", renderer, "text", 3, NULL );
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes( GTK_TREE_VIEW(treeview1), -1, "End", renderer, "text", 4, NULL );
	
	/* Set up list view information */
	liststore = (GtkWidget*)gtk_list_store_new( 5, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING );
	
	for( int i = 0; i < z64fs_num_entries( c->z ); i++ )
	{
		char buffs[5][32];
		
		for( int d = 0; d < 4; d++ )
			snprintf( buffs[d], sizeof(buffs[d]), "0x%08X", U32( &c->z->fs_table[i * 16 + d * 4] ) );
		snprintf( buffs[4], sizeof(buffs[4]), "%04u", i );
		
		z64fs_read_next( c->z );
		gtk_list_store_append( GTK_LIST_STORE(liststore), &iter);
		gtk_list_store_set( GTK_LIST_STORE(liststore), &iter, 0, buffs[4], 1, buffs[0],
			2, buffs[1], 3, buffs[2], 4, buffs[3], -1 );
	}
	
	/* Finalize */
	gtk_tree_view_set_model( GTK_TREE_VIEW(treeview1), GTK_TREE_MODEL(liststore) );
	
	/* ~~ */
	after_tree:
	Action_buttons_organizer = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (Action_buttons_organizer);
	gtk_box_pack_start (GTK_BOX (Right_pane), Action_buttons_organizer, FALSE, TRUE, 0);
	gtk_alignment_set_padding (GTK_ALIGNMENT (Action_buttons_organizer), 4, 0, 0, 0);

	Action_buttons_seperator = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (Action_buttons_seperator);
	gtk_container_add (GTK_CONTAINER (Action_buttons_organizer), Action_buttons_seperator);

	Extract_button = gtk_button_new ();
	gtk_widget_show (Extract_button);
	gtk_box_pack_start (GTK_BOX (Action_buttons_seperator), Extract_button, TRUE, TRUE, 0);

	alignment10 = gtk_alignment_new (0.5, 0.5, 0, 0);
	gtk_widget_show (alignment10);
	gtk_container_add (GTK_CONTAINER (Extract_button), alignment10);

	hbox6 = gtk_hbox_new (FALSE, 2);
	gtk_widget_show (hbox6);
	gtk_container_add (GTK_CONTAINER (alignment10), hbox6);

	image4 = gtk_image_new_from_stock ("gtk-save", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show (image4);
	gtk_box_pack_start (GTK_BOX (hbox6), image4, FALSE, FALSE, 0);

	label16 = gtk_label_new_with_mnemonic (_("Extract"));
	gtk_widget_show (label16);
	gtk_box_pack_start (GTK_BOX (hbox6), label16, FALSE, FALSE, 0);

	View_button = gtk_button_new ();
	gtk_widget_show (View_button);
	gtk_box_pack_start (GTK_BOX (Action_buttons_seperator), View_button, TRUE, TRUE, 0);

	alignment9 = gtk_alignment_new (0.5, 0.5, 0, 0);
	gtk_widget_show (alignment9);
	gtk_container_add (GTK_CONTAINER (View_button), alignment9);

	hbox5 = gtk_hbox_new (FALSE, 2);
	gtk_widget_show (hbox5);
	gtk_container_add (GTK_CONTAINER (alignment9), hbox5);

	image3 = gtk_image_new_from_stock ("gtk-zoom-100", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show (image3);
	gtk_box_pack_start (GTK_BOX (hbox5), image3, FALSE, FALSE, 0);

	label15 = gtk_label_new_with_mnemonic (_("View"));
	gtk_widget_show (label15);
	gtk_box_pack_start (GTK_BOX (hbox5), label15, FALSE, FALSE, 0);

	Replace_button = gtk_button_new ();
	gtk_widget_show (Replace_button);
	gtk_box_pack_start (GTK_BOX (Action_buttons_seperator), Replace_button, TRUE, TRUE, 0);

	alignment8 = gtk_alignment_new (0.5, 0.5, 0, 0);
	gtk_widget_show (alignment8);
	gtk_container_add (GTK_CONTAINER (Replace_button), alignment8);

	hbox4 = gtk_hbox_new (FALSE, 2);
	gtk_widget_show (hbox4);
	gtk_container_add (GTK_CONTAINER (alignment8), hbox4);

	image2 = gtk_image_new_from_stock ("gtk-jump-to", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show (image2);
	gtk_box_pack_start (GTK_BOX (hbox4), image2, FALSE, FALSE, 0);

	label14 = gtk_label_new_with_mnemonic (_("Replace"));
	gtk_widget_show (label14);
	gtk_box_pack_start (GTK_BOX (hbox4), label14, FALSE, FALSE, 0);

	Disassemble_button = gtk_button_new ();
	gtk_widget_show (Disassemble_button);
	gtk_box_pack_start (GTK_BOX (Action_buttons_seperator), Disassemble_button, TRUE, TRUE, 0);

	alignment7 = gtk_alignment_new (0.5, 0.5, 0, 0);
	gtk_widget_show (alignment7);
	gtk_container_add (GTK_CONTAINER (Disassemble_button), alignment7);

	hbox3 = gtk_hbox_new (FALSE, 2);
	gtk_widget_show (hbox3);
	gtk_container_add (GTK_CONTAINER (alignment7), hbox3);

	image1 = gtk_image_new_from_stock ("gtk-zoom-fit", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show (image1);
	gtk_box_pack_start (GTK_BOX (hbox3), image1, FALSE, FALSE, 0);

	label13 = gtk_label_new_with_mnemonic (_("Disassemble"));
	gtk_widget_show (label13);
	gtk_box_pack_start (GTK_BOX (hbox3), label13, FALSE, FALSE, 0);
	
	/* No disassembler in this version! */
	gtk_widget_set_sensitive( Disassemble_button, FALSE );

	App_status = gtk_statusbar_new ();
	gtk_widget_show (App_status);
	gtk_box_pack_start (GTK_BOX (Main_vbox), App_status, FALSE, FALSE, 0);

	/* Signals - file menu */
	g_signal_connect_swapped( G_OBJECT(open1),   "activate", G_CALLBACK(gzrt_wfilesel_show), NULL );
	g_signal_connect_swapped( G_OBJECT(reload1), "activate", G_CALLBACK(gzrt_wmain_update), c );
	g_signal_connect_swapped( G_OBJECT(close1),  "activate", G_CALLBACK(gzrt_wmain_close), c );
	g_signal_connect_swapped( G_OBJECT(quit1),   "activate", G_CALLBACK(gzrt_gui_quit), NULL );
	
	//gzrt_wmain_update
	
	/* Signals - operations menu */
	//g_signal_connect_swapped( G_OBJECT(file_search1),    "activate", G_CALLBACK(), c );
	g_signal_connect_swapped( G_OBJECT(extract_files1),  "activate", G_CALLBACK(gzrt_wextract_show),      c );
	g_signal_connect_swapped( G_OBJECT(decompress_rom1), "activate", G_CALLBACK(gzrt_wdecompress_create), c );
	g_signal_connect_swapped( G_OBJECT(byteswap_rom1),   "activate", G_CALLBACK(gzrt_wbyteswap_create),   c );
	g_signal_connect_swapped( G_OBJECT(fix_crc1),        "activate", G_CALLBACK(create_CRC_Checker),      c );
	
	/* Signals - help menu */
	g_signal_connect_swapped( G_OBJECT(about1),"activate", G_CALLBACK(gzrt_wabout_show), NULL );
	#ifdef GZRT_DEBUG
	 g_signal_connect_swapped( G_OBJECT(wai1),"activate", G_CALLBACK(gzrt_wmain_focus), c );
	#endif
	
	/* Signals - window itself */
	c->hid = (void*)g_signal_connect_swapped( G_OBJECT(Main_Window), "destroy", G_CALLBACK(gzrt_wmain_closed), c );

	/* Store pointers to all widgets, for use by lookup_widget(). */
	GLADE_HOOKUP_OBJECT_NO_REF (Main_Window, Main_Window, "Main_Window");
	GLADE_HOOKUP_OBJECT (Main_Window, Main_vbox, "Main_vbox");
	GLADE_HOOKUP_OBJECT (Main_Window, Menu, "Menu");
	GLADE_HOOKUP_OBJECT (Main_Window, File_menu, "File_menu");
	GLADE_HOOKUP_OBJECT (Main_Window, File_menu_menu, "File_menu_menu");
	GLADE_HOOKUP_OBJECT (Main_Window, open1, "open1");
	GLADE_HOOKUP_OBJECT (Main_Window, reload1, "reload1");
	GLADE_HOOKUP_OBJECT (Main_Window, close1, "close1");
	GLADE_HOOKUP_OBJECT (Main_Window, image5, "image5");
	GLADE_HOOKUP_OBJECT (Main_Window, quit1, "quit1");
	GLADE_HOOKUP_OBJECT (Main_Window, Operations_menu, "Operations_menu");
	GLADE_HOOKUP_OBJECT (Main_Window, Operations_menu_menu, "Operations_menu_menu");
	GLADE_HOOKUP_OBJECT (Main_Window, extract_files1, "extract_files1");
	GLADE_HOOKUP_OBJECT (Main_Window, decompress_rom1, "decompress_rom1");
	GLADE_HOOKUP_OBJECT (Main_Window, file_search1, "file_search1");
	GLADE_HOOKUP_OBJECT (Main_Window, separator1, "separator1");
	GLADE_HOOKUP_OBJECT (Main_Window, byteswap_rom1, "byteswap_rom1");
	GLADE_HOOKUP_OBJECT (Main_Window, fix_crc1, "fix_crc1");
	GLADE_HOOKUP_OBJECT (Main_Window, Help_menu, "Help_menu");
	GLADE_HOOKUP_OBJECT (Main_Window, Help_menu_menu, "Help_menu_menu");
	GLADE_HOOKUP_OBJECT (Main_Window, about1, "about1");
	GLADE_HOOKUP_OBJECT (Main_Window, Main_window_padding, "Main_window_padding");
	GLADE_HOOKUP_OBJECT (Main_Window, Main_window_seperator, "Main_window_seperator");
	GLADE_HOOKUP_OBJECT (Main_Window, hpaned1, "hpaned1");
	GLADE_HOOKUP_OBJECT (Main_Window, Side_frame_seperator, "Side_frame_seperator");
	GLADE_HOOKUP_OBJECT (Main_Window, Frame_1_alignment, "Frame_1_alignment");
	GLADE_HOOKUP_OBJECT (Main_Window, ROM_Info, "ROM_Info");
	GLADE_HOOKUP_OBJECT (Main_Window, Frame_1_label_alignment, "Frame_1_label_alignment");
	GLADE_HOOKUP_OBJECT (Main_Window, Frame_1_label_seperator, "Frame_1_label_seperator");
	GLADE_HOOKUP_OBJECT (Main_Window, f1l1, "f1l1");
	GLADE_HOOKUP_OBJECT (Main_Window, f1l2, "f1l2");
	GLADE_HOOKUP_OBJECT (Main_Window, f1l3, "f1l3");
	GLADE_HOOKUP_OBJECT (Main_Window, f1l4, "f1l4");
	GLADE_HOOKUP_OBJECT (Main_Window, f1l5, "f1l5");
	GLADE_HOOKUP_OBJECT (Main_Window, Frame_1_label, "Frame_1_label");
	GLADE_HOOKUP_OBJECT (Main_Window, Frame_2_alignment, "Frame_2_alignment");
	GLADE_HOOKUP_OBJECT (Main_Window, Filesystem_info, "Filesystem_info");
	GLADE_HOOKUP_OBJECT (Main_Window, Frame_2_label_alignment, "Frame_2_label_alignment");
	GLADE_HOOKUP_OBJECT (Main_Window, Frame_2_label_seperator, "Frame_2_label_seperator");
	GLADE_HOOKUP_OBJECT (Main_Window, f2l1, "f2l1");
	GLADE_HOOKUP_OBJECT (Main_Window, f2l2, "f2l2");
	GLADE_HOOKUP_OBJECT (Main_Window, f2l3, "f2l3");
	GLADE_HOOKUP_OBJECT (Main_Window, f2l4, "f2l4");
	GLADE_HOOKUP_OBJECT (Main_Window, f2l5, "f2l5");
	GLADE_HOOKUP_OBJECT (Main_Window, Frame_2_label, "Frame_2_label");
	GLADE_HOOKUP_OBJECT (Main_Window, alignment12, "alignment12");
	GLADE_HOOKUP_OBJECT (Main_Window, Right_pane, "Right_pane");
	GLADE_HOOKUP_OBJECT (Main_Window, Column_view_scroller, "Column_view_scroller");
	GLADE_HOOKUP_OBJECT (Main_Window, treeview1, "treeview1");
	GLADE_HOOKUP_OBJECT (Main_Window, Action_buttons_organizer, "Action_buttons_organizer");
	GLADE_HOOKUP_OBJECT (Main_Window, Action_buttons_seperator, "Action_buttons_seperator");
	GLADE_HOOKUP_OBJECT (Main_Window, Extract_button, "Extract_button");
	GLADE_HOOKUP_OBJECT (Main_Window, alignment10, "alignment10");
	GLADE_HOOKUP_OBJECT (Main_Window, hbox6, "hbox6");
	GLADE_HOOKUP_OBJECT (Main_Window, image4, "image4");
	GLADE_HOOKUP_OBJECT (Main_Window, label16, "label16");
	GLADE_HOOKUP_OBJECT (Main_Window, View_button, "View_button");
	GLADE_HOOKUP_OBJECT (Main_Window, alignment9, "alignment9");
	GLADE_HOOKUP_OBJECT (Main_Window, hbox5, "hbox5");
	GLADE_HOOKUP_OBJECT (Main_Window, image3, "image3");
	GLADE_HOOKUP_OBJECT (Main_Window, label15, "label15");
	GLADE_HOOKUP_OBJECT (Main_Window, Replace_button, "Replace_button");
	GLADE_HOOKUP_OBJECT (Main_Window, alignment8, "alignment8");
	GLADE_HOOKUP_OBJECT (Main_Window, hbox4, "hbox4");
	GLADE_HOOKUP_OBJECT (Main_Window, image2, "image2");
	GLADE_HOOKUP_OBJECT (Main_Window, label14, "label14");
	GLADE_HOOKUP_OBJECT (Main_Window, Disassemble_button, "Disassemble_button");
	GLADE_HOOKUP_OBJECT (Main_Window, alignment7, "alignment7");
	GLADE_HOOKUP_OBJECT (Main_Window, hbox3, "hbox3");
	GLADE_HOOKUP_OBJECT (Main_Window, image1, "image1");
	GLADE_HOOKUP_OBJECT (Main_Window, label13, "label13");
	GLADE_HOOKUP_OBJECT (Main_Window, App_status, "App_status");
	#ifdef GZRT_DEBUG
	 GLADE_HOOKUP_OBJECT (Main_Window, wai1, "wai1");
	#endif

	/* Store window pointer & show */
	c->window = Main_Window;
	gtk_widget_show_all( Main_Window );
	
	gtk_window_add_accel_group (GTK_WINDOW (Main_Window), accel_group);
}

/* Update the display in the main window */
void gzrt_wmain_update ( MAINWIN *c )
{
	/*
	** This function will reload the opened ROM and in turn
	** update all of the controls in the main window.
	*/
	GtkWidget *ROM_Info;
	GtkWidget *Frame_1_label_alignment;
	GtkWidget *Frame_1_label_seperator;
	GtkWidget *f1l1;
	GtkWidget *f1l2;
	GtkWidget *f1l3;
	GtkWidget *f1l4;
	GtkWidget *f1l5;
	GtkWidget *f1l6;
	GtkWidget *Frame_1_label;
	GtkWidget *Filesystem_info;
	GtkWidget *Frame_2_label_alignment;
	GtkWidget *Frame_2_label_seperator;
	GtkWidget *f2l1;
	GtkWidget *f2l2;
	GtkWidget *f2l3;
	GtkWidget *f2l4;
	GtkWidget *f2l5;
	GtkWidget *f2l6;
	GtkWidget *Frame_2_label;

	
	char buffer[256];
	
	
	/* Destroy old frames */
	GtkWidget *i = g_object_get_data( G_OBJECT(c->window), "ROM_Info" );
	gtk_widget_destroy( i );
	i = g_object_get_data( G_OBJECT(c->window), "Filesystem_info" );
	gtk_widget_destroy( i );
	i = g_object_get_data( G_OBJECT(c->window), "Frame_1_alignment" ); /* ROM info */
	
	ROM_Info = gtk_frame_new (NULL);              
	gtk_widget_show (ROM_Info);
	gtk_container_add (GTK_CONTAINER (i), ROM_Info);

	Frame_1_label_alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (Frame_1_label_alignment);
	gtk_container_add (GTK_CONTAINER (ROM_Info), Frame_1_label_alignment);
	gtk_alignment_set_padding (GTK_ALIGNMENT (Frame_1_label_alignment), 8, 8, 12, 12);

	Frame_1_label_seperator = gtk_vbox_new (FALSE, 2);
	gtk_widget_show (Frame_1_label_seperator);
	gtk_container_add (GTK_CONTAINER (Frame_1_label_alignment), Frame_1_label_seperator);

	
	/*
	** Set ROM information
	*/
	
	/* ROM internal name */
	gzrt_wmain_frame_add
	(
		Frame_1_label_seperator, 
		"Name: %.24s", c->c->header + 0x20 
	);
	
	/* Game country code */
	gzrt_wmain_frame_add
	(
		Frame_1_label_seperator, 
		"Code: %.4s", 
		c->c->header + 0x3B
	);
	
	/* ROM filesize */
	gzrt_wmain_frame_add
	(
		Frame_1_label_seperator, 
		"Size: %.2fMB (%.2fMBits)", 
		(float)c->c->filesize / 1024.0 / 1024.0, 
		(float)c->c->filesize / 1024.0 / 1024.0 * 8.0 
	);
	
	/* ROM CRC # 1 */
	gzrt_wmain_frame_add
	(
		Frame_1_label_seperator, 
		"CRC 1: <span font_desc=\"Courier\">0x%08X</span>", 
		U32(c->c->header + 0x10) 
	);
	
	/* ROM CRC # 2 */
	gzrt_wmain_frame_add
	(
		Frame_1_label_seperator, 
		"CRC 2: <span font_desc=\"Courier\">0x%08X</span>", 
		U32(c->c->header + 0x14) 
	);
	
	/* ROM Entry point */
	gzrt_wmain_frame_add
	(
		Frame_1_label_seperator, 
		"Entry point: <span font_desc=\"Courier\">0x%08X</span>", 
		U32(c->c->header + 0x08) 
	);
	
	/*
	** Set filesystem information
	*/
	
	/* Set filesystem information */
	Frame_1_label = gtk_label_new (_("<b>ROM Info</b>"));
	gtk_widget_show (Frame_1_label);
	gtk_frame_set_label_widget (GTK_FRAME (ROM_Info), Frame_1_label);
	gtk_label_set_use_markup (GTK_LABEL (Frame_1_label), TRUE);
	
	
	i = g_object_get_data( G_OBJECT(c->window), "Frame_2_alignment" ); /* FS info */

	Filesystem_info = gtk_frame_new (NULL);
	gtk_widget_show (Filesystem_info);
	gtk_container_add (GTK_CONTAINER (i), Filesystem_info);
	/* | */
	Frame_2_label_alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (Frame_2_label_alignment);
	gtk_container_add (GTK_CONTAINER (Filesystem_info), Frame_2_label_alignment);
	gtk_alignment_set_padding (GTK_ALIGNMENT (Frame_2_label_alignment), 8, 8, 12, 13);
	/* | */
	Frame_2_label_seperator = gtk_vbox_new (FALSE, 2);
	gtk_widget_show (Frame_2_label_seperator);
	gtk_container_add (GTK_CONTAINER (Frame_2_label_alignment), Frame_2_label_seperator); 
	
	
	
	/* Filesystem start */
	gzrt_wmain_frame_add
	(
		Frame_2_label_seperator, 
		"Filesystem start: %s0x%08X%s", 
		MONO(c->z->rstart) 
	);
	
	/* Filesystem end */
	gzrt_wmain_frame_add
	(
		Frame_2_label_seperator, 
		"Filesystem end: %s0x%08X%s", 
		MONO(c->z->rend) 
	);

	/* File count */
	gzrt_wmain_frame_add
	(
		Frame_2_label_seperator, 
		"File count: %u", 
		z64fs_num_entries( c->z ) 
	);

	/* Total filesize */
	gzrt_wmain_frame_add
	(
		Frame_2_label_seperator, 
		"Total size: %.2fMB", 
		(float)z64fs_calc_size_decompressed( c->z ) / 1024.0 / 1024.0 
	);

	/* Name table */
	if( c->t )
	{
		/* Name table start */
		gzrt_wmain_frame_add
		(
			Frame_2_label_seperator, 
			"Name table start: %s0x%08X%s", 
			MONO(__NAME_TABLE_START) 
		);
		
		/* Name table end */
		gzrt_wmain_frame_add
		(
			Frame_2_label_seperator, 
			"Name table end: %s0x%08X%s", 
			MONO(__NAME_TABLE_END) 
		);
	}
	else
	{
		gzrt_wmain_frame_add
		(
			Frame_2_label_seperator, 
			"Name table: no"
		);
	}
	
	Frame_2_label = gtk_label_new (_("<b>Filesystem Info</b>"));
	gtk_widget_show (Frame_2_label);
	gtk_frame_set_label_widget (GTK_FRAME (Filesystem_info), Frame_2_label);
	gtk_label_set_use_markup (GTK_LABEL (Frame_2_label), TRUE);
	
	/* Debug */
	GZRTD_MESG( "Window %u redrawn.", c->id );
}

/* Add a new element to a side frame */
void gzrt_wmain_frame_add ( GtkWidget *vbox, char *fmt, ... )
{
	char buffer[256];
	GtkWidget * label;
	va_list ap;
	
	/* Generate text */
	va_start( ap, fmt );
	vsnprintf( buffer, sizeof(buffer) - 1, fmt, ap );
	va_end( ap );
	
	/* Create label */
	label = gtk_label_new( "" );
	gtk_label_set_markup( GTK_LABEL(label), buffer );
	
	/* Set alignment prefs */
	gtk_misc_set_alignment( GTK_MISC(label), 0.0f, 0.5f );
	
	/* Show */
	gtk_widget_show( label );
	
	/* Pack it */
	gtk_box_pack_start( GTK_BOX(vbox), label, TRUE, TRUE, 0 );
}

/* Disable an item */
void gzrt_wmain_disable_item ( MAINWIN *w, char *name )
{
	GtkWidget *i = g_object_get_data( G_OBJECT(w->window), name );
	gtk_widget_set_sensitive( i, FALSE );
}

/* Enable an item */
void gzrt_wmain_enable_item ( MAINWIN *w, char *name )
{
	GtkWidget *i = g_object_get_data( G_OBJECT(w->window), name );
	gtk_widget_set_sensitive( i, TRUE );
}

/* Set the status bar */
void gzrt_wmain_status_addmsg ( MAINWIN *w, char *msg )
{
	GtkWidget *i = g_object_get_data( G_OBJECT(w->window), "App_status" );
	gtk_statusbar_push( GTK_STATUSBAR(i), w->sstack++, msg );
	GZRTD_MESG( "Status message changed to: %s", msg );
}

/* Remove a message */
void gzrt_wmain_status_rmmsg ( MAINWIN *w )
{
	GtkWidget *i = g_object_get_data( G_OBJECT(w->window), "App_status" );
	gtk_statusbar_pop( GTK_STATUSBAR(i), --w->sstack );
	GZRTD_MESG( "Status reverted." );
}
