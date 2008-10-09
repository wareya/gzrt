/********************
* Memory management *
********************/
#include <gzrt.h>
#include <errno.h>

typedef struct MemDB
{
	unsigned	   size;
	void 	     * ptr;
	struct MemDB * prev;
	struct MemDB * next;
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
static MEM        initial;
static MEM      * last;
static unsigned   mem_use;

/* Store a memory entry */
static void gzrt_mem_store ( unsigned size, void * addr )
{
	MEM * tmp = &initial;
	
	/* Find end of list or empty entry */
	while( tmp->next )
		tmp = tmp->next;
	
	/* Initialized? */
	if( tmp->size )
	{
		/* Create new */
		tmp->next = calloc( sizeof(MEM), 1 );
		tmp->next->prev = tmp;
		
		/* Transfer */
		tmp = tmp->next;
	}
	
	/* Set values */
	tmp->ptr  = addr;
	tmp->size = size;
	
	/* Store last used */
	last = tmp;
}

/* Allocate memory, but store usage info */
void * gzrt_malloc ( unsigned size )
{
	void * a;
	
	/* Word align */
	if( size % 4 )
		size = ((size >> 2) << 2) + 4;
	
	/* Allocate */
	if( !(a = malloc( size )) )
	{
		/* Not good */
		ERROR( "Unable to allocate %u bytes of memory. Error:\n%s", strerror(errno) );
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
	
	/* Word align */
	if( size % 4 )
		size = ((size >> 2) << 2) + 4;
	
	/* Allocate */
	if( !(a = calloc( size, 1 )) )
	{
		/* Not good */
		ERROR( "Unable to allocate %u bytes of memory. Error:\n%s", strerror(errno) );
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

/* Free a memory block */
void gzrt_free ( void * j )
{
	MEM * tmp = last;
	void ** p = j;
	
	/* Are we being passed a NULL pointer? */
	if( !*p )
	{
		/* Not good */
		ERROR( "CRITICAL: NULL passed to gzrt_free()! Fix this!" );
		return;
	}
	
	/* Uninitialize stored entry */
	while( tmp->ptr != *p )
		
		/* Seek to previous */
		if( tmp->prev ) tmp = tmp->prev;
		
	/* Did we find it? */
	if( tmp->ptr == *p )
	{
		MEM * a = tmp->prev, * b = tmp->next;
		unsigned i;
		
		/* Free the pointer */
		free( *p );
		
		/* Update size */
		mem_use -= (i = tmp->size);
		
		/* Unlink entry */
		if( tmp != &initial ) 
		{
			/* Repoint the previous one */
			a->next = b;
			
			/* Free the old one */
			free( tmp );
		}
	
		/* Debug message */
		GZRT_MEMORY_DEBUG( "0x%08X: released, freeing %u bytes. Total is now %u.", *p, i, gzrt_mem_use() );
		
		/* Set pointer to zero */
		*p = NULL;
	}
}

/* Return the amount of memory currently allocated */
unsigned gzrt_mem_use ( void )
{
	return mem_use;
}
