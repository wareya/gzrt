/************************
* GZRT - Plugin manager *
************************/
#ifndef __GZRT_PLUGIN_MGR_H__
#define __GZRT_PLUGIN_MGR_H__

/* ----------------------------------------------
   Constants
   ---------------------------------------------- */

/* Path to plugins relative to GZRT root */
#define GZRT_PLUGIN_PATH	"./plugins"

/* This word is sent to identify a plugin */
#define GZRT_PLUGIN_MAGIC	0x05A5A540


/* ----------------------------------------------
   Structures
   ---------------------------------------------- */

/* Plugin meta information */
struct gzrtPluginMeta
{
    const char * name;
    const char * author;
    const char * group;
    const char * version;
    const char * desc;
};

/* Plugin descriptors */
typedef struct gzrtPlugin
{
    /* Generic information */
    char * filename;
    
    /* Plugin meta information */
    struct gzrtPluginMeta * meta;
    
    /* Process IDs */
    GList * pids;
}
GzrtPlugin;

/* A plugin instance */
typedef struct gzrtPluginInstance
{
	/* File descriptor for communication */
	int fd[2];
	
	/* PID */
	pid_t pid;
	
	/* Plugin descriptor */
	GzrtPlugin * plugin;
}
GzrtPluginInstance;


/* ----------------------------------------------
   IPC Structures
   ---------------------------------------------- */

/* A packet */
typedef struct gzrtPacket
{
	/* Packet magic */
	int magic;			/* See `GZRT_PLUGIN_MAGIC` */
	
	/* Source of packet */
	pid_t origin;
	
	/* Time sent */
	struct timeval time;
	
	/* Length of data */
	int length;
	
	/* Function ID */
	int funct;
	
	/* Data */
	unsigned char data[];
}
GzrtPacket;


/* ----------------------------------------------
   Global functions
   ---------------------------------------------- */

/* Functions -- plugin.c */
extern void gzrt_plugin_manager_init ( void );
extern GList * gzrt_plugin_manager_get_plugins ( void );
extern GList * gzrt_plugin_manager_get_instances ( void );
extern void gzrt_plugin_manager_start_plugin ( GzrtPlugin * P );
extern void gzrt_plugin_manager_kill_all ( void );
extern void gzrt_plugin_manager_kill_plugin ( GzrtPluginInstance * P );
extern void gzrt_plugin_manager_kill_plugin_by_pid ( pid_t pid );
extern void gzrt_plugin_manager_unref_by_pid ( pid_t pid );

/* Shorthand */
#define gzrt_plugin_kill		gzrt_plugin_manager_kill_plugin
#define gzrt_plugin_kill_pid	gzrt_plugin_manager_kill_plugin_by_pid
#define gzrt_plugin_unref_pid	gzrt_plugin_manager_unref_by_pid

#endif /* !__GZRT_PLUGIN_MGR_H__ */
