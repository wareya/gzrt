/*********************
* GZRT Plugin System *
*********************/
#ifndef __GZRT_PLUGINS_H
#define __GZRT_PLUGINS_H

/* An open Zelda ROM context */
struct RomSpec
{
	void * rom;
	void * fs;
	void * nt;
};

/* File spec */
struct PluginFileSpec
{
	/* File info */
	int 	    id;
	char        filename[64];
	unsigned 	vstart;
	unsigned 	vend;
	unsigned 	start;
	unsigned 	end;
	
	/* File contents */
	unsigned char * file;
	unsigned		filesize;
	
	/* ROM info */
	char  * rom_filename;
	FILE  * rom_handle;
};

/* Plugin transaction */
struct PluginTransac
{
	/* Plugin target */
	const void * plugin;
	
	/* File information */
	struct PluginFileSpec * file;
};

/* GZRT inherited functions */
struct Functions
{
	/* Memory management */
	void     * (*malloc) ( unsigned );
	void     * (*calloc) ( unsigned );
	void       (*free)   ( void *   );
	unsigned   (*mused)  ( void     );
	
	/* Debug/error handling */
	void	   (*error)  ( char *, int, char *, ... );
	void	   (*debug)  ( char *, int, char *, ... );
	void       (*notice) ( char *, char *           );
	void     *	reserved2;
	
	/* Plugin ending */
	void       (*close)  ( const struct PluginTransac * );
	void     *	reserved3;
};

/* Init function handler */
typedef int (*GZRT_P_INIT)
(
	const struct Functions *
);

/* Menu bar handler */
typedef int (*GZRT_P_MENU)
(
	const struct Functions *,
	const struct RomSpec *
);

/* File action handler */
typedef int (*GZRT_P_ACTION)
(
	const struct Functions *,
	const struct RomSpec *,
	const struct PluginTransac *
);

/* Plugin information header */
struct PluginMeta
{
	/* Basic stuff */
	char * short_name;		/* Short name - displayed in menu		*/
	char * long_name;		/* Long program name					*/
	char * author;			/* Author - such as, "ZZT32 [zz@64.vg]" */
	char * group;			/* Website or group 					*/
	char * version;			/* Version number - such as, "v0.1"		*/
	
	/* Extended description */
	char * desc;
	
	/* Functions */
	GZRT_P_INIT		init;
	GZRT_P_MENU		menu_bar;
	GZRT_P_ACTION	action;
};

/*
 *
*  NOTICE: This file is for use by both the GZRT utility and
 * eventual plugins. Hence, function definitions and further
*  includes only apply when compiling GZRT.
 *
*/

#ifdef IN_GZRT

/* Mem management */
#include <app/mem.h>

/* Function declarations */
void        gzrt_load_plugins        ( void                        );
GtkWidget * gzrt_plugins_menu        ( void                        );
int         gzrt_plugins_count       ( void                        );
void        gzrt_call_plugin         ( void * RomSpec, void * file );
void        gzrt_plugin_cleanup      ( struct PluginTransac * t    );
GtkWidget * gzrt_plugins_preferences ( int action                  );
void        gzrt_set_default_plugin  ( GtkWidget * window          );
char *      gzrt_plugin_curname      ( void                        );

#endif

#endif /* __GZRT_PLUGINS_H */
