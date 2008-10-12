/**************************
* Nintendo 64 ROM Library *
**************************/
#ifndef __LN64ROM_H
#define __LN64ROM_H

/* Depend */
#include <stdio.h>

/* Types */
typedef unsigned int u32;
typedef unsigned char u8;

/* Standard N64 header */
typedef struct Nintendo64Header
{
	unsigned		ident;
	unsigned		ClockRate;
	unsigned		EntryPoint;
	unsigned		Release;
	unsigned		CRC1;
	unsigned		CRC2;
	unsigned		Unknown0;
	unsigned		Unknown1;
	unsigned char 	name[20];
	unsigned		Unknown2;
	unsigned char	Unknown3[3];
	unsigned char	code[4];
	unsigned char	Unknown4;
}
N64Header;

/* Endian types */
enum N64Endian
{
	N64_ENDIAN_BIG    = 0x80,	/* ABCD */
	N64_ENDIAN_LITTLE = 0x40,	/* DCBA */
	N64_ENDIAN_V64    = 0x37	/* BADC */
};

/* N64 ROM context */
typedef struct
{
	/* Readable header */
	N64Header		head;
	
	/* Raw first 0x1060 bytes (guaranteed big endian) */
	unsigned char	makerom[0x1060];
	
	/* File handle */
	FILE          * handle;
	
	/* File information */
	unsigned char * filename;
	unsigned		filesize;
	
	/* Endian */
	enum N64Endian	endian;
}
N64Rom;

/* Functions - swap.c */
int          n64_byteswap     ( void * data, int size, enum N64Endian to, enum N64Endian from );
N64Rom *     n64rom_load      ( char * filename  );
void         n64rom_close     ( N64Rom * h       );
const char * n64rom_strendian ( enum N64Endian e );

#endif /* __LN64ROM_H */
