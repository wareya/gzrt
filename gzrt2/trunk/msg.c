/**************************
* GZRT - Message handling *
**************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "gzrt.h"
#include "ansi-color.h"
#include "msg.h"

/* ----------------------------------------------
   List of debug messages
   ---------------------------------------------- */
struct DebugMesg
{
    char * mesg;
    struct timeval tv;
    struct DebugMesg * next;
};

static struct DebugMesg dmesgbase;

/* ----------------------------------------------
   Print a debug status message
   ---------------------------------------------- */
   
void gzrt_kappend ( char * mesg )
{
    static struct DebugMesg * last;
    struct DebugMesg * ptr = &dmesgbase;
	static volatile int lock;
	
	/* Locked? */
	while( lock );
	lock = 1;
    
    /* Last already set? */
    if( last )
        ptr = last;
    
    /* Find end of list */
    while( ptr->next )
        ptr = ptr->next;
    
    /* New node? */
    if( ptr->mesg )
    {
        /* Allocate memory */
        ptr->next = calloc( sizeof(struct DebugMesg), 1 );
        
        /* Walk */
        ptr = ptr->next;
    }
    
    /* Store latest */
    if( last != ptr )
        last = ptr;
	
	/* Store message */
	ptr->mesg = mesg;
    
    /* Store time */
    gettimeofday( &ptr->tv, NULL );
	
	/* Print */
	fprintf( stderr, 
	  "[" ANSI_SET_FG_CYAN "%8.2f" ANSI_RESET_DEFAULT "] %s\n", 
	  timeDiff( &ptr->tv, &conf.gzrt_start ),
	  mesg );
	
	/* Unlock */
	lock = 0;
}

int gzrt_kprintf ( char * fmt, ... )
{
    va_list ap;
	char * msg;
	
	/* Prepare message */
    va_start( ap, fmt );
    msg = g_strdup_vprintf( fmt, ap );
    va_end( ap );
	
	/* Append message */
	gzrt_kappend( msg ); 
    
    /* Return length */
    return strlen(msg);
}

/* ----------------------------------------------
   Show a regular notice/warning/error message
   ---------------------------------------------- */

int gzrt_message ( int flags, char * title, char * fmt, ... )
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
	
	/* Print message to command line? */
	if( (flags & MSG_F_CMD) || (flags & MSG_F_ECHO) )
	{
		if( (flags & MSG_F_STDOUT) )
			fputs( buffer, stdout );
		else
			fputs( buffer, stderr );
		
		/* Done */
		if( (flags & MSG_F_CMD) )
			goto cleanup;
	}
	
	/* Check title */
	if( !title )
	  title = "Notice";

	/* Get message type */
	switch( flags & 0xFF000000 )
	{
		case MSG_INFO: case MSG_MESSAGE:
		 icon = GTK_STOCK_DIALOG_INFO;
		break;
		
		case MSG_ERROR:
		 icon = GTK_STOCK_DIALOG_ERROR;
		break;
		
		case MSG_WARNING:
		 icon = GTK_STOCK_DIALOG_WARNING;
		break;
		
		default:
		 icon = GTK_STOCK_DIALOG_INFO;
		break;
	}
	
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
	
	/* Fatal? */
	if( (flags & MSG_F_FATAL) )
	{
		char quit;
		
		/* Get quit code */
		quit = flags >> 16 & 0xFF;
		
		/* Exit */
		gzrt_exit( quit );
	}
	
	/* Return amount of text processed */
	return len;
}
