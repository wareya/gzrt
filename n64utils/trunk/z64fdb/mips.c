/************************************
* ZFDB: Minimalist MIPS Interpreter *
************************************/
#include <stdio.h>
#include "zfdb.h"

/* Macros */
#define OP(x)	((x)>>26&0x3F)
#define RS(x)	((x)>>21&0x1F)
#define RT(x)	((x)>>16&0x1F)
#define RD(x)	((x)>>11&0x1F)
#define OF(x)   (((S16)((x)&0xFFFF))*4)
#define JT(x)   (((x)&0x3FFFFFF)<<2)

/* Local types */
typedef u32 U32;
typedef s16 S16;

/* MIPS CPU resources */
typedef struct 
{
	/* Registers */
	U32 pc;
	U32 gpr[32];
}
MIPS;

/* Global CPU handle */
MIPS cpu;

/* Current instruction */
U32	 in;

/* Other variables */
U32 mem_start = 0x80000000;

/* Counts... */
int count_bt;
int count_fc;
int count_ar;

/* Initialize CPU */
void cpu_init ( void )
{
	cpu.pc = mem_start;
}

/* Check if an address has been located */
void check_address ( void )
{
	/* Range valid? */
	if( cpu.gpr[RT(in)] >= mem_start && cpu.gpr[RT(in)] < mem_start /* + filesize */ )
	{
		count_ar++;
		
		/* Okay, add a referer 
		dbInsertReferer( cpu.gpr[RT(in)], cpu.pc, LabelAddressReference ); */
	}
}

/*
** Jump table declarations
*/

static void (*jt[])( void );
static void (*jt_imm[])( void );
static void (*jt_cop1[])( void );
static void (*jt_bc[])( void );

/* Execute an instruction */
void cpu_exec ( U32 i )
{
	jt[OP(i)]();
}

/*
 *  Opcodes
 */

static void LUI ( void ) 
{
	cpu.gpr[RT(in)] = in << 16;
}

static void ADDI ( void ) 
{
	cpu.gpr[RT(in)] = cpu.gpr[RS(in)] + (S16)(in & 0xFFFF);
	check_address();
}

#define ADDIU ADDI	/* They are the same without exceptions */

static void ORI ( void )
{
	cpu.gpr[RT(in)] = cpu.gpr[RS(in)] | (in & 0xFFFF);
	check_address();
}

static void NOP ( void )
{
	return;
}

/* ==> Jumping instructions */

static void J ( void )
{
	count_bt++;
	
	/* A jump is really just an extended branch
	dbInsertReferer( JT(in) + mem_start, cpu.pc, LabelBranchTarget ); */
}

static void JAL ( void )
{
	count_fc++;
	char name[512];
	
	sprintf( name, "FNC_0x%08X", JT(in) );
	if( dbFunctionExists( JT(in) ) )
		return;
	if( JT(in) < fsize && JT(in) >= 0x190 )
		dbInsertFunction( JT(in), name, "( int, ... )", "int", "", "" );
}

/* ==> Branching instructions */

static void BEQ ( void )
{
	count_bt++;
	
	/* Record branch destination
	dbInsertReferer( cpu.pc + OF(in), cpu.pc, LabelBranchTarget ); */
}

/* Other branch instructions that are handled identically */
#define BC1F     BEQ
#define BC1T     BEQ
#define BC1FL    BEQ
#define BC1TL    BEQ
#define BEQL 	 BEQ
#define BGEZ 	 BEQ
#define BGEZAL 	 BEQ
#define BGEZALL  BEQ
#define BGEZL 	 BEQ
#define BGTZ 	 BEQ
#define BGTZL	 BEQ
#define BLEZ 	 BEQ
#define BLEZL 	 BEQ
#define BLTZ 	 BEQ
#define BLTZAL 	 BEQ
#define BLTZALL  BEQ
#define BLTZL 	 BEQ
#define BNE 	 BEQ
#define BNEL 	 BEQ

/* ==> jump table patching instructions */

static void regimm ( void )
{
	jt_imm[in >> 16 & 0x1F]();
}

static void cop1 ( void )
{
	jt_cop1[in >> 21 & 0x1F]();
}

static void bc ( void )
{
	jt_bc[in >> 16 & 3]();
}


/*
 *  Instruction jump tables
 */

/* Main jump table */
static void (*jt[])( void ) =
{
	NOP,  regimm, J,   JAL, BEQ,  BNE,  BLEZ,  BGTZ,	/* 0 - 7 	*/
	ADDI, ADDIU,  NOP, NOP, NOP,  ORI,  NOP,   LUI,		/* 8 - F	*/
	NOP,  cop1,   NOP, NOP, BEQL, BNEL, BLEZL, BGTZL,	/* 10 - 17	*/
	NOP,  NOP,    NOP, NOP, NOP,  NOP,  NOP,   NOP,		/* 18 - 1F	*/
	NOP,  NOP,    NOP, NOP, NOP,  NOP,  NOP,   NOP,		/* 20 - 27	*/
	NOP,  NOP,    NOP, NOP, NOP,  NOP,  NOP,   NOP,		/* 28 - 2F	*/
	NOP,  NOP,    NOP, NOP, NOP,  NOP,  NOP,   NOP,		/* 30 - 37	*/
	NOP,  NOP,    NOP, NOP, NOP,  NOP,  NOP,   NOP		/* 38 - 3F	*/
};

/* Register immediate jump table */
static void (*jt_imm[])( void ) =
{
	BLTZ,   BGEZ,   BLTZL,   BGEZL,   NOP, NOP, NOP, NOP,
	BLTZAL, BGEZAL, BLTZALL, BGEZALL, NOP, NOP, NOP, NOP,
	NOP,    NOP,    NOP,     NOP,     NOP, NOP, NOP, NOP,
	NOP,    NOP,    NOP,     NOP,     NOP, NOP, NOP, NOP,
};

/* Coprocessor 1 jump table */
static void (*jt_cop1[])( void ) =
{
	NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, 
	bc,  NOP, NOP, NOP, NOP, NOP, NOP, NOP, 
	NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, 
	NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, 
};

/* Branch COP 1 instructions */
static void (*jt_bc[])( void ) =
{
	BC1F, BC1T, BC1FL, BC1TL
};

/* Begin analyzer */
void mips_process_file ( void )
{
	int i;
	
	cpu_init();
	
	for( i = 0; i < fsize; i += 4 )
	{
		/* Read next word */
		in = U32(fbuff + i);
		
		/* Execute it */
		cpu_exec( in );
		
		/* Status message */
		if( !((i + 4) % (fsize/32)) )
		{
			MESSAGE( "\r%.2f%%", (double)i / fsize * 100.0 );
		}
		
		cpu.pc += 4;
	}
	MESSAGE("\n");
	/* */ kprintf( "Analyzation done." );
}
