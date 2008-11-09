/******************************
* Zelda 64 Filetype Detection *
******************************/
#include <stdio.h>
#include <string.h>
#include <glib.h>

/* Macros */
#define U64(x)              \
(                           \
    (guint64)(x)[0] << 56 | \
    (guint64)(x)[1] << 48 | \
    (guint64)(x)[2] << 40 | \
    (guint64)(x)[3] << 32 | \
    (guint64)(x)[4] << 24 | \
    (guint64)(x)[5] << 16 | \
    (guint64)(x)[6] << 8  | \
    (guint64)(x)[7]         \
)
#define U32(x)          \
(                       \
    (x)[0] << 24 |      \
    (x)[1] << 16 |      \
    (x)[2] << 8  |      \
    (x)[3]              \
)

/* Checking functions */
typedef gboolean (*CheckRaw) (unsigned char *, int);
typedef gboolean (*CheckName)(unsigned char *     );

/* File types */
typedef struct FileTypes
{
    const char     * name;          /* Description              */
    const char     * suffix;        /* File extension           */
    const char     * prefix;        /* Filename prefix (if any) */
    const CheckRaw   check_raw;     /* Analyze the raw data     */
    const CheckName  check_name;    /* Analyze filename         */
}
FT;

/* Local function declarations - raw */
static gboolean detect_zactor_raw ( unsigned char *, int );
static gboolean detect_zobj_raw   ( unsigned char *, int );
static gboolean detect_zmap_raw   ( unsigned char *, int );
static gboolean detect_zscene_raw ( unsigned char *, int );
static gboolean detect_zasm_raw   ( unsigned char *, int );

/* Local function declarations - names */
static gboolean detect_zactor_name ( unsigned char * );
static gboolean detect_zobj_name   ( unsigned char * );
static gboolean detect_zmap_name   ( unsigned char * );
static gboolean detect_zscene_name ( unsigned char * );
static gboolean detect_zasm_name   ( unsigned char * );

/* File type descriptor */
static const FT ftypes[] =
{
    { "Unknown",     "zdata",  NULL,     NULL,              NULL               },
    { "Actor file",  "zactor", "ovl",    detect_zactor_raw, detect_zactor_name },
    { "Object file", "zobj",   "object", detect_zobj_raw,   detect_zobj_name   },
    { "Room file",   "zmap",   NULL,     detect_zmap_raw,   detect_zmap_name   },
    { "Scene file",  "zscene", NULL,     detect_zscene_raw, detect_zscene_name },
    { "Game core",   "zasm",   NULL,     detect_zasm_raw,   detect_zasm_name   }
};



/*                                                                            /*
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ **
**                                                                            **
** Filename-based detection                                                   **
**                                                                            **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ **
/*                                                                            */

/* Check for zactor filename */
static gboolean 
detect_zactor_name ( unsigned char * name )
{
    if( !strncmp("ovl_", name, 4) )
        return TRUE;
    else
        return FALSE;
}

/* Check for zobj filename */
static gboolean 
detect_zobj_name ( unsigned char * name )
{
    if( !strncmp("object_", name, 7) )
        return TRUE;
    else
        return FALSE;
}

/* Check for zmap filename */
static gboolean 
detect_zmap_name ( unsigned char * name )
{
    int len = strlen(name);
    
    if( strstr(name, "_room_") )
        return TRUE;
    else
        return FALSE;
}

/* Check for zscene filename */
static gboolean 
detect_zscene_name ( unsigned char * name )
{
    int len = strlen(name);
    
    if( !strncmp(name + len - 6, "_scene", 6) )
        return TRUE;
    else
        return FALSE;
}

/* Check for zasm filename */
static gboolean 
detect_zasm_name ( unsigned char * name )
{
    if( !strcmp(name, "code") )
        return TRUE;
    else
        return FALSE;
}



/*                                                                            /*
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ **
**                                                                            **
** Raw data based detection                                                   **
**                                                                            **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ **
/*                                                                            */

