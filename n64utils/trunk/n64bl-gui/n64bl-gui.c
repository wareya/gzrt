/*
*   Nintendo 64 Binary Loader GTK frontend
*   Copyright (c) 2009  ZZT32 [mbr@64.vg]
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public
*   Licence along with this program; if not, write to the Free
*   Software Foundation, Inc., 51 Franklin Street, Fifth Floor, 
*   Boston, MA  02110-1301, USA
*/

#include <stdio.h>
#include <gtk/gtk.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* Binary name */
#if defined( __WIN32__ ) || defined( __CYGWIN__ )
# define BINARY_NAME    "./n64bl.exe"
#else
# define BINARY_NAME    "./n64bl"
#endif

#define ICONS   "res/"


/* ----------------------------------------------
   Local variables
   ---------------------------------------------- */

/* Configuration */
static char * rom_name;
static char * bin_name;
static unsigned rom_addr = -1;
static unsigned entry_point = -1;

/* GTK items */
static GtkWidget * window;
static GtkWidget * entry_rom_file;
static GtkWidget * entry_bin_file;
static GtkWidget * entry_rom_addr;
static GtkWidget * entry_ram_ep;
static GtkWidget * image_rom_addr;
static GtkWidget * image_ram_ep;
static GtkWidget * button_insert;


/* ----------------------------------------------
   Invoke the binary
   ---------------------------------------------- */

#if !defined( __WIN32__ ) || defined( __CYGWIN__ )
 /* Call function for *nix environments */
 int n64bl ( void )
 {
     int status, ret;
     char * ep, * romaddr;
     
     /* Fork a new process */
     if( !fork() )
     {
         /* Text representation */
         romaddr = g_strdup_printf( "0x%08X", rom_addr );
         ep = g_strdup_printf( "0x%08X", entry_point );
         
         /* Execute the binary */
         ret = execl( BINARY_NAME, BINARY_NAME, rom_name, bin_name, romaddr, ep, NULL );
         
         /* Something's wrong! */
         if( ret == -1 )
         {
             g_print( "execl: %s\n", strerror(errno) );
             exit( -1 );
         }
     }
     
     /* Wait for program to finish... */
     wait( &status );
     
     /* Return the return value of the program */
     return WEXITSTATUS(status);
 }
#else
 /* Call function for Windows environments */
 int n64bl ( void )
 {
     static char buffer[2048];
     
     /* Prepare command */
     snprintf( buffer, BINARY_NAME " %s %s 0x%08X 0x%08X", rom_name, bin_name,
     romaddr, ep );
     
     /* Execute command */
     return system( buffer );
 }
#endif


/* ----------------------------------------------
   GTK Functions
   ---------------------------------------------- */

