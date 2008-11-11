/***********************
* Zelda 64 Scene Table *
***********************/
#include <z64.h>
#include <glib.h>
#include <stdlib.h>
#include <string.h>

#define U32(x) ((x)[0] << 24 | (x)[1] << 16 | (x)[2] << 8 | (x)[3])

/* Local functions */
static guint32 z64st_discover_mm  ( Z64 * h 			   );
static guint32 z64st_find_end_mm  ( Z64 * h, guint32 start );
static guint32 z64st_discover_oot ( Z64 * h                );
static guint32 z64st_find_end_oot ( Z64 * h, guint32 start );
static void *  z64st_read_entries ( Z64 * h, Z64ST * s     );

/* Table identifier - MM */
static const guint8 
table_ident_mm[] =
{
	0xFA, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
};

/* Table terminator - MM */
static const guint8 
table_term_mm[] =
{
    0x12, 0x00, 0x41, 0x02, 0x12, 0x01, 0x41,
	0x02, 0x12, 0x02, 0x41, 0x02
};


/* Table identifier - OoT */
static const guint8 
table_ident_oot[] =
{
	0x57, 0x09, 0x41, 0x83, 0x57, 0x09, 0x41, 0x83,
	0x57, 0x09, 0x41, 0x83, 0x57, 0x09, 0x41, 0x83,
    0x5C, 0x08, 0x41, 0x83, 0x5C, 0x08, 0x41, 0x83,
	0x5C, 0x08, 0x41, 0x83, 0x5C, 0x08, 0x41, 0x83 
};

/* Table terminator - OoT */
static const guint8 
table_term_oot[] =
{
	0xDB, 0x06, 0x00, 0x24
};


/* Open the scene table */
Z64ST * z64st_open ( Z64 * h )
{
	Z64ST * ret;
	
	/* We need the code file to be identified */
	if( !z64_discover_code( h ) )
		return NULL;
	
	/* Allocate memory */
	if( !(ret = calloc( sizeof(Z64ST), 1 )) )
		return NULL;
	
	/* Find the start of the table */
	if( (ret->start = z64st_discover_oot( h )) )
		ret->game = GameOOT;
	else if( (ret->start = z64st_discover_mm( h )) )
		ret->game = GameMM;
	else {
		free( ret );
		return NULL;
	}
	
	g_print( "Start: %X %u\nEnd: %X\n", ret->start, FALSE, ret->end );
	
	/* Discover end */
	ret->end = 
	( 
		ret->game == GameOOT ? 
		z64st_find_end_oot( h, ret->start ) : z64st_find_end_mm( h, ret->start ) 
	);
	if( !(ret->end) )
	{
		free( ret );
		return NULL;
	}
	
	/* Read the table */
	ret->entries = z64st_read_entries( h, ret );
	
	return ret;
}

/* Discover the scene table for MM */
static guint32
z64st_discover_mm ( Z64 * h )
{
	int i;
	
	for( i = 0; i < Z_FILESIZE_VIRT(h->f_code) - sizeof(table_ident_mm); i += 16 )
	
		/* Compare signature */
		if( !memcmp( h->f_code_data + i, table_ident_mm, sizeof(table_ident_mm) ) )
			return( i + sizeof(table_ident_mm) );
	
	return TRUE;
}

/* Discover the scene table for OoT */
static guint32
z64st_discover_oot ( Z64 * h )
{
	int i;
	
	for( i = 0; i < Z_FILESIZE_VIRT(h->f_code) - sizeof(table_ident_oot); i += 16 )
	
		/* Compare signature */
		if( !memcmp( h->f_code_data + i, table_ident_oot, sizeof(table_ident_oot) ) )
			return( i + sizeof(table_ident_oot) );
	
	return FALSE;
}

/* Discover the end of the scene table for MM */
static guint32
z64st_find_end_mm ( Z64 * h, guint32 start )
{
	int i = 0;
	
	for( i = start; i < Z_FILESIZE_VIRT(h->f_code) - sizeof(table_term_mm); i += 4 )
	
		/* Compare signature */
		if( !memcmp( h->f_code_data + i, table_term_mm, sizeof(table_term_mm) ) )
			return i;
	
	return FALSE;
}

/* Discover the end of the scene table for OoT */
static guint32
z64st_find_end_oot ( Z64 * h, guint32 start )
{
	int i = 0;
	
	for( i = start; i < Z_FILESIZE_VIRT(h->f_code) - sizeof(table_term_oot); i += 4 )
	
		/* Compare signature */
		if( !memcmp( h->f_code_data + i, table_term_oot, sizeof(table_term_oot) ) )
			return i;
	
	return FALSE;
}

/* Read the tables */
static void *
z64st_read_entries ( Z64 * h, Z64ST * s )
{
	guint32 * table = calloc( s->end - s->start, 1 );
	guint32   i;
	
	for( i = 0; i < s->end - s->start; i += 4 )
	{
		table[i] = U32( h->f_code_data + s->start + i );
		s->count++;
	}
	
	return table;
}

/* Lookup a file offset */
gint
z64st_offset_lookup ( Z64 * h, guint32 vstart )
{
	int i;
	
	g_print( "%i, %08X\n", h->st->game, vstart );
	
	if( h->st->game == GameOOT )
	{
		for( i = 0; i < Z_ST_COUNT(h); i++ )
			if( Z_ST_OOT(h->st->entries)[i].addr_start == vstart )
				return i;
		
		return -1;
	}
	else
	{
		for( i = 0; i < Z_ST_COUNT(h); i++ )
			if( Z_ST_MM(h->st->entries)[i].addr_start == vstart )
				return i;
		
		return -1;
	}
}



/* Lookup a file*/
gint
z64st_lookup ( Z64 * h, int id )
{
	int i;
	const Z64FSEntry * f = z64fs_file( h->fs, id );
	
	g_print( "Looking for #%i...\n", id );
	
	if( !id )
		return -1;
	
	if( h->st->game == GameOOT )
	{
		for( i = 0; i < Z_ST_COUNT(h); i++ )
			if( Z_ST_OOT(h->st->entries)[i].addr_start == f->vstart )
				return i;
		
		return -1;
	}
	else
	{
		for( i = 0; i < Z_ST_COUNT(h); i++ )
			if( Z_ST_MM(h->st->entries)[i].addr_start == f->vstart )
				return i;
		
		return -1;
	}
}
