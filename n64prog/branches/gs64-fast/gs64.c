/*************************
* Builtin Gameshark Hack *
*************************/
#include <stdio.h>
#include <stdlib.h>
#include "gs64.h"

/* Local variables */
static int loaded;
static struct Code * ptr;
static int count;

/* Copy codes */
static void gs_load ( void )
{
    /* Load count */
    count = *((int*)GS_ROM_OFFSET);
    
    /* Allocate memory */
    ptr = malloc( count * 8 );
    
    /* Tag on data */
    dma_read( ptr, (void*)(GS_ROM_OFFSET + 4), count * 8 );
}

#define JAL(x)  ((3 << 26) | ((((u32)(x)) & 0x1FFFFFFF) >> 2))

/* Initialization function */
void n64start ( void )
{
    extern void hookfunc ( void );
    
    /* No memory clearing! */
    *((u32*)0xA0000484) = 0x3C050030;
    
    /* No clearing GP */
    *((u32*)0xA0002C64) = 0;    
    
    /* Set up a hook */
    *((u32*)0xA00027A4) = JAL(hookfunc);
}

/* Main function */
void n64main ( void )
{
    /* Are the codes loaded? */
    if( !loaded )
    {
        loaded = 1;
        gs_load();
        /* gs_compile( ptr, count ); */ 
    }
    
    /* Apply codes */
    gs_process( ptr, count );
}
