/************************************
* Wrapper for Wareya's Model Viewer *
************************************/
#include <stdio.h>
#include <gzrtplugin.h>
#include <pthread.h>

#define BINNAME	"_zev.exe"
#define FILENAME "_tmp.bin"

int init ( const struct Functions * f );
int show ( const struct Functions *, const struct RomSpec *, const struct PluginTransac * );

/* Plugin information */
struct PluginMeta gzrt_plugin_info =
{
	"ZEV",
	"Wareya's ZEV",
	"Wareya",
	"",
	"v0",
	
	/* Description */
	NULL,
	
	/* Init, menu & file action funcs */
	init, NULL, show
};

/* GZRT inherited function storage */
struct Functions * functions;

/* ZEV binary */
extern unsigned char executable[];
extern unsigned int  exesize;

/* Waiting thread */
pthread_t	thread;

/* Init */
int init ( const struct Functions * f )
{
	functions = (void*)f;
}

/* Invoke */
int show ( const struct Functions * f, const struct RomSpec * r, const struct PluginTransac * t )
{
	extern void asdfdhjfh ( struct PluginTransac * t  );
	
	pthread_create( &thread, NULL, asdfdhjfh, (void*)t );
}

/* */
void asdfdhjfh ( struct PluginTransac * t  )
{
	FILE * h;
	char buffer[128];
	
	/* write binary */
	if( !(h = fopen(BINNAME, "wb")) )
		return;
	fwrite( executable, exesize, 1, h );
	fclose(h);
	
	/* write OoT file */
	if( !(h = fopen(FILENAME, "wb")) )
		return;
	fwrite( t->file->file, t->file->filesize, 1, h );
	fclose( h );
	
	/* Call it */
	sprintf( buffer, "%s %s 1 yes", BINNAME, FILENAME );
	system( buffer );
	
	/* Cleanup */
	unlink( BINNAME );
	unlink( FILENAME );
	
	functions->close( t );
}
