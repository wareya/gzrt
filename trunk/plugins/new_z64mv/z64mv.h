/*******************************
* Zelda 64 Model Viewer Plugin *
*******************************/
#ifndef GZRT_Z64MV_H
#define GZRT_Z64MV_H

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <sys/time.h>
#include <math.h>
#include <glib.h>
#include "SDL.h"
#include "SDL_ttf.h"

/* Plugin stuff */
#include <gzrtplugin.h>

/* Constants */
#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480
#define SCREEN_BPP		16
#define SCREEN_FPS		20.0	/* Initial FPS limit */

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

/* Local includes */
#include <dlist.h>

/* Context */
typedef struct
{
	/* SDL Thread */
	SDL_Thread		* proc;
	
	/* SDL elements */
	SDL_Surface		* surface;
	SDL_VideoInfo	* vinfo;
	SDL_Event		  event;
	
	/* Viewer config */
	struct timeval	  invoke;
	float			  fps_cap;
	u32				  entry;
	int				  vtx_source;
	
	/* File information */
	struct PluginFileSpec * data;
}
Z64MV;

/* Global variables */
extern struct PluginMeta	gzrt_plugin_info;
extern struct Functions *	GZRT_FUNC;

/* Functions - plugin */
int z64mv_init ( const struct Functions * );
int z64mv_main ( void                     );
int z64mv_load ( struct PluginTransac *   );

/* Functions - viewer */
void z64mv_init_context   ( Z64MV * h                    );
void z64mv_display_resize ( Z64MV * handle, int w, int h );
void z64mv_display_fps	  ( Z64MV * h, int fps           );
void z64mv_set_title	  ( char  * title, char * icon   );
void z64mv_draw_loop	  ( Z64MV * h					 );

#endif /* GZRT_Z64MV_H */
