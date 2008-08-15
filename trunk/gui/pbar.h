/**********************
* Progress bar window *
**********************/
#ifndef __GZRT_WPBAR_H
#define __GZRT_WPBAR_H

/* Constants */
#define GZRT_WPBAR_NAME "Working..."
#define GZRT_WPBAR_W	350
#define GZRT_WPBAR_H	28

/* Struct */
typedef struct _gzrt_wpbar
{
	GtkWidget *window;
	GtkWidget *progbar;
}
PROGRESS;

/* Function declarations */
void       gzrt_wpbar_text  ( PROGRESS * w, char *fmt, ... );
PROGRESS * gzrt_wpbar_new   ( void                         );
void       gzrt_wpbar_hide  ( PROGRESS * w                 );
void       gzrt_wpbar_show  ( PROGRESS * w                 );
void       gzrt_wpbar_close ( PROGRESS * w                 );
void       gzrt_wpbar_set   ( PROGRESS * w, double p       );

#endif /* __GZRT_WPBAR_H */
