/***************************
* MIPS Instruction Builder *
***************************/
#ifndef __MIPS_H__

/* Cached -> Uncached */
#define UNCACHED_ADDR(x)    ((void*)((u32)(x) | 0xA0000000))

/* Assembly? */
#ifdef __LANGUAGE_ASSEMBLY__

/* Register names */
#define v0      2
#define v1      3
#define a0      4
#define a1      5
#define a2      6
#define a3      7
#define t0      8
#define t1      9
#define t2      10
#define t3      11
#define t4      12
#define t5      13
#define t6      14
#define t7      15
#define s0      16
#define s1      17
#define s2      18
#define s3      19
#define s4      20
#define s5      21
#define s6      22
#define s7      23
#define t8      24
#define t9      25
#define k0      26
#define k1      27

#else

/* Quick... */
#define OP(x)   ((x)<<26)
#define OF(x)   (((unsigned)(x)>>2)&0xFFFF)
#define SA(x)   (((x)&0x1F)<<6)
#define RD(x)   (((x)&0x1F)<<11)
#define RT(x)   (((x)&0x1F)<<16)
#define RS(x)   (((x)&0x1F)<<21)
#define IM(x)   ((unsigned)(x)&0xFFFF)
#define JT(x)   (((unsigned)(x)>>2)&0x3FFFFFF)

/* Do we want prefixes? */
#ifndef _NO_MIPS_PREFIX

/* MIPS Registers */
enum
{
    MIPS_R0,
    MIPS_AT,
    MIPS_V0,
    MIPS_V1,
    MIPS_A0,
    MIPS_A1,
    MIPS_A2,
    MIPS_A3,
    MIPS_T0,
    MIPS_T1,
    MIPS_T2,
    MIPS_T3,
    MIPS_T4,
    MIPS_T5,
    MIPS_T6,
    MIPS_T7,
    MIPS_S0,
    MIPS_S1,
    MIPS_S2,
    MIPS_S3,
    MIPS_S4,
    MIPS_S5,
    MIPS_S6,
    MIPS_S7,
    MIPS_T8,
    MIPS_T9,
    MIPS_K0,
    MIPS_K1,
    MIPS_GP,
    MIPS_SP,
    MIPS_FP,
    MIPS_RA
};