/* Display a dialog */
void msg ( char * title, char * fmt, ... )
{
    GtkWidget * dialog;
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

/* Get a filename */
void get_file ( char * title, char ** dest )
{
    GtkWidget * dialog = gtk_file_chooser_dialog_new
    ( 
        title, NULL,
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
        
         *dest = strdup(gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(dialog) ));
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

/* Get ROM file... */
static void get_rom_file ( void )
{
    get_file( "Please choose a ROM", &rom_name );
    
    /* Set? */
    if( rom_name )
    {
        gtk_entry_set_text( GTK_ENTRY(entry_rom_file), rom_name );
    }
}

/* Get binary file... */
static void get_bin_file ( void )
{
    get_file( "Please choose a binary", &bin_name );
    
    /* Set? */
    if( bin_name )
    {
        gtk_entry_set_text( GTK_ENTRY(entry_bin_file), bin_name );
    }
}

/* Check for hex number */
int check_hex ( char * s )
{
    if( !*s )
        return FALSE;
    
    if( !strncmp( s, "0x", 2 ) )
        s += 2;
    
    if( !*s )
        return FALSE;
    
    while( *s )
    {
        if( !(isdigit(*s) || (tolower(*s) >= 'a' && tolower(*s) <= 'f')) )
            return FALSE;
        s++;
    }
    
    return TRUE;
}

/* Check an address & store */
static int check_address ( char * text, unsigned * dest, GtkWidget * image )
{
    if( check_hex(text) )
    {
        gtk_image_set_from_stock( GTK_IMAGE(image), GTK_STOCK_YES, GTK_ICON_SIZE_BUTTON );
        sscanf( text, "%x", dest );
    }
    else
    {
        gtk_image_set_from_stock( GTK_IMAGE(image), GTK_STOCK_NO, GTK_ICON_SIZE_BUTTON );
        *dest = -1;
    }
}
        

/* Handle input */
static int check_ram_ep ( GtkEditable * entry, unsigned * dest )
{
    char * text;
    int amnt, tmp;
    
    text = gtk_editable_get_chars( entry, 0, -1 );
    check_address( text, dest, image_ram_ep );
    g_free( text );
    
    return FALSE;
}

/* Handle input */
static int check_rom_addr ( GtkEditable * entry, unsigned * dest )
{
    char * text;
    int amnt, tmp;
    
    text = gtk_editable_get_chars( entry, 0, -1 );
    check_address( text, dest, image_rom_addr );
    g_free( text );
    
    return FALSE;
}

/* Insert */
static void insert ( void )
{
    int ret;
    
    /* Check... */
    if( !rom_name )
    {
        msg( "Error", "You need to choose a ROM." );
        return;
    }
    if( !bin_name )
    {
        msg( "Error", "You need to choose a binary to inject." );
        return;
    }
    if( (int)rom_addr == -1 )
    {
        msg( "Error", "Destination address in ROM invalid. 0x%08X", rom_addr );
        return;
    }
    if( (int)entry_point == -1 )
    {
        msg( "Error", "RAM entry point is invalid." );
        return;
    }
    if( rom_addr % 8 )
    {
        msg( "Error", "ROM address not aligned to 8 bytes." );
        return;
    }
    if( entry_point % 8 )
    {
        msg( "Error", "RAM entry point not aligned to 8 bytes." );
        return;
    }
    
    /* Gogo */
    ret = n64bl();
    
    if( ret )
    {
        msg( "Error", BINARY_NAME "failed with error code %i", ret );
    }
    else
    {
        msg( "Done", "Operation completed successfully." );
    }
}

/* Create the window */
void create_window ( void )
{
    GtkWidget * vbox;
    GtkWidget * hbox;
    GtkWidget * label;
    GtkWidget * button;
    GtkWidget * align;
    GList * gicons = NULL;
    PangoFontDescription * desc;
    int i;
    static char * icons[] =
    {
        ICONS "icon16x16.png",
        ICONS "icon32x32.png",
        ICONS "icon64x64.png",
        NULL
    };
    for( i = 0; icons[i]; i++ )
    {
        GdkPixbuf *g = gdk_pixbuf_new_from_file(icons[i], NULL);
        gicons = g_list_append( gicons, g );
    }
    gtk_window_set_default_icon_list( gicons );
    #ifdef WIN32
     desc = pango_font_description_from_string( "Courier New" );
    #else
     desc = pango_font_description_from_string( "Monospace" );
    #endif
    
    window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    vbox = gtk_vbox_new( FALSE, 4 );
    
    gtk_window_set_policy( GTK_WINDOW(window), FALSE, FALSE, FALSE );
    g_signal_connect_swapped( G_OBJECT(window), "destroy", G_CALLBACK(exit), (void*)-1 );
    gtk_container_add( GTK_CONTAINER(window), vbox );
    gtk_container_set_border_width( GTK_CONTAINER(window), 8 );
    gtk_window_set_title( GTK_WINDOW(window), "N64 Binary Loader" );
    
    /* Create window items */
    label = gtk_label_new( "ROM filename:" );
    gtk_misc_set_alignment( GTK_MISC(label), 0.0, 0.5 );
    gtk_box_pack_start( GTK_BOX(vbox), label, FALSE, TRUE, 0 );
    hbox = gtk_hbox_new( FALSE, 4 );
    gtk_box_pack_start( GTK_BOX(vbox), hbox, FALSE, TRUE, 0 );
    entry_rom_file = gtk_entry_new( );
    button = gtk_button_new_with_label( "Browse" );
    g_signal_connect_swapped( G_OBJECT(button), "clicked", G_CALLBACK(get_rom_file), NULL );
    gtk_box_pack_start( GTK_BOX(hbox), entry_rom_file, TRUE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX(hbox), button, FALSE, FALSE, 0 );
    label = gtk_label_new( "Binary filename:" );
    gtk_misc_set_alignment( GTK_MISC(label), 0.0, 0.5 );
    gtk_box_pack_start( GTK_BOX(vbox), label, FALSE, TRUE, 0 );
    hbox = gtk_hbox_new( FALSE, 4 );
    gtk_box_pack_start( GTK_BOX(vbox), hbox, FALSE, TRUE, 0 );
    entry_bin_file = gtk_entry_new( );
    button = gtk_button_new_with_label( "Browse" );
    g_signal_connect_swapped( G_OBJECT(button), "clicked", G_CALLBACK(get_bin_file), NULL );
    gtk_box_pack_start( GTK_BOX(hbox), entry_bin_file, TRUE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX(hbox), button, FALSE, FALSE, 0 );
    
    label = gtk_label_new( "ROM address:" );
    gtk_misc_set_alignment( GTK_MISC(label), 0.0, 0.5 );
    gtk_box_pack_start( GTK_BOX(vbox), label, FALSE, TRUE, 0 );
    hbox = gtk_hbox_new( FALSE, 4 );
    entry_rom_addr = gtk_entry_new( );
    gtk_widget_modify_font( entry_rom_addr, desc );
    image_rom_addr = gtk_image_new_from_stock( GTK_STOCK_NO, GTK_ICON_SIZE_BUTTON );
    gtk_box_pack_start( GTK_BOX(hbox), entry_rom_addr, TRUE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX(hbox), image_rom_addr, FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX(vbox), hbox, FALSE, TRUE, 0 );
    g_signal_connect( G_OBJECT(entry_rom_addr), "changed", G_CALLBACK(check_rom_addr), &rom_addr );
    
    
    label = gtk_label_new( "RAM entry point:" );
    gtk_misc_set_alignment( GTK_MISC(label), 0.0, 0.5 );
    gtk_box_pack_start( GTK_BOX(vbox), label, FALSE, TRUE, 0 );
    hbox = gtk_hbox_new( FALSE, 4 );
    entry_ram_ep = gtk_entry_new( );
    gtk_widget_modify_font( entry_ram_ep, desc );
    image_ram_ep = gtk_image_new_from_stock( GTK_STOCK_NO, GTK_ICON_SIZE_BUTTON );
    gtk_box_pack_start( GTK_BOX(hbox), entry_ram_ep, TRUE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX(hbox), image_ram_ep, FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX(vbox), hbox, FALSE, TRUE, 0 );
    g_signal_connect( G_OBJECT(entry_ram_ep), "changed", G_CALLBACK(check_ram_ep), &entry_point );
    
    align = gtk_alignment_new( 0.0, 1.0, 0.0, 0.0 );
    gtk_alignment_set_padding( GTK_ALIGNMENT(align), 8, 0, 0, 0 );
    hbox = gtk_hbox_new( TRUE, 4 );
    gtk_container_add( GTK_CONTAINER(align), hbox );
    button_insert = button = gtk_button_new_with_label( "  Insert  " );
    g_signal_connect( G_OBJECT(button), "clicked", G_CALLBACK(insert), NULL );
    gtk_box_pack_start( GTK_BOX(hbox), button, FALSE, TRUE, 0 );
    button = gtk_button_new_with_label( "Quit" );
    g_signal_connect_swapped( G_OBJECT(button), "clicked", G_CALLBACK(exit), (void*)-1 );
    gtk_box_pack_start( GTK_BOX(hbox), button, FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX(vbox), align, TRUE, TRUE, 0 );
    
    gtk_widget_show_all( window );
}

int main ( int argc, char ** argv )
{
    gtk_init( &argc, &argv );
    
    create_window();
    
    gtk_main();
    
    return 0;
}

