/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *\
@  Zelda 64: Filetype Detection  @
\* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef __Z64LIB_DETECT
#define __Z64LIB_DETECT

/* Filetypes */
enum {
	type_none, type_yaz0, type_zmap, type_zscene, type_zobj, type_zactor, 
	type_zdata, type_ztxt, type_zasm, type_zres, type_zgi
};

/* Functions */
char *string_filetype ( zfu8 type );
/* main detection function, calls all of the below */
int zd_detect_main ( zfu8 *s, zfu32 length );
int zd_detect_name_main ( char *D );
int zd_detect_main_old     (/*zfu8 *s,zfu32 l*/Z64FB *file); 
zfu8 zd_detect_yaz0     (zfu8 *s);
zfu8 zd_detect_zmap     (zfu8 *s,zfu32 length);
zfu8 zd_detect_zscene   (zfu8 *s,zfu32 length);
zfu8 zd_detect_zobj     (zfu8 *s,zfu32 length);
zfu8 zd_detect_zactor   (zfu8 *s,zfu32 length);
zfu8 zd_detect_ztxt     (zfu8 *s,zfu32 length);
zfu8 zd_detect_zasm     (zfu8 *s,zfu32 length);
zfu8 zd_detect_zres     (zfu8 *s,zfu32 length);
zfu8 zd_detect_zgi      (zfu8 *s,zfu32 length);

#endif
