/*****************************
* GZRT About Window (credit) *
*****************************/
#ifndef __GZRT_WABOUT_H
#define __GZRT_WABOUT_H

/* Defaults */
#define GZRT_WABOUT_W		190
#define GZRT_WABOUT_H		90
#define GZRT_WABOUT_TITLE	"About"
#define GZRT_WABOUT_MSG		"By ZZT32\nhttp://www.64.vg/"

/* Struct */
struct _gzrt_wabout
{
	GtkWidget *window;
	GtkWidget *label;
	GtkWidget *button;
	GtkWidget *vbox;
};

/* Function definitions */
void gzrt_wabout_init ( void );
void gzrt_wabout_show ( void );
void gzrt_wabout_hide ( void );

#endif /* __GZRT_WABOUT_H */
