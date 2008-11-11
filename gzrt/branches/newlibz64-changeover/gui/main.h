/**********************
* GZRT Main Window(s) *
**********************/
#ifndef __GZRT_WMAIN_H
#define __GZRT_WMAIN_H

/* Deps */
#include <n64rom.h>
#include <z64.h>
#include <app/plugins.h>

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
	/* Window */
	GtkWidget *window;
	
	/* Status stack */
	int sstack;
	
	union
	{
		struct RomSpec spec;
		struct 
		{
			/* ROM Context */
			N64Rom *c;
			
			/* Zelda Filesystem */
			Z64 * z;
		};
	};
	
	/* The plugin action button text */
	GtkWidget * plugin_label;
	
	/* The file listing */
	GtkListStore * model;
	
	/* Timeouts that need to be stopped */
	GList * timeouts;
	
	/* Reserved... */
	glong	reserved3;
	glong	reserved4;
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
int gzrt_wmain_count ( void );
void gzrt_wmain_action_button_text ( char * text );
void gzrt_wmain_set_plugin ( char * n );
void gzrt_wmain_timeout_add ( MAINWIN * c, guint32 interval, void * func, void * object );
void gzrt_wmain_remove_timeouts ( MAINWIN * c );
void gzrt_wmain_voffset_clipboard ( MAINWIN * c );
void gzrt_wmain_roffset_clipboard ( MAINWIN * c );

#endif /* __GZRT_WMAIN_H */
