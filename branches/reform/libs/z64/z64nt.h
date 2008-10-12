/****************************
* Zelda 64 Filename Library *
****************************/
#ifndef __LZ64_NAME_TABLE_H
#define __LZ64_NAME_TABLE_H

/* GLib stuff */
#include <glib.h>

typedef struct Zelda64NameTable
{
	/* Name table information */
	unsigned	start;
	unsigned 	end;
	
	/* Data */
	unsigned	amount;	/* Amount of entries			*/
	GList     * names;	/* Hash table for fast lookup	*/
}
Z64NT;

/* Functions */
Z64NT *      z64nt_open     ( FILE * handle          );
void         z64nt_dump     ( Z64NT * h              );
const char * z64nt_filename ( Z64NT * h, int id      );
int          z64nt_id       ( Z64NT * h, char * name );
unsigned     z64nt_start    ( Z64NT * h              );
unsigned     z64nt_end      ( Z64NT * h              );

#endif /* __LZ64_NAME_TABLE_H */
