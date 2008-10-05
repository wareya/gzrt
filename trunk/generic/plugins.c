/*********************
* GZRT Plugin System *
*********************/
#include <gzrt.h>
#include <dirent.h>
#include <dlfcn.h>

#define PLUGINS_DIR	"plugins"

typedef struct
{
	/* Dynamic library context */
	void * dl;
	
	/* Filename */
	char * fn;
	
	/* Next plugin */
	void * next;
}
PLUGINS;


static PLUGINS		plugins;
static int			total;

/* Add a plugin to list */
static void add_plugin ( char * name, void * p )
{
	PLUGINS * j = &plugins;
	
	/* Discover end of list */
	while( j->next )
		j = j->next;
	
	/* Is this instance used? */
	if( !j->dl )
	{
		/* Nope */
		j->dl = p;
		j->fn = strdup( name );
	}
	else
	{
		/* Yes, new space */
		j->next = calloc( sizeof(PLUGINS), 1 );
		j = j->next;
		
		/* Store */
		j->dl = p;
		j->fn = strdup( name );
	}
	
	/* Increase total */
	total++;
}

/* Load a plugin + append to list */
static int load_plugin ( char * name )
{
	void * p;
	
	/* Open */
	if( !(p = dlopen(name, RTLD_LOCAL | RTLD_LAZY)) )
		return 0;
	
	/* Loaded - append to list */
	add_plugin( name, p );
	
	/* Success */
	return 1;
}

/* Load plugins */
void gzrt_load_plugins ( void )
{
	DIR				* handle;
    struct dirent	* ent;
	char			  buffer[512];
	
	/* Status message */
	GZRTD_MESG( "Loading plugins from dir \"%s\".", PLUGINS_DIR );
	
	/* Open directory */
	if( !(handle = opendir(PLUGINS_DIR)) )
	{
		GZRTD_MESG( "Failed to open directory for reading." );
		return;
	}
	
	/* Loop through each file */
	while( (ent = readdir(handle)) != NULL )
	{
		/* No "." or ".." */
		if( !strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..") )
			continue;
		
		/* Concatenate */
		sprintf( buffer, "%s" GZRT_SLASH "%s", PLUGINS_DIR, ent->d_name );
		
		/* Load it */
		if( !load_plugin( buffer ) )
		{
			GZRTD_MESG( "Error: %s", dlerror() );
			return;
		}
		else
			
			GZRTD_MESG( "Loaded plugin \"%s\".", buffer );
	}
}
