#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#ifndef __Z64LIB_FS
#define __Z64LIB_FS

#define ZF_FILESIZE(x,y) { \
	fseek((y),0,SEEK_END); \
	x=ftell((y)); \
	fseek((y),0,SEEK_SET); \
}

/*
	#define Z64FS_ERROR(x) { \
		fprintf(stderr,"Error encountered in %s:%u - \n%s\n",__FILE__,__LINE__,(x)); \
		getchar(); exit(0); \
	}
*/

typedef struct Z64FS Z64FS;
struct Z64FS
{
	zfu32 rstart;
	zfu32 rend;
	zfu32 file_start;
	zfu32 file_end;
	zfu32 file_vstart;
	zfu32 file_vend;
	zfu32 pos;
	zfu8 *fs_table;
	zfu8 *fs_creator;
	zfu8 *fs_date;
	zfu8 *fs_time;
	zfu8  init;
};

/* Functions */
void 	 z64fs_close_handle           (Z64FS *handle);
void 	 z64fs_read_next              (Z64FS *fs_handle);
void 	 z64fs_read_arbitrary         (Z64FS *fs_handle, zfu16 id);
void 	 z64fs_dump_file_info         (Z64FS *handle);
void 	 z64fs_dump_handle            (Z64FS *fs_handle);
zfu32	 z64fs_calc_size_decompressed (Z64FS *handle);
zfu32	 z64fs_calc_size_compressed   (Z64FS *handle);
Z64FS	*z64fs_init                   (char *file);

/* Macros */
enum { Z1START, Z1END, Z2START, Z2END };
#define z64fs_num_entries(x)   ((((x)->rend)-((x)->rstart))/16)
#define z64fs_file_exists(x)   ((x)->file_vend==0xFFFFFFFFUL?0:1)
#define z64fs_is_compressed(x) ((x)->file_vend==0?0:1)
#define z64fs_fs_set(x,y,z)    { \
	switch(y){ \
		case Z1START: \
		(x)->file_start = (z); \
		break; \
		case Z1END: \
		(x)->file_end = (z); \
		break; \
		case Z2START: \
		(x)->file_vstart = (z); \
		break; \
		case Z2END: \
		(x)->file_vend = (z); \
		break; \
	} \
}

/* Shorthand Macros */
#define Z1FSIZE(handle)  (ZF1END(handle)-ZF1START(handle))
#define Z2FSIZE(handle)  (ZF2END(handle)-ZF2START(handle))
#define ZFCPOS(handle)   ((handle)->pos/16)
#define ZF1START(handle) ((handle)->file_start)
#define ZF1END(handle)   ((handle)->file_end)
#define ZF2START(handle) ((handle)->file_vstart)
#define ZF2END(x)   	 ((x)->file_vend?(x)->file_vend: \
	ZF2START(x) + (ZF1END(x) - ZF1START(x)))

#endif
