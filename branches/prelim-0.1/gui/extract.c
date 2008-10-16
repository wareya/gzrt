/************************
* ROM Extraction Window *
************************/
#include <gzrt.h>
#include <string.h>

/* Connect data to a widget */
#define HOOKUP( component, widget, name ) \
    g_object_set_data( G_OBJECT(component), name, widget )

/* Lookup connected data */
#define LOOKUP( component, name ) \
    g_object_get_data( G_OBJECT(component), name )
    
/* Options */
typedef struct
{
    gboolean    remove_prefix;
    gboolean    add_fid_prefix;
}
OPT;

/* List of parents */
static GList * parents;
static OPT     options;

/* Set options */
static void opt_1 ( void )
{
    options.remove_prefix = (~options.remove_prefix) & 1;
}
static void opt_2 ( void )
{
    options.add_fid_prefix = (~options.add_fid_prefix) & 1;
}

/* Close extractor window */
static void wclose ( GtkWidget * w )
{
    MAINWIN * p = LOOKUP( w, "parent" );
    
    parents = g_list_remove( parents, p );
    
    gtk_widget_destroy( w );
}

/* Extractor window closed */
static void wclosed ( MAINWIN * c )
{
    parents = g_list_remove( parents, c );
}

/* Set progress bar text */
static void pbarset ( GtkWidget * p, double percent, char * fmt, ... )
{
    va_list  ap;
    char     buffer[128];
    
    va_start( ap, fmt );
    vsnprintf( buffer, sizeof(buffer), fmt, ap );
    va_end( ap );
    
    gtk_progress_bar_set_fraction( GTK_PROGRESS_BAR(p), percent );
    gtk_progress_bar_set_text( GTK_PROGRESS_BAR(p), buffer );
    
    while( gtk_events_pending() )
        gtk_main_iteration();
}

/* Find a destination directory */
static void finddest ( GtkWindow * w )
{
    GtkWidget * entry = LOOKUP( w, "opt-dest" );
    GtkWidget * dialog;
    FILE      * h;
    int         result;
    
    /* Create dir choosing dialog */
    dialog = gtk_file_chooser_dialog_new
    ( 
        "Choose a destination", NULL,
        GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, 
        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
        GTK_STOCK_OPEN,   GTK_RESPONSE_ACCEPT, 
        NULL
    );
    gtk_widget_show_all( dialog );
    
    /* Run the dialog and fetch the result */
    while( (result = gtk_dialog_run( GTK_DIALOG(dialog) )) )
    switch( result )
    {
        /* A dir has been chosen */
        case GTK_RESPONSE_ACCEPT:
        {
            /* Try creating a file */
            char pathbuff[2048];
            snprintf( pathbuff, sizeof(pathbuff), "%s" GZRT_SLASH "__gzrt_tmp", gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(dialog) ) );
            if( !(h = fopen(pathbuff, "wb") ) )
                gzrt_notice("Error", "Can't write to this directory!" );
            else {
                fclose( h );
                gtk_entry_set_text( GTK_ENTRY(entry), gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(dialog) ) );
                gtk_widget_destroy( dialog );
                unlink( pathbuff );
                goto endloop;
            }
        }
        break;
        
        /* Cancel */
        case GTK_RESPONSE_REJECT:
          gtk_widget_destroy( dialog );
          goto endloop;
        break;
        
        /* Default */
        default: 
          gtk_widget_destroy( dialog );
          goto endloop;
    }
    
endloop: ;
}

/* Extract handler */
static void extract ( GtkWidget * w )
{
    MAINWIN * c = LOOKUP( w, "parent" );
    unsigned char * buffer;
    FILE * out;
    GtkWidget * field = LOOKUP( w, "opt-dest" );
    const char * dest = gtk_entry_get_text( GTK_ENTRY(field) );
    char name[2048];
    int i;
    
    /* GTK elements */
    GtkWidget * window;
    GtkWidget * pbar;
    
    /* Create window */
    window = gtk_window_new( GTK_WINDOW_POPUP );
    gtk_widget_set_size_request( window, 400, 30 );
    gtk_window_set_title( GTK_WINDOW(window), " " );
    gtk_window_set_position( GTK_WINDOW(window), GTK_WIN_POS_CENTER_ALWAYS );
    gtk_window_set_keep_above( GTK_WINDOW(window), TRUE );
    gtk_window_set_modal( GTK_WINDOW(window), TRUE );
    
    /* Create progress window */
    pbar = gtk_progress_bar_new();
    gtk_container_add( GTK_CONTAINER(window), pbar );
    gtk_widget_show_all( window );
    
    /* Set initial */
    pbarset( pbar, 0.0, "%.2f%%", 0.0 );
    
    /* Create file buffer */
    buffer = gzrt_malloc( 8 * 1024 * 1024 );
    
    /* Begin */
    for( i = 0; i < z64fs_entries(c->z); i++ )
    {
        char * nptr = name;
        int type;
        
        /* Does this file exist? */
        if( !ZFileExists(c->z, i) )
            continue;
        
        /* Read the file */
        z64fs_read_file( c->z, i, buffer );
        
        /* Detect type */
        if( c->t )
            type = z64detect_name( z64nt_filename(c->t, i) );
        else
            type = z64detect_raw( buffer, ZFileVirtSize(c->z, i) );
        
        /* Prepare filename */
        nptr += sprintf( nptr, "%s" GZRT_SLASH, dest );
        if( options.add_fid_prefix )
            nptr += sprintf( nptr, "%04u - ", i );
        if( c->t )
            nptr += sprintf( nptr, "%s", z64nt_filename(c->t, i) );
        else
            nptr += sprintf( nptr, "0x%08X - 0x%08X", ZFileVirtStart(c->z, i), ZFileVirtEnd(c->z, i) );
        nptr += sprintf( nptr, ".%s", z64detect_fileext(type) );
        
        g_print( "%s\n", name );
        
        /* Write it */
        out = fopen( name, "wb" );
        fwrite( buffer, 1, ZFileVirtSize(c->z, i), out );
        fclose( out );
        
        /* Update progress bar */
        if( !((i + 1) % (z64fs_entries(c->z) / 32)) )
            pbarset( pbar, (double)i / z64fs_entries(c->z), "%.2f%%", (double)i / z64fs_entries(c->z) * 100.0 );
    }
    
    /* Free resources */
    gzrt_free( buffer );
    
    /* Last progress bar update */
    pbarset( pbar, 1.0, "%.2f%%", 100.0 );
    
    /* Display finished notice */
    gzrt_notice( "Notice", "Extraction finished." );
    
    /* Destroy window */
    gtk_widget_destroy( window );
}

