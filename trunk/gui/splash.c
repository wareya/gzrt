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
	
	/* Opacity (only windows) */
	#ifdef WIN32
	 gtk_window_set_opacity( GTK_WINDOW(w->window), 0.00 );
	#else
	 gtk_widget_show( w->window );
	#endif
	
	/* Load image */
	w->image = gtk_image_new_from_file( GRT_SPLASH_IMAGE );
	SHOW( w->image );
	
	/* Add image to window */
	gtk_container_add( GTK_CONTAINER(w->window), w->image );
	
	/* If Windows, fade in */
	#ifdef WIN32
	 g_timeout_add( 100, (GSourceFunc)gzrt_wsplash_fade, NULL );
	#else /* Otherwise, just display */
	 SHOW(    w->window );
	 PRESENT( w->window );
	 
	 /* Key event */
	 g_signal_connect( G_OBJECT(w->window), "key-press-event", 
		 G_CALLBACK(gzrt_wsplash_handler), NULL );
	#endif
	
	/* Set handler (if any) */
	w->handler = handler;
}

/* Fade the window into view */
#ifdef WIN32
int gzrt_wsplash_fade ( void )
{
	double	f;
	clock_t	s;
	
	/* Unhide window */
	SHOW( w->window );
	PRESENT( w->window );
	
	/* Fade loop */
	for( f = 0.0; f < 1.05; f += 0.05 )
	{
		/* Update window attrs */
		gtk_window_set_opacity( GTK_WINDOW(w->window), f );
		gtk_main_iteration_do( FALSE );
		
		/* Wait a little */
		s = clock();
		while
		( 
		  (double)(clock() - s)/CLOCKS_PER_SEC < 0.0125  
		);
	}
	
	/* Set signal */
	if( w->handler )
		g_signal_connect( G_OBJECT(w->window), "key-press-event", G_CALLBACK(gzrt_wsplash_handler), NULL );
	
	/* Return false to stop timeout */
	return FALSE;
}
#endif

/* Close the splash window */
void gzrt_wsplash_close ( void )
{
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
