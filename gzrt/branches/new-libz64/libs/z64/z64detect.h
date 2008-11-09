/*********************
* Filetype detection *
*********************/
#ifndef __LZ64_DETECT_H
#define __LZ64_DETECT_H

/* Filetypes */
enum Z64Filetypes
{
	Z64_DATA,
	Z64_ACTOR, 
	Z64_OBJECT,
	Z64_MAP,
	Z64_SCENE,
	Z64_ASM
};

/* Detection functions */
int          z64detect_raw     ( unsigned char * data, int size );
int          z64detect_name    ( unsigned char * name           );
const char * z64detect_fileext ( int id                         );
	
#endif /* __LZ64_DETECT_H */
