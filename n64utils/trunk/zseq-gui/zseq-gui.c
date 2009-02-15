/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   ZSEQ GTK GUI - config.c                                               *
 *    - GUI (C) 2009 ZZT32 (http://64.vg/)								   *
 *    - App (C) 2009 messiaen											   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

#define BIN_PATH "./zseq.exe"

/* Declarations */
static int run ( void );

/* Window entities */
static GtkWidget * window;
static GtkWidget * vbox;
static GtkWidget * hbox1;
static GtkWidget *  open_seq;
static GtkWidget *  open_rom;
static GtkWidget *  go;
static GtkWidget * hbox2;
static GtkWidget *  combo;
static GtkWidget *  check;

/* Other */
static GList * songs;
static char * seq_path;
static char * rom_path;
static int seq_all;

/* Display a dialog */
static void
msg ( char * title, char * fmt, ... )
{    GtkWidget * dialog;
    GtkWidget * image;
    GtkWidget * vbox;
    GtkWidget * hbox;
    GtkWidget * label;
    va_list ap;
    static char buffer[2048];
    char * icon;
    int len, result;
    
    /* Start VA args */
    va_start( ap, fmt );
    
    /* Prepare message */
    len = vsnprintf( buffer, sizeof(buffer), fmt, ap );
    
    /* Check title */
    if( !title )
      title = "Notice";

    /* Get message type */
    icon = GTK_STOCK_DIALOG_INFO;
    
    /* Create GTK dialog */
    dialog = gtk_dialog_new_with_buttons
    (
        title, NULL, GTK_DIALOG_MODAL, 
        GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, 
        NULL
    );
    vbox = GTK_DIALOG(dialog)->vbox;
    
    /* Create an hbox */
    hbox = gtk_hbox_new( FALSE, 4 );
    gtk_box_pack_start( GTK_BOX(vbox), hbox, TRUE, TRUE, 0 );
    
    /* Get stock image */
    image = gtk_image_new_from_stock( icon, GTK_ICON_SIZE_DIALOG );
    gtk_box_pack_start( GTK_BOX(hbox), image, FALSE, FALSE, 8 );
    
    /* Create message */
    label = gtk_label_new( buffer );
    gtk_box_pack_start( GTK_BOX(hbox), label, FALSE, TRUE, 8 );
    
    /* Finalize dialog */
    gtk_widget_show_all( dialog );
    
    /* Run the dialog */
    while( result = gtk_dialog_run( GTK_DIALOG(dialog) ) )
      switch( result )
      {
        case GTK_RESPONSE_ACCEPT: case GTK_RESPONSE_DELETE_EVENT:
         goto loopdone;
        break;
      }
      
    /* Destroy the dialog */
loopdone:
    gtk_widget_destroy( dialog );
    
    /* End VA list */
cleanup:
    va_end( ap );
}

/* Get a ROM filename */
static void
load_rom ( void )
{
	GtkWidget * dialog = gtk_file_chooser_dialog_new
	( 
		"Load ROM", NULL,
		GTK_FILE_CHOOSER_ACTION_OPEN, 
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, 
		NULL
	);
	
diagrun: ;
	/* Check outcome */
	switch( gtk_dialog_run(GTK_DIALOG(dialog)) )
	{
		/* Chose? */
		case GTK_RESPONSE_ACCEPT:
	    
		 rom_path = strdup(gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(dialog) ));
		 gtk_widget_destroy( dialog );
		 goto outloop;
		 
		break;
		
		/* NO! */
		case GTK_RESPONSE_CANCEL: case GTK_RESPONSE_DELETE_EVENT:
		
		 gtk_widget_destroy( dialog );
		 goto outloop;
		
		break;
	}
	
	outloop: ;
}

/* Get a sequence filename */
static void
load_seq ( void )
{
	GtkWidget * dialog = gtk_file_chooser_dialog_new
	( 
		"Load ROM", NULL,
		GTK_FILE_CHOOSER_ACTION_OPEN, 
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, 
		NULL
	);
	
diagrun: ;
	/* Check outcome */
	switch( gtk_dialog_run(GTK_DIALOG(dialog)) )
	{
		/* Chose? */
		case GTK_RESPONSE_ACCEPT:
	    
		 seq_path = strdup(gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(dialog) ));
		 gtk_widget_destroy( dialog );
		 goto outloop;
		 
		break;
		
		/* NO! */
		case GTK_RESPONSE_CANCEL: case GTK_RESPONSE_DELETE_EVENT:
		
		 gtk_widget_destroy( dialog );
		 goto outloop;
		
		break;
	}
	
	outloop: ;
}