/* MIPS assembly */
#define MIPS_ADD(rd, rs, rt)        (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x20)
#define MIPS_ADDI(rt, rs, immd)     (OP(0x08) | RS(rs) | RT(rt) | IM(immd))
#define MIPS_ADDIU(rt, rs, immd)    (OP(0x09) | RS(rs) | RT(rt) | IM(immd))
#define MIPS_ANDI(rt, rs, immd)     (OP(0x0C) | RS(rs) | RT(rt) | IM(immd))
#define MIPS_BC1F(off)              (OP(0x11) | RS(0x08) | OF(off))
#define MIPS_BC1FL(off)             (OP(0x11) | RS(0x08) | RT(0x02) | OF(off))
#define MIPS_BC1T(off)              (OP(0x11) | RS(0x08) | RT(0x01) | OF(off))
#define MIPS_BC1TL(off)             (OP(0x11) | RS(0x08) | RT(0x03) | OF(off))
#define MIPS_BEQ(rs, rt, off)       (OP(0x04) | RS(rs) | RT(rt) | OF(off))
#define MIPS_BEQL(rs, rt, off)      (OP(0x14) | RS(rs) | RT(rt) | OF(off))
#define MIPS_BGEZ(rs, off)          (OP(0x01) | RS(rs) | RT(0x01) | OF(off))
#define MIPS_BGEZAL(rs, off)        (OP(0x01) | RS(rs) | RT(0x11) | OF(off))
#define MIPS_BGEZALL(rs, off)       (OP(0x01) | RS(rs) | RT(0x13) | OF(off))
#define MIPS_BGEZL(rs, off)         (OP(0x01) | RS(rs) | RT(0x03) | OF(off))
#define MIPS_BGTZ(rs, off)          (OP(0x07) | RS(rs) | OF(off))
#define MIPS_BGTZL(rs, off)         (OP(0x17) | RS(rs) | OF(off))
#define MIPS_BLEZ(rs, off)          (OP(0x06) | RS(rs) | OF(off))
#define MIPS_BLEZL(rs, off)         (OP(0x16) | RS(rs) | OF(off))
#define MIPS_BLTZ(rs, off)          (OP(0x01) | RS(rs) | OF(off))
#define MIPS_BLTZAL(rs, off)        (OP(0x01) | RS(rs) | RT(0x10) | OF(off))
#define MIPS_BLTZALL(rs, off)       (OP(0x01) | RS(rs) | RT(0x12) | OF(off))
#define MIPS_BLTZL(rs, off)         (OP(0x01) | RS(rs) | RT(0x02) | OF(off))
#define MIPS_BNE(rs, rt, off)       (OP(0x05) | RS(rs) | RT(rt) | OF(off))
#define MIPS_BNEL(rs, rt, off)      (OP(0x15) | RS(rs) | RT(rt) | OF(off))
#define MIPS_BREAK(code)            ((code) << 6 | 0x0D)
#define MIPS_CACHE(base, op, off)   (OP(0x2F) | RS(base) | RT(op) | OF(off))
#define MIPS_CFC1(rt, rd)           (OP(0x11) | RS(0x02) | RT(base) | RD(rd))
#define MIPS_COP1(cofun)            (OP(0x11) | (1 << 25) | ((cofun) & 0x1FFFFFF))
#define MIPS_CTC1(rt, rd)           (OP(0x11) | RS(0x06) | RT(base) | RD(rd))
#define MIPS_DADD(rd, rs, rt)       (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x2C)
#define MIPS_DADDI(rt, rs, immd)    (OP(0x18) | RS(rs) | RT(rt) | IM(immd))
#define MIPS_DADDIU(rt, rs, immd)   (OP(0x19) | RS(rs) | RT(rt) | IM(immd))
#define MIPS_DADDU(rd, rs, rt)      (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x2D)
#define MIPS_DDIV(rs, rt)           (OP(0x00) | RS(rs) | RT(rt) | 0x1E)
#define MIPS_DDIVU(rs, rt)          (OP(0x00) | RS(rs) | RT(rt) | 0x1F)
#define MIPS_DIV(rs, rt)            (OP(0x00) | RS(rs) | RT(rt) | 0x1A)
#define MIPS_DIVU(rs, rt)           (OP(0x00) | RS(rs) | RT(rt) | 0x1B)
#define MIPS_DMFC0(rt, rd)          (OP(0x10) | RS(0x01) | RT(rt) | RD(rd))
#define MIPS_DMTC0(rt, rd)          (OP(0x10) | RS(0x05) | RT(rt) | RD(rd))
#define MIPS_DMULT(rs, rt)          (OP(0x00) | RS(rs) | RT(rt) | 0x1C)
#define MIPS_DMULTU(rs, rt)         (OP(0x00) | RS(rs) | RT(rt) | 0x1D)
#define MIPS_DSLL(rd, rt, sa)       (OP(0x00) | RT(rt) | RD(rd) | SA(sa) | 0x38)
#define MIPS_DSLLV(rd, rt, rs)      (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x14)
#define MIPS_DSLL32(rd, rt, sa)     (OP(0x00) | RT(rt) | RD(rd) | SA(sa) | 0x3C)
#define MIPS_DSRA(rd, rt, sa)       (OP(0x00) | RT(rt) | RD(rd) | SA(sa) | 0x3B)
#define MIPS_DSRAV(rd, rt, rs)      (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x17)
#define MIPS_DSRA32(rd, rt, sa)     (OP(0x00) | RT(rt) | RD(rd) | SA(sa) | 0x3F)
#define MIPS_DSRL(rd, rt, sa)       (OP(0x00) | RT(rt) | RD(rd) | SA(sa) | 0x3A)
#define MIPS_DSRLV(rd, rt, rs)      (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x16)
#define MIPS_DSRL32(rd, rt, sa)     (OP(0x00) | RT(rt) | RD(rd) | SA(sa) | 0x3E)
#define MIPS_DSUB(rd, rs, rt)       (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x2E)
#define MIPS_DSUBU(rd, rs, rt)      (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x2F)
#define MIPS_ERET()                 (OP(0x10) | 1 << 25 | 0x18)
#define MIPS_J(target)              (OP(0x02) | JT(target))
#define MIPS_JAL(target)            (OP(0x03) | JT(target))
#define MIPS_JALR(rd, rs)           (OP(0x00) | RS(rs) | RD(rd) | 0x09)
#define MIPS_JR(rs)                 (OP(0x00) | RS(rs) | 0x08)
#define MIPS_LB(rt, off, base)      (OP(0x20) | RS(base) | RT(rt) | IM(off))
#define MIPS_LBU(rt, off, base)     (OP(0x24) | RS(base) | RT(rt) | IM(off))
#define MIPS_LD(rt, off, base)      (OP(0x37) | RS(base) | RT(rt) | IM(off))
#define MIPS_LDC1(rt, off, base)    (OP(0x35) | RS(base) | RT(rt) | IM(off))
#define MIPS_LDL(rt, off, base)     (OP(0x1A) | RS(base) | RT(rt) | IM(off))
#define MIPS_LDR(rt, off, base)     (OP(0x1B) | RS(base) | RT(rt) | IM(off))
#define MIPS_LH(rt, off, base)      (OP(0x21) | RS(base) | RT(rt) | IM(off))
#define MIPS_LHU(rt, off, base)     (OP(0x25) | RS(base) | RT(rt) | IM(off))
#define MIPS_LL(rt, off, base)      (OP(0x30) | RS(base) | RT(rt) | IM(off))
#define MIPS_LLD(rt, off, base)     (OP(0x34) | RS(base) | RT(rt) | IM(off))
#define MIPS_LUI(rt, immd)          (OP(0x0F) | RT(rt) | IM(immd))
#define MIPS_LW(rt, off, base)      (OP(0x23) | RS(base) | RT(rt) | IM(off))
#define MIPS_LWC1(rt, off, base)    (OP(0x31) | RS(base) | RT(rt) | IM(off))
#define MIPS_LWL(rt, off, base)     (OP(0x22) | RS(base) | RT(rt) | IM(off))
#define MIPS_LWR(rt, off, base)     (OP(0x26) | RS(base) | RT(rt) | IM(off))
#define MIPS_LWU(rt, off, base)     (OP(0x27) | RS(base) | RT(rt) | IM(off))
#define MIPS_MFC0(rt, rd)           (OP(0x10) | RT(rt) | RD(rd))
#define MIPS_MFC1(rt, rd)           (OP(0x11) | RT(rt) | RD(rd))
#define MIPS_MFHI(rd)               (OP(0x00) | RD(rd) | 0x10)
#define MIPS_MFLO(rd)               (OP(0x00) | RD(rd) | 0x12)
#define MIPS_MTC0(rt, rd)           (OP(0x10) | RS(0x04) | RT(rt) | RD(rd))
#define MIPS_MTC1(rt, rd)           (OP(0x11) | RS(0x04) | RT(rt) | RD(rd))
#define MIPS_MTHI(rd)               (OP(0x00) | RD(rd) | 0x11)
#define MIPS_MTLO(rd)               (OP(0x00) | RD(rd) | 0x13)
#define MIPS_MULT(rs, rt)           (OP(0x00) | RS(rs) | RT(rt) | 0x18)
#define MIPS_MULTU(rs, rt)          (OP(0x00) | RS(rs) | RT(rt) | 0x19)
#define MIPS_NOR(rd, rs, rt)        (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x27)
#define MIPS_OR(rd, rs, rt)         (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x25)
#define MIPS_ORI(rt, rs, immd)      (OP(0x0D) | RS(rs) | RT(rt) | IM(immd))
#define MIPS_SB(rt, off, base)      (OP(0x28) | RS(base) | RT(rt) | IM(off))
#define MIPS_SC(rt, off, base)      (OP(0x38) | RS(base) | RT(rt) | IM(off))
#define MIPS_SCD(rt, off, base)     (OP(0x3C) | RS(base) | RT(rt) | IM(off))
#define MIPS_SD(rt, off, base)      (OP(0x3F) | RS(base) | RT(rt) | IM(off))
#define MIPS_SDC1(rt, off, base)    (OP(0x3D) | RS(base) | RT(rt) | IM(off))
#define MIPS_SDL(rt, off, base)     (OP(0x2C) | RS(base) | RT(rt) | IM(off))
#define MIPS_SDR(rt, off, base)     (OP(0x2D) | RS(base) | RT(rt) | IM(off))
#define MIPS_SH(rt, off, base)      (OP(0x29) | RS(base) | RT(rt) | IM(off))
#define MIPS_SLL(rd, rt, sa)        (OP(0x00) | RT(rt) | RD(rd) | SA(sa))
#define MIPS_SLLV(rd, rt, rs)       (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x04)
#define MIPS_SLT(rd, rs, rt)        (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x2A)
#define MIPS_SLTI(rt, rs, immd)     (OP(0x0A) | RS(rs) | RT(rt) | IM(immd))
#define MIPS_SLTIU(rt, rs, immd)    (OP(0x0B) | RS(rs) | RT(rt) | IM(immd))
#define MIPS_SLTU(rd, rs, rt)       (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x2B)
#define MIPS_SRA(rd, rt, sa)        (OP(0x00) | RT(rt) | RD(rd) | SA(sa) | 0x03)
#define MIPS_SRAV(rd, rt, rs)       (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x07)
#define MIPS_SRL(rd, rt, sa)        (OP(0x00) | RT(rt) | RD(rd) | SA(sa) | 0x02)
#define MIPS_SRLV(rd, rt, rs)       (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x06)
#define MIPS_SUB(rd, rs, rt)        (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x22)
#define MIPS_SUBU(rd, rs, rt)       (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x23)
#define MIPS_SW(rt, off, base)      (OP(0x2B) | RS(base) | RT(rt) | IM(off))
#define MIPS_SWC1(rt, off, base)    (OP(0x39) | RS(base) | RT(rt) | IM(off))
#define MIPS_SWL(rt, off, base)     (OP(0x2A) | RS(base) | RT(rt) | IM(off))
#define MIPS_SWR(rt, off, base)     (OP(0x2E) | RS(base) | RT(rt) | IM(off))
#define MIPS_SYNC()                 (0x0F)
#define MIPS_SYSCALL(code)          ((code) << 6 | 0x0C)
#define MIPS_TEQ(rs, rt, code)      (OP(0x00) | RS(rs) | RT(rt) | ((code)&0x3FF) << 6 | 0x34)
#define MIPS_TEQI(rs, immd)         (OP(0x01) | RS(rs) | RT(0x0C) | IM(immd))
#define MIPS_TGE(rs, rt, code)      (OP(0x00) | RS(rs) | RT(rt) | ((code)&0x3FF) << 6 | 0x34)
#define MIPS_TGEI(rs, immd)         (OP(0x01) | RS(rs) | RT(0x08) | IM(immd))
#define MIPS_TGEIU(rs, immd)        (OP(0x01) | RS(rs) | RT(0x09) | IM(immd))
#define MIPS_TGEU(rs, rt, code)     (OP(0x00) | RS(rs) | RT(rt) | ((code)&0x3FF) << 6 | 0x31)
#define MIPS_TLBP()                 (OP(0x10) | 1 << 25 | 0x08)
#define MIPS_TLBR()                 (OP(0x10) | 1 << 25 | 0x01)
#define MIPS_TLBWI()                (OP(0x10) | 1 << 25 | 0x02)
#define MIPS_TLBWR()                (OP(0x10) | 1 << 25 | 0x06)
#define MIPS_TLT(rs, rt, code)      (OP(0x00) | RS(rs) | RT(rt) | ((code)&0x3FF) << 6 | 0x32)
#define MIPS_TLTI(rs, immd)         (OP(0x01) | RS(rs) | RT(0x0A) | IM(immd))
#define MIPS_TLTIU(rs, immd)        (OP(0x01) | RS(rs) | RT(0x0B) | IM(immd))
#define MIPS_TLTU(rs, rt, code)     (OP(0x00) | RS(rs) | RT(rt) | ((code)&0x3FF) << 6 | 0x33)
#define MIPS_TNE(rs, rt, code)      (OP(0x00) | RS(rs) | RT(rt) | ((code)&0x3FF) << 6 | 0x36)
#define MIPS_TNEI(rs, immd)         (OP(0x01) | RS(rs) | RT(0x0E) | IM(immd))
#define MIPS_XOR(rd, rs, rt)        (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x26)
#define MIPS_XORI(rt, rs, immd)     (OP(0x0E) | RS(rs) | RT(rt) | IM(immd))

