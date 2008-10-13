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
};

/* Global configuration struct */
extern struct GZRTConfig GZRTConfig;

/* Functions */
int gzrt_config_load ( void );

#endif /* __GZRT_SETTINGS_H */
