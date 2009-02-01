/*****************************
* GZRT File (ROM) Management *
*****************************/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <glib.h>
#include <n64rom.h>
#include <z64.h>
#include "gzrt.h"
#include "msg.h"
#include "file.h"

/* List of files that are open */
static GList * files;

/* Open a new Zelda ROM */
GzrtFile * gzrt_file_open ( int what, char * path )
{
    GzrtFile * ret;
    
    ret = calloc( sizeof(GzrtFile), 1 );
    
    /* Store some file info */
    ret->path = strdup(path);
    ret->name = filename(ret->path);
    
    /* Stat the file */
    if( stat(path, &ret->info) )
    {
        /* Didn't work... */
        gzrt_message( MSG_ERROR, "Error", "Couldn't stat file \"%s\":\n%s",
        path, strerror(errno) );
        
        free( ret );
        
        return NULL;
    }
    
    /* Check filesize... */
    if( ret->info.st_size < 16 * 1024 * 1024 )
    {
        /* Didn't work... */
        gzrt_message( MSG_ERROR, "Error", "Filesize inconsistent with a Zelda ROM." );
        
        free( ret );
        
        return NULL;
    }
    
    /* Abnormal filesize */
    if( (ret->info.st_size % (1024)) )
    {
        gzrt_message( MSG_WARNING, "Warning", "Your ROM has an abnormal filesize." );
    }
	
	/* File handle? */
	if( (what & GZRT_FILE_FH) )
	{
		/* Check that we can open it */
		if( !(ret->fh = fopen(path, "r+")) )
		{
			/* Probably not */
			gzrt_message( MSG_ERROR, "Error", "Error while opening \"%s\":\n%s",
			ret->name, strerror(errno) );
		}
	}
    
	/* Load N64 handle? */
	if( (what & GZRT_FILE_ROM) )
	{
		/* Check that it's an N64 ROM... */
		if( !(ret->rom = n64rom_load(path)) )
		{
			/* Probably not */
			gzrt_message( MSG_ERROR, "Error", "The N64 ROM library was unable to open \"%s\":\n%s",
			ret->name, n64rom_error() );
			
			free( ret );
			
			return NULL;
		}
	}
	
	/* Load Z64 handle? */
	if( (what & GZRT_FILE_Z64) )
	{
		/* Check that it's a Zelda ROM */
		if( !(ret->z64 = z64_open(ret->rom)) )
		{
			/* Probably not */
			gzrt_message( MSG_ERROR, "Error", "The Zelda 64 library was unable to open \"%s\":\n%s",
			ret->name, "unimplemented" );
			
			/* Close ROM handle */
			n64rom_close( ret->rom );
			
			free( ret );
			
			return NULL;
		}
	}
	
	/* Store what was loaded */
	ret->what = what;
    
    /* Append to list of open files */
    files = g_list_append( files, ret );
    
    DEBUG( "Now managing \"%s\"", ret->path );
    
    /* Return final struct */
    return ret;
}


/* Close a ROM */
void gzrt_file_close ( GzrtFile * file )
{
	DEBUG( "Freed file \"%s\" from our control.", file->name );
	
	/* Free preserved path */
    free( file->path );
	
	/* File handle? */
	if( file->what & GZRT_FILE_FH )
	  fclose( file->fh );
  
    /* N64 ROM */
	if( file->what & GZRT_FILE_ROM )
	  n64rom_close( file->rom );
    
    /* Z64 library */
	if( file->what & GZRT_FILE_Z64 )
	  z64_close( file->z64 );
	
    /* Free object memory */
    free( file );
	
	/* Remove pointer from list */
	files = g_list_remove( files, file );
}

