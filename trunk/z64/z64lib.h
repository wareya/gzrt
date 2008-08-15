/****************************
* ZELDA 64 FUNCTION LIBRARY *
****************************/

/* Version 2.0; by ZZT32; http://64.vg/ */
#ifndef __GZRT_LIBZ64
#define __GZRT_LIBZ64

#define ZFS_BYTES2LONG(x) ((x)[0] << 24 | \
                           (x)[1] << 16 | \
                           (x)[2] << 8  | \
                           (x)[3])

/* Includes - */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

#define ZERROR(x, ...) { \
	fprintf(stderr, "Error encountered in %s:%u -\n" x \
	"\nPress enter to quit.\n",__FILE__,__LINE__,##__VA_ARGS__); \
	getchar(); exit(0); \
}

/* Filetype standards */
enum { FT_STD, FT_ZELED, FT_UOT };
#define __FILE_EXTENSIONS FT_STD

/* types */
typedef unsigned char        zfu8;
typedef signed   char    	 zfs8;
typedef unsigned long 		zfu32;
typedef signed   long   	zfs32;
typedef unsigned long long	zfu64;
typedef signed   long long 	zfs64;
typedef unsigned short 		zfu16;
typedef signed   short   	zfs16;

/* Zelda 64 filebuffer */
typedef struct Z64FB Z64FB;
struct Z64FB 
{
	zfu8 *data;
	zfu8 type;
	zfu32 length;
	zfu8 *name;
};

/* Filesystem */
#include "z_filesystem.h"
#include "z_nametable.h"
#include "z_detect.h"
#include "z_scenetable.h"
#include "z_actortable.h"

/* Function definitions */
void yaz0_decode (unsigned char * src, unsigned char * dst, int uncompressedSize);
Z64AT *zat_init ( zfu8 *p, zfu32 length );
void zat_dump_handle ( Z64AT *h );
zfu32 zat_count_valid ( Z64AT *h );

#endif /* __GZRT_LIBZ64 */
