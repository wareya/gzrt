/****************************
* GZRT Configuration System *
****************************/
#ifndef __GZRT_SETTINGS_H
#define __GZRT_SETTINGS_H

/* We need some defs from here */
#include <gzrt.h>

/* Location of config file */
#define CONFIGURATION_FILE	"resources" GZRT_SLASH "gzrt.ini"
#define MAIN_GROUP_NAME		"main"

/* Configuration settings */
struct GZRTConfig
{
	char * default_rom;
	char * default_plugin;
	
	char * handler_zdata;
	char * handler_zactor;
	char * handler_zobj;
	char * handler_zmap;
	char * handler_zscene;
	char * handler_zasm;
};

/* Global configuration struct */
extern struct GZRTConfig GZRTConfig;

/* Functions */
int  gzrt_config_load ( void );
void gzrt_config_save ( void );

#endif /* __GZRT_SETTINGS_H */
