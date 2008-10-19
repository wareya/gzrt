/*********************************
* MIPS R4000 Disassembler - Save *
*********************************/
#include <stdio.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <disassembler.h>

/* Connect data to a widget */
#define HOOKUP( component, widget, name )                       \
    g_object_set_data_full( G_OBJECT(component), name,          \
    widget, NULL )

/* Lookup connected data */
#define LOOKUP( component, name )   \
    g_object_get_data( G_OBJECT(component), name )
	
#define U32(b) ( (b)[0] << 24 | \
                        (b)[1] << 16 | \
                        (b)[2] <<  8 | \
                        (b)[3] )
	
/* Functions */
static void dasm_write_disassembly ( GtkWidget * w );
static void dasm_find_file ( GtkWidget * w );

/* Close the window */
static void close_window ( GtkWidget * window )
{
	gtk_widget_destroy( window );
}

/* Show the window */
void dasm_save_show ( DASM * h )
{
	GtkWidget * window;
	GtkWidget * vbox;
	
	GtkWidget * _1_hbox;
	GtkWidget * _2_exp;
	GtkWidget * _3_alg;
	
	/* Create window */
	window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	gtk_window_set_title( GTK_WINDOW(window), "Save disassembly" );
	gtk_window_set_position( GTK_WINDOW(window), GTK_WIN_POS_CENTER );
	gtk_window_set_modal( GTK_WINDOW(window), TRUE );
	gtk_window_set_resizable( GTK_WINDOW(window), FALSE );
	gtk_container_set_border_width( GTK_CONTAINER(window), 12 );
	
	/* Hookup */
	HOOKUP( window, h, "parent" );
	
	/* Create vbox */
	vbox = gtk_vbox_new( FALSE, 8 );
	gtk_container_add( GTK_CONTAINER(window), vbox );
	
/*  File saving row
*/{
	GtkWidget * label;
	GtkWidget * entry;
	GtkWidget * button;
	
	/* Create destination */
	_1_hbox = gtk_hbox_new( FALSE, 8 );
	
	/* Contents... */
	label = gtk_label_new( "Output file:" );
	entry = gtk_entry_new( );
	button = gtk_button_new_with_label( "Browse..." );
	HOOKUP( window, entry, "dest" );
	g_signal_connect_swapped( G_OBJECT(button), "clicked", G_CALLBACK(dasm_find_file), window );
	
	/* Pack 'em */
	gtk_box_pack_start( GTK_BOX(_1_hbox), label,  FALSE, FALSE, 0 );
	gtk_box_pack_start( GTK_BOX(_1_hbox), entry,  TRUE,  TRUE,  0 );
	gtk_box_pack_start( GTK_BOX(_1_hbox), button, FALSE, FALSE, 0 );
}/*
 */
 
/*  Option specifying row
*/{
	GtkWidget * label;
	GtkWidget * entry;
	GtkWidget * table;
	GtkWidget * vbox;
	GtkWidget * check;
	GtkWidget * align;
	char buffer[64];
	
	/* Create destination */
	_2_exp = gtk_expander_new( "Options" );
	
	/* Create alignment */
	align = gtk_alignment_new( 0.5f, 0.5f, 1.0f, 1.0f );
	gtk_alignment_set_padding( GTK_ALIGNMENT(align), 0, 0, 12, 0 );
	gtk_container_add( GTK_CONTAINER(_2_exp), align );
	
	/* Create vbox */
	vbox = gtk_vbox_new( FALSE, 8 );
	gtk_container_add( GTK_CONTAINER(align), vbox );
	
	/* Create table */
	table = gtk_table_new( 2, 2, FALSE );
	gtk_table_set_row_spacings( GTK_TABLE(table), 4 );
	gtk_table_set_col_spacings( GTK_TABLE(table), 4 );
	gtk_box_pack_start( GTK_BOX(vbox), table, FALSE, TRUE, 0 );
	
	/* Start address */
	sprintf(buffer, "%08X", h->pc_start + h->pc );
	label = gtk_label_new( "Start address:" );
	entry = gtk_entry_new();
	gtk_entry_set_text( GTK_ENTRY(entry), buffer );
	HOOKUP( window, entry, "addr-1" );
	gtk_misc_set_alignment( GTK_MISC(label), 0.0f, 0.5f );
	gtk_table_attach( GTK_TABLE(table), label, 0, 1, 0, 1, GTK_FILL, 0, 0, 0 );
	gtk_table_attach( GTK_TABLE(table), entry, 1, 2, 0, 1, GTK_FILL | GTK_EXPAND, 0, 0, 0 );
	
	/* End address */
	sprintf(buffer, "%08X", h->pc_start + h->filesize );
	label = gtk_label_new( "End address:" );
	entry = gtk_entry_new();
	gtk_entry_set_text( GTK_ENTRY(entry), buffer );
	HOOKUP( window, entry, "addr-2" );
	gtk_misc_set_alignment( GTK_MISC(label), 0.0f, 0.5f );
	gtk_table_attach( GTK_TABLE(table), label, 0, 1, 1, 2, GTK_FILL, 0, 0, 0 );
	gtk_table_attach( GTK_TABLE(table), entry, 1, 2, 1, 2, GTK_FILL | GTK_EXPAND, 0, 0, 0 );
	
	/* Create checkboxes */
	check = gtk_check_button_new_with_label( "Add instruction offset" );
	HOOKUP( window, check, "opt-1" );
	gtk_box_pack_start( GTK_BOX(vbox), check, FALSE, FALSE, 0 );
	check = gtk_check_button_new_with_label( "Include raw instruction (hex)" );
	HOOKUP( window, check, "opt-2" );
	gtk_box_pack_start( GTK_BOX(vbox), check, FALSE, FALSE, 0 );
}/*
 */	
 
/*  Action buttons
*/{
	GtkWidget * hbox;
	GtkWidget * button;
	
	/* Create destination */
	_3_alg = gtk_alignment_new( 0.0f, 0.5f, 0.2f, 1.0f );
	
	/* Create hbox */
	hbox = gtk_hbox_new( TRUE, 8 );
	gtk_container_add( GTK_CONTAINER(_3_alg), hbox );
	
	/* Buttons... */
	button = gtk_button_new_with_label( "Close" );
	gtk_box_pack_start( GTK_BOX(hbox), button, FALSE, TRUE, 0 );
	g_signal_connect_swapped( G_OBJECT(button), "clicked", G_CALLBACK(close_window), window );
	button = gtk_button_new_with_label( "Write" );
	gtk_box_pack_start( GTK_BOX(hbox), button, FALSE, TRUE, 0 );
	g_signal_connect_swapped( G_OBJECT(button), "clicked", G_CALLBACK(dasm_write_disassembly), window );
}/*
 */

 	/* Pack rows */
	gtk_box_pack_start( GTK_BOX(vbox), _1_hbox, FALSE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(vbox), _2_exp,  TRUE,  TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(vbox), _3_alg,  TRUE,  TRUE, 0 );
	
	/* Show everything */
	gtk_widget_show_all( window );
}

