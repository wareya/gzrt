/***********************************
* Memory searching library for N64 *
***********************************/
#ifndef __LIBSRCH_H__
#define __LIBSRCH_H__


/* Depends on `n64.h` */
#include <n64.h>


/* ----------------------------------------------
   Macros
   ---------------------------------------------- */

#define MS_DECLARE_READER( func )               \
 int func ( MS_SEARCH, void *, u32, void * )

#define MS_DECLARE_COMPARISON( func )           \
 int func ( MS_SEARCH, void *, void *, void * )


/* ----------------------------------------------
   Data types & declarations
   ---------------------------------------------- */

/* Structure declarations */
struct msearch_def;
struct msearch_t;

/* Properly declared shorthand */
typedef const struct msearch_def * MS_TYPE_DEF;
typedef struct msearch_t * MS_SEARCH;


/* ----------------------------------------------
   Function pointers
   ---------------------------------------------- */

/* A callback function */
typedef int (*msearch_cb)
(
    MS_SEARCH,  /* Search context from which invoked */
    void *      /* Specified user argument           */
);

/* A reader function */
typedef int (*msearch_read)
(
    MS_SEARCH,  /* Search context from which invoked */
    void *,     /* Target buffer (always big enough) */
    u32,        /* Memory address to read from       */
    void *      /* Specified user argument           */
);

/* A comparison function */
typedef int (*msearch_cmp)
(
    MS_SEARCH,  /* Search context from which invoked */
    void *,     /* Value 1 to compare                */
    void *,     /* Value 2 to compare                */
    void *      /* Specified user argument           */
);


/* ----------------------------------------------
   Structure: search type definition
   ---------------------------------------------- */

struct msearch_def
{
    const char *    name;       /* Human readable name of type (not required) */
    int             size;       /* Size of type in bytes                      */
    int             flags;      /* Flags for the type                         */
    msearch_read    reader;     /* Function for reading new values            */
    void *          areader;    /* User argument for above function           */
    msearch_cmp     cmp;        /* Function for comparing two values          */
    void *          acmp;       /* User argument for above function           */
};


/* ----------------------------------------------
   Structure: search context
   ---------------------------------------------- */

struct msearch_t
{
    MS_TYPE_DEF     type;       /* Type definition structure for context      */
    void *          value;      /* Value to match (assumed size type->size)   */
    int             flags;      /* Flags for search context                   */
    msearch_cb      callback;   /* Callback to invoke every so many searches  */
    int             cb_freq;    /* Callback interval                          */
    void *          cb_arg;     /* User argument for callback                 */
    u32             range[2];   /* Memory range to search through             */
    u32             cur;        /* Current working address of the searcher    */
    int             invoke;     /* Invoke count of the search (refinement)    */
    int             mcount;     /* Amount of matches for last search          */
    int *           matcharr;   /* Bit array of all the matches encountered   */
};



/* ----------------------------------------------
   Global variables
   ---------------------------------------------- */

/* Built-in type definitions */
extern MS_TYPE_DEF  __MTYPE_U8;
extern MS_TYPE_DEF  __MTYPE_U16;
extern MS_TYPE_DEF  __MTYPE_U32;
extern MS_TYPE_DEF  __MTYPE_U64;


/* ----------------------------------------------
   Global functions
   ---------------------------------------------- */

/* Type definition management */
extern MS_TYPE_DEF ms_type_create_full ( char *, int, int, msearch_read, void *, msearch_cmp, void * );
extern MS_TYPE_DEF ms_type_create ( char *, int );
extern void ms_type_free ( MS_TYPE_DEF );

/* Search context management */
extern MS_SEARCH ms_new_full ( MS_TYPE_DEF, void *, u32, u32, int, msearch_cb, int, void * );
extern MS_SEARCH ms_new ( MS_TYPE_DEF, void *, u32, u32 );
extern int ms_apply ( MS_SEARCH );
extern void ms_clear ( MS_SEARCH );
extern void ms_free ( MS_SEARCH );


#endif /* !__LIBSRCH_H__ */
