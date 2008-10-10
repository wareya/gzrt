/**********************
* Z64MV OpenGL whooha *
**********************/
#include <z64mv.h>
#include <signal.h>
#include <sys/time.h>

/* Storage of key statuses */
static int	key_status[256];

/*
** Main draw loop
*/
void z64mv_draw_loop ( Z64MV * h )
{
	double   start, diff;
	unsigned stime;
	
	/* The rotation (in degrees) of the object -
	   calculated based on framerate and how many degrees
	   to turn per sec */
	static double i = 0.0;

	/* Constant scale/zoom values for camera */
	static double scale = 1.0, zoom = -6.0;
	
	mainloop:
		
	/* Set time */
	start = time_sec( h );
		
	/* Any events? */
	while ( SDL_PollEvent( &h->event ) )
	{
		/* Keyboard input */
		if( h->event.type == SDL_KEYDOWN )
		
			/* Store it */
			key_status[h->event.key.keysym.sym] = TRUE;
		
		/* Key released */
		else if( h->event.type == SDL_KEYUP )
		
			/* Store it */
			key_status[h->event.key.keysym.sym] = FALSE;
	}
	
	/* Update zoom & scale */
	if( key_status[SDLK_w] )
		scale *= 0.75;
	if( key_status[SDLK_s] )
		scale /= 0.75;
	
	/* Quit */
	if( key_status[SDLK_q] )
	{
		SDL_Quit( );
		SDL_KillThread( h->proc );
	}
	
	/* Clear the window */
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glLoadIdentity();
	
	/* Center display */
    glTranslatef( 0.0f, 0.0f, zoom );
	
	/* Rotate! */
	glRotated( i, 0, 1, 0 );
	i = ANIMATION_DPS * time_sec( h );
	
	/* Scale */
	glScaled( scale, scale, scale );
	
	/* Display */
	z64mv_dlist_process( h, h->contents, h->filesize, h->entry );
	
    /* Draw it to the screen */
    SDL_GL_SwapBuffers( );
	
	/* Put thread to sleep for FPS lock */
	stime = ((1.0 / h->fps_cap) - (time_sec( h ) - start)) * 1000.0; 
	SDL_Delay( stime );
	
	/* Loop again */
	goto mainloop;
}