/* Find a file */
static void dasm_find_file ( GtkWidget * w )
{
	GtkWidget * entry = LOOKUP( w, "dest" );
	GtkWidget * dialog;
	FILE      * h;
	int			result;
	char      * n;
	
	/* Create dir choosing dialog */
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
			
			/* Check that we can write to it */
			if( !(h = fopen(n, "wb")) )
				continue;
			
			/* Destroy window */
			gtk_widget_destroy( dialog );
			
			/* Looks good, set file */
			gtk_entry_set_text( GTK_ENTRY(entry), n );
			return;
		}
		break;
		
		/* Cancel */
		case GTK_RESPONSE_REJECT:
		 gtk_widget_destroy(dialog);
		 goto skiploop;
		break;
		
		/* Default */
		default:
		 gtk_widget_destroy(dialog);
		 goto skiploop;
	}
	
	/* Out of loop */
skiploop: ;
	return;
}

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

/* Write disassembly */
static void dasm_write_disassembly ( GtkWidget * w )
{
	DASM      * h  = LOOKUP( w, "parent" );
	GtkWidget * en = LOOKUP( w, "dest" );
	GtkWidget * _s = LOOKUP( w, "addr-1" );
	GtkWidget * _e = LOOKUP( w, "addr-2" );
	GtkWidget * c1 = LOOKUP( w, "opt-1" );
	GtkWidget * c2 = LOOKUP( w, "opt-2" );
	gboolean
		op1 = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(c1) ),
		op2 = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(c2) );
	GtkWidget * window;
	GtkWidget * pbar;
	FILE      * fh;
	int         i;
	unsigned	start;
	unsigned	end;
	
	/* Get start/end */
	sscanf( gtk_entry_get_text( GTK_ENTRY(_s) ), "%08X", &start );
	sscanf( gtk_entry_get_text( GTK_ENTRY(_e) ), "%08X", &end );
	start &= 0x00FFFFFFUL;
	end   &= 0x00FFFFFFUL;
	
	/* Create window for progress indicator */
	window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    gtk_widget_set_size_request( window, 400, 30 );
	gtk_window_set_title( GTK_WINDOW(window), "Working..." );
	gtk_window_set_position( GTK_WINDOW(window), GTK_WIN_POS_CENTER_ALWAYS );
	gtk_window_set_keep_above( GTK_WINDOW(window), TRUE );
	gtk_window_set_modal( GTK_WINDOW(window), TRUE );
	gtk_window_set_resizable( GTK_WINDOW(window), FALSE );
	gtk_window_set_deletable( GTK_WINDOW(window), FALSE );
	
	/* progress bar */
	pbar = gtk_progress_bar_new( );
	gtk_container_add( GTK_CONTAINER(window), pbar );
	
	/* Show it */
	gtk_widget_show_all( window );
	
	/* Open file for writing */
	fh = fopen( gtk_entry_get_text( GTK_ENTRY(en) ), "wb" );
	
	/* Loop */
	g_print( "%X, %X\n", start, MIN(end, h->filesize) );
	for( i = start; i < MIN(end, h->filesize); i += 4 )
	{
		char buffer[128];
		char * p = buffer;
		
		/* Address? */
		if( op1 )
			p += sprintf( p, "%08X  ", h->pc_start + i );
		
		/* hexadecimal? */
		if( op2 )
			p += sprintf( p, "%08X  ", U32(h->data + i) );
		
		/* Disassemble */
		mr4kd_disassemble( U32(h->data + i), h->pc_start + i, p );
		
		/* Write it */
		fprintf( fh, "%s\n", buffer );
		
		/* Update progress bar */
		if( !((i + 4) % (h->filesize / 32)) )
			pbarset( pbar, (double)i / h->filesize, "%.2f%%", (double)i / h->filesize * 100.0 );
	}
	
	gtk_widget_destroy( window );
}
