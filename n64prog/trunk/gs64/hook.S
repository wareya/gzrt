#
# This will write a hook which will handle the gameshark codes.
#  - 0x80002964 <- function to be called
#  - 0x800027A4 <- call location
#

#include "regs.h"
    
    .global hookfunc

hookfunc:
    
    # Store return address
    la      $v0, tmpstorage
    sw      $ra, 0($v0)
    
    # Call function
    jal     0x00002964
    
    # Grab some stack space...
    addi    $sp, 8
    
    # Store return address
    sw      $v0, 0($sp)
    
    # Call our function
    jal     n64main
    
    # Restore return address & value
    lw      $v0, 0($sp)
    la      $v0, tmpstorage
    lw      $ra, 0($v0)
    
    # Restore stack
    addi    $sp, 4
    
    # Return
    jr      $ra
    
tmpstorage:
    .word 0
