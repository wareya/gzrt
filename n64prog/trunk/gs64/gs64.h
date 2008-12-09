/****************
* N64 Gameshark *
****************/
#ifndef __GS64_H__
#define __GS64_H__

/* Storage in ROM of codes */
#define GS_ROM_OFFSET   (0xB0000000 | ((32 * 1024 * 1024) - (1024)))

/* Data types */
typedef unsigned int    u32;
typedef unsigned short  u16;
typedef unsigned char   u8;

/* Code data type */
struct Code
{
    unsigned type : 8;
    unsigned address : 24;
    unsigned value;
};

/* Return status */
enum CodeStatus
{
    Skip,
    Continue
};

/* A handler */
typedef enum CodeStatus (*CodeHandler)( struct Code * );

/* GS handler jump table */
extern CodeHandler gs_handlers[];

/* Function definitions */
extern void gs_process ( struct Code * list, int length );
extern int dma_read ( void * dst, void * src, u32 length );

/* Apply a single gameshark code */
static inline enum CodeStatus
gs_apply ( struct Code * c )
{
    return gs_handlers[c->type](c);
}

#endif /* __GS64_H__ */
