/***************************
* Memory searching library *
***************************/
#include <n64.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "search.h"


/* ----------------------------------------------
   Local macros/constants
   ---------------------------------------------- */

#define COMPARE(search, val1, val2)     \
 (search)->type->cmp                    \
 (                                      \
     search,                            \
     val1,                              \
     val2,                              \
     search->type->acmp                 \
 )

/* Shorthand for invoking reader */
#define READ(search, buffer)            \
 (search)->type->reader                 \
 (                                      \
    search,                             \
    buffer,                             \
    (search)->cur,                      \
    (search)->type->areader             \
 )
 
/* Calculate size of bit array */
#define CALC_ARR_SIZE(s, e, bs)         \
    ((e - s) / bs / 8)


/* ----------------------------------------------
   Assembly-implemented declarations
   ---------------------------------------------- */
   
extern int __libscmp_u8  ( MS_SEARCH, void *, void *, void * );
extern int __libscmp_u16 ( MS_SEARCH, void *, void *, void * );
extern int __libscmp_u32 ( MS_SEARCH, void *, void *, void * );
extern int __libscmp_u64 ( MS_SEARCH, void *, void *, void * );


/* ----------------------------------------------
   Local declarations
   ---------------------------------------------- */

/* Functions */
static MS_DECLARE_READER( read_generic );
static MS_DECLARE_COMPARISON( compare_generic );


/* ----------------------------------------------
   Local variables
   ---------------------------------------------- */

/* Local complete definition */
static struct msearch_def
type_definition[] =
{
    { "byte",        1, 0, NULL, NULL, __libscmp_u8,  NULL },
    { "half-word",   2, 0, NULL, NULL, __libscmp_u16, NULL },
    { "word",        4, 0, NULL, NULL, __libscmp_u32, NULL },
    { "double-word", 8, 0, NULL, NULL, __libscmp_u64, NULL },
    { "float",       4, 0, NULL, NULL, __libscmp_u32, NULL },
    { "double",      8, 0, NULL, NULL, __libscmp_u64, NULL }
};


/* ----------------------------------------------
   Local functions
   ---------------------------------------------- */

/* Generic read function */
static int read_generic ( MS_SEARCH s, void * t, u32 addr, void * u )
{
    /* Use memcpy to read raw */
    memcpy( t, (void*)addr, s->type->size );
    
    /* Return success */
    return 1;
}

/* Generic comparison function */
static int compare_generic ( MS_SEARCH s, void * v1, void * v2, void * u )
{
    /* Use memcmp to compare */
    return memcmp( v1, v2, s->type->size );
}

/* Turn on a bit */
static void bit_on ( int * arr, int bit )
{
    int idx = bit / 8, left = bit % 8;
    
    arr[idx] |= 1 << left;
}

/* Turn off a bit */
static void bit_off ( int * arr, int bit )
{
    int idx = bit / 8, left = bit % 8;
    
    arr[idx] &= ~(1 << left);
}


/* ----------------------------------------------
   Global variables
   ---------------------------------------------- */

/* Pointers to type definitions */
MS_TYPE_DEF  __MTYPE_U8  = &type_definition[0];
MS_TYPE_DEF  __MTYPE_U16 = &type_definition[1];
MS_TYPE_DEF  __MTYPE_U32 = &type_definition[2];
MS_TYPE_DEF  __MTYPE_U64 = &type_definition[3];
MS_TYPE_DEF  __MTYPE_F32 = &type_definition[4];
MS_TYPE_DEF  __MTYPE_F64 = &type_definition[5];


/* ----------------------------------------------
   Global functions - types
   ---------------------------------------------- */

/* Create a new type definition */
MS_TYPE_DEF
ms_type_create_full ( char * name, int size, int flags, msearch_read read, 
    void * aread, msearch_cmp cmp, void * acmp )
{
    struct msearch_def * ret;
    
    /* Reader? */
    if( !read )
        read = read_generic;
    
    /* Compare? */
    if( !cmp )
        cmp = compare_generic;
    
    /* Allocate memory for type definition */
    if( !(ret = malloc( sizeof(struct msearch_def) )) )
        return NULL;
    
    /* Populate structure */
    ret->name    = strdup( name );
    ret->size    = size;
    ret->flags   = flags;
    ret->reader  = read;
    ret->areader = aread;
    ret->cmp     = cmp;
    ret->acmp    = acmp;
    
    /* Finished */
    return ret;
}