/* Discover a zactor from raw data */
static gboolean
detect_zactor_raw ( unsigned char * data, int size )
{
    unsigned    word;
    unsigned    offset;
    unsigned    total = 0;
    unsigned    headloc;
    int         i;
    
    /* Check header address */
    if( !(headloc = U32(data + size - 4)) )
        return FALSE;
    
    /* Check addresses */
    for( i = 0; i < 3; i++ )
    {
        /* Add section size */
        if( size - headloc + i * 4 >= size )
            return FALSE;
        total += U32(data + size - headloc + i * 4);
        
        /* Is it greater than the file? */
        if( total >= size )
            return FALSE;
    }
    
    /* Check amount of relocations in relation to size */
    word = U32(data + size - headloc + 0x10);
    offset = size - headloc + 0x14;
    if( word * 4 + offset >= size )
        return FALSE;
    
    /* Looks good */
    return TRUE;
}

/* Discover a zobj from raw data */
static gboolean
detect_zobj_raw ( unsigned char * data, int size )
{
    /*
    ** This is just a preliminary implementation. It'll work on a few objects 
    ** but certainly not all of them. Please remind me to fix this!
    */
    
    guint64     instruction;
    unsigned    seeker;
    gboolean    indlist = FALSE;
    unsigned    dlists = 0;
    
    /* Loop through each RSP instruction and check for inconsistencies */
    for( seeker = 0; seeker < size - 8; seeker += 8 )
    {
        /* Read next instruction */
        instruction = U64(data + seeker);
        
        /* Check for command with no operands */
        if( instruction & 0xFFFFFFFFFFFFFFULL )
            continue;
        
        /* Check for display list start */
        if( (instruction >> 56) == 0xE7 )
        {
            /* Are we already in a display list? */
            if( indlist == TRUE )
                
                /* Keep going */
                continue;
            
            else
            {
                /* Nope, but we are now */
                indlist = TRUE;
                dlists++;
                continue;
            }
        }
        
        /* Check for display list end */
        if( (instruction >> 56) == 0xDF )
        {
            /* Are we in a display list? */
            if( indlist == FALSE )
                
                /* Then this is not a model file */
                return FALSE;
                
            else
                
                /* Yep. But not anymore */
                indlist = FALSE;
        }
    }
    
    /* Have there been any display lists? */
    if( dlists && !indlist )
        return TRUE;
    else
        return FALSE;
}

/* Discover a zmap from raw data */
static gboolean
detect_zmap_raw ( unsigned char * data, int size )
{
    return FALSE;
}

/* Discover a zscene from raw data */
static gboolean
detect_zscene_raw ( unsigned char * data, int size )
{
    return FALSE;
}

/* Discover a zasm from raw data */
static gboolean
detect_zasm_raw ( unsigned char * data, int size )
{
    return FALSE;
}



/*                                                                            /*
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ **
**                                                                            **
** Main detection functions                                                   **
**                                                                            **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ **
/*                                                                            */

/* Detect filetype based on data */
int
z64detect_raw ( unsigned char * data, int size )
{
    int i;
    
    /* Step through each entry */
    for( i = 0; i < sizeof(ftypes)/sizeof(FT); i++ )
    {
        /* Try this filetype */
        if( ftypes[i].check_raw )
        if( ftypes[i].check_raw(data, size) )
            return i;
    }
    
    /* Nothing found */
    return 0;
}

/* Detect filetype based on filename */
int
z64detect_name ( unsigned char * name )
{
    int i;
    
    /* Step through each entry */
    for( i = 0; i < sizeof(ftypes)/sizeof(FT); i++ )
    {
        /* Try this filetype */
        if( ftypes[i].check_name )
        if( ftypes[i].check_name(name) )
            return i;
    }
    
    /* Nothing found */
    return 0;
}

/* Return the string representation of a filetype */
const char *
z64detect_fileext ( int id )
{
    /* Invalid ID? */
    if( id < 0 || id > sizeof(ftypes)/sizeof(FT) )
        return NULL;
    
    /* Return pointer */
    return ftypes[id].suffix;
}
