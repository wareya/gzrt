/**********************
* Progress bar window *
**********************/
#include <gzrt.h>

/* Create a new progress bar */
PROGRESS * gzrt_wpbar_new ( void )
{
	PROGRESS * w = malloc( sizeof(PROGRESS) );
	
	/* Create new window */
	w->window = gtk_window_new( GTK_WINDOW_POPUP);
	
	/* Set attributes */
	gtk_window_set_position( GTK_WINDOW(w->window), GTK_WIN_POS_CENTER_ALWAYS );
	gtk_widget_set_size_request( w->window, GZRT_WPBAR_W, GZRT_WPBAR_H );
	gtk_window_set_title( GTK_WINDOW(w->window), GZRT_WPBAR_NAME );
	gtk_window_set_keep_above( GTK_WINDOW(w->window), TRUE );
	
	/* Create progress bar */
	w->progbar = gtk_progress_bar_new();
	
	/* Add progress bar & show */
	gtk_container_add( GTK_CONTAINER(w->window), w->progbar );
	
	return w;
}

/* Progress bar hide */
void gzrt_wpbar_hide ( PROGRESS * w )
{
	gtk_widget_hide( w->window );
}

/* Progress bar show */
void gzrt_wpbar_show ( PROGRESS * w )
{
	gtk_widget_show_all( w->window );
	gtk_window_set_keep_above( GTK_WINDOW(w->window), TRUE );
	gtk_window_present( GTK_WINDOW(w->window) );
}

/* Close progress bar window */
void gzrt_wpbar_close ( PROGRESS * w )
{
	gtk_widget_destroy( w->window );
	free( w );
}

/* Update percentage */
void gzrt_wpbar_set ( PROGRESS * w, double p )
{
	gtk_progress_bar_set_fraction( GTK_PROGRESS_BAR(w->progbar), p );
}

/* Set text */
void gzrt_wpbar_text ( PROGRESS * w, char *fmt, ... )
{
	va_list ap;
	char buffer[512];
	int len;
	va_start( ap, fmt );
	len = vsnprintf( buffer, sizeof(buffer) - 1, fmt, ap );
	va_end( ap );
	gtk_progress_bar_set_text( GTK_PROGRESS_BAR(w->progbar), buffer );
}

