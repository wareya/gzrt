/**************************
* File replacement window *
**************************/
#include <gzrt.h>

/* Connect data to a widget */
#define HOOKUP( component, widget, name )                       \
    g_object_set_data_full( G_OBJECT(component), name,          \
    gtk_widget_ref(widget), (GDestroyNotify)gtk_widget_unref )

/* Lookup connected data */
#define LOOKUP( component, name )   \
    g_object_get_data( G_OBJECT(component), name )
	
/* Preexisting windows with parents */
static GList * parents;

/* Create the window */
GtkWidget * gzrt_wreplace_create ( MAINWIN * c )
{
	GtkWidget * window;			/* Window to be returned		*/
	GtkWidget * vbox;			/* Vbox for the entire window	*/
	GtkWidget * table;			/* Table for storing elements	*/
	GtkWidget * framealign;		/* Frame contents alignment		*/
	GtkWidget * framelabel;		/* For the frame's title		*/
	GtkWidget * frame;			/* The frame					*/
	GtkWidget * hbox;			/* For close/write buttons		*/
	GtkWidget * button;			/* For the actual buttons		*/
	GtkWidget * buttonalign;	/* Maintaining button scale		*/
	
	/* Text boxes */
	GtkWidget * addr_start;		/* File start address field	*/
	GtkWidget * addr_end;		/* File end address field 	*/
	GtkWidget * file_path;		/* File end address field 	*/
	GtkWidget * browse_button;	/* File end address field 	*/
	
	/* Labels */
	char *labels[] = { "Start offset:", "End offset:", "Source file:" };
	
	/* General */
	int	         i;
	int	         fid = gzrt_select_file_id(c);
	static char  addrs[64];
	static char  addre[64];
	
	/* Check for parent */
	if( parents && g_list_find( parents, c->window ) )
		return NULL;
	
	/* Append parent to list */
	parents = g_list_append( parents, c->window );
	
	/* Create window */
	window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	gtk_container_set_border_width( GTK_CONTAINER(window), 12 );
	gtk_window_set_resizable( GTK_WINDOW(window), FALSE );
	gtk_window_set_title( GTK_WINDOW(window), "Insert file" );
	HOOKUP( window, c, "parent" );
	
	/* Create vbosx */
	vbox = gtk_vbox_new( FALSE, 12 );
	gtk_container_add( GTK_CONTAINER(window), vbox );
	
	/* Create frame */
	frame = gtk_frame_new( NULL );
	framelabel = gtk_label_new( "<b>Options</b>" );
	gtk_label_set_use_markup( GTK_LABEL(framelabel), TRUE );
	gtk_frame_set_label_widget( GTK_FRAME(frame), framelabel );
	gtk_box_pack_start( GTK_BOX(vbox), frame, TRUE, FALSE, 0 );
	
	/* Create frame contents alignment */
	framealign = gtk_alignment_new( 0.5, 0.5f, 1.0f, 1.0f );
	gtk_alignment_set_padding( GTK_ALIGNMENT(framealign), 8, 8, 12, 12 );
	gtk_container_add( GTK_CONTAINER(frame), framealign );
	
	/* Create table */
	table = gtk_table_new( 3, 2, FALSE );
	gtk_table_set_col_spacings( GTK_TABLE(table), 8 );
	gtk_table_set_row_spacings( GTK_TABLE(table), 8 );
	gtk_container_add( GTK_CONTAINER(framealign), table );
	
	/* Create labels */
	for( i = 0; i < 3; i++ )
	{
		GtkWidget * label = gtk_label_new( labels[i] );
		
		/* Set alignment */
		gtk_misc_set_alignment( GTK_MISC(label), 0.0f, 0.5f );
		
		/* Pack it */
		gtk_table_attach( GTK_TABLE(table), label, 0, 1, i, i + 1, 
		GTK_FILL, GTK_FILL, 0, 0 );
	}
	
	/* Create text boxes */
	addr_start = gtk_entry_new();
	addr_end   = gtk_entry_new();
	HOOKUP( window, addr_start, "text-start" );
	HOOKUP( window, addr_end,   "text-end"   );
	gtk_table_attach( GTK_TABLE(table), addr_start, 1, 2, 0, 1, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0 );
	gtk_table_attach( GTK_TABLE(table), addr_end  , 1, 2, 1, 2, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0 );
	
	/* Set defaults */
	sprintf( addrs, "0x%08X", U32(&c->z->fs_table[fid*16])   );
	sprintf( addre, "0x%08X", U32(&c->z->fs_table[fid*16+4]) );
	gtk_entry_set_text( GTK_ENTRY(addr_start), addrs );
	gtk_entry_set_text( GTK_ENTRY(addr_end),   addre );
	
	/* File entry */
	hbox = gtk_hbox_new( FALSE, 4 );
	gtk_table_attach( GTK_TABLE(table), hbox, 1, 2, 2, 3, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0 );
	file_path = gtk_entry_new();
	HOOKUP( window, file_path, "text-file" );
	gtk_box_pack_start( GTK_BOX(hbox), file_path, TRUE, TRUE, 0 );
	browse_button = gtk_button_new_with_label( "Browse" );
	g_signal_connect_swapped( G_OBJECT(browse_button), "clicked", G_CALLBACK(gzrt_wreplace_file), window );
	HOOKUP( window, browse_button, "browse-button" );
	gtk_box_pack_start( GTK_BOX(hbox), browse_button, FALSE, FALSE, 0 );
	
	/* Create buttons */
	hbox = gtk_hbox_new( FALSE, 4 );
	button = gtk_button_new_with_label( "Close" );
	g_signal_connect_swapped( G_OBJECT(button), "clicked", G_CALLBACK(gzrt_wreplace_close), window );
	HOOKUP( window, button, "close-button" );
	buttonalign = gtk_alignment_new( 0.5f, 0.5f, 0.5f, 0.0f );
	gtk_container_add( GTK_CONTAINER(buttonalign), button );
	gtk_box_pack_start( GTK_BOX(hbox), buttonalign, TRUE, TRUE, 0 );
	button = gtk_button_new_with_label( "Write" );
	g_signal_connect_swapped( G_OBJECT(button), "clicked", G_CALLBACK(gzrt_wreplace_write), window );
	HOOKUP( window, button, "write-button" );
	buttonalign = gtk_alignment_new( 0.5f, 0.5f, 0.5f, 0.0f );
	gtk_container_add( GTK_CONTAINER(buttonalign), button );
	gtk_box_pack_start( GTK_BOX(hbox), buttonalign, TRUE, TRUE, 0 );
	
	/* Pack button box */
	gtk_box_pack_start( GTK_BOX(vbox), hbox, TRUE, FALSE, 0 );
	
	/* On destroy, remove record of parent */
	g_signal_connect_swapped( G_OBJECT(window), "destroy", G_CALLBACK(gzrt_wreplace_destroy), c );
	
	/* Show all */
	gtk_widget_show_all( window );
	
	/* Return it */
	return window;
}

