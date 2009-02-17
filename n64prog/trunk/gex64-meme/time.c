#include <stdio.h>
#include <stdint.h>
#include "gex.h"
#include "time.h"

/* ----------------------------------------------
   Quick implementation of time functions
   ---------------------------------------------- */

static u64 full_count;
static u32 last_count;

float getTimeNow ( void )
{
	u32 cur;
	
	/* Get current count */
	cur = osGetCount();
	
	/* Did we set anything last time? */
	if( !last_count )
	{
		/* Nope, this'll do then */
		full_count = cur;
	}
	else
	{
		/* Has it increased? */
		if( cur > last_count )
		{
			/* Increment full timer accordingly */
			full_count += cur - last_count;
		}
		else
		{
			/* The counter has rolled over */
			full_count += cur + (-1 - last_count);
		}
	}
	
	/* Store last count */
	last_count = cur;
	
	/* Return float of time */
	return bigTimeToFloat( &full_count );
}
