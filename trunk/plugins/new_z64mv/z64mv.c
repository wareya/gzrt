#include <stdio.h>
#include <stdlib.h>
#include <z64mv.h>

/* GZRT Inherited functions */
struct Functions * GZRT_FUNC;

/* Plugin information */
struct PluginMeta gzrt_plugin_info =
{
	"Model Viewer",
	"Zelda 64 Model Viewer",
	"ZZT32 & Wareya",
	"http://gzrt.googlecode.com/",
	"v0",
	
	/* Description */
	NULL,
	
	/* Init, menu & file action funcs */
	z64mv_init, z64mv_main, z64mv_load
};


/*
** Plugin init
*/
int z64mv_init ( const struct Functions * f )
{
	/* Update function pointer */
	GZRT_FUNC = (void*)f;
	
	/* Debug */
	Z64MV_DEBUG( "Z64MV init'd." );
}


/*
** Call from menu
*/
int z64mv_main ( void )
{
	/* Not implemented */
	Z64MV_DEBUG( "Calling from menu bar not yet implemented." );
}


/*
** Open a file from Zelda 64
*/
int z64mv_load ( struct PluginTransac * t )
{
	Z64MV * handle;
	
	/* Allocate memory */
	if( !(handle = CALLOC( sizeof(Z64MV))) )
		return -1;
	
	/* Set data */
	handle->data = t->file;
	
	/* Create the thread */
	handle->proc = SDL_CreateThread( (void*)z64mv_init_context, handle );
	
	/* Done */
	return 0;
}

/*
*
 *
    *             Plugin stuff ends here
	     *
		      *
			      *
				     *
					   *
					    *
						*/

/* Initialize a context */
void z64mv_init_context ( Z64MV * h )
{
	int videoFlags;
	
	/* Set invoke time */
	gettimeofday( &h->invoke, NULL );
	
	/* Set FPS */
	if( !h->fps_cap )
		h->fps_cap = SCREEN_FPS;
	
	/* Set up SDL */
	SDL_Init( SDL_INIT_EVERYTHING );
	h->vinfo = (SDL_VideoInfo*)SDL_GetVideoInfo();
	
	/* Set video flags */
	videoFlags  = SDL_OPENGL;
    videoFlags |= SDL_GL_DOUBLEBUFFER;
    videoFlags |= SDL_HWPALETTE;
    videoFlags |= SDL_RESIZABLE;
	
	/* Hardware or software? */
    if ( h->vinfo->hw_available )
		videoFlags |= SDL_HWSURFACE;
    else
		videoFlags |= SDL_SWSURFACE;
	
	/* Hw blits? */
	if ( h->vinfo->blit_hw )
		videoFlags |= SDL_HWACCEL;
	
	/* Enable double buffering in OGL display */
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	
	/* Create the SDL surface */
    h->surface = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, videoFlags );
	
	/* Initialize openGL */
	glShadeModel( GL_SMOOTH );
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glClearDepth( 1.0f );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	
	/* Set initial screen size */
	z64mv_display_resize( h, SCREEN_WIDTH, SCREEN_HEIGHT );
	
	/* Draw it */
	h->entry = z64mv_ep_find( h->data->file, h->data->filesize );
	z64mv_draw_loop( h );
}

/* Handle display resize */
void z64mv_display_resize ( Z64MV * handle, int w, int h )
{
	/* Ratio */
    GLfloat ratio;
	
	/* Prevent / 0 */
	if( !h )
		h = 1;
	
	/* Set ratio */
	ratio = (float)w / (float)h;
	
    /* Set up viewport. */
    glViewport( 0, 0, ( GLsizei )w, ( GLsizei )h );
	
    /* change to the projection matrix and set our viewing volume. */
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
	
    /* Set our perspective */
    gluPerspective( 45.0f, ratio, 0.1f, 100.0f );
	
	/* Make sure we're chaning the model view and not the projection */
    glMatrixMode( GL_MODELVIEW );
	
    /* Reset The View */
    glLoadIdentity( );
	
	/* We want ctrl-c to work */
	#ifndef WIN32
	 signal( SIGINT,  SIG_DFL );
     signal( SIGQUIT, SIG_DFL );
	#endif
}

/* Get the current time since invoke in seconds */
double time_sec ( Z64MV * h )
{
	struct timeval cur;
	
	gettimeofday( &cur, NULL );
	
	return 
		((cur.tv_sec * 1000000.0 + cur.tv_usec) -
		 (h->invoke.tv_sec * 1000000.0 + h->invoke.tv_usec))
		/ 1000000.0;
}

/* Set framerate cap */
void z64mv_display_fps ( Z64MV * h, int fps )
{
	h->fps_cap = (float)fps;
}

/* Set the SDL window title */
void z64mv_set_title ( char * title, char * icon )
{
	SDL_WM_SetCaption( title, icon );
}
