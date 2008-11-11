/******************************
* Zelda 64 Filesystem Library *
******************************/
#include <z64.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <z64yaz0.h>
#include <z64fs.h>

/* Constants */
#define SEARCH_STRING   "zelda@"
#define CHUNK_SIZE      (512 * 1024)
#define END_ADDR        (1 * 1024 * 1024)

/* Macros */
#define U32(x)      ((x)[0] << 24 | (x)[1] << 16 | (x)[2] << 8 | (x)[3])

/* Create a filesystem context */
struct Zelda64FileTable *
z64fs_open ( FILE * handle )
{
    unsigned char * buffer, * seek;
    Z64FS         * ret;
    unsigned        i, count, tstart, k;
    unsigned        dmad_start, dmad_end;
    
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
    
    /* All done */
    free( buffer );
    return ret;
    
    /* ~~ */
    
    /* Filesystem not found */
fs_not_found:
    free( buffer );
    free( ret    );
    return NULL;
}

/* Return the decompressed size of a filesystem */
unsigned z64fs_size_virt ( Z64FS * h )
{
    unsigned i, total = 0;
    
    /* Loop through each entry */
    for( i = 0; i < z64fs_entries(h); i++ )
    {
        /* Does this file exist? */
        if( ZFileExists(h, i) )
            total += ZFileVirtSize(h, i);
    }
    
    return total;
}

/* Return the physical size of a filesystem */
unsigned z64fs_size_phys ( Z64FS * h )
{
    unsigned i, total = 0;
    
    /* Loop through each entry */
    for( i = 0; i < z64fs_entries(h); i++ )
    {
        /* Does this file exist? */
        if( ZFileExists(h, i) )
            total += ZFileRealSize(h, i);
    }
    
    return total;
}

/* Close a handle */
void z64fs_close ( Z64FS * h )
{
    free( h->files );
    free( h );
}

/* Read a file */
void z64fs_read_file ( Z64 * h, int id, unsigned char * dest )
{
	const Z64FSEntry * f = z64fs_file( h->fs, id );
	
	fseek( h->handle, f->start, SEEK_SET );
	fread( dest, 1, Z_FILESIZE_PHYS(f), h->handle );
	
	/* Compressed? */
	if( Z_COMPRESSED(f) )
	{
		unsigned char * tmp = malloc( 1 * 1024 * 1024 );
		z64yaz0_decode( dest + 16, tmp, Z_FILESIZE_VIRT(f) );
		memcpy( dest, tmp, Z_FILESIZE_VIRT(f) );
		free( tmp );
	}
}
