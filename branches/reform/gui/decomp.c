/*****************************
* Zelda 64 Decompressor Menu *
*****************************/
#include <gzrt.h>
#include <z64.h>

static GList * parents;

/* Create a new zdec window */
void gzrt_wdecompress_create ( MAINWIN *mw )
{
}

/* Close a ZDEC window */
void gzrt_wdecompress_close ( struct _gzrt_wdecompress * i )
{
}

/* Close a ZDEC window */
void gzrt_wdecompress_closed ( struct _gzrt_wdecompress * i )
{
}

/* Decompress a ROM */
void gzrt_wdecompress_routine ( struct _gzrt_wdecompress * i )
{
}

/* Show extractor window */
void gzrt_wdecomp_show ( MAINWIN * c )
{
	GtkWidget * window;
	GtkWidget * pbar;
	GtkWidget * dialog;
	int			result;
	
	/* Does the parent exist? */
	if( g_list_find( parents, c ) )
		
		/* Yep... */
		return;
		
	/* Add parent */
	parents = g_list_append( parents, c );
		
	/* Create file saving dialog */
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
	while( result = gtk_dialog_run( GTK_DIALOG(dialog) ) )
	switch( result )
	{
		/* A file has been chosen */
		case GTK_RESPONSE_ACCEPT:
		{
			FILE * h;
			char * n = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(dialog) );
			
			/* Check that file doesn't exist */
			if( (h = fopen(n, "rb")) )
			{
				gzrt_notice("Error", "File already exists.");
				fclose( h ); continue;
			}
				
			/* Check that we can write to it */
			if( !(h = fopen(n, "wb")) )
			{
				gzrt_notice("Error", "Can't write to that file!");
				continue;
			}
		}
		break;
		
		/* Cancel */
		case GTK_RESPONSE_CANCEL:
		 gtk_widget_destroy(dialog);
		 parents = g_list_remove( parents, c );
		 goto skiploop;
		break;
		
		/* Default */
		default:
		 parents = g_list_remove( parents, c );
		 goto skiploop;
	}
	
	/* Out of loop */
skiploop: ;
}
