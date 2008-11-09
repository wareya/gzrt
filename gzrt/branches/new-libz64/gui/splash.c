/****************************
* Splash Screen GUI Element *
****************************/
#include <gzrt.h>

/* Structure */
struct _gzrt_wsplash gzrt_wsplash;

/* Shorthand pointer variable */
static struct _gzrt_wsplash * w = &gzrt_wsplash;

/* Initialize the splash window */
void gzrt_wsplash_init ( int (*handler)( void ) )
{
	/* Create window element */
	w->window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	
	/* Set attributes */
	gtk_window_set_decorated(    GTK_WINDOW(w->window), FALSE                             );
	gtk_window_set_position(     GTK_WINDOW(w->window), GTK_WIN_POS_CENTER_ALWAYS         );
	gtk_window_set_default_size( GTK_WINDOW(w->window), GRT_SPLASH_W, GRT_SPLASH_H        );
	gtk_window_set_deletable(    GTK_WINDOW(w->window), FALSE                             );
	gtk_window_set_type_hint(    GTK_WINDOW(w->window), GDK_WINDOW_TYPE_HINT_SPLASHSCREEN );
	
	/* Invisible & fade */
	#ifdef WIN32
	 gtk_window_set_opacity( GTK_WINDOW(w->window), 0.00 );
	 g_timeout_add( 500, (GSourceFunc)gzrt_wsplash_fade, NULL );
	#endif
	
	/* Timeout */
	g_timeout_add( 3000, (GSourceFunc)gzrt_wsplash_close, NULL );
	
	/* Load image */
	w->image = gtk_image_new_from_file( GRT_SPLASH_IMAGE );
	
	/* Add image to window */
	gtk_container_add( GTK_CONTAINER(w->window), w->image );
	
	/* Key press */
	g_signal_connect( G_OBJECT(w->window), "destroy", G_CALLBACK(gzrt_wsplash_handler), NULL );
	g_signal_connect( G_OBJECT(w->window), "key-press-event", G_CALLBACK(gzrt_wsplash_close), NULL );
	
	/* Set handler (if any) */
	w->handler = handler;
	
	/* Show all */
	gtk_widget_show_all( w->window );
}

/* Fade the window into view */
#ifdef WIN32
 int gzrt_wsplash_fade ( void )
 {
 	double	f;
 	
 	/* Fade loop */
 	for( f = 0.0; f < 1.05; f += 0.05 )
 	{
 		/* Update window attrs */
 		gtk_window_set_opacity( GTK_WINDOW(w->window), f );
 		
 		/* Update */
 		while( gtk_events_pending() )
 			gtk_main_iteration();
 		
 		/* Wait a little */
 		g_usleep( 1000000 * 0.0125 );
 	}
 	
 	/* Return false to stop timeout */
 	return FALSE;
 }
#endif

/* Close the splash window */
void gzrt_wsplash_close ( void )
{
	static int called;
	
	if( called )
		return;
	else
		called++;
	
	gtk_widget_destroy( w->window );
}

/* Handler */
void gzrt_wsplash_handler ( void )
{
	if( w->handler )
	{
		gtk_widget_destroy( w->window );
		(*w->handler)();
	}
}
