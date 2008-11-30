#include <z64.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Debug */
#define STATUS( fmt, ... )  printf( fmt, ##__VA_ARGS__ )
#define LogRelocation( section, type, address, fmt, ... )   \
{                                                           \
    printf( "%s:%s:%08X\t\t", section, type, address );     \
    printf( fmt, ##__VA_ARGS__ );                           \
    fflush( stdout );                                       \
}

/* Macros - read */
#define U32(x)      ((x)[0] << 24 | (x)[1] << 16 | (x)[2] << 8 | (x)[3])
#define U24(x)      ((x)[0] << 16 | (x)[1] << 8  | (x)[2] )
#define U16(x)      ((x)[0] << 8  | (x)[1])

/* Macros - write */
#define WU32(x,w)   { (x)[0] = (w) >> 24; (x)[1] = (w) >> 16 & 0xFF; \
                      (x)[2] = (w) >> 8 & 0xFF; (x)[3] = (w) & 0xFF; }
#define WU16(x,w)   { (x)[0] = (w) >> 8; (x)[1] = (w) & 0xFF; }

/* Macros - relocation: get */
#define REL_GET_SECTION(x)  ((x) >> 28 & 15)
#define REL_GET_TYPE(x)     ((x) >> 24 & 15)
#define REL_GET_ADDRESS(x)  ((x) & 0xFFFFFF)

/* Macros - relocation: set */
#define REL_SET_SECTION(x,s)    { (x) &= 0x0FFFFFFF; (x) |= ((s) & 15) << 28; }
#define REL_SET_TYPE(x,t)       { (x) &= 0xF0FFFFFF; (x) |= ((s) & 15) << 24; }
#define REL_SET_ADDRESS(x,a)    { (x) &= 0xFF000000; (x) |= (a) & 0x00FFFFFF; }

/* Constants */
#define OVL_START   0x00BCEF30

/* OoT file spec */
struct OoTFile
{
    unsigned    start;
    unsigned    end;
};

/* Relocation enumerations */
enum RelocationSection
{
    SECTION_TEXT    = 0x04,
    SECTION_DATA    = 0x08,
    SECTION_RODATA  = 0x0C
};
enum RelocationType
{
    RELOC_ADDR  = 0x02,
    RELOC_JUMP  = 0x04,
    RELOC_LUI   = 0x05,
    RELOC_IM    = 0x06
};

struct RelocationEntry
{
    /* Specs */
    unsigned    type    :  4;
    unsigned    section :  4;
    
    /* Location */
    unsigned    address : 24;
};

/* Relocation table */
struct ReloationTable
{
    /* Total entry count */
    unsigned                    amount;
    
    /* Table */
    struct RelocationEntry    * table;
};

struct ActorHeader
{
    /* Addresses */
    union
    {
        struct 
        {
            unsigned    section_text;   /* Code section (exe)   */
            unsigned    section_data;   /* Data section (r/w)   */
            unsigned    section_rodata; /* Read-only data       */
            unsigned    section_bss;    /* Zero'd memory space  */
        };
        
        unsigned    sizes[4];
    };
    
    /* Amount of relocations */
    unsigned    relocations;
};

/* Actor struct */
struct Actor
{
    /* Storage for different sections */
    unsigned char   * section[4];
    
    /* Sizes for each section */
    unsigned          size[4];
    unsigned          address[4];
};

/* Handle */
typedef struct
{
    /* File data */
    unsigned        filesize;
    unsigned char * file;
    
    /* Relocation table */
    struct ReloationTable   rt;
    
    /* File info */
    struct ActorEntry     * f;
    
    /* Actor header */
    struct ActorHeader      header;
    unsigned                header_addr;
    
    /* Output */
    struct Actor            output;
}
RELOC;

/* String name for a section */
const char * strsection ( int section )
{
    const char *names[] =
    {
        "TEXT", "DATA", "RODATA", "BSS"
    };
    return names[section];
}

/* String name for type */
const char * strtype ( int type )
{
    const char *types[] = 
    {
        NULL, NULL, "ptr", NULL,
        "jmp", "lui", "imd"
    };
    return types[type];
}

/* Filesize */
unsigned filesize ( FILE * h )
{
    unsigned size;
    
    fseek( h, 0, SEEK_END );
    size = ftell( h );
    fseek( h, 0, SEEK_SET );
    
    return size;
}

