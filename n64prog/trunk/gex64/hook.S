# Hook for Gex64 Hack
# Function: 0x8003F458
# Call loc: 0x8003BDB4

#include "mips.h"
#include "gex.h"

    .ent    hookFunc
    .global hookFunc

hookFunc:

    # Preserve 
    la      $v0, RetAddrStor
    sw      $ra, ($v0)
    
    # Call the original function
    jal     (__func_addr_gexDrawText & 0x003FFFFF)
    
    # Grab stack space...
    addiu   $sp, -4
    
    # Preserve return value
    sw      $v0, ($sp)
    
    # Call our main
    jal     gexMain
    
    # Load return address
    la      $v0, RetAddrStor
    lw      $ra, ($v0)
    
    # Load return value
    lw      $v0, ($sp)
    
    # Update stack pointer
    addiu   $sp, 4
    
    # Return
    jr      $ra 
    
    # Storage
RetAddrStor:
    .word 0
    
    .end hookFunc
