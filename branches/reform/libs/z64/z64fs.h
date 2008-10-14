/******************************
* Zelda 64 Filesystem Library *
******************************/
#ifndef __LZ64_FILE_TABLE_H
#define __LZ64_FILE_TABLE_H

/* File entry */
typedef struct Zelda64FSEntry
{
	unsigned	vstart;
	unsigned	vend;
	unsigned	start;
	unsigned	end;
}
Z64FSEntry;

/* File table */
typedef struct Zelda64FileTable
{
	/* Carrier */
	FILE * fhandle;
	
	/* Table information */
	char creator[32];
	char date[32];
	int  filecount;
	
	/* Address */
	unsigned	start;
	unsigned	end;
	
	/* File entries */
	struct Zelda64FSEntry * files;
}
Z64FS;

/* Functions */
Z64FS *            z64fs_open      ( FILE * handle                           );
const Z64FSEntry * z64fs_file      ( Z64FS * h, int id                       );
void               z64fs_read_file ( Z64FS * h, int id, unsigned char * dest );
void			   z64fs_close	   ( Z64FS * h                               );

/* Macros */
#define ZFileStart(h, id)			((h)->files[id].start)
#define ZFileEnd(h, id)				(!(h)->files[id].end ? (h)->files[id].start + (h)->files[id].vend - (h)->files[id].vstart : (h)->files[id].end)
#define ZFileSize(h, id)			(ZFileEnd(h, id) - ZFileStart(h, id))
#define ZFileRealStart(h, id)		((h)->files[id].start)
#define ZFileRealEnd(h, id)			(ZFileEnd(h, id))
#define ZFileRealSize(h, id)		(ZFileRealEnd(h, id) - ZFileRealStart(h, id))
#define ZFileVirtStart(h, id)		((h)->files[id].vstart)
#define ZFileVirtEnd(h, id)			((h)->files[id].vend)
#define ZFileVirtSize(h, id)		(ZFileVirtEnd(h, id) - ZFileVirtStart(h, id))
#define ZFileIsCompressed(h, id)	((h)->files[id].end ? 1 : 0)
#define ZFileExists(h, id)			((~(h)->files[id].end)|(~(h)->files[id].start))
#define ZFSStart(h)					((h)->start)
#define ZFSEnd(h)					((h)->end)

/* Function macros */
#define z64fs_entries(h)	(((h)->filecount))

#endif /* __LZ64_FILE_TABLE_H */