/* Read from a file at an offset */
void read_file ( unsigned char * d, FILE * h, unsigned addr, unsigned len )
{
    /* Seek to the address */
    fseek( h, addr, SEEK_SET );
    
    /* Read into buffer */
    fread( d, len, 1, h );
}

/* Read byteswapped words into array */
void read_words ( unsigned * a, FILE * h, unsigned address, int amount )
{
    int i;
    
    /* Seek to address */
    fseek( h, address, SEEK_SET );
    
    /* Read requested amount of words */
    for( i = 0; i < amount; i++ )
    {
        unsigned char buffer[4];
        
        fread( buffer, 1, 4, h );
        a[i] = U32( buffer );
    }
}

/* Open a file for reading */
RELOC * reloc_init ( Z64 * rom, u32 id )
{
    RELOC     * ret = calloc( sizeof(RELOC), 1 );
    unsigned    head_addr, i, p;
    
    /* Load file */
    ret->file = malloc( ZFvSize(rom->fs, id) );
    z64_read_file( rom, id, ret->file );
    
    /* Get size */
    ret->filesize = ZFvSize(rom->fs, id);
    
    /*
    ** Relocation header
    */
    
    /* Load header */
    head_addr = ret->filesize - U32(ret->file + (ret->filesize - 4));
    ret->header_addr = head_addr;
    
    /* Check header */
    if( head_addr & 0xFFF00000 )
        return NULL;
    
    /* Read each word */
    for( i = 0; i < sizeof(struct ActorHeader); i += 4 )
        ((unsigned*)&ret->header)[i/4] = U32(ret->file + head_addr + i);
    
    /* Actor file */
    if( !(ret->f = z64at_entry_get(rom, z64at_lookup(rom, id))) )
        exit( 27 );
    
    /*
    ** Load relocation table
    */
    
    /* Allocate memory */
    ret->rt.table  = malloc( ret->header.relocations * sizeof(struct RelocationEntry) );
    ret->rt.amount = ret->header.relocations;
    
    /* Read and interpret */
    for( i = 0; i < ret->header.relocations; i++ )
    {
        /* Next word */
        unsigned    w = U32( ret->file + ret->header_addr + sizeof(struct ActorHeader) + i * 4 );
        
        /* Set type */
        ret->rt.table[i].type = REL_GET_TYPE(w);
        
        /* Set section */
        ret->rt.table[i].section = REL_GET_SECTION(w) / 4 - 1;
        
        /* Set address */
        ret->rt.table[i].address = REL_GET_ADDRESS(w);
    }
    
    /*
    ** Set aside space for output 
    */
    
    p = 0;
    
    /* Allocate memory for sections */
    for( i = 0; i < 4; i++ )
    {
        ret->output.section[i] = calloc( ret->header.sizes[i], 1 );
        ret->output.size[i]    = ret->header.sizes[i];
        
        /* Don't copy BSS section */
        if( i + 1 == 4 )
            continue;
        
        /* Copy original contents */
        memcpy( ret->output.section[i], ret->file + p, ret->header.sizes[i] );
        
        /* Update pointer */
        p += ret->header.sizes[i];
    }
    
    /* All done */
    return ret;
}

