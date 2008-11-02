/*****************************
* File Selection GUI Element *
*****************************/
#ifndef __GZRT_WFILESEL_H
#define __GZRT_WFILESEL_H

/* Functions */
void gzrt_wfilesel_show   ( void );
void gzrt_wfilesel_check  ( GtkWidget *w, GtkFileSelection *fs );
void gzrt_wfilesel_cancel ( void );
const char * gzrt_wfilesel_get ( void );

#endif /* __GZRT_WFILESEL_H */
