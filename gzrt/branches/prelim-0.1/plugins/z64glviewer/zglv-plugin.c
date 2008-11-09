/****************************
* Z64 Viewer Plugin Wrapper *
****************************/
#include <gtk/gtk.h>
#include <glib.h>
#include <gzrtplugin.h>
#include <zglv-plugin.h>
#include <GL/glew.h>
#include <pthread.h>
#include <signal.h>
#include <dlist.h>
#include "SDL.h"

/* Declarations - plugin core */
int z64v_init ( const struct Functions * f );
int z64v_main ( void                       );
int z64v_load ( struct PluginTransac * t   );

/* Declarations */
GtkWidget * z64v_ep_window ( struct PluginTransac * t         );
void        z64v_begin     ( struct PluginTransac * t, u32 ep );

/* Plugin information */
struct PluginMeta gzrt_plugin_info =
{
    "Model Viewer",
    "Z64Viewer",
    "_Demo_",
    "",
    "-",
    
    /* Description */
    NULL,
    
    /* Init, menu & file action funcs */
    z64v_init, z64v_main, z64v_load
};

/* Inherited function storage */
struct Functions * GZRT_FUNC;

/* In use status */
static int in_use;

/* Thread */
pthread_t   viewer;

/*
*
 *
    *              Plugin functions start here
        *          
             *      
                 *     
                    *
                      *
                       *
                      */

/* Init function */
int z64v_init ( const struct Functions * f )
{
    /* Set function pointer */
    GZRT_FUNC = (void*)f;
    
    /* Memory */
    extern unsigned char * databuffer;
    databuffer=malloc(8*1024*1024);
    
    /* Init notice */
    DEBUG( "Z64Viewer plugin wrapper initialized." );
}

/* Call from menu */
int z64v_main ( void )
{
    DEBUG( "Not implemented.\n" );
}

/* Open a file from Zelda */
int z64v_load ( struct PluginTransac * t )
{
    /* Only once instance, please */
    if( in_use )
    {
        PLUGIN_DATA_FREE( t);
        return 0;
    }
    else
        in_use = TRUE;
    
    /* Entry point selection */
    z64v_ep_window( t );
}

/* Connect data to a widget */
#define HOOKUP( component, widget, name )                       \
    g_object_set_data_full( G_OBJECT(component), name,          \
    gtk_widget_ref(widget), (GDestroyNotify)gtk_widget_unref )
    
/* Connect generic data to widget */
#define HOOKUP_GEN( component, item, name )                     \
    g_object_set_data_full( G_OBJECT(component), name, item, NULL )

/* Lookup connected data */
#define LOOKUP( component, name )   \
    g_object_get_data( G_OBJECT(component), name )

/* Create window with a list of entry points from data */
GtkWidget * z64v_ep_window ( struct PluginTransac * t )
{
    /* File vars */
    unsigned char   * data = t->file->file;
    unsigned long     size = t->file->filesize;
    
    /* Other */
    int     i;
    char    buffer[256];
    GList * eps;            /* Entry points storing */
    
    /* Gtk stuff */
    GtkWidget * window;
    GtkWidget * hbox;
    GtkWidget * combo;
    GtkWidget * button;
    
    /* Functions... */
    extern void z64v_ep_check ( GtkWidget * window );
    
    /* Create window */
    window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    gtk_container_set_border_width( GTK_CONTAINER(window), 12 );
    gtk_window_set_resizable( GTK_WINDOW(window), FALSE );
    gtk_window_set_title( GTK_WINDOW(window), "Select an entry point" );
    
    /* Create organizer */
    hbox = gtk_hbox_new( FALSE, 8 );
    
    /* Create combo box */
    combo = gtk_combo_box_entry_new_text();
    
    /* Create button */
    button = gtk_button_new_with_label( "Ok" );
    
    /* Pack */
    gtk_container_add( GTK_CONTAINER(window), hbox );
    gtk_box_pack_start( GTK_BOX(hbox), combo, TRUE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX(hbox), button, TRUE, TRUE, 0 );
    
    /* Discover entry points */
    for( i = 0; i < size; i += 8 )
    {
        /* Next instruction */
        u64 instr = U64(&data[i]);
        
        /* Is it an entry point? */
        if( (instr >> 56 & 0xFF) == 0xE7 )
        {
            /* Append to combo box */
            sprintf( buffer, "0x%08X", i );
            gtk_combo_box_append_text( GTK_COMBO_BOX(combo), buffer );
        }
    }
    
    /* Show all */
    gtk_widget_show_all( window );
    
    /* Signal connects */
    g_signal_connect_swapped( G_OBJECT(button), "clicked", G_CALLBACK(z64v_ep_check), window );
    
    /* Hookup data */
    HOOKUP( window, combo, "selected-ep" );
    HOOKUP_GEN( window, t, "transac" );
    
    /* Return it */
    return window;
}

/* Check the chosen entry point */
void z64v_ep_check ( GtkWidget * window )
{
    GtkWidget * box = LOOKUP( window, "selected-ep" );
    char * text = gtk_combo_box_get_active_text( GTK_COMBO_BOX(box) );
    struct PluginTransac * t = LOOKUP( window, "transac" );
    u32 addr;
    
    /* Check */
    if( !sscanf( text, "%X", &addr ) || addr >= t->file->filesize - 8 )
    {
        NOTICE( "Error", "Invalid entry point selected." );
        return;
    }
    
    /* Destroy the window */
    gtk_widget_destroy( window );
    
    /* Begin drawing the model */
    z64v_begin( t, addr );
}

/* Show the model viewer */
void z64v_begin ( struct PluginTransac * t, u32 ep )
{
    GtkWidget * window;
    GtkWidget * button;
    
    /* Passing data to viewer */
    static struct Data  file;
    file.data = t->file->file;
    file.size = t->file->filesize;
    file.ep   = ep;
    
    /* Funct */
    extern void draw_from_data ( struct Data * k );
    extern void kill_viewer    ( GtkWidget * win );
    
    /* Create window */
    window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    gtk_container_set_border_width( GTK_CONTAINER(window), 12 );
    gtk_window_set_resizable( GTK_WINDOW(window), FALSE );
    gtk_window_set_title( GTK_WINDOW(window), " " );
    
    /* Create button */
    button = gtk_button_new_with_label( "Close Z64Viewer" );
    
    /* Attach & show */
    gtk_container_add( GTK_CONTAINER(window), button );
    gtk_widget_show_all( window );
    HOOKUP_GEN( window, t, "data" );
    
    /* Create new thread with viewer */
    pthread_create( &viewer, NULL, draw_from_data, &file );
    
    /* Hookup signal  */
    g_signal_connect_swapped( G_OBJECT(button), "clicked", G_CALLBACK(kill_viewer), window );
}

/* Kill the viewer thread */
void kill_viewer ( GtkWidget * win )
{
    extern int __quit;
    extern int __quited;
    struct PluginTransac * t = LOOKUP( win, "data" );
    
    __quit = 1;
    
    while( !__quited )
        g_usleep( 1000000 * 0.25 );
    
    PLUGIN_DATA_FREE( t );
    gtk_widget_destroy( win );
    
    in_use = FALSE;
    __quited = 0;
    __quit = 0;
}
