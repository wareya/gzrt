/*****************************
* File Selection GUI Element *
*****************************/
#ifndef __GZRT_WFILESEL_H
#define __GZRT_WFILESEL_H

/* Constants */
#define GZRT_FILE_SELECT_TITLE	"Please select a Zelda ROM"
#define GZRT_FILE_DEFAULT_NAME	"ZELDA64.Z64"

/* Structure */
struct _gzrt_wfilesel
{
	GtkWidget *fswin;
};

/* Functions */
void gzrt_wfilesel_show   ( void );
void gzrt_wfilesel_check  ( GtkWidget *w, GtkFileSelection *fs );
void gzrt_wfilesel_cancel ( void );

#endif /* __GZRT_WFILESEL_H */
