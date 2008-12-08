/****************************
* Nintendo 64 DMA Functions *
****************************/
#include "gs64.h"

/* DMA regs struct */
typedef struct
{
    /* Pointers to data */
    void *ramp;
    void *romp;
    
    /* Filesizes (8-byte aligned) */
    u32 size_ramrom; /* RAM -> ROM */
    u32 size_romram; /* RAM <- ROM */
    
    /* Status register */
    u32 status;
} 
DMA_REG;

/* DMA status codes */
enum
{
    DMA_BUSY  = 0x00000001,
    DMA_ERROR = 0x00000008
};

/* DMA registers */
static volatile DMA_REG * dmaregs = (DMA_REG*)0xA4600000;

/* Copy data from ROM to RAM */
int dma_read ( void * dst, void * src, u32 length )
{
    /* Check that DMA is not busy already */
    while( dmaregs->status & DMA_BUSY );
    
    /* Write addresses */
    dmaregs->ramp = (void*)((u32)src & 0x00FFFFFF); /* ram pointer */
    dmaregs->romp = (void*)((u32)dst & 0x1FFFFFFF); /* rom pointer */
    
    /* Write size */
    dmaregs->size_romram = length - 1;
    
    /* Wait for transfer to finish */
    while( dmaregs->status & DMA_BUSY );
    
    /* Return size written */
    return length & 0xFFFFFFF8;
}
