#include <stdio.h>
#include "gex.h"
#include "mips.h"

extern int kprintf ( char *, ... );

/* Create our box struct */
gexMakeBox(status_box, 180, 40, 40, 40, 0, 0, 0, 128);

/* Entry point */
void n64start ( void )
{
    /* Set up hook */
    *((u32*)0xA003BDB4) = JAL(hookFunc);
    
    /* No GP overwriting */
    *((u32*)0xA005DC10) = 0;
}

/* Our main() */
void gexMain ( void )
{
    char buffer[0x100];
    static int count = 0;
    
    /* Draw box  
    gexDrawBox(&status_box); */
    
    /* Prepare text */
    gexSprintf( buffer, "ITERATION %i", count++ );
    
    /* Print text */
    gexDrawText( buffer, 12, 12, 1 );
    
    /* Print text */
    gexDrawText( "[ZZT32", 12, 230, 1 );
	
    kprintf( "OK\n" );
}
