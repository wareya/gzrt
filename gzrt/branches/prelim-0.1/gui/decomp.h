/*****************************
* Zelda 64 Decompressor Menu *
*****************************/
#ifndef __GZRT_WDECOMPRESS_H
#define __GZRT_WDECOMPRESS_H

/* Constants */
#define GZRT_WDECOMPRESS_T      "Decompress ROM"
#define GZRT_WDECOMPRESS_MAX    ( 5 * 1024 * 1024 )
#define GZRT_WDECOMPRESS_UPDATE_INVERVAL ( 1 * 1024 * 1024 )

/* Structure */
struct _gzrt_wdecompress
{
    WINDOW *win;
    
    GtkWidget *vbox_outer;
    GtkWidget *vbox_inner;
    GtkWidget *frame;
    GtkWidget *label;
    GtkWidget *entry;
    GtkWidget *hbox;
    GtkWidget *button;
    GtkWidget *pad1;
    GtkWidget *pad2;
    GtkWidget *ok_button;
    
    MAINWIN *mw;
};

/* Functions */
void gzrt_wdecompress_create    ( MAINWIN *mw );
void gzrt_wdecompress_close     ( struct _gzrt_wdecompress * i );
void gzrt_wdecompress_routine   ( struct _gzrt_wdecompress * i );
void gzrt_wdecompress_closed    ( struct _gzrt_wdecompress * i );
void gzrt_wdecomp_show          ( MAINWIN * c );

#endif /* __GZRT_WDECOMPRESS_H */
