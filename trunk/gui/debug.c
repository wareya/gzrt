/************************
* Debug Box GUI Element *
************************/
#include <gzrt.h>

/* Structure */
static struct _gzrt_wdebug gzrt_wdebug;

/* Pointer */
static struct _gzrt_wdebug * w = &gzrt_wdebug;

/* Initialize the debug box and show it */
void gzrt_gui_debug_create ( void )
{
	/* Create a new window */
	w->window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	gtk_widget_set_size_request( w->window, GZRT_WDEBUG_W, GZRT_WDEBUG_H );
	gtk_window_set_title( GTK_WINDOW(w->window), GZRT_WDEBUG_T );
	
	/* Set icon */
	gtk_window_set_icon_from_file( GTK_WINDOW(w->window), GZRT_WDEBUG_ICO, NULL );
	
	/* On destroyed, remake */
	g_signal_connect( G_OBJECT(w->window), "destroy", gzrt_gui_debug_create, NULL );
	
	/* Create text buffer */
	if( !w->buffer )
		w->buffer = (GtkWidget*)gtk_text_buffer_new( NULL );
	
	/* Set up text box */
	w->text = gtk_text_view_new_with_buffer( GTK_TEXT_BUFFER(w->buffer) );
	gtk_text_view_set_editable( GTK_TEXT_VIEW(w->text), FALSE );
	gtk_text_view_set_wrap_mode( GTK_TEXT_VIEW(w->text), GTK_WRAP_NONE );

	/* Font */
	PangoFontDescription *font_desc;
	font_desc = pango_font_description_from_string( "Courier 10" );
	gtk_widget_modify_font( w->text, font_desc );
	
	/* Create a scroll window */
	w->scroll = gtk_scrolled_window_new( NULL, NULL );
	
	/* Attach text box to scroll window */
	gtk_container_add( GTK_CONTAINER(w->scroll), w->text );
	
	/* Attach scroll to window */
	gtk_container_add( GTK_CONTAINER(w->window), w->scroll );
	
	/* Show all */
	gtk_widget_show_all( w->window );
}

/* Add text */
void gzrt_gui_debug_add ( char *file, int line, char *fmt, ... )
{
	va_list ap;
	char buffer[2048], minbuffer[128];
	int len;
	GtkTextIter st, en;
	
	#ifndef GZRT_DEBUG
	 /* In case this somehow gets called */
	 return;
	#endif
	
	/* Navigate to the end */
	gtk_text_buffer_get_bounds( GTK_TEXT_BUFFER(w->buffer), &st, &en );
	gtk_text_buffer_place_cursor( GTK_TEXT_BUFFER(w->buffer), &en );
	
	/* Print time/file info */
	snprintf( minbuffer, sizeof(minbuffer), "[%s:%04u:%.2f] ", file, line, time_since_start() );
	len = snprintf( buffer, sizeof(buffer), "%-34s", minbuffer );
	
	/* Append */
	gtk_text_buffer_insert_at_cursor( GTK_TEXT_BUFFER(w->buffer), buffer, len );
	
	/* User specified format */
	va_start( ap, fmt );
	len = vsnprintf( buffer, sizeof(buffer) - 1, fmt, ap );
	va_end( ap );
	
	/* Insert text */
	gtk_text_buffer_insert_at_cursor( GTK_TEXT_BUFFER(w->buffer), buffer, len );
	
	/* Console mimic? */
	#ifdef GZRT_DEBUG_CONSOLE_MIMIC
	 g_print( "[%s:%04u:%.2f] %s", file, line, time_since_start(), buffer );
	 fflush( stdout );
	#endif
}
