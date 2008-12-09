#include "regs.h"

init:
    
    # Grab some stack space...
    addi    $sp, -4
    
    # Store the return address
    sw      $ra, 0($sp)
    
    # Initialize data pointer
    la      $gp, _gp
    
    # Clear BSS
    la      $v0, __bss_start
    la      $v1, __bss_end - 8
bss_clear:
    sw      $zero, ($v0)
    addi    $v0, 4
    bltu    $v0, $v1, bss_clear
    
    # Call our function
    jal     n64start
    
    # Restore stuff
    lw      $ra, 0($sp)
    
    # Restore stack pointer
    addi    $sp, 4
    
    # Return
    jr      $ra

