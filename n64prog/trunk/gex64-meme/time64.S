/*
   To ensure binary compatibility with the code compiled in N64 games, you need
   to compile 32-bit code (ironically enough).
   
   This source file is here so I can quickly get some 64-bit stuff out of the
   way.
*/
#include "mips.h"

	.set	noreorder

	.global bigTimeToFloat
	.ent	bigTimeToFloat
	
bigTimeToFloat:
	
	# Load the 64-bit word
	ld			$v0, ($a0)
	
	# Send it to a floating point register
	dmtc1		$v0, $f0
	
	# Convert it to a single float
	cvt.s.l 	$f2, $f0
	
	# Load our conversion value
	li			$v0, 0x4C3532B8
	mtc1		$v0, $f0
	
	# Return
	jr			$ra
	div.s		$f0, $f2, $f0	# And divide!

	.end	bigTimeToFloat
	
