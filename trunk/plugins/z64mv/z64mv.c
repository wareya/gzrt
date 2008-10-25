/********************************
* Zelda 64 Model Viewer Library *
********************************/
#include <z64mv.h>
#include <sys/time.h>

#include "gzrtplugin.h"

/* GZRT Inherited functions */
struct Functions * GZRT_FUNC;

/*
** Plugin init
*/
int z64mv_init ( const struct Functions * f )
{
	/* Update function pointer */
	GZRT_FUNC = (void*)f;
	
	/* Debug */
	Z64MV_DEBUG( "Z64MV init'd." );
	
	/* _ */
}

/*
** Call from menu
*/
int z64mv_main ( const struct Functions * f, const struct RomSpec * r )
{
	/* Not implemented */
	Z64MV_DEBUG( "Calling from menu bar not yet implemented." );
}

/*
** Open a file from Zelda 64
*/
int z64mv_load ( const struct Functions * f, const struct RomSpec * r, const struct PluginTransac * t )
{
	/* Allocate space */
	Z64MV *ret = calloc(sizeof(Z64MV), 1);
	
	/* Allocate space & copy */
	ret->contents = t->file->file;
	ret->filesize = t->file->filesize;
	
	/* Create a thread */
	ret->proc = SDL_CreateThread( (void*)z64mv_init_context, ret );
}

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

/* SDL init boolean */
int sdlstatus = 0;

/* Create a new model viewing context */
Z64MV * z64mv_create_from_file ( char *filename )
{
	/* Allocate space */
	Z64MV *ret = calloc(sizeof(Z64MV), 1);
	
	/* Check that the file is readable */
	if( !(ret->handle = fopen(filename, "rb")) )
		return NULL;
	
	/* Allocate space & read it in */
	fseek(  ret->handle, 0, SEEK_END );
	ret->contents = malloc(ret->filesize = ftell(ret->handle));
	fseek(  ret->handle, 0, SEEK_SET );
	fread(  ret->contents, ret->filesize, 1, ret->handle );
	fseek(  ret->handle, 0, SEEK_SET );
	
	/* Set filename */
	ret->filename = malloc(strlen(filename));
	strcpy( ret->filename, filename );
	
	/* Create a thread */
	ret->proc = SDL_CreateThread( (void*)z64mv_init_context, ret );
	
	/* Return final product */
	return ret;
}

/* Create a new model viewing context */
Z64MV * z64mv_create_from_bin ( u8 *file, u32 size )
{
	/* Allocate space */
	Z64MV *ret = calloc(sizeof(Z64MV), 1);
	
	/* Allocate space & copy */
	ret->contents = malloc( size );
	ret->filesize = size;
	memcpy( ret->contents, file, size );
	
	/* Create a thread */
	ret->proc = SDL_CreateThread( (void*)z64mv_init_context, ret );
	
	/* Return final product */
	return ret;
}

/*
** Destroy a model viewing context
*/
void z64mv_destroy ( Z64MV * handle )
{	
	/* Free pointers */
	pfree( (void**)&handle->contents );
	pfree( (void**)&handle->filename );
	
	/* End the thread */
	SDL_KillThread( handle->proc );
	
	/* Free the context */
	free( handle );
}

/*
** Initialize the graphics and such
*/ 
void z64mv_init_context ( Z64MV * h )
{
	int videoFlags;
	
	/* Set invoke time */
	gettimeofday( &h->invoke, NULL );
	
	/* Set FPS */
	if( !h->fps_cap )
		h->fps_cap = SCREEN_FPS;
	
	/* Set default error handler */
	if( !h->err )
		h->err = (z64mv_errh)z64mv_error;
	
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
	h->entry = z64mv_ep_find( h->contents, h->filesize );
	z64mv_draw_loop( h );
}

/*
** Resize window
*/
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

/*
** Free & set pointer to zero
*/
void pfree ( void **p )
{
	if( !*p )
		return;
	free( *p );
	*p = 0;
}

/*
** Get the current time since start in seconds
*/
double time_sec ( Z64MV * h )
{
	struct timeval cur;
	
	gettimeofday( &cur, NULL );
	
	return 
		((cur.tv_sec * 1000000.0 + cur.tv_usec) -
		 (h->invoke.tv_sec * 1000000.0 + h->invoke.tv_usec))
		/ 1000000.0;
}

/*
** Set the framerate cap
*/
void z64mv_display_fps ( Z64MV * h, int fps )
{
	h->fps_cap = (float)fps;
}

/*
** Set the error handler
*/
void z64mv_set_error_handler ( Z64MV * h, z64mv_errh f )
{
	h->err = f;
}

/*
** Default error handler
*/
void z64mv_error ( Z64MV * h, char * file, int line, char * fmt, ... )
{
	va_list ap;
	char buffer[512];
	
	va_start( ap, fmt );
	vsnprintf( buffer, sizeof(buffer) - 1, fmt, ap );
	va_end( ap );
	
	fprintf( stderr, "[%s:%u] %s\n", file, line, buffer );
}

/*
** Set the SDL window title
*/
void z64mv_set_title ( char * title, char * icon )
{
	SDL_WM_SetCaption( title, icon );
}
