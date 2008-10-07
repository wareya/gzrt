/**********************
* Z64MV OpenGL whooha *
**********************/
#include <z64mv.h>
#include <signal.h>
#include <time.h>

/*
** Main draw loop
*/
void z64mv_draw_loop ( Z64MV * h )
{
	double start, diff;
	struct timespec t;
	static double i = 0.0;
	static double scale = 1.0, zoom = -6.0;
	
	double zoom_val = 0;
	double scale_val = 1;
	
	double rot_x = 0.0, rot_y = 0.0, rot_z = 0.0;
	double rvl_x = 0.0, rvl_y = 0.0, rvl_z = 0.0;
	
	glDisable ( GL_CULL_FACE );
	
	mainloop:
		
	/* Set time */
	start = time_sec( h );
		
	/* Any events? */
	while ( SDL_PollEvent( &h->event ) )
	{
		/* Keyboard input */
		if( h->event.type == SDL_KEYDOWN )
		{
			/* Quit */
			if( h->event.key.keysym.sym == SDLK_w )
				scale_val = 0.90;
			else if( h->event.key.keysym.sym == SDLK_s )
				scale_val = 1 / 0.90;
			else if( h->event.key.keysym.sym == SDLK_a )
				zoom_val = 1.0;
			else if( h->event.key.keysym.sym == SDLK_d )
				zoom_val = -1.0;
			else if( h->event.key.keysym.sym == SDLK_z )
			{
				SDL_Quit( );
				exit( 1 );
			}
		}
		else
			
		/* Keys released */
		if( h->event.type == SDL_KEYUP )
		{
			if( h->event.key.keysym.sym == SDLK_w )
				scale_val = 1;
			else if( h->event.key.keysym.sym == SDLK_s )
				scale_val = 1;
			else if( h->event.key.keysym.sym == SDLK_a )
				zoom_val = 0;
			else if( h->event.key.keysym.sym == SDLK_d )
				zoom_val = 0;
		}
		else
			
		/* Mouse input */
		if( h->event.type == SDL_MOUSEBUTTONDOWN  )
		{
			/* Zoom in */
			if( h->event.button.button == SDL_BUTTON_LEFT )
				zoom_val = 1.0;
			else if( h->event.button.button == SDL_BUTTON_RIGHT )
				zoom_val -= 1.0;
		}
	}
	
	/* Update zoom & scale */
	zoom += zoom_val * 0.5;
	scale *= scale_val;
	
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
	t.tv_nsec = ((1.0 / h->fps_cap) - (time_sec( h ) - start)) * 1000000000.0; 
	t.tv_sec  = 0;
	nanosleep( &t, NULL );
	
	/* Loop again */
	goto mainloop;
}