/* Initialize the window */
void gzrt_wextract_show ( MAINWIN * c )
{
    FILE          * h;
    int             result;
    
    /* GTK Elements */
    GtkWidget * window;
    GtkWidget * pbar;
    GtkWidget * dialog;
    GtkWidget * button;
    GtkWidget * expander;
    GtkWidget * entry;
    GtkWidget * hbox;
    GtkWidget * vbox;
    GtkWidget * align;
    GtkWidget * label;
    GtkWidget * optvbox;
    GtkWidget * check;
    
    /* Does this parent already exist? */
    if( g_list_find( parents, c ) )
        
        /* Yeah */
        return;
        
    /* Clear options */
    memset( &options, 0, sizeof(options) );
    
    /* Add parent */
    parents = g_list_append( parents, c );
    
    /* Create window */
    window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    gtk_window_set_title( GTK_WINDOW(window), "Resource Extractor" );
    gtk_window_set_position( GTK_WINDOW(window), GTK_WIN_POS_CENTER );
    gtk_window_set_modal( GTK_WINDOW(window), TRUE );
    gtk_window_set_resizable( GTK_WINDOW(window), FALSE );
    gtk_container_set_border_width( GTK_CONTAINER(window), 12 );
    
    /* Create vbox */
    vbox = gtk_vbox_new( FALSE, 8 );
    
    /* Create file destination row */
    hbox   = gtk_hbox_new( FALSE, 8 );
    label  = gtk_label_new( "Output directory:" );
    entry  = gtk_entry_new( );
    button = gtk_button_new_with_label( "Browse..." );
    
    /* Pack */
    gtk_box_pack_start( GTK_BOX(hbox), label,  FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX(hbox), entry,  FALSE, TRUE,  0 );
    gtk_box_pack_start( GTK_BOX(hbox), button, FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX(vbox), hbox,   FALSE, TRUE,  0 );
    
    /* Hookup */
    HOOKUP( window, entry, "opt-dest" );
    g_signal_connect_swapped( G_OBJECT(button), "clicked", G_CALLBACK(finddest), window );
    
    /* Create expander */
    expander = gtk_expander_new( "Options" );
    
    /* Create contents */
    align = gtk_alignment_new( 0.5f, 0.5f, 1.0f, 1.0f );
    optvbox = gtk_vbox_new( FALSE, 4 );
    check = gtk_check_button_new_with_label( "Strip filename prefixes" );
    g_signal_connect_swapped( G_OBJECT(check), "clicked", G_CALLBACK(opt_1), NULL );
    gtk_box_pack_start( GTK_BOX(optvbox), check, FALSE, TRUE, 0 );
    check = gtk_check_button_new_with_label( "Add file ID as prefix" );
    g_signal_connect_swapped( G_OBJECT(check), "clicked", G_CALLBACK(opt_2), NULL );
    gtk_box_pack_start( GTK_BOX(optvbox), check, FALSE, TRUE, 0 );
    
    /* Pack & set attribs */
    gtk_alignment_set_padding( GTK_ALIGNMENT(align), 0, 0, 12, 0 );
    gtk_container_add( GTK_CONTAINER(align), optvbox );
    gtk_container_add( GTK_CONTAINER(expander), align );
    gtk_box_pack_start( GTK_BOX(vbox), expander, FALSE, TRUE, 0 );
    
    /* Create buttons */
    align  = gtk_alignment_new( 0.0f, 0.5f, 0.2f, 1.0f );
    hbox   = gtk_hbox_new( TRUE, 8 );
    button = gtk_button_new_with_label( "Close" );
    g_signal_connect_swapped( G_OBJECT(button), "clicked", G_CALLBACK(wclose), window );
    gtk_box_pack_start( GTK_BOX(hbox), button, FALSE, TRUE, 0 );
    button = gtk_button_new_with_label( "Extract" );
    g_signal_connect_swapped( G_OBJECT(button), "clicked", G_CALLBACK(extract), window );
    gtk_box_pack_start( GTK_BOX(hbox), button, FALSE, TRUE, 0 );
    
    /* Pack */
    gtk_container_add( GTK_CONTAINER(align), hbox );
    gtk_box_pack_start( GTK_BOX(vbox), align, FALSE, TRUE, 0 ); 
    
    /* Hookup any additional data */
    HOOKUP( window, c, "parent" );
    
    /* Add to window and show */
    g_signal_connect_swapped( G_OBJECT(window), "destroy", G_CALLBACK(wclosed), c );
    gtk_container_add( GTK_CONTAINER(window), vbox );
    gtk_widget_show_all( window );
}

