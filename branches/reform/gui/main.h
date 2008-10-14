/**********************
* GZRT Main Window(s) *
**********************/
#ifndef __GZRT_WMAIN_H
#define __GZRT_WMAIN_H

/* Deps */
#include <n64rom.h>
#include <z64.h>

/* Constants */
#define GZRT_WMAIN_W	500
#define GZRT_WMAIN_H	320
#define GZRT_WMAIN_NAME	"GZRomTool"

/* String compare function */
#ifdef WIN32	
# define	FILENAME_CMP(s, e)	stringcmp(s, e)
#else
# define	FILENAME_CMP(s, e)	strcmp(s, e)
#endif

/* Menu text */
#define GZRT_WMAIN_M_F			"File"
#define GZRT_WMAIN_M_F_OPEN		"Open"
#define GZRT_WMAIN_M_F_CLOSE	"Close"
#define GZRT_WMAIN_M_F_QUIT		"Quit"
#define GZRT_WMAIN_M_O			"Operations"
#define GZRT_WMAIN_M_O_EX		"Extract files"
#define GZRT_WMAIN_M_O_DEC		"Decompress ROM"
#define GZRT_WMAIN_M_O_LIST		"File listing"
#define GZRT_WMAIN_M_O_BSWAP	"Byteswap ROM"
#define GZRT_WMAIN_M_O_CRC		"Fix CRC"
#define GZRT_WMAIN_M_O_FS		"File search"
#define GZRT_WMAIN_M_H			"Help"
#define GZRT_WMAIN_M_H_ABOUT	"About"
#define GZRT_WMAIN_M_H_WAI		"Who am I"

/* Structure */
typedef struct _gzrt_wmain
{
	int   id;
	void *hid;
	
	/* Window */
	GtkWidget *window;
	
	/* Status stack */
	int sstack;
	
	/* ROM Context */
	N64Rom *c;
	
	/* Zelda Filesystem */
	Z64FS *z;
	Z64NT *t;
}
MAINWIN;

/* Global variables */
extern int window_amount;

/* Functions */
int gzrt_wmain_create_new ( N64Rom * rc );
void gzrt_wmain_fill ( MAINWIN *c );
void gzrt_wmain_destroyed ( GtkWidget *w, MAINWIN * h );
void gzrt_wmain_destroy ( GtkWidget *w, MAINWIN * h );
void gzrt_wmain_focus ( MAINWIN *w );
void gzrt_wmain_close ( MAINWIN *w );
void gzrt_wmain_closed ( MAINWIN *w );
void pbu ( int a, int b );
void gzrt_wmain_frame_add ( GtkWidget *vbox, char *fmt, ... );
void gzrt_wmain_disable_item ( MAINWIN *w, char *name );
void gzrt_wmain_enable_item  ( MAINWIN *w, char *name );
void gzrt_wmain_status_rmmsg ( MAINWIN *w );
void gzrt_wmain_status_addmsg ( MAINWIN *w, char *msg );
void gzrt_wmain_update ( MAINWIN *c );
void gzrt_wmain_plugin_action ( MAINWIN * w );
GtkWidget * gzrt_wmain_main_generate ( MAINWIN * w );
int	gzrt_select_file_id ( MAINWIN * w );
void gzrt_wmain_byteswap ( N64Rom * rc );
void gzrt_wmain_extract ( MAINWIN * w );


		
#endif /* __GZRT_WMAIN_H */
