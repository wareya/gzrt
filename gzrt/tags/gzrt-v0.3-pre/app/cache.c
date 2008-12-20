/***************************
* GZRT Filetype Cache File *
***************************/
#include <glib.h>
#include <gzrt.h>
#include <app/md5.h>

struct CacheHeader
{
	guint8	rom_name[20];
	guint32	file_count;
	guint32	crc1;
	guint32	crc2;
};

struct CacheFile
{
	FILE  * ptr;
	gint	cur;
};

typedef struct
{
	struct CacheHeader	* head;
	struct CacheFile	* file;
	MAINWIN				* rom;
}
CACHE;

/* Create a cache file for a ROM */
CACHE * gzrt_cache_create ( MAINWIN * handle )
{
	char    checksum[33];
	char    checkstr[17];
	CACHE * ret;
	
	/* Generate checksum for filename */
	snprintf( checkstr, sizeof(checkstr), "%08X%08X", handle->c->head.CRC1, handle->c->head.CRC2 );
	md5_string( checksum, checkstr );
}
