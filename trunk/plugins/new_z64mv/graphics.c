/**********************
* Z64MV OpenGL whooha *
**********************/
#include <z64mv.h>
#include <signal.h>
#include <sys/time.h>
#include <math.h>
#include <glib.h>

/* Storage of key statuses */
static int	key_status[256];

/*
** Main draw loop
*/
void z64mv_draw_loop ( Z64MV * h )
{        

	
	double xtrans=0;
	double ytrans=0;
	double ztrans=0;
	double xpos=0;
	double ypos=0;
	double zpos=0;
	double udrot=0;
	double yrot=0;
	double heading=0;
	double piover180=0.0174532925;
	
	double   start, diff;
	unsigned stime;
	
	/* The rotation (in degrees) of the object -
	   calculated based on framerate and how many degrees
	   to turn per sec */
	static double i = 0.0;
	
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
	
	
	/* Quit */
	if( key_status[SDLK_q] )
	{
		memset( key_status, 0, sizeof(key_status) );
		SDL_Quit( );
		SDL_KillThread( h->proc );
	}
	
	/* Clear the window */
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glLoadIdentity();
	
	/*  Use rotations */
	glRotatef(udrot, 1.0, 0,0);
	glRotatef(yrot, 0, 1.0, 0);
	glScaled(0.25, 0.25, 0.25);
	
	/* Use translations */
	
	xtrans = - xpos;
	ztrans = - zpos;
	ytrans = - ypos;
    glTranslatef( xtrans, ytrans, ztrans );
	
	/* Control */
	if( key_status[SDLK_d] ){
		xpos = xpos + cos(heading*piover180) * 2.5;
		zpos = zpos + sin(heading*piover180) * 2.5;
	}
	
	if( key_status[SDLK_a] ){
		xpos = xpos - cos(heading*piover180) * 2.5;
		zpos = zpos - sin(heading*piover180) * 2.5;
	}
	
	if( key_status[SDLK_s] ){
		xpos = xpos + cos(heading*piover180+(3.141592654/2)) * 2.5;
		zpos = zpos + sin(heading*piover180+(3.141592654/2)) * 2.5;
	}
	
	if( key_status[SDLK_w] ){
		xpos = xpos - cos(heading*piover180+(3.141592654/2)) * 2.5;
		zpos = zpos - sin(heading*piover180+(3.141592654/2)) * 2.5;
	}
	
	if( key_status[SDLK_r] ){
		ypos = ypos + 2.5;
	}
    
	if( key_status[SDLK_f] ){
		ypos = ypos - 2.5;
	}
	if( key_status[SDLK_UP] ){
		udrot = udrot - 2.5;
	}
	
	if( key_status[SDLK_DOWN] ){
		udrot = udrot + 2.5;
	}
	
	if( key_status[SDLK_LEFT] ){
		heading = heading - 2.5;
		yrot = heading;
	}
	
	if( key_status[SDLK_RIGHT] ){
		heading = heading + 2.5;
		yrot = heading;
	}
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