/*
** Relocate a file
*/
void reloc_apply ( RELOC * h, unsigned address )
{
    unsigned    i;
    int         pair = 0;
    
    /* Go through each entry */
    for( i = 0; i < h->rt.amount; i++ )
    {
        /* Read the data word */
        unsigned  * src  = (unsigned*)(h->output.section[h->rt.table[i].section] + h->rt.table[i].address);
        unsigned    data = U32((unsigned char*)src);
        
        /* Check the relocation type */
        switch( h->rt.table[i].type )
        {
            /* lui/addi pair*/
            case RELOC_LUI:
            {
              /* Grab next relocation entry */
              unsigned next = U32((unsigned char*)
              (
                  h->output.section[h->rt.table[i+1].section] + h->rt.table[i+1].address
              ));
              
              /* Create offset */
              unsigned d = ((data & 0xFFFF)<< 16) + (unsigned)(next & 0xFFFF);
              d -= h->f->dma_start;
              
              /* Debug */
              LogRelocation
              ( 
                    strsection(h->rt.table[i].section), 
                    strtype(h->rt.table[i].type), 
                    h->rt.table[i].address,
                    "%08X -> %08X\n",
                    data<<16|next&0xFFFF, d
              );
              
              /* Skip ahead one */
              i++;
            }
            break;
            
            /* absolute pointer */
            case RELOC_ADDR:
            {
              /* New address */
              unsigned d = data - h->f->dma_start;
              
              /* Debug */
              LogRelocation
              ( 
                    strsection(h->rt.table[i].section), 
                    strtype(h->rt.table[i].type), 
                    h->rt.table[i].address,
                    "%08X -> %08X\n",
                    data, d
              );
              
              /* Write it */
              WU32( ((unsigned char*)src), d );
            }
            break;
            
            /* jump target */
            case RELOC_JUMP:
            {
              /* New jump target */
              unsigned o = data & 0xFC000000;           /* Opcode bitmask   */
              unsigned t = (((data & 0x03FFFFFF) << 2)&0x7FFFFF);
              unsigned d = o | ((t - h->f->dma_start) >> 2);
              
              /* Check range */
              if( t >= (h->f->dma_start - 0x80800000) && t < (h->f->dma_end - 0x80800000) )
              {
                /* Debug */
                LogRelocation
                ( 
                    strsection(h->rt.table[i].section), 
                    strtype(h->rt.table[i].type), 
                    h->rt.table[i].address,
                    "%08X -> %08X\n",
                    t, t - (h->f->dma_start - 0x80800000)
                );
                
                /* Write it */
                WU32( ((unsigned char*)src), d );
              }
              else
              {
                /* Debug */
                LogRelocation
                ( 
                    strsection(h->rt.table[i].section), 
                    strtype(h->rt.table[i].type), 
                    h->rt.table[i].address,
                    "%08X -> External call, %d bytes offset\n",
                    t, (int)(t - (h->f->dma_start - 0x80800000)) 
                );
              }
            }
            break;
            
            default:
              printf( "Unknown type %u.\n", h->rt.table[i].type );
        }
    }
    
    /* Write the output */
    FILE * out = fopen( "out.bin", "wb" );
    for( i = 0; i < 4; i++ )
        fwrite( h->output.section[i], 1, h->output.size[i], out );
    fclose( out );
}

/* Dump the relocation table */
void reloc_table_dump ( RELOC * h )
{
    int i;
    
    for( i = 0; i < h->header.relocations; i++ )
        printf( "%08X:%u:%u\n", h->rt.table[i].address, 
        h->rt.table[i].section, h->rt.table[i].type );
}

/* Dump header */
void dump_header ( struct ActorHeader * h )
{
    unsigned    a = 0, old, i;
    char       *fmts[] =
    {
      "TEXT:\t$%08X - $%08X\n",   "DATA:\t$%08X - $%08X\n",
      "RODATA:\t$%08X - $%08X\n", "BSS:\t$%08X - $%08X\n"
    };
    
    for( i = 0; i < 4; i++ )
    {
        old = a;
        a += ((unsigned*)h)[i];
        
        printf( fmts[i], old, a );
    }
    
    printf( "Reloc.:\t%u\n", h->relocations );  
}

/* Dump information */
void reloc_info ( RELOC * h )
{
    dump_header( &h->header );
}

int main ( int argc, char **argv )
{
    struct OoTFile  f;
    Z64 * h;
    N64Rom * rom;
    RELOC * j;
    
    if( argc != 3 )
    {
        printf( "Prototype:\n  %s [rom] [file id]\n", argv[0] );
        exit( -1 );
    }
    
    /* Open the ROM */
    if( !(rom = n64rom_load(argv[1])) )
    {
        printf( "Unable to open ROM.\n" );
        exit( -1 );
    }
    
    /* Process the ROM */
    if( !(h = z64_open(rom)) )
    {
        printf( "Unable to load game elements.\n" );
        exit( -1 );
    }
    
    /* Show handle status */
    printf
    ( 
        "ROM status:\n"
        " - FS:   %i\n"
        " - NT:   %i\n"
        " - AT:   %i\n"
        " - OT:   %i\n"
        " - ST:   %i\n"
        " - Code: %i\n",
        !!ZLoadedFS(h),
        !!ZLoadedNT(h),
        !!ZLoadedAT(h),
        !!ZLoadedOT(h),
        !!ZLoadedST(h),
        !!ZLoadedCode(h)
    );
    
    printf( "Scanning relocations...\n" );
    
    if( !(j = reloc_init( h, atoi(argv[2]) )) )
    {
        puts( "No header" );
        return 0;
    }
    reloc_info( j );
    reloc_apply( j, 0x80000000 );
    
    return 0;
}
