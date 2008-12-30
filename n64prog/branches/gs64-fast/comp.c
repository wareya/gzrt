/************************************
* GameShark Code Recompilers (MIPS) *
************************************/
#include <stdio.h>
#include <stdlib.h>
#include "gs64.h"
#include "mips.h"

#define INSTR_BUFFER_SIZE   512

/* A MIPS function handle */
struct MipsFunc
{
    struct MipsInstr * chain;   /* The start of the list of instructions */
    struct MipsInstr * cur;     /* The last used instruction pointer     */
    int size;                   /* Size of all instructions combined     */
    u32 * real;                 /* A pointer to the function, if comp'd  */
};

/* A MIPS instruction handle */
struct MipsInstr
{
    /* Linked list... */
    struct MipsInstr * next;
    
    /* Data */
    u32 * data;
    int size;
};

/* Functions not in this file */
extern void gs_compile_call ( struct MipsFunc * );

/* Initialize */
struct MipsFunc *
gs_compile_init ( void )
{
    struct MipsFunc * ret;
    
    ret = malloc( sizeof(struct MipsFunc) );
    
    ret->chain = calloc( sizeof(struct MipsInstr), 1 );
    ret->chain->data = malloc( INSTR_BUFFER_SIZE );
    ret->cur = ret->chain;
    ret->size = 0;
    
    return ret;
}

/* Append an instruction */
void gs_compile_instr_append ( struct MipsFunc * h, u32 instr )
{
    struct MipsInstr * cur = h->cur;
    
    /* Buffer full? */
    if( cur->size >= INSTR_BUFFER_SIZE )
    {
        /* Add another node */
        struct MipsInstr * new = calloc( sizeof(struct MipsInstr), 1 );
        
        /* Link it up */
        cur->next = new;
        
        /* Create data buffer */
        new->data = malloc( INSTR_BUFFER_SIZE );
        
        /* Set current buffer */
        h->cur = new;
        cur = new;
    }
    
    /* Store instruction */
    *(u32*)((u8*)cur->data + cur->size) = instr;
    
    /* Update node size */
    cur->size += 4;
    
    /* Update total size */
    h->size += 4;
}

/* Finalize list */
void gs_compile_finalize ( struct MipsFunc * h )
{
    int i = 0;
    int dest = 0;
    struct MipsInstr * j;
    void * prev = NULL;
    
    /* Append a JR $RA and a nop */
    gs_compile_instr_append( h, JR(RA) );
    gs_compile_instr_append( h, 0      );
    
    /* Allocate memory */
    h->real = malloc( h->size );
    
    /* Step through list */
    for( j = h->chain; j; j = j->next )
    {
        /* Write instructions */
        for( i = 0; i < j->size; i++ )
            h->real[dest++] = j->data[i];
    }
    
    /* Free memory */
    for( j = h->chain; j; j = j->next )
    {
        if( prev )
            free( prev );
        free( j->data );
        prev = j;
        
        /* Last item in the list? */
        if( !j->next )
        {
            free( j );
            break;
        }
    }
}


/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ***
*** Functions for generating MIPS machine code based on code type ***
*** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ **/

/* The standard definition... */
#define GS_COMP(name)       void name ( struct MipsFunc * h, u32 c, u32 v )
#define COMPILE(instr)      gs_compile_instr_append( h, instr )

/* 8-bit write */
GS_COMP(gsc_80)
{
    COMPILE( LUI(V0, (c >> 16)) );              /* LUI  $v0, upper      */
    COMPILE( ORI(V1, R0, (v & 0xFF)) );         /* ORI  $v1, value      */
    COMPILE( SB(V1, c - ((c>>16)<<16), V0) );   /* SB   $v1, addr($v0)  */
}

/* 16-bit write */
GS_COMP(gsc_81)
{
    COMPILE( LUI(V0, (c >> 16)) );              /* LUI  $v0, upper      */
    COMPILE( ORI(V1, R0, v) );                  /* ORI  $v1, value      */
    COMPILE( SH(V1, c - ((c>>16)<<16), V0) );   /* SH   $v1, addr($v0)  */
}
