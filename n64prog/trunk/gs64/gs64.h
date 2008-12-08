/****************
* N64 Gameshark *
****************/
#ifndef __GS64_H__
#define __GS64_H__

/* Storage in ROM of codes */
#define GS_ROM_OFFSET	0xB1FFF000

/* Data types */
typedef unsigned int	u32;
typedef unsigned short	u16;
typedef unsigned char	u8;

/* Code data type */
struct Code
{
	unsigned type : 8;
	unsigned address : 24;
	unsigned value;
};

/* Function definitions */
extern enum CodeStatus gs_apply ( struct Code * c );
extern void gs_process ( struct Code * list, int length );
extern int dma_read ( void * dst, void * src, u32 length );

#endif /* __GS64_H__ */
