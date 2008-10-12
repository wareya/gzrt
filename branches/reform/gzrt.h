/************************
* GUI Zelda 64 ROM Tool *
************************/
#ifndef __GZRT_H
#define __GZRT_H

/* Macros */
#define U32(x)		((x)[0] << 24 | (x)[1] << 16 | (x)[2] << 8 | (x)[3])
#define WU32(x, w)          \
{                           \
	(x)[0] = (w)>>24; 		\
	(x)[1] = (w)>>16& 0xFF; \
	(x)[2] = (w)>>8 & 0xFF; \
	(x)[3] = (w) & 0xFF;    \
}

/* Windows or Unix? */
#ifdef WIN32

/* Constants */
#define	GZRT_SLASH "\\"

/* Macros */
#define	MKDIR(x)	mkdir((x))
#else

/* Constants */
#define	GZRT_SLASH "/"

/* Macros */
#define	MKDIR(x)	mkdir((x), 0777)
#endif

/* Standard stuff */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <dirent.h>
#include <sys/time.h>
#include <time.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>
#include <ctype.h>

/* Zelda 64 library */
#include <z64/z64lib.h>
#include <rom/rom.h>

/* GUI */
#include <gui/gui.h>

/* Other */
#include <generic/generic.h>
#include <generic/plugins.h>
#include <generic/mem.h>

/* Application globals */
extern struct timeval program_start;

/* Functions */
void gzrt_init ( int argc, char **argv );

#endif
