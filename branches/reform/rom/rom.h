/**************************
* Minimal N64 ROM Library *
**************************/
#ifndef __N64ROM_H
#define __N64ROM_H

#define N64ROM_U32(x)	((x)[0] << 24 | (x)[1] << 16 | (x)[2] << 8 | (x)[3])
#define N64ROM_ERRBUFF	n64rom_err
#define N64ROM_ERR(x, ... ) \
{	\
	snprintf( N64ROM_ERRBUFF, sizeof(N64ROM_ERRBUFF),	\
	(x), ##__VA_ARGS__ );	\
	return NULL; \
}

/* CRC */
#include <rom/crc.h>

typedef enum
{
	N64_BIG,
	N64_V64,
	N64_LITTLE
} ENDIAN;

/* ROM Context */
typedef struct N64Rom
{
	/* File stuff */
	char *filename;
	FILE *handle;
	int filesize;
	
	/* ROM information */
	unsigned char header[0x40];
	ENDIAN endian;
	
	/* ROM contents */
	unsigned char *data;
} N64ROM;

N64ROM * n64rom_load ( char *filename );
char * n64rom_error ( void );
void n64rom_byteswap_mini ( unsigned char *data, int len, ENDIAN from, ENDIAN to );
int n64rom_read ( N64ROM *c, void (*cb)(int, int) );
void n64rom_close ( N64ROM * h );
clock_t n64rom_swap ( N64ROM *h, int fws, int fmode, int tws, int tmode );

/* Macros */
#define REV32(x) 		(((x)&0xFF)<<24|((x)>>8&0xFF)<<16|((x)>>16&0xFF)<<8|((x)>>24&0xFF))
#define REV16(x) 		(((x)&0xFF)<<8|((x>>8)&0xFF))
#define WORD_REV16(w)	(REV16(w >> 16) << 16 | REV16(w))

#endif /* __N64ROM_H */
