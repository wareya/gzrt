/******************************
* Zelda 64 Filesystem Library *
******************************/
#ifndef __LZ64_FILE_TABLE_H
#define __LZ64_FILE_TABLE_H

/* File entry */
typedef struct Zelda64FSEntry
{
    unsigned    vstart;
    unsigned    vend;
    unsigned    start;
    unsigned    end;
}
Z64FSEntry;

/* File table */
typedef struct Zelda64FileTable
{
	/* Table information */
    char creator[32];
    char date[32];
    int  filecount;
    
    /* Address */
    unsigned    start;
    unsigned    end;
    
    /* File entries */
    struct Zelda64FSEntry * files;
}
Z64FS;

/* Functions */
Z64FS * z64fs_open ( FILE * handle );
void z64fs_close ( Z64FS * h );
unsigned z64fs_size_virt ( Z64FS * h );
unsigned z64fs_size_phys ( Z64FS * h );

/* Macros */
#define ZFSStart(h)				((h)->start)
#define ZFSEnd(h)				((h)->end)
#define ZFSCount(h)				(ZFSEnd(h) - ZFSStart(h))
#define ZFvSize(h, id)			(ZFvEnd(h, id) - ZFvStart(h, id))
#define ZFSize(h, id)			(ZFEnd(h, id) - ZFStart(h, id))
#define ZFvStart(h, id)			((h)->files[id].vstart)
#define ZFvEnd(h, id)			((h)->files[id].vend)
#define ZFStart(h, id)			((h)->files[id].start)
#define ZFEnd(h, id)			(!(h)->files[id].end ? (ZFStart(h, id) + ZFvSize(h, id)) : (h)->files[id].end)
#define ZFExists(h, id)			((~(h)->files[id].end)|(~(h)->files[id].start))
#define ZFCompressed(h, id)		(!(h)->files[id].end)

/* Functions that are really macros */
#define z64fs_entries(h)    	(((h)->filecount))

/*
** Fast functions
*/

/* Return a pointer to the file entry of an id */
static inline const Z64FSEntry * 
z64fs_file ( Z64FS * h, int id )
{
    return &h->files[id];
}

#endif /* __LZ64_FILE_TABLE_H */
