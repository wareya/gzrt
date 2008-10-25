/****************************
* GZRT Configuration Window *
****************************/
#include <gzrt.h>

static GList * parents;

/* Create the window */
void gzrt_wconf_show ( MAINWIN * c )
{
	GtkWidget * window;
	GtkWidget * vbox;
	GtkWidget * frame;
	GtkWidget * align;
	GtkWidget * hbox;
	GtkWidget * button;
	GtkWidget * label;
	GtkWidget * entry;
	GtkWidget * combo;
	GtkWidget * table;
	
	/* Does this parent already exist? */
	if( g_list_find( parents, c ) )
		
		/* Yeah */
		return;
	
	/* Add parent */
	parents = g_list_append( parents, c );
	
	/* Create window */
	window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	gtk_window_set_title( GTK_WINDOW(window), "Configuration" );
	gtk_window_set_position( GTK_WINDOW(window), GTK_WIN_POS_CENTER );
	gtk_window_set_modal( GTK_WINDOW(window), TRUE );
	gtk_container_set_border_width( GTK_CONTAINER(window), 12 );
	
	/* Create vbox */
	vbox = gtk_vbox_new( FALSE, 12 );
	gtk_container_add( GTK_CONTAINER(window), vbox );
	
	/* Create frame */
	frame = gtk_frame_new( "Application settings" );
	gtk_box_pack_start( GTK_BOX(vbox), frame, TRUE, TRUE, 0 );
	
	/* Create content alignment */
	align = gtk_alignment_new( 0.5f, 0.5f, 1.0f, 1.0f );
	gtk_alignment_set_padding( GTK_ALIGNMENT(align), 8, 8, 12, 12 );
	gtk_container_add( GTK_CONTAINER(frame), align );
	
	/* Create table */
	table = gtk_table_new( 2, 2, FALSE );
	gtk_table_set_row_spacings( GTK_TABLE(table), 8 );
	gtk_table_set_col_spacings( GTK_TABLE(table), 8 );
	gtk_container_add( GTK_CONTAINER(align), table );
	
	/* Default ROM */
	label = gtk_label_new( "Default ROM:" );
	gtk_misc_set_alignment( GTK_MISC(label), 0.0f, 0.5f );
	gtk_table_attach( GTK_TABLE(table), label, 0, 1, 0, 1, GTK_FILL, 0, 0, 0 );
	hbox = gtk_hbox_new( FALSE, 4 );
	gtk_table_attach( GTK_TABLE(table), hbox, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, 0, 0, 0 );
	entry = gtk_entry_new();
	gtk_box_pack_start( GTK_BOX(hbox), entry, TRUE, TRUE, 0 );
	button = gtk_button_new_with_label( "Current" );
	gtk_box_pack_start( GTK_BOX(hbox), button, FALSE, FALSE, 0 );
	button = gtk_button_new_with_label( "Other..." );
	gtk_box_pack_start( GTK_BOX(hbox), button, FALSE, FALSE, 0 );
	
	/* Default plugin */
	label = gtk_label_new( "Default plugin:" );
	gtk_misc_set_alignment( GTK_MISC(label), 0.0f, 0.5f );
	gtk_table_attach( GTK_TABLE(table), label, 0, 1, 1, 2, GTK_FILL, 0, 0, 0 );
	combo = gtk_combo_box_new();
	gtk_table_attach( GTK_TABLE(table), combo, 1, 2, 1, 2, GTK_FILL, 0, 0, 0 );
	
	/* Create buttons */
	align = gtk_alignment_new( 0.0f, 0.5f, 0.25f, 1.0f );
	gtk_box_pack_start( GTK_BOX(vbox), align, FALSE, TRUE, 0 );
	hbox = gtk_hbox_new( TRUE, 8 );
	gtk_container_add( GTK_CONTAINER(align), hbox );
	button = gtk_button_new_with_label( "Close" );
	gtk_box_pack_start( GTK_BOX(hbox), button, TRUE, TRUE, 0 );
	button = gtk_button_new_with_label( "Apply" );
	gtk_box_pack_start( GTK_BOX(hbox), button, TRUE, TRUE, 0 );
	button = gtk_button_new_with_label( "Reload" );
	gtk_box_pack_start( GTK_BOX(hbox), button, TRUE, TRUE, 0 );
	
	/* Show everything */
	gtk_widget_show_all( window );
}
	
	
	
