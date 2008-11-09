/************************
* Debug Box GUI Element *
************************/
#include <gzrt.h>
#include <malloc.h>

/* Global textbuffer */
GtkTextBuffer * debug_console = NULL;

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
	
	/* Create it? */
	if( !debug_console )
		debug_console = gtk_text_buffer_new(NULL);
	
	/* Navigate to the end */
	gtk_text_buffer_get_bounds( GTK_TEXT_BUFFER(debug_console), &st, &en );
	gtk_text_buffer_place_cursor( GTK_TEXT_BUFFER(debug_console), &en );
	
	/* Print time/file info */
	snprintf( minbuffer, sizeof(minbuffer), "[%s:%04u:%.2f] ", file, line, time_since_start() );
	len = snprintf( buffer, sizeof(buffer), "%-34s", minbuffer );
	
	/* Append */
	gtk_text_buffer_insert_at_cursor( GTK_TEXT_BUFFER(debug_console), buffer, len );
	
	/* User specified format */
	va_start( ap, fmt );
	len = vsnprintf( buffer, sizeof(buffer), fmt, ap );
	va_end( ap );
	
	/* Insert text */
	gtk_text_buffer_insert_at_cursor( GTK_TEXT_BUFFER(debug_console), buffer, len );
	
	/* Console mimic? */
	#ifdef GZRT_DEBUG_CONSOLE_MIMIC
	 g_print( "[%s:%04u:%.2f] %s", file, line, time_since_start(), buffer );
	 fflush( stdout );
	#endif
}

