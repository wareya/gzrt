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

/* Open a new ROM */
GzrtFile * gzrt_file_open ( char * path )
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
    
    /* Check that it's an N64 ROM... */
    if( !(ret->rom = n64rom_load(path)) )
    {
        /* Probably not */
        gzrt_message( MSG_ERROR, "Error", "The N64 ROM library was unable to open \"%s\":\n%s",
        path, n64rom_error() );
        
        free( ret );
        
        return NULL;
    }
    
    /* Check that it's a Zelda ROM */
    if( !(ret->z64 = z64_open(ret->rom)) )
    {
        /* Probably not */
        gzrt_message( MSG_ERROR, "Error", "The Zelda 64 library was unable to open \"%s\":\n%s",
        path, "unimplemented" );
        
        /* Close ROM handle */
        n64rom_close( ret->rom );
        
        free( ret );
        
        return NULL;
    }
    
    /* Append to list of open files */
    files = g_list_append( files, ret );
    
    DEBUG( "Now managing \"%s\"", ret->path );
    
    /* Return final struct */
    return ret;
}

/* Close a ROM */
void gzrt_file_close ( GzrtFile * file )
{
    free( file->path );
    z64_close( file->z64 );
    n64rom_close( file->rom );
    free( file );
}
