#include <stdio.h>
#include "gex.h"

/* Create our box struct */
gexMakeBox(status_box, 180, 40, 10, 10, 0, 0, 0, 255);

int _start ( void )
{
	char buffer[0x100];
	static int j = 0;
	
	/* Draw box */
	gexDrawBox(&status_box);
	
	/* Prepare text */
	snprintf( buffer, sizeof(buffer), "NUMBER %08X\n", j++ );
	
	/* Print text */
	gexDrawText( buffer, 12, 12, 1 );
	
	return 0;
}
	
