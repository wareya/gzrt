#include "regs.h"

copyback:

    # Target address
    lui     $v0, 0xA000
    ori     $v0, $v0, 0x0400
    
    # Source address
    lui     $v1, 0x1234
    ori     $v1, $v1, 0x4321
    
    # Size
    lui     $a0, 0x1111
    ori     $a0, $a0, 0x1111
    addu    $a0, $a0, $v0
    
    # Copy
cloop:
    lw      $a1, 0($v1)
    sw      $a1, 0($v0)
	
	# Update pointers
    addi    $v0, 4
    addi    $v1, 4
	
	# Make sure what we wrote gets into the cache
    cache	4, ($v0)
	
	# Branch back
    bne     $v0, $a0, cloop
	
    # Call the binary
    lui     $a0, 0x8888
    ori     $a0, $a0, 0x8888
    jalr    $a0
    
    # Go back to the entry point
    j       0x00000400
