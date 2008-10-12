/**********************
* GUI Padding Element *
**********************/
#ifndef __GZRT_WPADDING_H
#define __GZRT_WPADDING_H

typedef struct _gzrt_wpadding
{
	GtkWidget *hbox;
	GtkWidget *vbox;
	GtkWidget *main;
} PADDING;

PADDING * gzrt_wpadding_create ( GtkWidget *window );
void 	  gzrt_wpadding_close  ( PADDING * h );

#endif
