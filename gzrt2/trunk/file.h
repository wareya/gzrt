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
	FILE       * fh;
	int          what;
}
GzrtFile;

/* What to load */
#define GZRT_FILE_FH	(1 << 1) /* Open a file handle */
#define GZRT_FILE_ROM	(1 << 2) /* Init ROM library   */
#define GZRT_FILE_Z64	((1 << 3) | GZRT_FILE_ROM) /* Init Z64 library   */

/* Functions -- file.c */
extern GzrtFile * gzrt_file_open ( int, char * );
extern void gzrt_file_close ( GzrtFile * );

#endif /* __GZRT_FILE_H__ */
