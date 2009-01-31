/*****************************
* GZRT File (ROM) Management *
*****************************/
#ifndef __GZRT_FILE_H__
#define __GZRT_FILE_H__

#include <sys/stat.h>
#include <z64.h>
#include <n64rom.h>

/* A file */
typedef struct
{
	char       * path;
	char       * name;
	FILE       * file;
	N64Rom     * rom;
	Z64        * z64;
	struct stat  info;
}
GzrtFile;

/* Functions -- file.c */
extern GzrtFile * gzrt_file_open ( char * path );

#endif /* __GZRT_FILE_H__ */
