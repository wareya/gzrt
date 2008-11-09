/********************
* Memory management *
********************/
#include <gzrt.h>
#include <errno.h>

typedef struct
{
	unsigned size;
	void   * addr;
}
MEM;

/* Debuggan' */
#ifdef GZRT_MEMDBG
# define GZRT_MEMORY_DEBUG(x, ...) { fprintf( stderr, "[%s:%4u] " x "\n", __FILE__, __LINE__,  ##__VA_ARGS__ ); fflush(stderr); }
#else
# define GZRT_MEMORY_DEBUG(x, ...)
#endif
#define ERROR(x, ...)	{ fprintf( stderr, "[%s:%4u] " x "\n", __FILE__, __LINE__,  ##__VA_ARGS__ ); fflush(stderr); }

/* Storage */
static GList  * blocks;
static unsigned mem_use;

/* Store a memory entry */
static inline void gzrt_mem_store ( unsigned size, void * addr )
{
	MEM * d = malloc( sizeof(MEM) );
	d->size = size;
	d->addr = addr;
	
	/* Append element to the list */
	blocks = g_list_append( blocks, d );
}

/* Allocate memory, but store usage info */
void * gzrt_malloc ( unsigned size )
{
	void * a;
	
	/* Dword align */
	if( size % 8 )
		size = ((size >> 3) << 3) + 8;
	
	/* Allocate */
	if( !(a = malloc( size )) )
	{
		/* Not good */
		ERROR( "Unable to allocate %u bytes of memory. Error:\n%s", size, strerror(errno) );
		return NULL;
	}
	
	/* Update total */
	mem_use += size;
	
	/* Store it */
	gzrt_mem_store( size, a );
	
	/* Debug message */
	GZRT_MEMORY_DEBUG( "0x%08X: allocated %u bytes. Total is now %u.", a, size, gzrt_mem_use() );
	
	/* Return memory region */
	return a;
}

/* Allocate zero'd memory */
void * gzrt_calloc ( unsigned size )
{
	void * a;
	
	/* Dword align */
	if( size % 8 )
		size = ((size >> 3) << 3) + 8;
	
	/* Allocate */
	if( !(a = calloc( size, 1 )) )
	{
		/* Not good */
		ERROR( "Unable to allocate %u bytes of memory. Error:\n%s", size, strerror(errno) );
		return NULL;
	}
	
	/* Update total */
	mem_use += size;
	
	/* Store it */
	gzrt_mem_store( size, a );
	
	/* Debug message */
	GZRT_MEMORY_DEBUG( "0x%08X: allocated %u bytes. Total is now %u.", a, size, gzrt_mem_use() );
	
	/* Return memory region */
	return a;
}

/* Find a malloc()'d block in the list */
static int find_malloc_block ( MEM * h, void * a )
{
	if( !h )
		return -1;
	
	if( h->addr == a )
		return 0;
	else
		return -1;
}

/* Free a memory block */
void gzrt_free ( void * pass )
{
	GList * result;
	MEM * k;
	
	if( !pass )
		return;
	
	result = g_list_find_custom( blocks, pass, (GCompareFunc)find_malloc_block );
	
	if( !result ) {
		return;
	}
	
	k = result->data;
	
	mem_use -= k->size;
	
	free( k->addr );
	
	blocks = g_list_remove( blocks, k );
	
	free( k );
}

/* Return the amount of memory currently allocated */
unsigned gzrt_mem_use ( void )
{
	return mem_use;
}
