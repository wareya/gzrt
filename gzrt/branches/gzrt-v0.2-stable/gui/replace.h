/**************************
* File replacement window *
**************************/
#ifndef __GZRT_WREPLACE_H
#define __GZRT_WREPLACE_H

GtkWidget * gzrt_wreplace_create ( MAINWIN * c );
void gzrt_wreplace_file ( GtkWidget * window );
void gzrt_wreplace_close ( GtkWidget * window );
void gzrt_wreplace_destroy ( MAINWIN * c );
void gzrt_wreplace_write ( GtkWidget * window );

#endif /* __GZRT_WREPLACE_H */
