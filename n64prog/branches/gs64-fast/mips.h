/***************************
* MIPS Instruction Builder *
***************************/
#ifndef __MIPS_H__

/* Cached -> Uncached */
#define UNCACHED_ADDR(x)    ((void*)((u32)(x) | 0xA0000000))

/* MIPS Registers */
enum MIPS_Reg
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

/* Quick... */
#define OP(x)   ((x)<<26)
#define OF(x)   (((unsigned)(x)>>2)&0xFFFF)
#define SA(x)   (((x)&0x1F)<<6)
#define RD(x)   (((x)&0x1F)<<11)
#define RT(x)   (((x)&0x1F)<<16)
#define RS(x)   (((x)&0x1F)<<21)
#define IM(x)   ((unsigned)(x)&0xFFFF)
#define JT(x)   (((x)>>2)&0x3FFFFFF)

/* MIPS assembly */
#define MIPS_ADD(rd, rs, rt)        (OP(0b000000) | RS(rs) | RT(rt) | RD(rd) | 0b100000)
#define MIPS_ADDI(rt, rs, immd)     (OP(0b001000) | RS(rs) | RT(rt) | IM(immd))
#define MIPS_ADDIU(rt, rs, immd)    (OP(0b001001) | RS(rs) | RT(rt) | IM(immd))
#define MIPS_ANDI(rt, rs, immd)     (OP(0b001100) | RS(rs) | RT(rt) | IM(immd))
#define MIPS_BC1F(off)              (OP(0b010001) | RS(0b01000) | OF(off))
#define MIPS_BC1FL(off)             (OP(0b010001) | RS(0b01000) | RT(0b00010) | OF(off))
#define MIPS_BC1T(off)              (OP(0b010001) | RS(0b01000) | RT(0b00001) | OF(off))
#define MIPS_BC1TL(off)             (OP(0b010001) | RS(0b01000) | RT(0b00011) | OF(off))
#define MIPS_BEQ(rs, rt, off)       (OP(0b000100) | RS(rs) | RT(rt) | OF(off))
#define MIPS_BEQL(rs, rt, off)      (OP(0b010100) | RS(rs) | RT(rt) | OF(off))
#define MIPS_BGEZ(rs, off)          (OP(0b000001) | RS(rs) | RT(0b00001) | OF(off))
#define MIPS_BGEZAL(rs, off)        (OP(0b000001) | RS(rs) | RT(0b10001) | OF(off))
#define MIPS_BGEZALL(rs, off)       (OP(0b000001) | RS(rs) | RT(0b10011) | OF(off))
#define MIPS_BGEZL(rs, off)         (OP(0b000001) | RS(rs) | RT(0b00011) | OF(off))
#define MIPS_BGTZ(rs, off)          (OP(0b000111) | RS(rs) | OF(off))
#define MIPS_BGTZL(rs, off)         (OP(0b010111) | RS(rs) | OF(off))
#define MIPS_BLEZ(rs, off)          (OP(0b000110) | RS(rs) | OF(off))
#define MIPS_BLEZL(rs, off)         (OP(0b010110) | RS(rs) | OF(off))
#define MIPS_BLTZ(rs, off)          (OP(0b000001) | RS(rs) | OF(off))
#define MIPS_BLTZAL(rs, off)        (OP(0b000001) | RS(rs) | RT(0b10000) | OF(off))
#define MIPS_BLTZALL(rs, off)       (OP(0b000001) | RS(rs) | RT(0b10010) | OF(off))
#define MIPS_BLTZL(rs, off)         (OP(0b000001) | RS(rs) | RT(0b00010) | OF(off))
#define MIPS_BNE(rs, rt, off)       (OP(0b000101) | RS(rs) | RT(rt) | OF(off))
#define MIPS_BNEL(rs, rt, off)      (OP(0b010101) | RS(rs) | RT(rt) | OF(off))
#define MIPS_BREAK(code)            ((code) << 6 | 0b001101)
#define MIPS_CACHE(base, op, off)   (OP(0b101111) | RS(base) | RT(op) | OF(off))
#define MIPS_CFC1(rt, rd)           (OP(0b010001) | RS(0b00010) | RT(base) | RD(rd))
#define MIPS_COP1(cofun)            (OP(0b010001) | (1 << 25) | ((cofun) & 0x1FFFFFF))
#define MIPS_CTC1(rt, rd)           (OP(0b010001) | RS(0b00110) | RT(base) | RD(rd))
#define MIPS_DADD(rd, rs, rt)       (OP(0b000000) | RS(rs) | RT(rt) | RD(rd) | 0b101100)
#define MIPS_DADDI(rt, rs, immd)    (OP(0b011000) | RS(rs) | RT(rt) | IM(immd))
#define MIPS_DADDIU(rt, rs, immd)   (OP(0b011001) | RS(rs) | RT(rt) | IM(immd))
#define MIPS_DADDU(rd, rs, rt)      (OP(0b000000) | RS(rs) | RT(rt) | RD(rd) | 0b101101)
#define MIPS_DDIV(rs, rt)           (OP(0b000000) | RS(rs) | RT(rt) | 0b011110)
#define MIPS_DDIVU(rs, rt)          (OP(0b000000) | RS(rs) | RT(rt) | 0b011111)
#define MIPS_DIV(rs, rt)            (OP(0b000000) | RS(rs) | RT(rt) | 0b011010)
#define MIPS_DIVU(rs, rt)           (OP(0b000000) | RS(rs) | RT(rt) | 0b011011)
#define MIPS_DMFC0(rt, rd)          (OP(0b010000) | RS(0b00001) | RT(rt) | RD(rd))
#define MIPS_DMTC0(rt, rd)          (OP(0b010000) | RS(0b00101) | RT(rt) | RD(rd))
#define MIPS_DMULT(rs, rt)          (OP(0b000000) | RS(rs) | RT(rt) | 0b011100)
#define MIPS_DMULTU(rs, rt)         (OP(0b000000) | RS(rs) | RT(rt) | 0b011101)
#define MIPS_DSLL(rd, rt, sa)       (OP(0b000000) | RT(rt) | RD(rd) | SA(sa) | 0b111000)
#define MIPS_DSLLV(rd, rt, rs)      (OP(0b000000) | RS(rs) | RT(rt) | RD(rd) | 0b010100)
#define MIPS_DSLL32(rd, rt, sa)     (OP(0b000000) | RT(rt) | RD(rd) | SA(sa) | 0b111100)
#define MIPS_DSRA(rd, rt, sa)       (OP(0b000000) | RT(rt) | RD(rd) | SA(sa) | 0b111011)
#define MIPS_DSRAV(rd, rt, rs)      (OP(0b000000) | RS(rs) | RT(rt) | RD(rd) | 0b010111)
#define MIPS_DSRA32(rd, rt, sa)     (OP(0b000000) | RT(rt) | RD(rd) | SA(sa) | 0b111111)
#define MIPS_DSRL(rd, rt, sa)       (OP(0b000000) | RT(rt) | RD(rd) | SA(sa) | 0b111010)
#define MIPS_DSRLV(rd, rt, rs)      (OP(0b000000) | RS(rs) | RT(rt) | RD(rd) | 0b010110)
#define MIPS_DSRL32(rd, rt, sa)     (OP(0b000000) | RT(rt) | RD(rd) | SA(sa) | 0b111110)
#define MIPS_DSUB(rd, rs, rt)       (OP(0b000000) | RS(rs) | RT(rt) | RD(rd) | 0b101110)
#define MIPS_DSUBU(rd, rs, rt)      (OP(0b000000) | RS(rs) | RT(rt) | RD(rd) | 0b101111)
#define MIPS_ERET()                 (OP(0b010000) | 1 << 25 | 0b011000)
#define MIPS_J(target)              (OP(0b000010) | JT(target))
#define MIPS_JAL(target)            (OP(0b000011) | JT(target))
#define MIPS_JALR(rd, rs)           (OP(0b000000) | RS(rs) | RD(rd) | 0b001001)
#define MIPS_JR(rs)                 (OP(0b000000) | RS(rs) | 0b001000)
#define MIPS_LB(rt, off, base)      (OP(0b100000) | RS(base) | RT(rt) | IM(off))
#define MIPS_LBU(rt, off, base)     (OP(0b100100) | RS(base) | RT(rt) | IM(off))
#define MIPS_LD(rt, off, base)      (OP(0b110111) | RS(base) | RT(rt) | IM(off))
#define MIPS_LDC1(rt, off, base)    (OP(0b110101) | RS(base) | RT(rt) | IM(off))
#define MIPS_LDL(rt, off, base)     (OP(0b011010) | RS(base) | RT(rt) | IM(off))
#define MIPS_LDR(rt, off, base)     (OP(0b011011) | RS(base) | RT(rt) | IM(off))
#define MIPS_LH(rt, off, base)      (OP(0b100001) | RS(base) | RT(rt) | IM(off))
#define MIPS_LHU(rt, off, base)     (OP(0b100101) | RS(base) | RT(rt) | IM(off))
#define MIPS_LL(rt, off, base)      (OP(0b110000) | RS(base) | RT(rt) | IM(off))
#define MIPS_LLD(rt, off, base)     (OP(0b110100) | RS(base) | RT(rt) | IM(off))
#define MIPS_LUI(rt, immd)          (OP(0b001111) | RT(rt) | IM(immd))
#define MIPS_LW(rt, off, base)      (OP(0b100011) | RS(base) | RT(rt) | IM(off))
#define MIPS_LWC1(rt, off, base)    (OP(0b110001) | RS(base) | RT(rt) | IM(off))
#define MIPS_LWL(rt, off, base)     (OP(0b100010) | RS(base) | RT(rt) | IM(off))
#define MIPS_LWR(rt, off, base)     (OP(0b100110) | RS(base) | RT(rt) | IM(off))
#define MIPS_LWU(rt, off, base)     (OP(0b100111) | RS(base) | RT(rt) | IM(off))
#define MIPS_MFC0(rt, rd)           (OP(0b010000) | RT(rt) | RD(rd))
#define MIPS_MFC1(rt, rd)           (OP(0b010001) | RT(rt) | RD(rd))
#define MIPS_MFHI(rd)               (OP(0b000000) | RD(rd) | 0b010000)
#define MIPS_MFLO(rd)               (OP(0b000000) | RD(rd) | 0b010010)
#define MIPS_MTC0(rt, rd)           (OP(0b010000) | RS(0b00100) | RT(rt) | RD(rd))
#define MIPS_MTC1(rt, rd)           (OP(0b010001) | RS(0b00100) | RT(rt) | RD(rd))
#define MIPS_MTHI(rd)               (OP(0b000000) | RD(rd) | 0b010001)
#define MIPS_MTLO(rd)               (OP(0b000000) | RD(rd) | 0b010011)
#define MIPS_MULT(rs, rt)           (OP(0b000000) | RS(rs) | RT(rt) | 0b011000)
#define MIPS_MULTU(rs, rt)          (OP(0b000000) | RS(rs) | RT(rt) | 0b011001)
#define MIPS_NOR(rd, rs, rt)        (OP(0b000000) | RS(rs) | RT(rt) | RD(rd) | 0b100111)
#define MIPS_OR(rd, rs, rt)         (OP(0b000000) | RS(rs) | RT(rt) | RD(rd) | 0b100101)
#define MIPS_ORI(rt, rs, immd)      (OP(0b001101) | RS(rs) | RT(rt) | IM(immd))
#define MIPS_SB(rt, off, base)      (OP(0b101000) | RS(base) | RT(rt) | IM(off))
#define MIPS_SC(rt, off, base)      (OP(0b111000) | RS(base) | RT(rt) | IM(off))
#define MIPS_SCD(rt, off, base)     (OP(0b111100) | RS(base) | RT(rt) | IM(off))
#define MIPS_SD(rt, off, base)      (OP(0b111111) | RS(base) | RT(rt) | IM(off))
#define MIPS_SDC1(rt, off, base)    (OP(0b111101) | RS(base) | RT(rt) | IM(off))
#define MIPS_SDL(rt, off, base)     (OP(0b101100) | RS(base) | RT(rt) | IM(off))
#define MIPS_SDR(rt, off, base)     (OP(0b101101) | RS(base) | RT(rt) | IM(off))
#define MIPS_SH(rt, off, base)      (OP(0b101001) | RS(base) | RT(rt) | IM(off))
#define MIPS_SLL(rd, rt, sa)        (OP(0b000000) | RT(rt) | RD(rd) | SA(sa))
#define MIPS_SLLV(rd, rt, rs)       (OP(0b000000) | RS(rs) | RT(rt) | RD(rd) | 0b000100)
#define MIPS_SLT(rd, rs, rt)        (OP(0b000000) | RS(rs) | RT(rt) | RD(rd) | 0b101010)
#define MIPS_SLTI(rt, rs, immd)     (OP(0b001010) | RS(rs) | RT(rt) | IM(immd))
#define MIPS_SLTIU(rt, rs, immd)    (OP(0b001011) | RS(rs) | RT(rt) | IM(immd))
#define MIPS_SLTU(rd, rs, rt)       (OP(0b000000) | RS(rs) | RT(rt) | RD(rd) | 0b101011)
#define MIPS_SRA(rd, rt, sa)        (OP(0b000000) | RT(rt) | RD(rd) | SA(sa) | 0b000011)
#define MIPS_SRAV(rd, rt, rs)       (OP(0b000000) | RS(rs) | RT(rt) | RD(rd) | 0b000111)
#define MIPS_SRL(rd, rt, sa)        (OP(0b000000) | RT(rt) | RD(rd) | SA(sa) | 0b000010)
#define MIPS_SRLV(rd, rt, rs)       (OP(0b000000) | RS(rs) | RT(rt) | RD(rd) | 0b000110)
#define MIPS_SUB(rd, rs, rt)        (OP(0b000000) | RS(rs) | RT(rt) | RD(rd) | 0b100010)
#define MIPS_SUBU(rd, rs, rt)       (OP(0b000000) | RS(rs) | RT(rt) | RD(rd) | 0b100011)
#define MIPS_SW(rt, off, base)      (OP(0b101011) | RS(base) | RT(rt) | IM(off))
#define MIPS_SWC1(rt, off, base)    (OP(0b111001) | RS(base) | RT(rt) | IM(off))
#define MIPS_SWL(rt, off, base)     (OP(0b101010) | RS(base) | RT(rt) | IM(off))
#define MIPS_SWR(rt, off, base)     (OP(0b101110) | RS(base) | RT(rt) | IM(off))
#define MIPS_SYNC()                 (0b001111)
#define MIPS_SYSCALL(code)          ((code) << 6 | 0b001100)
#define MIPS_TEQ(rs, rt, code)      (OP(0b000000) | RS(rs) | RT(rt) | ((code)&0x3FF) << 6 | 110100)
#define MIPS_TEQI(rs, immd)         (OP(0b000001) | RS(rs) | RT(0b01100) | IM(immd))
#define MIPS_TGE(rs, rt, code)      (OP(0b000000) | RS(rs) | RT(rt) | ((code)&0x3FF) << 6 | 110100)
#define MIPS_TGEI(rs, immd)         (OP(0b000001) | RS(rs) | RT(0b01000) | IM(immd))
#define MIPS_TGEIU(rs, immd)        (OP(0b000001) | RS(rs) | RT(0b01001) | IM(immd))
#define MIPS_TGEU(rs, rt, code)     (OP(0b000000) | RS(rs) | RT(rt) | ((code)&0x3FF) << 6 | 110001)
#define MIPS_TLBP()                 (OP(0b010000) | 1 << 25 | 0b001000)
#define MIPS_TLBR()                 (OP(0b010000) | 1 << 25 | 0b000001)
#define MIPS_TLBWI()                (OP(0b010000) | 1 << 25 | 0b000010)
#define MIPS_TLBWR()                (OP(0b010000) | 1 << 25 | 0b000110)
#define MIPS_TLT(rs, rt, code)      (OP(0b000000) | RS(rs) | RT(rt) | ((code)&0x3FF) << 6 | 110010)
#define MIPS_TLTI(rs, immd)         (OP(0b000001) | RS(rs) | RT(0b01010) | IM(immd))
#define MIPS_TLTIU(rs, immd)        (OP(0b000001) | RS(rs) | RT(0b01011) | IM(immd))
#define MIPS_TLTU(rs, rt, code)     (OP(0b000000) | RS(rs) | RT(rt) | ((code)&0x3FF) << 6 | 110011)
#define MIPS_TNE(rs, rt, code)      (OP(0b000000) | RS(rs) | RT(rt) | ((code)&0x3FF) << 6 | 110110)
#define MIPS_TNEI(rs, immd)         (OP(0b000001) | RS(rs) | RT(0b01110) | IM(immd))
#define MIPS_XOR(rd, rs, rt)        (OP(0b000000) | RS(rs) | RT(rt) | RD(rd) | 0b100110)
#define MIPS_XORI(rt, rs, immd)     (OP(0b001110) | RS(rs) | RT(rt) | IM(immd))

#endif /* __MIPS_H__ */
