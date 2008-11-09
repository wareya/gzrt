/************************************
* Wrapper for Wareya's Model Viewer *
************************************/
#include <stdio.h>
#include <gzrtplugin.h>
#include <pthread.h>

#define PYFILE   "plugins/zvf/zveiwfile.py"
#define FILENAME "_tmp.bin"

int init ( const struct Functions * f );
int show ( struct PluginTransac * t );

/* Plugin information */
struct PluginMeta gzrt_plugin_info =
{
	"ZVF",
	"ZeldaViewFile",
	"Spinout",
	"",
	"v0",
	
	/* Description */
	NULL,
	
	/* Init, menu & file action funcs */
	init, NULL, show
};

/* GZRT inherited function storage */
struct Functions * functions;

/* Waiting thread */
pthread_t	thread;

/* Init */
int init ( const struct Functions * f )
{
	functions = (void*)f;
}

/* Invoke */
int show ( struct PluginTransac * t )
{
	extern void asdfdhjfh ( struct PluginTransac * t  );
	
	pthread_create( &thread, NULL, asdfdhjfh, (void*)t );
}

/* */
void asdfdhjfh ( struct PluginTransac * t  )
{
	FILE * h;
	char buffer[128];
	
	/* write OoT file */
	if( !(h = fopen(FILENAME, "wb")) )
		return;
	fwrite( t->file->file, t->file->filesize, 1, h );
	fclose( h );
	
	/* Call it */
	sprintf( buffer, "\"plugins/zvf/ZeldaFileViewer 0.0/zfileviewer.exe\" %s", FILENAME );
	printf( buffer );
	fflush(stdout);
	system( buffer );
	
	/* Cleanup */
	system("del " PYFILE   );
	system("del " FILENAME );
	
	functions->close( t );
}
