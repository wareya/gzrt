/****************************
* GZRT Configuration System *
****************************/
#include <gzrt.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <glib.h>
#include "settings.h"
#include <gui/debug.h>

/* Config */
struct GZRTConfig GZRTConfig = { 0 };

/* Config types */
typedef enum
{
	tPOINTER,
	tSTRING,
	tINT
}
TYPE;

/* Rundown of available configuration strings */
static struct ConfEntry
{
	char * name;
	TYPE   type;
	void * value;
}
entries[] = 
{
	{ "DefaultRom",    tSTRING, &GZRTConfig.default_rom    },
	{ "DefaultPlugin", tSTRING, &GZRTConfig.default_plugin }
};

/* Load configuration */
int gzrt_config_load ( void )
{
	GKeyFile *  config = g_key_file_new();
	GError * happening = NULL;
	int			i;
	
	/* Can't load config file? */
	if( !g_key_file_load_from_file( config, CONFIGURATION_FILE, 0, &happening ) )
	{
		GZRTD_MESG( "Error while loading configuration:", GZRTConfig.default_rom, happening->message );
		GZRTD_MESG( "%s", happening->message );
		return FALSE;
	}
	
	/* Restore the values present */
	for( i = 0; i < sizeof(entries) / sizeof(struct ConfEntry); i++ )
	{
		/* Load val */
		const gchar * val = g_key_file_get_value( config, MAIN_GROUP_NAME, entries[i].name, NULL );
		
		/* Did we get it? */
		if( !val )
			continue;
		
		/* Is this a string? */
		if( entries[i].type == tSTRING )
			
			/* Yeah, just strdup it */
			*((char**)entries[i].value) = strdup(val);
	}
	
	/* Finished */
	g_key_file_free( config );
	
	/* Notice */
	GZRTD_MESG( "Configuration loaded successfully." );
	
	/* Good */
	return TRUE;
}
