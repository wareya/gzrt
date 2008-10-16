#ifndef __LN64_R4000_DIS_H
#define __LN64_R4000_DIS_H

typedef unsigned int u32;

/* Disassembler flags */
enum
{
    /* Register display options */
    MR4KD_RTYPE0  = 0x00000001, /*  K0 */
    MR4KD_RTYPE1  = 0x00000002, /*  21 */
    MR4KD_RPREFIX = 0x00000004, /* $K0 */
    MR4KD_RLOWER  = 0x00000008, /*  k0 */
    
    /* Opcode display options */
    MR4KD_OLOWER  = 0x00000010, /* mfhi */
    
    /* Number display options */
    MR4KD_HLOWER  = 0x00000020, /* 0xffff */
};

/* Disassembler itself */
void mr4kd_disassemble ( u32 instruction, u32 counter, char * buffer );

/* Controlling functions (global) */
void mr4kd_flag_set   ( int flag  );
void mr4kd_flag_clear ( int flag  );
void mr4kd_spacing    ( int space );
int  mr4kd_flag_get   ( int flag  );

/* Controlling functions (app-wide) */
int mr4kd_sprintf ( char *dest, char *name, unsigned int instruction, unsigned int pc, char *fmt );

#endif /* __LN64_R4000_DIS_H */