/* Close a window */
void gzrt_wreplace_close ( GtkWidget * window )
{
	gtk_widget_destroy( window );
}

/* Destroy handler */
void gzrt_wreplace_destroy ( MAINWIN * c )
{
	parents = g_list_remove( parents, c->window );
}

/* Get a filename */
void gzrt_wreplace_file ( GtkWidget * window )
{
	static char ret[512];
	GtkWidget * dialog = gtk_file_chooser_dialog_new
	( 
		"Choose a file", NULL,
		GTK_FILE_CHOOSER_ACTION_OPEN, 
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, 
		NULL
	);
	GtkWidget * file_box = LOOKUP( window, "text-file" );
	
	/* Wait for filename */
	switch( gtk_dialog_run( GTK_DIALOG(dialog) ) )
	{
		/* Yes */
		case GTK_RESPONSE_ACCEPT:
		 strcpy( ret, gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(dialog) ) );
		 gtk_entry_set_text( GTK_ENTRY(file_box), ret );
		 gtk_widget_destroy( dialog );
		break;
		
		/* None? */
		default:
		 gtk_widget_destroy( dialog );
	}
}

/* Write the file */
void gzrt_wreplace_write ( GtkWidget * window )
{
	GtkWidget * field = LOOKUP( window, "text-file" );
	FILE * handle = fopen( gtk_entry_get_text( GTK_ENTRY(field) ), "rb" );
	GtkWidget * saddr = LOOKUP( window, "text-start" );
	GtkWidget * eaddr = LOOKUP( window, "text-end" );
	unsigned	s;
	unsigned	e;
	unsigned	filesize;
	unsigned char * buffer;
	MAINWIN * parent = LOOKUP( window, "parent" );
	
	if( !handle ) {
		gzrt_notice( "Notice", "Error reading file" );
		return;
	}
	
	/* Get filesize */
	fseek( handle, 0, SEEK_END );
	buffer = malloc(filesize = ftell(handle));
	fseek( handle, 0, SEEK_SET );
	fread( buffer, 1, filesize, handle );
	fclose( handle );
	
	/* Get offsets */
	sscanf( gtk_entry_get_text( GTK_ENTRY(saddr) ), "%X", &s );
	sscanf( gtk_entry_get_text( GTK_ENTRY(eaddr) ), "%X", &e );
	
	/* Check */
	if( e - s < filesize )
	{
		gzrt_notice( "Notice", "This file is too large!" );
		return;
	}
	
	fseek( parent->c->handle, s, SEEK_SET );
	fwrite( buffer, 1, filesize, parent->c->handle );
	
	gzrt_notice( "Notice", "All ok." );
	
	free( buffer );
}
