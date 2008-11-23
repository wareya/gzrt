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

/* Open a file */
GheFile * ghe_file_open ( char * path )
{
	GheFile * ret = calloc( sizeof(GheBuffer), 1 );
	
	if( !(ret->handle = fopen(path, "rb+")) )
		return NULL;
	
	/* Initialize */
	ret->path = strdup(path);
	ret->position = 0;
	
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

