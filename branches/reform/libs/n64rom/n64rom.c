/**************************
* Nintendo 64 ROM Library *
**************************/
#include <n64rom.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Load a word from a byte array */
#define U32(x)	((x)[0]<<24 | (x)[1]<<16 | (x)[2]<<8 | (x)[3])

/* Load a ROM */
N64Rom * n64rom_load ( char * filename )
{
	N64Rom * ret;
	int i;
	
	/* Allocate memory */
	if( !(ret = malloc( sizeof(N64Rom) )) )
		return NULL;
	
	/* Open ROM */
	if( !(ret->handle = fopen(filename, "rb+")) )
	{
		free( ret );
		return NULL;
	}
	
	/* Get filesize */
	fseek( ret->handle, 0, SEEK_END );
	ret->filesize = ftell( ret->handle );
	fseek( ret->handle, 0, SEEK_SET );
	
	/* Read the file */
	fread( ret->makerom, 1, sizeof(ret->makerom), ret->handle );
	
	/* Store original endian */
	ret->endian = ret->makerom[0];
	
	/* What endian is it? Do we need to swap? */
	if( ret->makerom[0] != N64_ENDIAN_BIG )
	{
		/* yeah, we do */
		if( !n64_byteswap( ret->makerom, sizeof(ret->makerom), 
			N64_ENDIAN_BIG, ret->makerom[0] ) )
		{
			/* Something went wrong */
			fclose( ret->handle );
			free( ret );
			return NULL;
		}
	}
	
	/* Read in header elements */
	for( i = 0; i < 9; i++ )
		((u32*)&ret->head)[i] = U32(&ret->makerom[i*4]);
	
	/* ROM name */
	memcpy( ret->head.name, ret->makerom + 0x20, sizeof(ret->head.name) );
	
	/* Rest */
	ret->head.Unknown2 = U32(&ret->makerom[0x34]);
	memcpy( ret->head.Unknown3, ret->makerom + 0x38, sizeof(ret->head.Unknown3) );
	memcpy( ret->head.code, ret->makerom + 0x3B, sizeof(ret->head.code) );
	ret->head.Unknown4 = ret->makerom[0x3F];
	
	/* Store filename */
	ret->filename = strdup( filename );
	
	/* Return it */
	return ret;
}

/* Close */
void n64rom_close ( N64Rom * h )
{
	free( h->filename );
	fclose( h->handle );
	free( h );
}
