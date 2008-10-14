/********************
* CRC Fixing Window *
********************/
#include <gzrt.h>
#include <gui/crc.h>
#include <n64rom.h>

/* Connect data to a widget */
#define HOOKUP( component, widget, name )                       \
    g_object_set_data( G_OBJECT(component), name, widget )

static GList * parents;

/* Create an instance */
void gzrt_wcrc_create ( MAINWIN *parent )
{
}

/* Create a new window */
GtkWidget * create_CRC_Checker ( MAINWIN *parent )
{
}

/* Write CRCs to ROM */
void gzrt_wcrc_write ( struct crcarg *c, GtkWidget *w )
{
}

/* Close the CRC window */
static void gzrt_wcrc_close ( GtkWidget * w )
{
	gtk_widget_destroy( w );
}

/* Destroy handler */
static void gzrt_wcrc_closed ( MAINWIN *w )
{
	/* Remove window from parent list */
	parents = g_list_remove( parents, w );
}

/* Show the CRC checking window */
void gzrt_wcrc_show ( MAINWIN * c )
{
	GtkWidget * window;
	GtkWidget * vbox;
	GtkWidget * hbox;
	GtkWidget * table;
	GtkWidget * label;
	GtkWidget * image;
	GtkWidget * entry;
	GtkWidget * button;
	unsigned	oldcrc[2];
	unsigned 	crcs[2];
	
	/* Does this parent window exist? */
	if( g_list_find( parents, c ) )
		
		/* Yeah */
		return;
		
	/* Add the parent */
	parents = g_list_append( parents, c );
	
	/* Calculate CRCs */
	n64rom_crc( c->c, crcs );
	
	/* Read in old CRCs */
	oldcrc[0] = c->c->head.CRC1;
	oldcrc[1] = c->c->head.CRC2;
	
	/* Set up the window */
	window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	gtk_window_set_title( GTK_WINDOW(window), "CRC Checker" );
	gtk_window_set_position( GTK_WINDOW(window), GTK_WIN_POS_CENTER );
	gtk_window_set_modal( GTK_WINDOW(window), TRUE );
	gtk_window_set_resizable( GTK_WINDOW(window), FALSE );
	gtk_container_set_border_width( GTK_CONTAINER(window), 12 );
	
	/* Create vbox */
	vbox = gtk_vbox_new( FALSE, 8 );
	
	/* Create table */
	table = gtk_table_new( 2, 2, FALSE );
	gtk_table_set_row_spacings( GTK_TABLE(table), 8 );
	gtk_table_set_col_spacings( GTK_TABLE(table), 8 );
	
	/* Create the two CRC rows */
	for( int i = 0; i < 2; i++ )
	{
		char buffer[64];
		
		/* Generate text */
		sprintf( buffer, "CRC %u:", i + 1 );
		
		/* Create label */
		label = gtk_label_new( buffer );
		
		/* Create hbox */
		hbox = gtk_hbox_new( FALSE, 4 );
		
		/* Create text entry */
		entry = gtk_entry_new();
		sprintf( buffer, "0x%08X", crcs[i] );
		gtk_entry_set_text( GTK_ENTRY(entry), buffer );		
		
		/* Create image */
		image = gtk_image_new_from_stock( (crcs[i]==oldcrc[i] ? "gtk-yes" : "gtk-no"), GTK_ICON_SIZE_SMALL_TOOLBAR );
		
		/* Hookup entries */
		sprintf( buffer, "crc-%u", i + 1 );
		HOOKUP( window, entry, buffer );
		sprintf( buffer, "crc-%u-img", i + 1 );
		HOOKUP( window, image, buffer );
		
		/* Pack */
		gtk_table_attach( GTK_TABLE(table), label, 0, 1, i, i + 1, GTK_FILL, 0, 0, 0 );
		gtk_table_attach( GTK_TABLE(table), hbox,  1, 2, i, i + 1, GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0 );
		gtk_box_pack_start( GTK_BOX(hbox), entry, TRUE,  TRUE, 0 );
		gtk_box_pack_start( GTK_BOX(hbox), image, FALSE, TRUE, 0 );
	}
	
	/* Create button hbox */
	hbox = gtk_hbox_new( TRUE, 12 );
	
	/* Create buttons */
	button = gtk_button_new_with_label( "Fix" );
	gtk_widget_set_sensitive( button, (!memcmp(crcs,oldcrc,8) ? FALSE : TRUE) );
	gtk_box_pack_start( GTK_BOX(hbox), button, TRUE, TRUE, 12 );
	button = gtk_button_new_with_label( "Close" );
	g_signal_connect_swapped( G_OBJECT(button), "clicked", G_CALLBACK(gzrt_wcrc_close), window );
	gtk_box_pack_start( GTK_BOX(hbox), button, TRUE, TRUE, 12 );
	
	/* Pack everything */
	gtk_box_pack_start( GTK_BOX(vbox), table, TRUE, TRUE,  0 );
	gtk_box_pack_start( GTK_BOX(vbox), hbox,  TRUE, FALSE, 0 );
	gtk_container_add( GTK_CONTAINER(window), vbox );
	
	/* Signals */
	g_signal_connect_swapped( G_OBJECT(window), "destroy", G_CALLBACK(gzrt_wcrc_closed), c );
	
	/* Show */
	gtk_widget_show_all( window );
}
