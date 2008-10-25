/***********************
* Zelda 64 Actor Table *
***********************/
#include <z64.h>
#include <stdio.h>
#include <glib.h>
#include <string.h>
#include <stdlib.h>

#define U32(x) ((x)[0] << 24 | (x)[1] << 16 | (x)[2] << 8 | (x)[3])

/* Actor table identifier - it lies immediately after this */
static const guint8 
table_ident[] =
{
	0xE2, 0x00, 0x1E, 0x01, 0x00, 0x00, 0x00, 0x01,
    0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

/* The list terminator (as far as I know) */
static const guint8
table_term[] = 
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x05,
    0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x10,
	0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x40 
};


Z64AT * z64at_open ( Z64 * h )
{
	int i;
	Z64AT * ret;
	
	/* We need the code file to be identified */
	if( !z64_discover_code( h ) )
		return NULL;
	
	/* Allocate us some memory */
	if( !(ret = calloc( sizeof(Z64AT), 1 )) )
		return NULL;
	
	/* Scan through it for the identifier */
	for( i = 0; i < Z_FILESIZE_VIRT(h->f_code); i += 8 )
		if( !memcmp( h->f_code_data + i, table_ident, sizeof(table_ident) ) )
			goto found_table;
			
	/* Not found */
	free( ret );
	return NULL;
	
	/* Located the table */
found_table:
	ret->start = i + sizeof(table_ident) + 0x30;
	
	/* Find the end of the table */
	for( ; i < Z_FILESIZE_VIRT(h->f_code) - sizeof(table_term); i += 8 )
		if( !memcmp( h->f_code_data + i, table_term, sizeof(table_term) ) )
			goto found_end;
	
	/* We've reached the end... */
	free( ret );
	return NULL;
	
	/* Located ending */
found_end:
	ret->end = i;
	
	/* Read in the table */
	int size = sizeof( struct ActorEntry ) * ((ret->end - ret->start) / 32);
	ret->entries = malloc( size );
	memset( ret->entries, 0xAB, size );
	for( i = ret->start; i < ret->end; i += 32 )
	{
		int k;
		
		for( k = 0; k < 8; k++ )
			((guint32*)ret->entries)[(i - ret->start)/32 + k] = U32( h->f_code_data + i + k * 4 );
	}
	
	return ret;
}
