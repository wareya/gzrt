/********************************
* GZRT Graphical User Interface *
********************************/
#include <stdio.h>
#include <gtk/gtk.h>
#include "gzrt.h"
#include "gui.h"

/* The startup window */
static GtkWidget * startup_window;

/* Set the window icon sets */
static void gzrt_gui_icons_init ( void )
{
    int i;
    GList * list = NULL;
    static const char * icons[] =
    {
        GZRT_GUI_RES_PATH "/ico16x16.png",
        GZRT_GUI_RES_PATH "/ico32x32.png",
        GZRT_GUI_RES_PATH "/ico64x64.png",
        NULL
    };
    
    /* Create GList of icons */
    for( i = 0; icons[i]; i++ )
    {
        /* Load icons */
        GdkPixbuf * g = gdk_pixbuf_new_from_file( icons[i], NULL );
        
        /* Append to list */
        list = g_list_append( list, g );
    }
    
    /* Set default icon */
    gtk_window_set_default_icon_list( list );
}

/* Create the quickstart menu */
static GtkWidget *
gzrt_gui_create_quickstart_menu ( void )
{
    GtkWidget * menu;
    GtkWidget * image;
    GtkWidget * item;
    int i;
    
    static const struct 
    {
        char * name;
        char * stock;
        void * handler;
    } 
    menu_spec[] = 
    {
        { "_CRC Tool",         GTK_STOCK_APPLY,        NULL },
        { "D_ecompress a ROM", GTK_STOCK_CONVERT,      NULL },
        { "E_xtract a ROM",    GTK_STOCK_DND_MULTIPLE, NULL },
        { "ROM _Byteswapper",  GTK_STOCK_REFRESH,      NULL },
        { NULL }
    };
    
    /* Create menu */
    menu = gtk_menu_new();
    
    /* Create items */
    for( i = 0; menu_spec[i].name; i++ )
    {
        /* Create menu item */
        item = gtk_image_menu_item_new_with_mnemonic( menu_spec[i].name );
        
        /* Stock image? */
        if( menu_spec[i].stock )
        {
            /* Get stock image */
            image = gtk_image_new_from_stock( menu_spec[i].stock, GTK_ICON_SIZE_LARGE_TOOLBAR );
            
            /* Set image */
            gtk_image_menu_item_set_image( GTK_IMAGE_MENU_ITEM(item), image );
        }
        
        /* Handler? */
        if( menu_spec[i].handler )
            g_signal_connect( G_OBJECT(item), "activated", G_CALLBACK(menu_spec[i].handler), NULL );
        
        /* Append to menu */
        gtk_menu_shell_append( GTK_MENU_SHELL(menu), item );
    }
    
    /* Make everything visible */
    gtk_widget_show_all( menu );
    
    /* Return it */
    return menu;
}

/* Quickstart button event handler */
static gboolean
gzrt_gui_qs_handler ( GtkWidget * b, GdkEventButton * event, GtkWidget * menu )
{
    /* Check event type... */
    if( event->type == GDK_BUTTON_PRESS )
        if( event->button == 1 )
        {
            /* Show menu */
            gtk_menu_popup( GTK_MENU(menu), NULL, NULL, NULL, NULL, event->button, event->time );
        }

    return FALSE;
}

/* Create the start screen */
void gzrt_gui_show_startup_screen ( void )
{
    GtkWidget * window;
    GtkWidget * image;
    GtkWidget * vbox;
    GtkWidget * hbox;
    GtkWidget * button1;
    GtkWidget * button2;
    GtkWidget * button3;
    GtkWidget * button4;
    GtkWidget * qs;
    
    /* Already active? */
    if( startup_window )
      return;
    
    /* Quickstart menu */
    qs = gzrt_gui_create_quickstart_menu();
    
    /* Create window */
    window  = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    image   = gtk_image_new_from_file( GZRT_GUI_RES_LOGO );
    vbox    = gtk_vbox_new( FALSE, 8 );
    hbox    = gtk_hbox_new( TRUE, 8 );
    button1 = gtk_button_new_with_mnemonic( "_Open ROM" );
    button2 = gtk_button_new_with_mnemonic( "_Quickstart" );
    button3 = gtk_button_new_with_mnemonic( "_About" );
    button4 = gtk_button_new_with_mnemonic( "E_xit" );
    gtk_widget_set_size_request( button1, -1, 35 );
    gtk_window_set_title( GTK_WINDOW(window), GZRT_GUI_WINDOW_TITLE );
    gtk_window_set_position( GTK_WINDOW(window), GTK_WIN_POS_CENTER_ALWAYS );
    gtk_window_set_resizable( GTK_WINDOW(window), FALSE );
    gtk_window_set_deletable( GTK_WINDOW(window), FALSE );
    gtk_container_set_border_width( GTK_CONTAINER(window), 10 );
    gtk_container_add( GTK_CONTAINER(window), vbox );
    gtk_box_pack_start( GTK_BOX(vbox), image, FALSE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX(vbox), hbox, TRUE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX(hbox), button1, TRUE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX(hbox), button2, TRUE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX(hbox), button3, TRUE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX(hbox), button4, TRUE, TRUE, 0 );
    gtk_widget_show_all( window );
    
    /* Set up signals */
    g_signal_connect_swapped( G_OBJECT(button4), "clicked", G_CALLBACK(gzrt_exit), (void*)0 );
    g_signal_connect( G_OBJECT(button3), "clicked", G_CALLBACK(gzrt_gui_about_show), NULL );
    g_signal_connect( G_OBJECT(button2), "event", G_CALLBACK(gzrt_gui_qs_handler), qs );
    g_signal_connect( G_OBJECT(button1), "clicked", G_CALLBACK(gzrt_gui_open_rom), NULL );
    
    /* Store window pointer */
    startup_window = window;
}

/* Close the startup screen */
void gzrt_gui_close_startup_screen ( void )
{
    if( !startup_window )
        return;
    
    gtk_widget_destroy( startup_window );
    startup_window = NULL;
}

/* Create the GUI */
void gzrt_gui_init ( void )
{
    /* Initialize icons */
    gzrt_gui_icons_init();
    
    /* Show start screen */
    gzrt_gui_show_startup_screen();
}
