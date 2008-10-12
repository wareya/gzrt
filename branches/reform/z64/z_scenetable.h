/***************************
* Scene Table Modification *
***************************/

#ifndef __Z64LIB_ST
#define __Z64LIB_ST

#define ZF_MAX(x,y) ((x)>(y)?(x):(y))

#define __ST_KEY_MM_STAR  /*"\xD9\xFF\xFF\xFF\x00\x00\x04\x00\xDF\x00\x00\x00\x00\x00\x00\x00"*/ \
				"\xE2\x00\x00\x1C\x0C\x18\x49\xF8\xDF\x00\x00\x00\x00\x00\x00\x00\xE2\x00\x00\x1C\xC8\x10\x49\xF8\xDF\x00\x00\x00\x00\x00\x00\x00"
				/*"\xE2\x00\x00\x1C\xC8\x10\x49\xD8\xDF\x00\x00\x00\x00\x00\x00\x00\xE2\x00\x00\x1C\x0C\x18\x49\xF8\xDF\x00\x00\x00\x00\x00\x00\x00"
			  "\xE2\x00\x00\x1C\x0C\x18\x49\xF8\xDF\x00\x00\x00\x00\x00\x00\x00\xE2\x00\x00\x1C\xC8\x10\x49\xF8\xDF\x00\x00\x00\x00\x00\x00"*/
#define __ST_KEY_MM_END   "\x12\x00\x41\x02\x12\x01\x41\x02\x12\x02\x41\x02"
#define __ST_KEY_OOT_STAR "\x57\x09\x41\x83\x57\x09\x41\x83\x57\x09\x41\x83\x57\x09\x41\x83" \
			  "\x5C\x08\x41\x83\x5C\x08\x41\x83\x5C\x08\x41\x83\x5C\x08\x41\x83"
#define z64st_mm_num_entries(x) (((x)->t_end-(x)->t_start)/16)

typedef struct {
	zfu8 *table;
	zfu32 t_start;
	zfu32 t_end;
	zfu16 pos;
	zfu32 off1;
	zfu32 off2;
	zfu64 flags; /* don't really know what these are */
} Z64ST_MM;

typedef struct {
	zfu8 *table;
	zfu32 t_start;
	zfu32 t_end;
	zfu16 pos;
	zfu32 off1;
	zfu32 off2;
	zfu32 timg_start;
	zfu32 timg_end;
	zfu32 flag; /* don't really know what this is */
} Z64ST_OOT;


/* Functions */
Z64ST_MM * z64st_init ( char * filename, zfu8 ram );
void z64st_mm_read_arbitrary (Z64ST_MM *h, zfu16 id);
void z64st_mm_read_next (Z64ST_MM *h);
void z64st_mm_dump (Z64ST_MM *h);
Z64ST_OOT *z64st_oot_init_r ( zfu8 *block, zfu32 length );
Z64ST_MM *z64st_mm_init_r ( zfu8 *block, zfu32 length );
void z64st_oot_read_arbitrary (Z64ST_OOT *h, zfu16 id);
void z64st_oot_read_next (Z64ST_OOT *h);
void z64st_oot_dump_small (Z64ST_OOT *h);
void z64st_oot_close (Z64ST_OOT *h);

#endif
