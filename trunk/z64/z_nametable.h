/*****************************
* Zelda 64 -- Filename table *
*****************************/

#ifndef __Z64LIB_NT
#define __Z64LIB_NT

#define __NAME_TABLE_START 0x0000BE80UL
#define __NAME_TABLE_END   0x00012312UL

typedef struct Z64NT Z64NT;
struct Z64NT
{
	zfu8 *nt;
	zfu8 *cur;
	zfu32 pos;
	zfu32 len;
};

zfu32 z64nt_count_entries ( Z64NT *handle );
Z64NT * z64nt_init ( char * filename );
void z64nt_read_next ( Z64NT *handle );

#endif
