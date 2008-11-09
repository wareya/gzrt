/******************************
* Zelda 64 Filesystem Library *
******************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <z64fs.h>
#include <z64yaz0.h>

/* Constants */
#define SEARCH_STRING   "zelda@"
#define CHUNK_SIZE      (512 * 1024)
#define END_ADDR        (1 * 1024 * 1024)

/* Macros */
#define U32(x)      ((x)[0] << 24 | (x)[1] << 16 | (x)[2] << 8 | (x)[3])

/* Create a filesystem context */
struct Zelda64FileTable *
z64fs_open ( char * filename )
{
    unsigned char * buffer, * seek;
    Z64FS         * ret;
    unsigned        i, count, tstart, k;
    unsigned        dmad_start, dmad_end;
    FILE          * handle;
    
    /* Open file */
    if( !(handle = fopen(filename, "rb")) )
        return NULL;
    fseek( handle, 0, SEEK_SET );
    
    /* Create return */
    ret = calloc( sizeof(Z64FS), 1 );
    
    /* Allocate buffer memory */
    buffer = malloc( CHUNK_SIZE );
    
    /* Seek past standard boot routines and such */
    fseek( handle, 0x1060, SEEK_SET );
    
    /* Fill buffer */
searchloop:
    fread( buffer, 1, CHUNK_SIZE, handle );
    
    /* Scan through it in search of key */
    for( i = 0; i < CHUNK_SIZE; i += 16 )
        
        /* Compare */
        if( !strncmp( SEARCH_STRING, buffer + i, 6 ) )
            
            /* Found */
            goto fs_found;
    
    /* Nothing found as of yet, check conditions */
    if( ftell(handle) + CHUNK_SIZE >= END_ADDR )
        goto fs_not_found; /* nothing */
    
    /* Otherwise, continue */
    goto searchloop;
    
    /* ~~ */
    
    /* Found filesystem */
fs_found:
    
    /* Repoint buffer */
    fseek( handle, ftell(handle) - CHUNK_SIZE + i, SEEK_SET );
    fread( buffer, 1, CHUNK_SIZE, handle );
    
    /* Read creator */
    seek  = buffer;
    seek += sprintf( ret->creator, "%s", buffer );
    
    /* Seek ahead to date */
    while( !*seek && seek - buffer < 64 )
        seek++;
    
    /* Copy it */
    seek += sprintf( ret->date, "%s", seek );
    
    /* Discover entry */
    for( k = ((seek - buffer >> 4) + 1) << 4; ; k += 16 )
        if( U32(&buffer[k + 4]) == 0x00001060 )
        {
            tstart = k;
            break;
        }
    
    /* Read address of DMA data */
    dmad_start = U32(buffer + tstart + 2 * 16);
    dmad_end   = U32(buffer + tstart + 2 * 16 + 4);
    
    /* Set filecount */
    for( count = 1; U32(&buffer[tstart+count*16]); count++ );
        ret->filecount = count;
    
    /* Set address */
    ret->start = ftell(handle) - CHUNK_SIZE + tstart;
    ret->end   = ret->start + count * 16;
    
    /* Allocate memory for final storage */
    ret->files = malloc( ret->end - ret->start );
    
    /* Fill */
    for( i = 0; i < ret->end - ret->start; i += 16 )
    {
        ret->files[i/16].vstart = U32(buffer + tstart + i     );
        ret->files[i/16].vend   = U32(buffer + tstart + i +  4);
        ret->files[i/16].start  = U32(buffer + tstart + i +  8);
        ret->files[i/16].end    = U32(buffer + tstart + i + 12);
    }
    
    /* Store file handle */
    ret->fhandle = handle;
    
    /* All done */
    free( buffer );
    return ret;
    
    /* ~~ */
    
    /* Filesystem not found */
fs_not_found:
    fflush(stdout);
    free( buffer );
    free( ret    );
    return NULL;
}

/* Return a pointer to the file entry of an id */
const Z64FSEntry * z64fs_file ( Z64FS * h, int id )
{
    return &h->files[id];
}

/* Read a file based on ID (and decompress, if necessary */
void z64fs_read_file ( Z64FS * h, int id, unsigned char * dest )
{
    Z64FSEntry * f = (void*)z64fs_file( h, id );
    
    /* Read the file */
    fseek( h->fhandle, ZFileRealStart(h, id), SEEK_SET );
    fread( dest, ZFileRealSize(h, id), 1, h->fhandle );
    
    /* Do we need to decompress it? */
    if( ZFileIsCompressed(h, id) && !strncmp("Yaz0", dest, 4) )
    {
        unsigned char * tmp = malloc(ZFileVirtSize(h, id));
        
        /* Decode it */
        z64yaz0_decode( dest + 16, tmp, ZFileVirtSize(h, id) );
        
        /* Copy it to destination */
        memcpy( dest, tmp, ZFileVirtSize(h, id) );
        
        /* Free temp */
        free(tmp);
    }
}

/* Close a handle */
void z64fs_close ( Z64FS * h )
{
    fclose( h->fhandle );
    free( h->files );
    free( h );
}

