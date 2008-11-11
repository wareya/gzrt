/************************
* ROM Extraction Window *
************************/
#ifndef __GZRT_WEXTRACT_H
#define __GZRT_WEXTRACT_H

/* Struct */
struct _gzrt_wextract
{
	GtkWidget *window;
	GtkWidget *button;
	
	/* Layout elements */
	GtkWidget *wvbox;
	GtkWidget *whbox;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *entry;
	GtkWidget *browse;
	GtkWidget *frame;
	GtkWidget *fmt;
	
	/* Debug options */
	#ifdef GZRT_DEBUG
	GtkWidget *dontwrite;
	#endif
	
	/* Check boxes */
	GtkWidget *b_strip_prefixes;
	GtkWidget *b_scene_table;
	GtkWidget *b_actor_table;
};

/* Statistics */
struct _gzrt_wextract_stats
{
	int files_extracted;
	Z64FS *fs;
	Z64NT *nt;
};

/* Constants */
#define GZRT_WEXTRACT_W		400
#define GZRT_WEXTRACT_H		300
#define GZRT_WEXTRACT_T		"Extract resources"
#define GZRT_WEXTRACT_OK	"Extract"
#define GZRT_WEXTRACT_C_1	"Strip filename prefixes"
#define GZRT_WEXTRACT_C_2	"Use scene table"
#define GZRT_WEXTRACT_C_3	"Use actor table"
#define GZRT_WEXTRACT_CD1	"Don't write to disk"
#define GZRT_WEXTRACT_UPD	32
#define GZRT_WEXTRACT_MAX	(5 * 1024 * 1024) /* Max filesize */

/* Default format strings */
#define GZRT_WEXTRACT_FMT_NT	"%L - %o1 %o2"
#define GZRT_WEXTRACT_FMT		"%L - %n"

/* Functions */
void gzrt_wextract_show ( MAINWIN *h );
void gzrt_wextract_init ( void );
void gzrt_wextract_regen ( void );
void gzrt_wextract_begin ( void );
int gzrt_wextract_soffsetf ( char *dest, struct _gzrt_wextract_stats * stats, char *fmt );

#endif /* __GZRT_WEXTRACT_H */
