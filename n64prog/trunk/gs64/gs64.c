/*************************
* Builtin Gameshark Hack *
*************************/
#include "gs64.h"

/* Local variables */
static int loaded;
static struct Code * ptr;
static int count;

/* Copy codes */
static void gs_load ( void )
{
	extern void * end;
	
	/* Load count */
	count = *((int*)GS_ROM_OFFSET);
	
	/* Load address to end of program */
	ptr = end;
	
	/* Tag on data */
	dma_read( ptr, (void*)((GS_ROM_OFFSET & 0x0FFFFFFF) | 0x10000000), count * 8 );
}

/* Main function */
void n64start ( void )
{
	/* Are the codes loaded? */
	if( !loaded )
	{
		loaded = 1;
		gs_load();
	}
	
	/* Apply codes */
	gs_process( ptr, count );
}