/* Create a new type definition with defaults */
MS_TYPE_DEF
ms_type_create ( char * name, int size )
{
    return ms_type_create_full
    ( 
        name, 
        size, 
        0, 
        NULL, 
        NULL, 
        NULL, 
        NULL 
    );
}

/* Free a type definition */
void ms_type_free ( MS_TYPE_DEF def )
{
    free( (void*)def->name );
    free( (void*)def );
}


/* ----------------------------------------------
   Global functions - search
   ---------------------------------------------- */

/* Create a new search context */
MS_SEARCH
ms_new_full ( MS_TYPE_DEF type, void * orig, u32 start, u32 end, int flags,
    msearch_cb callback, int cb_freq, void * cb_arg )
{
    /*
    ** type     = type definition
    ** orig     = value to match
    ** start    = starting address
    ** end      = ending address
    ** flags    = search flags
    ** callback = function to call
    ** cb_freq  = call frequency
    ** cb_arg   = argument to pass callback
    */
    
    MS_SEARCH ret;
    
    /* Check that memory area is word-aligned */
    if( (end - start) % 8 )
        return NULL;
    
    /* Allocate memory */
    if( !(ret = calloc( sizeof(struct msearch_t), 1 )) )
        return NULL;
    
    /* Set values */
    ret->type     = type;
    ret->value    = orig;
    ret->flags    = flags;
    ret->callback = callback;
    ret->cb_freq  = cb_freq;
    ret->cb_arg   = cb_arg;
    ret->range[0] = start;
    ret->range[1] = end;
    
    /* Allocate memory for result buffer */
    ret->matcharr = calloc( (end - start) / type->size / 8, 1 );
    
    /* Done */
    return ret;
}

/* Create a new search context (simple) */
MS_SEARCH
ms_new ( MS_TYPE_DEF type, void * orig, u32 start, u32 end )
{
    /* Just call the full function w/ defaults */
    return ms_new_full
    (
        type,
        orig,
        start,
        end,
        0,
        NULL,
        -1,
        NULL
    );
}

/* Apply a current search */
int ms_apply ( MS_SEARCH search )
{
    int i, itr, v;
    int count = search->range[1] - search->range[0];
    int step = search->type->size;
    static u8 buffer[32];
    
    /* Set initial values */
    search->cur = search->range[0];
    search->mcount = 0; /* Reset matches */
    
    /* Loop through */
    for( i = 0, itr = 0; i < count; i += step, itr++ )
    {
        /* Is the reader function used? */
        if( search->type->reader )
        {
            /* Read the next data */
            READ( search, buffer );
            
            /* Compare it */
            v = COMPARE( search, buffer, search->value );
        }
        else
        {
            /* Nope - let comparison function read */
            v = COMPARE( search, (void*)search->cur, search->value );
        }
        
        /* Match? */
        if( !v )
        {
            /* Yes - update match counter */
            search->mcount++;
            
            /* Set bit */
            bit_on( search->matcharr, itr );
        }
        else
        {
            /* No good - just in case... */
            bit_off( search->matcharr, itr );
        }
        
        /* Invoke callback? */
        if( !((itr + 1) % search->cb_freq) )
        {
            /* Yes, call it w/ user arg */
            search->callback( search, search->cb_arg );
        }
        
        /* Update current address */
        search->cur += step;
    }
    
    /* Update invoke counter */
    search->invoke++;
    
    /* Good */
    return search->mcount;
}

/* Clear a search context */
void ms_clear ( MS_SEARCH search )
{
    search->invoke = 0;
    search->cur = search->range[0];
    search->mcount = 0;
    memset
    (
        search->matcharr, 0, 
        CALC_ARR_SIZE(search->range[0], search->range[1], search->type->size)
    );
}

/* Free a search context */
void ms_free ( MS_SEARCH search )
{
    free( search->matcharr );
    free( search );
}

