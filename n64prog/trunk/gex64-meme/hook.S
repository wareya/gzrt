# Hook for Gex64 Hack

# We are hooking into the function call to 0x8003F458 at 0x8003BDB4

#include "mips.h"
#include "gex.h"

    .ent    hookFunc
    .global hookFunc

hookFunc:

    # Preserve 
    sw      $ra, (RetAddrStor)
    
    # Call the original function
    jal     0x0003F458
    
    # Grab stack space...
    addiu   $sp, -16
    
    # Preserve return value
    sw      $v0, ($sp)
    
    # Call our main
    jal     gexMain
    
    # Load return address
    lw      $ra, (RetAddrStor)
    
    # Load return value
    lw      $v0, ($sp)
    
    # Update stack pointer
    addiu   $sp, 16
    
    # Return
    jr      $ra 
    
    # Storage
RetAddrStor:
    .word 0
    
    .end hookFunc
