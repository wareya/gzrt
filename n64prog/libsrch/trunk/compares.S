##
## Quick functions for comparing
##

#include <n64.h>

##
## Local macros
##

#define START_FUNC(x)       \
    .global     x           \
    .ent        x
#define END_FUNC(x)         \
    .end        x


    

    # Don't reorder our assembly
    .set        noreorder
    
    # We're in the text section
    .text


#
# int __libscmp_u8 ( MS_SEARCH, void *, void *, void * )
#
    
    .global     __libscmp_u8
    .ent        __libscmp_u8

__libscmp_u8:
    
    # Default return value
    addi        v0, $zero, -1

    # Load the separate values
    lbu         t0, (a1)
    lbu         t1, (a1)
    
    # Compare
    beql        t0, t1, $L0
    addi        v0, -1
    
    # Return
$L0:
    jr          $ra
    addi        v0, 1
    
    .end        __libscmp_u8



#
# int __libscmp_u16 ( MS_SEARCH, void *, void *, void * )
#
    
    .global     __libscmp_u16
    .ent        __libscmp_u16

__libscmp_u16:
    
    # Default return value
    addi        v0, $zero, -1

    # Load the separate values
    lhu         t0, (a1)
    lhu         t1, (a1)
    
    # Compare
    beql        t0, t1, $L1
    addi        v0, -1
    
    # Return
$L1:
    jr          $ra
    addi        v0, 1
    
    .end        __libscmp_u16



#
# int __libscmp_u32 ( MS_SEARCH, void *, void *, void * )
#
    
    .global     __libscmp_u32
    .ent        __libscmp_u32

__libscmp_u32:
    
    # Default return value
    addi        v0, $zero, -1

    # Load the separate values
    lwu         t0, (a1)
    lwu         t1, (a1)
    
    # Compare
    beql        t0, t1, $L2
    addi        v0, -1
    
    # Return
$L2:
    jr          $ra
    addi        v0, 1
    
    .end        __libscmp_u32



#
# int __libscmp_u64 ( MS_SEARCH, void *, void *, void * )
#
    
    .global     __libscmp_u64
    .ent        __libscmp_u64

__libscmp_u64:
    
    # Default return value
    addi        v0, $zero, -1

    # Load the separate values
    ld          t0, (a1)
    ld          t1, (a1)
    
    # Compare
    beql        t0, t1, $L3
    addi        v0, -1
    
    # Return
$L3:
    jr          $ra
    addi        v0, 1
    
    .end        __libscmp_u64
