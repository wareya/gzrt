/***********************
* Zelda 64 Actor Table *
***********************/
#ifndef __LZ64_ACTOR_TABLE_H
#define __LZ64_ACTOR_TABLE_H

#include <z64.h>
#include <glib.h>

/* An actor's entry in the table */
struct ActorEntry
{
	guint32		addr_start;		/* Location of file in ROM	*/
	guint32		addr_end;		/* Location of file in ROM	*/
	guint32		dma_start;		/* Used for relocation		*/
	guint32		dma_end;		/* Used for relocation		*/
	guint32		unkn1;
	guint32		unkn2;
	guint32		ram_name;		/* Address to filename		*/
	guint16		type;			/* Type of actor (see list) */
	guint16		unkn3;
};

/* The actor table handle */
typedef struct
{
	guint32		start;
	guint32		end;
	guint32		count;
	
	struct ActorEntry * entries;
} 
Z64AT;

/* Functions */

#endif /* __LZ64_ACTOR_TABLE_H */
