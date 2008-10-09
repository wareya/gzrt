/*********************
* GZRT Plugin System *
*********************/
#ifndef __GZRT_PLUGINS_H
#define __GZRT_PLUGINS_H

/*
 *
*  NOTICE: This file is for use by both the GZRT utility and
 * eventual plugins. Hence, function definitions and further
*  includes only apply when compiling GZRT.
 *
*/

#ifdef __GZRT_H

#include <generic/mem.h>

void        gzrt_load_plugins  ( void        );
GtkWidget * gzrt_plugins_menu  ( void        );
int         gzrt_plugins_count ( void        );
void        gzrt_call_plugin   ( void * file );

#endif

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

/* GZRT inherited functions */
struct Functions
{
	/* Memory management */
	void     * (*malloc) ( unsigned );
	void     * (*calloc) ( unsigned );
	void       (*free)   ( void *   );
	unsigned   (*mused)  ( void     );
	
	/* Debug/error handling */
	void	 * (*error)  ( int, int, char *, ... );
	void	 * (*debug)  ( int, int, char *, ... );
	unsigned	reserved3;
	unsigned	reserved4;
};

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
	int (*init)     ( const struct Functions * ); 
	int (*menu_bar)	( void                     );
	int (*action)	( struct PluginFileSpec *  );
};

/* Free a plugin */
static inline void 
plugin_cleanup ( const struct Functions * f, struct PluginFileSpec * s )
{
	f->free( &s->file );
	f->free( &s       );
}

#endif /* __GZRT_PLUGINS_H */
