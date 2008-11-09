/****************************
* Splash Screen GUI Element *
****************************/
#ifndef __GZRT_WSPLASH_H
#define __GZRT_WSPLASH_H

/* Defines */
#define GRT_SPLASH_IMAGE    GZRT_GUI_ICON_DIR "splash.png"
#define GRT_SPLASH_W        520
#define GRT_SPLASH_H        253

/* Data structure */
struct _gzrt_wsplash
{
    GtkWidget *window;
    GtkWidget *image;
    int (*handler)( void );
};

/* Function declarations */
int  gzrt_wsplash_fade    ( void );
void gzrt_wsplash_init    ( int (*handler)( void ) );
void gzrt_wsplash_close   ( void );
void gzrt_wsplash_handler ( void );

#endif
