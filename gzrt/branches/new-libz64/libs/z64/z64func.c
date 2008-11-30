/*
	This file contains complex functions that utilize more than one level of the
	Z64 library
*/
#include <z64.h>
#include <stdlib.h>
#include <string.h>

/* Read a file */
void z64fs_read_file ( Z64 * h, int id, unsigned char * dest )
{
	const Z64FSEntry * f = z64fs_file( h->fs, id );
	
	fseek( h->handle, f->start, SEEK_SET );
	fread( dest, 1, ZFSize(h->fs, id), h->handle );
	
	/* Compressed? */
	if( ZFCompressed(h->fs, id) )
	{
		unsigned char * tmp = malloc( 1 * 1024 * 1024 );
		z64yaz0_decode( dest + 16, tmp, ZFvSize(h->fs, id) );
		memcpy( dest, tmp, ZFvSize(h->fs, id) );
		free( tmp );
	}
}
