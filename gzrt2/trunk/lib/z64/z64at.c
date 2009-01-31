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
table_ident_oot[] =
{
	0xE2, 0x00, 0x00, 0x1C, 0xC8, 0x10, 0x49, 0xF8,
	0xE2, 0x00, 0x1E, 0x01, 0x00, 0x00, 0x00, 0x01,
    0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* The list terminator (as far as I know) */
static const guint8
table_term_oot[] = 
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x05,
    0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x10,
	0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x40 
};

static const guint8
table_ident_mm[] =
{
	0x04, 0x09, 0x21, 0xE0, 0x04, 0x09, 0x23, 0xE0,
	0xAA, 0xFF, 0xFF, 0xFF, 0xC8, 0xC8, 0xFF, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0xBF, 0x80, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const guint8 *
table_term_mm = table_term_oot;


Z64AT * 
z64at_open ( Z64 * h )
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
	for( i = 0; i < Z_FILESIZE_VIRT(h->f_code); i += 16 )
	{
		/* Check for OoT's */
		if( !memcmp( h->f_code_data + i, table_ident_oot, sizeof(table_ident_oot) ) )
		{
			ret->start = i + sizeof(table_ident_oot);
			goto found_table;
		}
		
		/* Check for MM's */
		if( !memcmp( h->f_code_data + i, table_ident_mm, sizeof(table_ident_mm) ) )
		{
			ret->start = i + sizeof(table_ident_mm);
			goto found_table;
		}
	}
			
	/* Not found */
	free( ret );
	return NULL;
	
	/* Located the table */
found_table:
	
	/* Find the end of the table */
	for( ; i < Z_FILESIZE_VIRT(h->f_code) - 0x100; i += 16 )
	{
		/* Check for OoT's */
		if( !memcmp( h->f_code_data + i, table_term_oot, sizeof(table_term_oot) ) )
			goto found_end;
		
		/* Check for MM's */
		if( !memcmp( h->f_code_data + i, table_term_mm, sizeof(table_term_oot) ) )
			goto found_end;
	}
	
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
			((guint32*)ret->entries)[(i - ret->start)/32*8 + k] = U32( h->f_code_data + i + k * 4 );
	}
	
	return ret;
}

/* Lookup a file offset */
gint
z64at_offset_lookup ( Z64 * h, guint32 vstart )
{
	int i;
	
	for( i = 0; i < Z_AT_COUNT(h); i++ )
		if( h->at->entries[i].addr_start == vstart )
			return i;
	
	return -1; /* Not found */
}

/* Lookup a file*/
gint
z64at_lookup ( Z64 * h, int id )
{
	int i;
	const Z64FSEntry * f = z64fs_file( h->fs, id );
	
	if( !id )
		return -1;
	
	for( i = 0; i < Z_AT_COUNT(h); i++ )
	{
		if( h->at->entries[i].addr_start == f->vstart )
		{
			/* g_print( "Found! [%i]%08X == %08X\n", i, h->at->entries[i].addr_start, f->vstart ); */
			return i;
		}
		
		/* g_print( "%08X - %08X\n", h->at->entries[i].addr_start, h->at->entries[i].addr_end ); */
	}
	
	return -1; /* Not found */
}

/* Get an actor entry */
struct ActorEntry *
z64at_entry_get ( Z64 * h, guint32 id )
{
	return 
	(	/* Watch the limits! */
		id > Z_AT_COUNT(h) ?
		NULL : &h->at->entries[id]
	);
}
