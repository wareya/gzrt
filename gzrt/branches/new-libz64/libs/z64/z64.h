/*******************
* Zelda 64 Library *
*******************/
#ifndef __LZ64_H
#define __LZ64_H

#include <stdio.h>
#include <glib.h>

/* N64 ROM Library */
#include <n64rom.h>

/* Zelda 64 deps */
#include "z64fs.h"
#include "z64nt.h"
#include "z64at.h"
#include "z64st.h"
#include "z64detect.h"
#include "z64yaz0.h"

/* Macros */
#define Z_PHYS_START( e )       ((e)->start)
#define Z_PHYS_END( e )       	(!(e)->end ? Z_PHYS_START(e) + Z_FILESIZE_VIRT(e) : (e)->end)
#define Z_VIRT_START( e )		((e)->vstart)
#define Z_VIRT_END( e )			((e)->vend)
#define Z_FILESIZE_VIRT( e )	(Z_VIRT_END(e) - Z_VIRT_START(e))
#define Z_FILESIZE_PHYS( e )	(Z_PHYS_END(e) - Z_PHYS_START(e))
#define Z_COMPRESSED( e )		(Z_PHYS_END(e))
#define Z_EXISTS( e )			((~Z_PHYS_START(e))|(~Z_PHYS_END(e)))

/* Status flags */
enum
{
	Z64_LOADED_FS   = 0x0001,
	Z64_LOADED_NT   = 0x0002,
	Z64_LOADED_AT   = 0x0004,
	Z64_LOADED_OT   = 0x0008,
	Z64_LOADED_ST   = 0x0010,
	Z64_LOADED_CODE = 0x0020,
	Z64_NO_CODE     = 0x0040
};

/* ROM context */
typedef struct
{
	/* ROM information */
	char   * filename;
	FILE   * handle;
	int	     filesize;
	N64Rom * rom;

	/* Context status (what's loaded, errors etc) */
	int    status;
	char   errbuff[512];

	/* Filesystem element - basis */
	Z64FS * fs;

	/* Extra elements */
	Z64NT * nt;		/* Name table   */
	Z64AT * at;		/* Actor table  */
	void  * ot;		/* Object table */
	Z64ST * st;		/* Scene table  */

	/* Important files */
	const Z64FSEntry * f_code;	/* The game's Code file */
	guint8           * f_code_data;

	/* Reserved... */
	int	resv1;
	int	resv2;
	int	resv3;
	int	resv4;
}
Z64;

/* Functions */
extern Z64 *     z64_open          ( char *                      );
extern void      z64_close         ( Z64 *                       );
extern void      z64_read_file     ( Z64 *, int, unsigned char * );
extern gboolean  z64_discover_code ( Z64 *                       );
extern Z64AT *   z64at_open        ( Z64 *                       );
extern Z64ST *   z64st_open        ( Z64 *                       );

#endif
