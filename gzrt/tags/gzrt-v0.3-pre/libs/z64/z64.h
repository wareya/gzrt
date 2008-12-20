/*******************
* Zelda 64 Library *
*******************/
#ifndef __LZ64_H
#define __LZ64_H

#include "z64fs.h"
#include "z64nt.h"
#include "z64yaz0.h"

/* Detection functions */
int          z64detect_raw     ( unsigned char * data, int size );
int          z64detect_name    ( unsigned char * name           );
const char * z64detect_fileext ( int id                         );

#endif
