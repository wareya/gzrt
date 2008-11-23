/**********************
* GHE File Operations *
**********************/
#ifndef __GHE_FILE_H__
#define __GHE_FILE_H__

typedef struct GheFile
{
	char * hname;
	char * path;
	char * filename;
	FILE * handle;
	int position;
}
GheFile;

typedef struct GheBuffer
{
	char * bname;
	int size;
	int partcount;
	struct GheBufferPiece * parts;
}
GheBuffer;

struct GheBufferPiece
{
	struct GheBuffer * parent;
	struct GheBufferPiece * prev;
	struct GheBufferPiece * next;
	int length;
	unsigned char * data;
};

#endif /* __GHE_FILE_H__ */