/* No prefixes */
#else

/* MIPS Registers */
enum
{
    R0,
    AT,
    V0,
    V1,
    A0,
    A1,
    A2,
    A3,
    T0,
    T1,
    T2,
    T3,
    T4,
    T5,
    T6,
    T7,
    S0,
    S1,
    S2,
    S3,
    S4,
    S5,
    S6,
    S7,
    T8,
    T9,
    K0,
    K1,
    GP,
    SP,
    FP,
    RA
};

/* MIPS assembly */
#define ADD(rd, rs, rt)        (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x20)
#define ADDI(rt, rs, immd)     (OP(0x08) | RS(rs) | RT(rt) | IM(immd))
#define ADDIU(rt, rs, immd)    (OP(0x09) | RS(rs) | RT(rt) | IM(immd))
#define ANDI(rt, rs, immd)     (OP(0x0C) | RS(rs) | RT(rt) | IM(immd))
#define BC1F(off)              (OP(0x11) | RS(0x08) | OF(off))
#define BC1FL(off)             (OP(0x11) | RS(0x08) | RT(0x02) | OF(off))
#define BC1T(off)              (OP(0x11) | RS(0x08) | RT(0x01) | OF(off))
#define BC1TL(off)             (OP(0x11) | RS(0x08) | RT(0x03) | OF(off))
#define BEQ(rs, rt, off)       (OP(0x04) | RS(rs) | RT(rt) | OF(off))
#define BEQL(rs, rt, off)      (OP(0x14) | RS(rs) | RT(rt) | OF(off))
#define BGEZ(rs, off)          (OP(0x01) | RS(rs) | RT(0x01) | OF(off))
#define BGEZAL(rs, off)        (OP(0x01) | RS(rs) | RT(0x11) | OF(off))
#define BGEZALL(rs, off)       (OP(0x01) | RS(rs) | RT(0x13) | OF(off))
#define BGEZL(rs, off)         (OP(0x01) | RS(rs) | RT(0x03) | OF(off))
#define BGTZ(rs, off)          (OP(0x07) | RS(rs) | OF(off))
#define BGTZL(rs, off)         (OP(0x17) | RS(rs) | OF(off))
#define BLEZ(rs, off)          (OP(0x06) | RS(rs) | OF(off))
#define BLEZL(rs, off)         (OP(0x16) | RS(rs) | OF(off))
#define BLTZ(rs, off)          (OP(0x01) | RS(rs) | OF(off))
#define BLTZAL(rs, off)        (OP(0x01) | RS(rs) | RT(0x10) | OF(off))
#define BLTZALL(rs, off)       (OP(0x01) | RS(rs) | RT(0x12) | OF(off))
#define BLTZL(rs, off)         (OP(0x01) | RS(rs) | RT(0x02) | OF(off))
#define BNE(rs, rt, off)       (OP(0x05) | RS(rs) | RT(rt) | OF(off))
#define BNEL(rs, rt, off)      (OP(0x15) | RS(rs) | RT(rt) | OF(off))
#define BREAK(code)            ((code) << 6 | 0x0D)
#define CACHE(base, op, off)   (OP(0x2F) | RS(base) | RT(op) | OF(off))
#define CFC1(rt, rd)           (OP(0x11) | RS(0x02) | RT(base) | RD(rd))
#define COP1(cofun)            (OP(0x11) | (1 << 25) | ((cofun) & 0x1FFFFFF))
#define CTC1(rt, rd)           (OP(0x11) | RS(0x06) | RT(base) | RD(rd))
#define DADD(rd, rs, rt)       (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x2C)
#define DADDI(rt, rs, immd)    (OP(0x18) | RS(rs) | RT(rt) | IM(immd))
#define DADDIU(rt, rs, immd)   (OP(0x19) | RS(rs) | RT(rt) | IM(immd))
#define DADDU(rd, rs, rt)      (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x2D)
#define DDIV(rs, rt)           (OP(0x00) | RS(rs) | RT(rt) | 0x1E)
#define DDIVU(rs, rt)          (OP(0x00) | RS(rs) | RT(rt) | 0x1F)
#define DIV(rs, rt)            (OP(0x00) | RS(rs) | RT(rt) | 0x1A)
#define DIVU(rs, rt)           (OP(0x00) | RS(rs) | RT(rt) | 0x1B)
#define DMFC0(rt, rd)          (OP(0x10) | RS(0x01) | RT(rt) | RD(rd))
#define DMTC0(rt, rd)          (OP(0x10) | RS(0x05) | RT(rt) | RD(rd))
#define DMULT(rs, rt)          (OP(0x00) | RS(rs) | RT(rt) | 0x1C)
#define DMULTU(rs, rt)         (OP(0x00) | RS(rs) | RT(rt) | 0x1D)
#define DSLL(rd, rt, sa)       (OP(0x00) | RT(rt) | RD(rd) | SA(sa) | 0x38)
#define DSLLV(rd, rt, rs)      (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x14)
#define DSLL32(rd, rt, sa)     (OP(0x00) | RT(rt) | RD(rd) | SA(sa) | 0x3C)
#define DSRA(rd, rt, sa)       (OP(0x00) | RT(rt) | RD(rd) | SA(sa) | 0x3B)
#define DSRAV(rd, rt, rs)      (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x17)
#define DSRA32(rd, rt, sa)     (OP(0x00) | RT(rt) | RD(rd) | SA(sa) | 0x3F)
#define DSRL(rd, rt, sa)       (OP(0x00) | RT(rt) | RD(rd) | SA(sa) | 0x3A)
#define DSRLV(rd, rt, rs)      (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x16)
#define DSRL32(rd, rt, sa)     (OP(0x00) | RT(rt) | RD(rd) | SA(sa) | 0x3E)
#define DSUB(rd, rs, rt)       (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x2E)
#define DSUBU(rd, rs, rt)      (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x2F)
#define ERET()                 (OP(0x10) | 1 << 25 | 0x18)
#define J(target)              (OP(0x02) | JT(target))
#define JAL(target)            (OP(0x03) | JT(target))
#define JALR(rd, rs)           (OP(0x00) | RS(rs) | RD(rd) | 0x09)
#define JR(rs)                 (OP(0x00) | RS(rs) | 0x08)
#define LB(rt, off, base)      (OP(0x20) | RS(base) | RT(rt) | IM(off))
#define LBU(rt, off, base)     (OP(0x24) | RS(base) | RT(rt) | IM(off))
#define LD(rt, off, base)      (OP(0x37) | RS(base) | RT(rt) | IM(off))
#define LDC1(rt, off, base)    (OP(0x35) | RS(base) | RT(rt) | IM(off))
#define LDL(rt, off, base)     (OP(0x1A) | RS(base) | RT(rt) | IM(off))
#define LDR(rt, off, base)     (OP(0x1B) | RS(base) | RT(rt) | IM(off))
#define LH(rt, off, base)      (OP(0x21) | RS(base) | RT(rt) | IM(off))
#define LHU(rt, off, base)     (OP(0x25) | RS(base) | RT(rt) | IM(off))
#define LL(rt, off, base)      (OP(0x30) | RS(base) | RT(rt) | IM(off))
#define LLD(rt, off, base)     (OP(0x34) | RS(base) | RT(rt) | IM(off))
#define LUI(rt, immd)          (OP(0x0F) | RT(rt) | IM(immd))
#define LW(rt, off, base)      (OP(0x23) | RS(base) | RT(rt) | IM(off))
#define LWC1(rt, off, base)    (OP(0x31) | RS(base) | RT(rt) | IM(off))
#define LWL(rt, off, base)     (OP(0x22) | RS(base) | RT(rt) | IM(off))
#define LWR(rt, off, base)     (OP(0x26) | RS(base) | RT(rt) | IM(off))
#define LWU(rt, off, base)     (OP(0x27) | RS(base) | RT(rt) | IM(off))
#define MFC0(rt, rd)           (OP(0x10) | RT(rt) | RD(rd))
#define MFC1(rt, rd)           (OP(0x11) | RT(rt) | RD(rd))
#define MFHI(rd)               (OP(0x00) | RD(rd) | 0x10)
#define MFLO(rd)               (OP(0x00) | RD(rd) | 0x12)
#define MTC0(rt, rd)           (OP(0x10) | RS(0x04) | RT(rt) | RD(rd))
#define MTC1(rt, rd)           (OP(0x11) | RS(0x04) | RT(rt) | RD(rd))
#define MTHI(rd)               (OP(0x00) | RD(rd) | 0x11)
#define MTLO(rd)               (OP(0x00) | RD(rd) | 0x13)
#define MULT(rs, rt)           (OP(0x00) | RS(rs) | RT(rt) | 0x18)
#define MULTU(rs, rt)          (OP(0x00) | RS(rs) | RT(rt) | 0x19)
#define NOR(rd, rs, rt)        (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x27)
#define OR(rd, rs, rt)         (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x25)
#define ORI(rt, rs, immd)      (OP(0x0D) | RS(rs) | RT(rt) | IM(immd))
#define SB(rt, off, base)      (OP(0x28) | RS(base) | RT(rt) | IM(off))
#define SC(rt, off, base)      (OP(0x38) | RS(base) | RT(rt) | IM(off))
#define SCD(rt, off, base)     (OP(0x3C) | RS(base) | RT(rt) | IM(off))
#define SD(rt, off, base)      (OP(0x3F) | RS(base) | RT(rt) | IM(off))
#define SDC1(rt, off, base)    (OP(0x3D) | RS(base) | RT(rt) | IM(off))
#define SDL(rt, off, base)     (OP(0x2C) | RS(base) | RT(rt) | IM(off))
#define SDR(rt, off, base)     (OP(0x2D) | RS(base) | RT(rt) | IM(off))
#define SH(rt, off, base)      (OP(0x29) | RS(base) | RT(rt) | IM(off))
#define SLL(rd, rt, sa)        (OP(0x00) | RT(rt) | RD(rd) | SA(sa))
#define SLLV(rd, rt, rs)       (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x04)
#define SLT(rd, rs, rt)        (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x2A)
#define SLTI(rt, rs, immd)     (OP(0x0A) | RS(rs) | RT(rt) | IM(immd))
#define SLTIU(rt, rs, immd)    (OP(0x0B) | RS(rs) | RT(rt) | IM(immd))
#define SLTU(rd, rs, rt)       (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x2B)
#define SRA(rd, rt, sa)        (OP(0x00) | RT(rt) | RD(rd) | SA(sa) | 0x03)
#define SRAV(rd, rt, rs)       (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x07)
#define SRL(rd, rt, sa)        (OP(0x00) | RT(rt) | RD(rd) | SA(sa) | 0x02)
#define SRLV(rd, rt, rs)       (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x06)
#define SUB(rd, rs, rt)        (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x22)
#define SUBU(rd, rs, rt)       (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x23)
#define SW(rt, off, base)      (OP(0x2B) | RS(base) | RT(rt) | IM(off))
#define SWC1(rt, off, base)    (OP(0x39) | RS(base) | RT(rt) | IM(off))
#define SWL(rt, off, base)     (OP(0x2A) | RS(base) | RT(rt) | IM(off))
#define SWR(rt, off, base)     (OP(0x2E) | RS(base) | RT(rt) | IM(off))
#define SYNC()                 (0x0F)
#define SYSCALL(code)          ((code) << 6 | 0x0C)
#define TEQ(rs, rt, code)      (OP(0x00) | RS(rs) | RT(rt) | ((code)&0x3FF) << 6 | 0x34)
#define TEQI(rs, immd)         (OP(0x01) | RS(rs) | RT(0x0C) | IM(immd))
#define TGE(rs, rt, code)      (OP(0x00) | RS(rs) | RT(rt) | ((code)&0x3FF) << 6 | 0x34)
#define TGEI(rs, immd)         (OP(0x01) | RS(rs) | RT(0x08) | IM(immd))
#define TGEIU(rs, immd)        (OP(0x01) | RS(rs) | RT(0x09) | IM(immd))
#define TGEU(rs, rt, code)     (OP(0x00) | RS(rs) | RT(rt) | ((code)&0x3FF) << 6 | 0x31)
#define TLBP()                 (OP(0x10) | 1 << 25 | 0x08)
#define TLBR()                 (OP(0x10) | 1 << 25 | 0x01)
#define TLBWI()                (OP(0x10) | 1 << 25 | 0x02)
#define TLBWR()                (OP(0x10) | 1 << 25 | 0x06)
#define TLT(rs, rt, code)      (OP(0x00) | RS(rs) | RT(rt) | ((code)&0x3FF) << 6 | 0x32)
#define TLTI(rs, immd)         (OP(0x01) | RS(rs) | RT(0x0A) | IM(immd))
#define TLTIU(rs, immd)        (OP(0x01) | RS(rs) | RT(0x0B) | IM(immd))
#define TLTU(rs, rt, code)     (OP(0x00) | RS(rs) | RT(rt) | ((code)&0x3FF) << 6 | 0x33)
#define TNE(rs, rt, code)      (OP(0x00) | RS(rs) | RT(rt) | ((code)&0x3FF) << 6 | 0x36)
#define TNEI(rs, immd)         (OP(0x01) | RS(rs) | RT(0x0E) | IM(immd))
#define XOR(rd, rs, rt)        (OP(0x00) | RS(rs) | RT(rt) | RD(rd) | 0x26)
#define XORI(rt, rs, immd)     (OP(0x0E) | RS(rs) | RT(rt) | IM(immd))

#endif /* _NO_MIPS_PREFIX */

#endif /* LANGUAGE_ASSEMBLY */

#endif /* __MIPS_H__ */
