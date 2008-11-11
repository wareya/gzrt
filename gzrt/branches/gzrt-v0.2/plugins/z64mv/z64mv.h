/********************************
* Zelda 64 Model Viewer Library *
********************************/
#ifndef __Z64MVL_H
#define __Z64MVL_H

/* Global includes */
#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <signal.h>
#include <sys/time.h>
#include <gzrtplugin.h>
#include "SDL.h"

/* Defaults */
#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480
#define SCREEN_BPP		16
#define SCREEN_FPS		30.0	/* Initial FPS limit */

/* Inherited function storage */
#define GZRT_FUNC	__functions

/* Print stuff to debug console */
#define Z64MV_DEBUG( x, ... ) \
	(GZRT_FUNC)->debug( __FILE__, __LINE__, x "\n", ##__VA_ARGS__ )
	
/* Throw error */
#define Z64MV_ERROR( x, ... ) \
	(GZRT_FUNC)->error( __FILE__, __LINE__, x "\n", ##__VA_ARGS__ )

/* Close filespec */
#define Z64MV_TRANSAC_CLOSE( x ) \
	(GZRT_FUNC)->close( x )
	
/* Memory alloc */
#define MALLOC(x)	(GZRT_FUNC)->malloc( x )
#define CALLOC(x)	(GZRT_FUNC)->calloc( x )

/* Types */
typedef unsigned long long	u64;
typedef unsigned long 		u32;
typedef unsigned short		u16;
typedef unsigned char		 u8;
typedef long long			s64;
typedef long				s32;
typedef short				s16;
typedef char				 s8;

/* Generic macros */
#define U64(x) ( \
	(u64)(x)[0] << 56 | \
	(u64)(x)[1] << 48 | \
	(u64)(x)[2] << 40 | \
	(u64)(x)[3] << 32 | \
	(u64)(x)[4] << 24 | \
	(u64)(x)[5] << 16 | \
	(u64)(x)[6] << 8  | \
	(u64)(x)[7] )
#define U32(x)	((x)[0]<<24 | (x)[1]<<16 | (x)[2]<<8 | (x)[3])
#define U16(x)	((x)[0]<<8  | (x)[1])
#define F16(x)	((float)((short)((x)[0]<<8|(x)[1])))

/* Error handling function */
typedef void (*z64mv_errh)( void * h, char *file, int line, char *fmt, ... );

/* Context */
typedef struct
{
	/* Thread */
	SDL_Thread *proc;
	
	/* SDL Elements */
	SDL_Surface   *surface;
	SDL_Surface	  *message;
	SDL_Event	   event;
	SDL_VideoInfo *vinfo;
	
	SDL_Color      textColor;
	
	/* File information */
	char *filename;
	u32   filesize;
	u8	 *contents;
	FILE *handle;
	u32 entry;
	
	/* Preloaded 3D stuff */
	int vtx_source;
	
	/* Error handler */
	z64mv_errh err;
	
	/* Time since creation */
	struct timeval invoke;
	
	/* Viewer config */
	float fps_cap;
}
Z64MV;

/* Globals */
extern int sdlstatus;	/* We don't want to initialize sdl twice */
extern struct Functions * GZRT_FUNC;

/* Local includes */
#include <dlist.h>

/* Functions for managing instances */
Z64MV * z64mv_create_from_file  ( char *filename          ); 
Z64MV * z64mv_create_from_bin   ( u8 *file, u32 size      );
void    z64mv_destroy           ( Z64MV * handle          );

/* Functions */
void z64mv_dlist_process ( Z64MV *h, u8 *data, u32 size, u32 start );
void z64mv_dlist_tri1    ( VERTEX * vtx, u8 i1, u8 i2, u8 i3       );
u64 z64mv_ep_find ( u8 *data, u32 size );

/* Error handling */
void z64mv_error             ( Z64MV * h, char * file, int line, char * fmt, ... );
void z64mv_set_error_handler ( Z64MV * h, z64mv_errh f );

/* Functions for the display */
void z64mv_init_context   ( Z64MV * h                    );
void z64mv_display_resize ( Z64MV * handle, int w, int h );
void z64mv_display_fps	  ( Z64MV * h, int fps           );
void z64mv_set_title	  ( char * title, char * icon    );
void z64mv_draw_loop ( Z64MV * h );

/* Other generic functions */
void   pfree    ( void  **p );
double time_sec ( Z64MV * h );

#endif
