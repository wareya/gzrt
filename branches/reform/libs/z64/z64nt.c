/****************************
* Zelda 64 Filename Library *
****************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <z64nt.h>

/* Constants */
#define NAME_TABLE_START 	0x0000BE80UL
#define NAME_TABLE_END		0x00012312UL
#define SIZE				(NAME_TABLE_END - NAME_TABLE_START)
#define COUNT				1532

/* File entry */
struct Zelda64FileEntry
{
	unsigned	id;
	char		name[32];
};

/*
	This is -only- valid for the debug ROM.
*/

/* Create an instance from a file handle */
struct Zelda64NameTable *
z64nt_open ( FILE * handle )
{
	unsigned char * buffer, * seek;
	Z64NT		  * ret;
	unsigned		i;
	
	/* Allocate memory for handle */
	ret = calloc( sizeof(Z64NT), 1 );
	
	/* Initialize the handle */
	ret->amount = COUNT;
	
	/* Allocate memory for buffer */
	buffer = malloc( SIZE );
	
	/* Seek to the start of the table */
	fseek( handle, NAME_TABLE_START, SEEK_SET );
	
	/* Read in the table */
	fread( buffer, 1, SIZE, handle );
	
	/* Copy each string to the hash table */
	for( seek = buffer, i = 0; i < ret->amount; i++ )
	{
		struct Zelda64FileEntry * t = malloc( sizeof(struct Zelda64FileEntry) );
		
		/* Set the entry data */
		t->id = i;
		seek += sprintf(t->name, "%s", seek );
		
		/* Store the string in the hash table */
		ret->names = g_list_append( ret->names, t );
		
		/* Seek past next null bytes */
		while( !*seek && seek < buffer + SIZE )
			seek++;
	}
	
	/* Free resources */
	free( buffer );
	
	/* Return handle */
	return ret;
}

/* For the below function */
static void print_entry ( gpointer * data, gpointer * user )
{
	struct Zelda64FileEntry * t = (struct Zelda64FileEntry *)data;
	
	printf( "[%4u] %s\n", t->id, t->name );
}

/* Dump a name table */
void z64nt_dump ( Z64NT * h )
{
	g_list_foreach( h->names, (GFunc)print_entry, NULL );
}

/* Get filename by id */
const char * z64nt_filename ( Z64NT * h, int id )
{
	GList * j = g_list_nth( h->names, id );
	
	return ((struct Zelda64FileEntry *)(j->data))->name;
}

/* Search for filename */
static int search_name ( gpointer a, gpointer b )
{
	struct Zelda64FileEntry * j = a;
	
	return strcmp( j->name, (char*)b );
}

/* Get id from filename */
int z64nt_id ( Z64NT * h, char * name )
{
	GList * result = g_list_find_custom( h->names, name, (GCompareFunc)search_name );
	
	return g_list_position( h->names, result );
}
