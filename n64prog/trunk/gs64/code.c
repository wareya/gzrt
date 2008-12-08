#include "gs64.h"

/* Define a handler */
#define GS_HANDLER( name )	\
static enum CodeStatus 				\
name ( struct Code * c )			

/* Code types */
enum CodeTypes
{
	W8	= 0x80,
	W16	= 0x81,
	W32 = 0x82,
	C8  = 0xD0,
	C16 = 0xD1,
	C32 = 0xD4,
	D8	= 0xD2,
	D16 = 0xD3,
	D32 = 0xD5,
	REP = 0x50
};

/* Repeater code */
struct RepeaterCode
{
	unsigned identifier : 16;
	unsigned number : 8;
	unsigned offset : 8;
	unsigned x : 16;
	signed	 inc : 16;
};

/* Functions */
enum CodeStatus gs_apply ( struct Code * c );
void gs_process ( struct Code * list, int length );

/* Default gameshark codes */
u32 gs_codes[] = 
{
	0x50001004, 0x00000020,
	0x82400000, 0x88000000
};

/* Placeholder */
GS_HANDLER( gs_invalid ) { return Continue; }

/* Handlers - basic write */
GS_HANDLER( gs_type_w8 )  {	*((u8*)(*(u32*)c)) = c->value; return Continue; }
GS_HANDLER( gs_type_w16 ) { *((u16*)(c->address|0x80000000)) = c->value; return Continue; }
GS_HANDLER( gs_type_w32 ) { *((u32*)(c->address|0x80000000)) = c->value; return Continue; }

/* Conditionals - if equal to */
GS_HANDLER( gs_type_c8 )
{
	if(*((u8*)(c->address|0x80000000)) == (c->value & 0xFF))
	  return Continue;
	else
	  return Skip;
}
GS_HANDLER( gs_type_c16 )
{
	if(*((u16*)(c->address|0x80000000)) == (c->value & 0xFFFF))
	  return Continue;
	else
	  return Skip;
}
GS_HANDLER( gs_type_c32 )
{
	if(*((u32*)(c->address|0x80000000)) == c->value)
	  return Continue;
	else
	  return Skip;
}

/* Conditionals - if not equal to */
GS_HANDLER( gs_type_d8 )
{
	if(*((u8*)(c->address|0x80000000)) != (c->value & 0xFF))
	  return Continue;
	else
	  return Skip;
}
GS_HANDLER( gs_type_d16 )
{
	if(*((u16*)(c->address|0x80000000)) != (c->value & 0xFFFF))
	  return Continue;
	else
	  return Skip;
}
GS_HANDLER( gs_type_d32 )
{
	if(*((u32*)(c->address|0x80000000)) != c->value)
	  return Continue;
	else
	  return Skip;
}

/* Repeater code */
GS_HANDLER( gs_type_rep )
{
	int i;
	struct RepeaterCode * r = (void*)c;
	struct Code * next = c+1;
	struct Code new;
	
	/* Initialize struct */
	new.type = next->type;
	new.address = next->address;
	new.value = next->value;
	
	/* Loop through the required iterations */
	for( i = 0; i < r->number; i++ )
	{
		/* Apply code */
		gs_apply( &new );
				  
		/* Update code */
		new.address += r->offset;
		new.value += r->inc;
	}
		  
	/* Do not process the next code */
	return Skip;
}

/* Jump table */
CodeHandler gs_handlers[256] =
{
	/* 0 -> 31 */
	gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid,
	gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid,
	gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid,
	gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid,
	
	/* 32 -> 63 */
	gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid,
	gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid,
	gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid,
	gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid,
	
	/* 64 -> 95 */
	gs_invalid,  gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid,
	gs_invalid,  gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid,
	gs_type_rep, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid,
	gs_invalid,  gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid,
	
	/* 96 -> 127 */
	gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid,
	gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid,
	gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid,
	gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid,
	
	/* 128 -> 159 */
	gs_type_w8, gs_type_w16, gs_type_w32, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid,
	gs_invalid, gs_invalid,  gs_invalid,  gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid,
	gs_invalid, gs_invalid,  gs_invalid,  gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid,
	gs_invalid, gs_invalid,  gs_invalid,  gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid,
	
	/* 160 -> 191 */
	gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid,
	gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid,
	gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid,
	gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid,
	
	/* 192 -> 223 */
	gs_invalid, gs_invalid,  gs_invalid, gs_invalid,  gs_invalid,  gs_invalid,  gs_invalid, gs_invalid,
	gs_invalid, gs_invalid,  gs_invalid, gs_invalid,  gs_invalid,  gs_invalid,  gs_invalid, gs_invalid,
	gs_type_c8, gs_type_c16, gs_type_d8, gs_type_d16, gs_type_c32, gs_type_d32, gs_invalid, gs_invalid,
	gs_invalid, gs_invalid,  gs_invalid, gs_invalid,  gs_invalid,  gs_invalid,  gs_invalid, gs_invalid,
	
	/* 223 -> 255 */
	gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid,
	gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid,
	gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid,
	gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid, gs_invalid
};

/* Process an array of gameshark codes */
void gs_process ( struct Code * list, int length )
{
	int i;
	
	for( i = 0; i < length; i++, list++ )
	{
		int ret = gs_apply( list );
		
		if( ret == Skip )
			list++;
	}
}
