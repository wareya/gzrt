/**********************
* GHE File Operations *
**********************/
#include <ghe.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Currently open files */
struct GheFileStack
{
	struct GheFileStack * prev;
	struct GheFileStack * next;
	GheFile * f;
};

/* Currently open buffers */
struct GheBufferStack
{
	struct GheBufferStack * prev;
	struct GheBufferStack * next;
	GheBuffer * b;
};  

/* Size of hash table */
#define HASH_TABLE_SIZE 128

/* Hash tables for open files/buffers */
static struct GheFileStack   * htabfiles[HASH_TABLE_SIZE];
static struct GheBufferStack * htabbuffs[HASH_TABLE_SIZE];

/* Return an approximate hash for a string */
static unsigned
hash ( GheFile * f )
{
	unsigned ret;
	char * str = f->hname;
	
	for( ret = 0; *str != '\0'; str++ )
		ret = *str + 31 * ret;
	
	return ret % HASH_TABLE_SIZE;
}

/* Register a file handle */
int ghe_file_register ( GheFile * f )
{
	unsigned h = hash(f);
	struct GheFileStack * new = malloc( sizeof(struct GheFileStack) );
	struct GheFileStack * p = htabfiles[h];
	
	/* Is this entry NULL? */
	if( !p )
	{
		new->prev = NULL;
		new->next = NULL;
		new->f = f;
		htabfiles[h] = new;
		
		return TRUE;
	}
	
	/* Nope, find the end */
	do
	{
		/* Check filename */
		if( !strcmp(f->hname, p->f->hname) )
		{
			free( new );
			return FALSE;
		}
		
		/* Go to next */
		if( p->next )
			p = p->next;
	}
	while( p->next );
	
	/* We're at the end, append our own */
	new->prev = p;
	new->next = NULL;
	new->f = f;
	p->next = new;
	
	/* Finished */
	return TRUE;
}

/* Open a file */
GheFile * ghe_file_open ( char * path, char * bname )
{
	GheFile * ret = calloc( sizeof(GheBuffer), 1 );
	
	if( !(ret->handle = fopen(path, "rb+")) )
		return NULL;
	
	/* Initialize */
	ret->path = strdup(path);
	ret->position = 0;
	ret->filename = filename(ret->path);
	
	/* Set name */
	if( !bname )
	{
		ret->hname = ret->filename;
	}
	else
	{
		ret->hname = strdup(bname);
	}
	
	/* Register it */
	if( !ghe_file_register(ret) )
	{
		ghe_debug( "File \"%s\" already open.", path );
		free( ret->path );
		if( bname )
			free( ret->hname );
		free( ret );
		return NULL;
	}
	
	/* Debug notice */
	ghe_debug( "(GheFile*)0x%08X for file \"%s\".", ret, path );
	
	return ret;
}

/* Create a new data buffer */
GheBuffer * ghe_buffer_new ( void )
{
	GheBuffer * ret = calloc( sizeof(GheBuffer), 1 );
	
	return ret;
}

/* Append data */
int ghe_buffer_append ( GheBuffer * b, const ubyte * data, int size )
{
	struct GheBufferPiece 
	* e = calloc( sizeof(struct GheBufferPiece), 1 ),
	* seeker;
	
	/* Initialize it */
	e->length = size;
	e->data = (ubyte*)data;
	e->parent = b;
	
	/* Set information */
	b->size += size;
	b->partcount++;		
	
	/* First piece? */
	if( !b->parts )
	{
		/* Yep */
		b->parts = e;
		return TRUE;
	}
	
	/* Nope - find end of linked list */
	seeker = b->parts;
	while( seeker->next )
		seeker = seeker->next;
	
	/* Set */
	e->prev = seeker;
	seeker->next = e;
	return TRUE;
}

