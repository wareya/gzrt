/*****************************
* GZRT About Window (credit) *
*****************************/
#ifndef __GZRT_WABOUT_H
#define __GZRT_WABOUT_H

/* Defaults */
#define GZRT_WABOUT_W		190
#define GZRT_WABOUT_H		90
#define GZRT_WABOUT_TITLE	"About"
#define GZRT_WABOUT_MSG		"By ZZT32 (http://www.64.vg/)\nPlugins by their respective authors"

/* Function definitions */
void gzrt_wabout_show   ( void          );
void gzrt_wabout_closed ( void          );
void gzrt_wabout_close  ( GtkWidget * w );

#endif /* __GZRT_WABOUT_H */
