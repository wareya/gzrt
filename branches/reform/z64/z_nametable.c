/*****************************
* Zelda 64 -- Filename table *
*****************************/
#include <gzrt.h>

/*
	There is no support for autodetecting the table as of yet,
	since the debug ROM is the only ROM with such a table.
*/

Z64NT * z64nt_init ( char * filename )
{	
	/* Variables */
	Z64NT *h;
	zfu32 i;
	FILE *handle;
	
	/* Set variables */
	h = (Z64NT*)malloc(sizeof(Z64NT));
	h->len=__NAME_TABLE_END-__NAME_TABLE_START;
	h->nt = (zfu8*)malloc(h->len);
	h->pos=0;
	h->cur=h->nt;
	
	/* File handle */
	if(!(handle = fopen(filename,"rb")))
		ZERROR("Cannot open file \"%s\" for reading. The error returned was:\n%s\n",
		filename,strerror(errno));

	
	/* Read name table */
	fseek(handle,__NAME_TABLE_START,SEEK_SET);
	fread(h->nt,1,__NAME_TABLE_END-__NAME_TABLE_START,handle);
	fclose(handle);
	
	/* Check */
	#define M "makerom"
	if(strncmp(h->nt,M,sizeof(M))){
		free(h->nt);
		free(h);
		return NULL;
	} else
		return h;
	#undef M
}

zfu32 z64nt_count_entries ( Z64NT *handle ){
	
	/* Variables */
	zfu8 status = 0, *p = handle->nt;
	zfu32 pos, entries = 0;
	
	/* Go go go */
	for(pos = 0; pos < handle->len; pos++){
		if(*(p+pos)){
			entries++;
			while(*(p+pos)) ++pos;
		}
	}
	return entries;
}

void z64nt_read_next ( Z64NT *handle ){
	
	#define B (handle->pos)
	#define T (handle->nt)
	
	/* Loop to and past next null bytes */
	while(*(B+T)) ++B;
	while(!*(B+T)) ++B;
	
	/* Set char pointer */
	handle->cur = T + B;
	
	#undef B
	#undef T
}

void z64nt_close ( Z64NT *h ){
	free(h->nt);
	free(h);
}
