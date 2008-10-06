/*********************
* GZRT Plugin System *
*********************/
#ifndef __GZRT_PLUGINS_H
#define __GZRT_PLUGINS_H

#ifdef __GZRT_H

void        gzrt_load_plugins  ( void );
GtkWidget * gzrt_plugins_menu  ( void );
int         gzrt_plugins_count ( void );

#endif

/* File spec */
struct PluginFileSpec
{
	/* File info */
	int 	    id;
	char      * filename;
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
	int (*init)     ( void                    );
	int (*menu_bar)	( void                    );
	int (*action)	( struct PluginFileSpec * );
};

#endif /* __GZRT_PLUGINS_H */
