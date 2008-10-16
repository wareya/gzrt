/*****************************
* File Selection GUI Element *
*****************************/
#include <gzrt.h>

/* Structure */
static struct _gzrt_wfilesel gzrt_wfilesel;

/* Shorthand */
static struct _gzrt_wfilesel * w = &gzrt_wfilesel;

/* Vars */
static int status;

/* Open a file select box */
void gzrt_wfilesel_show ( void )
{
    char * f;
    N64Rom * ctx;
    GtkWidget * dialog = gtk_file_chooser_dialog_new
    ( 
        "Load ROM", NULL,
        GTK_FILE_CHOOSER_ACTION_OPEN, 
        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
        GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, 
        NULL
    );
    
    /* Wait for filename */
    while( 1 ) 
    {
        switch( gtk_dialog_run( GTK_DIALOG(dialog) ) )
        {
            /* Yes */
            case GTK_RESPONSE_ACCEPT:
              /* Create main window */
              if( (ctx = n64rom_load(gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(dialog) ))) )
              {
                if( gzrt_wmain_create_new( ctx ) )
                {
                    gtk_widget_destroy( dialog );
                    goto quitloop;
                }
              }
              else
                /* Display error */
                gzrt_notice( "Error occured!", n64rom_error() );
            break;
            
            /* Cancel */
            default:
              if( gzrt_wmain_count() == 0 )
                gzrt_gui_quit();
              else {
                gtk_widget_destroy( dialog );
                goto quitloop;
              }
        }
    }
    
    quitloop: ;
}

/* File selected. Perform checks */
void gzrt_wfilesel_check ( GtkWidget *wd, GtkFileSelection *fs )
{
}

/* Window closed - which action? */
void gzrt_wfilesel_cancel ( void )
{
}

/* Get a filename */
const char * gzrt_wfilesel_get ( void )
{
    static char ret[256];
    
    GtkWidget * dialog = gtk_file_chooser_dialog_new
    ( 
        "Load ROM", NULL,
        GTK_FILE_CHOOSER_ACTION_OPEN, 
        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
        GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, 
        NULL
    );
    
    /* Wait for filename */
    switch( gtk_dialog_run( GTK_DIALOG(dialog) ) )
    {
        /* Yes */
        case GTK_RESPONSE_ACCEPT:
         strcpy( ret, gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(dialog) ) );
         gtk_widget_destroy( dialog );
         return ret;
        break;
        
        /* None given? Cancel */
        default:
         return NULL;
    }
}
