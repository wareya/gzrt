#ifndef __Z64MV_DLIST_H
#define __Z64MV_DLIST_H

/* One RDP instruction is 64-bits long */
typedef u64 RDPINSTR;

/* Swap half word */
#define SWAP16(x) ((((x)&0xFF)<<8)|((x)>>8))

/**********************************
* Instruction manipulating macros *
**********************************/
#define ZGFX_INSTR(x)		((x) >> 56 & 0xFF)
#define ZGFX_BANK(x)		((x) >> 24 & 0xFF)
#define ZGFX_ADDR(x)		((x) & 0xFFFFFF)
#define ZGFX_VTX_SIZE(x)	((x) >> 40 & 0xFFFF)
#define ZGFX_VTX_VERTS(x)	(((x) >> 32 & 0xFF) >> 1)
#define ZGFX_VTX_ADDR(x)	((x) & 0xFFFFFF)
#define ZGFX_TRI2_T1V1(x)	(((x) >> 48 & 0xFF) >> 1)
#define ZGFX_TRI2_T1V2(x)	(((x) >> 40 & 0xFF) >> 1)
#define ZGFX_TRI2_T1V3(x)	(((x) >> 32 & 0xFF) >> 1)
#define ZGFX_TRI2_T2V1(x)	(((x) >> 16 & 0xFF) >> 1)
#define ZGFX_TRI2_T2V2(x)	(((x) >> 8  & 0xFF) >> 1)
#define ZGFX_TRI2_T2V3(x)	(((x)       & 0xFF) >> 1)
#define ZGFX_TRI1_V1(x)		(((x) >> 48 & 0xFF) >> 1)
#define ZGFX_TRI1_V2(x)		(((x) >> 40 & 0xFF) >> 1)
#define ZGFX_TRI1_V3(x)		(((x) >> 32 & 0xFF) >> 1)

/**********************
* F3DEX2 Instructions *
**********************/
#define	ZGFX_NOP			0x00
#define	ZGFX_RDPHALF_2		0xF1
#define	ZGFX_SETOTHERMODE_H	0xE3
#define	ZGFX_SETOTHERMODE_L	0xE2
#define	ZGFX_RDPHALF_1		0xE1
#define	ZGFX_SPNOOP			0xE0
#define	ZGFX_ENDDL			0xDF
#define	ZGFX_DL				0xDE
#define	ZGFX_LOAD_UCODE		0xDD
#define	ZGFX_MOVEMEM		0xDC
#define	ZGFX_MOVEWORD		0xDB
#define	ZGFX_MTX			0xDA
#define ZGFX_GEOMETRYMODE  	0xD9
#define	ZGFX_POPMTX			0xD8
#define	ZGFX_TEXTURE		0xD7
#define	ZGFX_SUBMODULE		0xD6
#define	ZGFX_VTX			0x01
#define	ZGFX_MODIFYVTX		0x02
#define	ZGFX_CULLDL			0x03
#define	ZGFX_BRANCH_Z		0x04
#define	ZGFX_TRI1			0x05
#define ZGFX_TRI2      		0x06
#define ZGFX_LINE3D    		0x07

/*
** Vertex format
*/
typedef struct ZVertex
{
	/* Coordinates */
	s16 x, y, z;
	
	/* Flags */
	u16 flags;
	
	/* Texture UV */
	s16 u, v;
	
	/* Color */
	u8 r, g, b, a;
} 
VERTEX;

#endif /* __Z64MV_DLIST_H */
