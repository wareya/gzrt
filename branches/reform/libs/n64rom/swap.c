/*******************************
* Nintendo 64 ROM Byteswapping *
*******************************/
#include <n64rom.h>
#include <stdio.h>
#include <glib.h>

/* Byteswap an array */
int n64_byteswap ( void * data, int size, enum N64Endian to, enum N64Endian from )
{
	u32	i;
	u8	buffer[4];
	
	/* Identical? */
	if( to == from )
		return 0;
	
	/* Which subroutine should we use? */
	if( from == N64_ENDIAN_BIG )
		
		/* From big endian... */
		switch( to )
		{
			case N64_ENDIAN_LITTLE: goto to_little; break;
			case N64_ENDIAN_V64:	goto to_v64;	break;
		}
	else if( from == N64_ENDIAN_LITTLE )
		
		/* From little endian... */
		switch( to )
		{
			case N64_ENDIAN_BIG: goto to_little; break;
			case N64_ENDIAN_V64: goto to_litv64; break;
		}
	else if( from == N64_ENDIAN_V64 )
		
		/* From V64 endian */
		switch( to )
		{
			case N64_ENDIAN_LITTLE: goto to_litv64; break;
			case N64_ENDIAN_BIG:	goto to_v64;	break;
		}
	
	/* Swap to V64 */
to_v64:
	for( i = 0; i < size; i += 4 )
	{
		buffer[0] = ((u8*)data)[i + 0];
		buffer[1] = ((u8*)data)[i + 1];
		buffer[2] = ((u8*)data)[i + 2];
		buffer[3] = ((u8*)data)[i + 3];
		((u8*)data)[i + 0] = buffer[1];
		((u8*)data)[i + 1] = buffer[0];
		((u8*)data)[i + 2] = buffer[3];
		((u8*)data)[i + 3] = buffer[2];
	}
	goto end;
	
	/* Swap to little */
to_little:
	for( i = 0; i < size; i += 4 )
	{
		buffer[0] = ((u8*)data)[i + 0];
		buffer[1] = ((u8*)data)[i + 1];
		buffer[2] = ((u8*)data)[i + 2];
		buffer[3] = ((u8*)data)[i + 3];
		((u8*)data)[i + 0] = buffer[3];
		((u8*)data)[i + 1] = buffer[2];
		((u8*)data)[i + 2] = buffer[1];
		((u8*)data)[i + 3] = buffer[0];
	}
	goto end;
	
	/* Swap little -> v64 */
to_litv64:
	for( i = 0; i < size; i += 4 )
	{
		buffer[0] = ((u8*)data)[i + 0];
		buffer[1] = ((u8*)data)[i + 1];
		buffer[2] = ((u8*)data)[i + 2];
		buffer[3] = ((u8*)data)[i + 3];
		((u8*)data)[i + 0] = buffer[2];
		((u8*)data)[i + 1] = buffer[3];
		((u8*)data)[i + 2] = buffer[0];
		((u8*)data)[i + 3] = buffer[1];
	}
	goto end;
	
	/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
	
	/* Finished */
end: 
	return TRUE;
}

/* strendian */
const char * n64rom_strendian ( enum N64Endian e )
{
	static const char * names[] =
	{
		"big endian", "little endian", "v64 endian"
	};
	
	if( e == N64_ENDIAN_BIG )
		return names[0];
	else if( e == N64_ENDIAN_LITTLE )
		return names[1];
	else if( e == N64_ENDIAN_V64 )
		return names[2];
	else
		return NULL;
}
