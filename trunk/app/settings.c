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
	GKeyFile * config    = g_key_file_new();
	GError   * happening = NULL;
	int		   i;
	
	/* Can't load config file? */
	if( !g_key_file_load_from_file( config, CONFIGURATION_FILE, 0, &happening ) )
	{
		GZRTD_MESG( "Error while loading configuration: %s", happening->message );
		g_error_free( happening );
		return FALSE;
	}
	
	/* Restore the values present */
	for( i = 0; i < sizeof(entries) / sizeof(struct ConfEntry); i++ )
	{
		/* Load val */
		const gchar * val = g_key_file_get_value( config, MAIN_GROUP_NAME, entries[i].name, &happening );
		
		/* Did we get it? */
		if( !val )
		{
			GZRTD_MESG( "Error loading key \"%s\": %s", entries[i].name, happening->message );
			g_error_free( happening );
			continue;
		}
		
		/* Sanity check */
		if( !entries[i].value )
		{
			GZRTD_MESG( "This shouldn't happen - \"%s\" has a NULL target.", entries[i].name );
			continue;
		}
		
		/* Is this a string? */
		switch( entries[i].type )
		{
			case tSTRING:
			 /* Yeah, just strdup it */
			 *((char**)entries[i].value) = strdup(val);
			break;
			
			default:
			 GZRTD_MESG( "Unhandled config datatype in \"%s\" (%i)!", entries[i].name, entries[i].type );
		}
	}
	
	/* Finished */
	g_key_file_free( config );
	
	/* Notice */
	GZRTD_MESG( "Configuration loaded successfully. Values:" );
	
	/* Dump the values */
	for( i = 0; i < sizeof(entries)/sizeof(struct ConfEntry); i++ )
		if( entries[i].value )
			GZRTD_MESG( "\"%s\" = \"%s\"", entries[i].name, *(char**)entries[i].value );
		
	/* Good */
	return TRUE;
}

/* Write configuration */
void gzrt_config_save ( void )
{
	FILE * out = fopen( CONFIGURATION_FILE, "wb" );
	int i;
	
	fprintf( out, "[%s]\n", MAIN_GROUP_NAME );
	
	for( i = 0; i < sizeof(entries) / sizeof(struct ConfEntry); i++ )
	{
		if( entries[i].value ) switch( entries[i].type )
		{
			case tSTRING:
			 fprintf( out, "%s = %s\n", entries[i].name, *(char**)entries[i].value );
			break;
			
			default:
			 g_print( "Unhandled type.\n" );
		}
	}
	
	fclose( out );
}

