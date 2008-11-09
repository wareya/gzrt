#ifndef __Z64T_H
#define __Z64T_H

/* OoT controller layout */
typedef struct
{
    /* All the other buttons; described above */
    u16 pad;
    
    /* Joystick */
    signed x : 8;
    signed y : 8;
} Controller;

/* Link's coordinates (raw - not float) */
typedef struct
{
    u32 x;
    u32 y;
    u32 z;
} CoordR;

/* Button colors */
typedef struct
{
    u8 P0;
    u8 r;
    u8 P1;
    u8 g;
    u8 P2;
    u8 b;
} Color;

/* Status word */
typedef struct
{
    /* Current slot */
    u8 curslot;
    
    /* Counter (free use) */
    u8 count;
    
    /* Flags */
    u16 flags;
} THandle;

/* Constants - pointers */
#define Z64T_CNST_BUTTON    0x801C84B4
#define Z64T_CNST_LINKXYZ   0x801DAA54
#define Z64T_CNST_RUPEE     0x8011A604

/* Size of the coordinates storage */
#define Z64T_LOC_AMT    8

#endif