/* Check handler */
static int
check_handler ( GtkWidget * w )
{
	seq_all = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(w) );
	
	if( !seq_all )
		gtk_widget_set_sensitive( combo, TRUE );
	else
		gtk_widget_set_sensitive( combo, FALSE );
	
	return FALSE;
}

/* Create window */
static void
create_window ( void )
{
	window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	vbox = gtk_vbox_new( FALSE, 8 );
	hbox1 = gtk_hbox_new( TRUE, 8 );
	open_seq = gtk_button_new_with_label( "Open seq file" );
	open_rom = gtk_button_new_with_label( "Open ROM file" );
	go = gtk_button_new_with_label( "Go!" );
	hbox2 = gtk_hbox_new( FALSE, 8 );
	combo = gtk_combo_box_new_text( );
	check = gtk_check_button_new_with_label( "Replace all sequences" );
	
	gtk_container_add( GTK_CONTAINER(window), vbox );
	gtk_box_pack_start( GTK_BOX(vbox), hbox1, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(vbox), hbox2, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(hbox1), open_seq, FALSE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(hbox1), open_rom, FALSE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(hbox1), go, FALSE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(hbox2), combo, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(hbox2), check, FALSE, TRUE, 0 );
	
	gtk_container_set_border_width( GTK_CONTAINER(window), 8 );
	gtk_window_set_title( GTK_WINDOW(window), "ZSEQ" );
	
	gtk_widget_show_all( window );
	
	g_signal_connect( G_OBJECT(go), "clicked", G_CALLBACK(run), NULL );
	g_signal_connect( G_OBJECT(open_seq), "clicked", G_CALLBACK(load_seq), NULL );
	g_signal_connect( G_OBJECT(open_rom), "clicked", G_CALLBACK(load_rom), NULL );
	g_signal_connect( G_OBJECT(check), "toggled", G_CALLBACK(check_handler), NULL );
	g_signal_connect_swapped( G_OBJECT(window), "destroy", G_CALLBACK(exit), (void*)-1 );
}

/* Add a song name to combo box (+value) */
static int
add_song ( char * name, int value )
{
	int r = g_list_length( songs );
	
	gtk_combo_box_append_text( GTK_COMBO_BOX(combo), name );
	songs = g_list_append( songs, (void*)value );
	
	return r;
}

/* Get currently selected ID */
static int
cur_song ( void )
{
	return gtk_combo_box_get_active( GTK_COMBO_BOX(combo) );
}

/* Get currently selected value */
static int
cur_song_value ( void )
{
	int v = cur_song();
	
	if( v == -1 )
		return v;
	
	return (int)g_list_nth( songs, v )->data;
}

/* run! */
static int
run ( void )
{
	static char cmdbuff[2048];
	char * p = cmdbuff;
	
	/* Check values... */
	if( !seq_path || !rom_path )
	{
		msg( NULL, "Sequence path or ROM path invalid." );
		return FALSE;
	}
	if( cur_song() == -1 && !seq_all  )
	{
		msg( NULL, "Please choose a song to replace or all." );
		return FALSE;
	}
	
	p += sprintf( p, BIN_PATH " \"%s\" \"%s\" ", seq_path, rom_path );
	
	if( !seq_all )
		
		p += sprintf( p, "%i", cur_song() );
	
	else
		
		p += sprintf( p, "-all" );
	
	puts( cmdbuff );
	
	/* ___________________________________________________ */
	
	system( cmdbuff );
	
	/* ___________________________________________________ */
	
	return TRUE;
}

int main ( int argc, char ** argv )
{
	static char buffer[64];
	int i;
	
	gtk_init( &argc, &argv );
	
	create_window();
	
	/* Populate song list */
	for( i = 0; i < 0x6E; i++ )
	{
		sprintf( buffer, "0x%02X", i );
		
		add_song( buffer, i );
	}
	
	gtk_main();
	
	return 0;
}

