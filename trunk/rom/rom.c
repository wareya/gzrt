/**************************
* Minimal N64 ROM Library *
**************************/
#include <gzrt.h>

/* ROM library error string */
static char n64rom_err[1024];

/* Load a ROM */
N64ROM * n64rom_load ( char *filename )
{
	unsigned char b[4];
	
	/* Set aside memory for a struct */
	N64ROM *ret = calloc( sizeof(N64ROM), 1 );
	
	/* File details */
	ret->filename = gzrt_calloc(strlen(filename) + 1);
	strcpy( ret->filename, filename );
	if( !(ret->handle = fopen( filename, "rb+" )) )
		N64ROM_ERR( "Unable to open file." );
	
	/* Read ROM header */
	fread( ret->header, sizeof(ret->header), 1, ret->handle );
	fseek( ret->handle, 0, SEEK_END );
	ret->filesize = ftell(ret->handle);
	
	/* Set endian based on this & swap */
	switch( N64ROM_U32(ret->header) )
	{
		/* Big endian */
		case 0x80371240: 
		 ret->endian = N64_BIG;    
		break; 
		
		/* Little endian */
		case 0x40123780: 
		 ret->endian = N64_LITTLE;
		 
		 /* Swap */
		 n64rom_byteswap_mini( ret->header, sizeof(ret->header), N64_LITTLE, N64_BIG );
		break; 
		
		/* V64 endian */
		case 0x37804012: 
		 ret->endian = N64_V64;  
		 
		 /* Swap */
		 n64rom_byteswap_mini( ret->header, sizeof(ret->header), N64_V64, N64_BIG );
		break; 
		
		/* Not sure */
		default:
		 N64ROM_ERR( "Unknown identifier \"0x%08X\".", N64ROM_U32(ret->header) );
	}
	
	/* Trim name */
	trim( ret->header + 0x20 );
	
	/* Return it */
	return ret;
}

/* Return errbuff */
char * n64rom_error ( void )
{
	return n64rom_err;
}

/* Mini byteswap */
void n64rom_byteswap_mini ( unsigned char *data, int len, ENDIAN from, ENDIAN to )
{
	int i, s;
	
	if( (from == N64_BIG && to == N64_V64) ||  (from == N64_V64 && to == N64_BIG) )
		goto v64;
	
	if( (from == N64_BIG && to == N64_LITTLE) || (from == N64_LITTLE && to == N64_BIG) )
		goto reverse;
	
	v64:
	for( i = 0; i < len; i += 4 )
	{
		s = N64ROM_U32( data + i );
		data[i]     = s >> 16 & 0xFF;
		data[i + 1] = s >> 24 & 0xFF;
		data[i + 2] = s & 0xFF;
		data[i + 3] = s >> 8 & 0xFF;
	}
	goto end;
	
	reverse:
	for( i = 0; i < len; i += 4 )
	{
		s = N64ROM_U32( data + i );
		data[i] = s & 0xFF;
		data[i + 1] = s >> 8 & 0xFF;
		data[i + 2] = s >> 16 & 0xFF;
		data[i + 3] = s >> 24 & 0xFF;
	}
	
	end: return;
}

/* Read ROM */
int n64rom_read ( N64ROM *c, void (*cb)(int, int) )
{
	int chunk_size = c->filesize / 32, i;
	
	/* Allocate memory */
	if( !(c->data = gzrt_malloc(c->filesize)) )
		N64ROM_ERR( "Memory allocation error." );
	
	fseek( c->handle, 0, SEEK_SET );
	
	/* Do it faggot */
	for( i = 0; i < c->filesize; i += chunk_size )
	{
		fread( c->data + i, chunk_size, 1, c->handle );
		if( cb )
			cb( i, c->filesize );
	}
	
	if( cb )
		cb( i, c->filesize );
	
	return TRUE;
}

/* Close ROM */
void n64rom_close ( N64ROM * h )
{
	if( h->data )
		gzrt_free( h->data );
	gzrt_free( h );
}

/* Mammoth byteswap routine */
clock_t n64rom_swap ( N64ROM *h, int fws, int fmode, int tws, int tmode )
{
	/*
	** WS = word size (16-bit or 32-bit)
	** MODE = 0 for little, 1 for big
	** f = from
	** t = to
	*/
	
	register unsigned int c, w;
	unsigned int buff[16];
	clock_t time_start = clock();
	
	/* To normal byte order? */
	if( tmode == 1 )
	{
		/* Check from mode */
		if( fmode == 1 )
			return 0; /* big to big? no thanks */
		
		/* Check bit size */
		if( tws == 32 && fws == 16 )
			goto reverse_16bit;
		else if( tws == 32 && fws == 32 )
			goto reverse_32bit;
	}
	else
		
	/* To little endian */
	if( tmode == 0 )
	{
		/* Little to little? */
		if( fmode == 0 )
			return 0;
		
		/* Check from bit size */
		if( fmode == 1 && fws == 32 && tws == 16 )
		{
			goto reverse_16bit;
		}
		else
		
		/* 32-bit reverse */
	    if( fmode == 1 && fws == 32 && tws == 32 )
		{
			goto reverse_32bit;
		}
	}
		
	/* What? */
	return 0;
	
	/* ~~ ~~~ ~~~~ ~~~ ~~ ~~~ ~~~~ ~~~ ~~ ~~~ ~~~~ ~~~ ~~ ~~~ ~~~~ ~~~ ~~ */
	
	/* Reverse in 16-bit words */
	reverse_16bit: for( c = 0; c < h->filesize; c += 64 )
	{
		buff[0 ] = U32( h->data + c + 0x00 );
		buff[1 ] = U32( h->data + c + 0x04 );
		buff[2 ] = U32( h->data + c + 0x08 );
		buff[3 ] = U32( h->data + c + 0x0C );
		buff[4 ] = U32( h->data + c + 0x10 );
		buff[5 ] = U32( h->data + c + 0x14 );
		buff[6 ] = U32( h->data + c + 0x18 );
		buff[7 ] = U32( h->data + c + 0x1C );
		buff[8 ] = U32( h->data + c + 0x20 );
		buff[9 ] = U32( h->data + c + 0x24 );
		buff[10] = U32( h->data + c + 0x28 );
		buff[11] = U32( h->data + c + 0x2C );
		buff[12] = U32( h->data + c + 0x30 );
		buff[13] = U32( h->data + c + 0x34 );
		buff[14] = U32( h->data + c + 0x38 );
		buff[15] = U32( h->data + c + 0x3C );
		WU32( h->data + c + 0x00, WORD_REV16(buff[0 ]) );
		WU32( h->data + c + 0x04, WORD_REV16(buff[1 ]) );
		WU32( h->data + c + 0x08, WORD_REV16(buff[2 ]) );
		WU32( h->data + c + 0x0C, WORD_REV16(buff[3 ]) );
		WU32( h->data + c + 0x10, WORD_REV16(buff[4 ]) );
		WU32( h->data + c + 0x14, WORD_REV16(buff[5 ]) );
		WU32( h->data + c + 0x18, WORD_REV16(buff[6 ]) );
		WU32( h->data + c + 0x1C, WORD_REV16(buff[7 ]) );
		WU32( h->data + c + 0x20, WORD_REV16(buff[8 ]) );
		WU32( h->data + c + 0x24, WORD_REV16(buff[9 ]) );
		WU32( h->data + c + 0x28, WORD_REV16(buff[10]) );
		WU32( h->data + c + 0x2C, WORD_REV16(buff[11]) );
		WU32( h->data + c + 0x30, WORD_REV16(buff[12]) );
		WU32( h->data + c + 0x34, WORD_REV16(buff[13]) );
		WU32( h->data + c + 0x38, WORD_REV16(buff[14]) );
		WU32( h->data + c + 0x3C, WORD_REV16(buff[15]) );
	}
	
	/* Done */
	goto finish;
	
	/* ~~ ~~~ ~~~~ ~~~ ~~ ~~~ ~~~~ ~~~ ~~ ~~~ ~~~~ ~~~ ~~ ~~~ ~~~~ ~~~ ~~ */
	
	/* Reverse in 32-bit words */
	
	/*
	** NOTE:
	** This takes advantage of the X86's endian, and hence will not work
	** properly on big-endian architectures.
	*/
	
	reverse_32bit: for( c = 0; c < h->filesize; c += 64 )
	{
		w = U32( h->data + c + 0x00 );
		*((unsigned int*) (h->data + c + 0x00) ) = w;
		w = U32( h->data + c + 0x04 );
		*((unsigned int*) (h->data + c + 0x04) ) = w;
		w = U32( h->data + c + 0x08 );
		*((unsigned int*) (h->data + c + 0x08) ) = w;
		w = U32( h->data + c + 0x0C );
		*((unsigned int*) (h->data + c + 0x0C) ) = w;
		w = U32( h->data + c + 0x10 );
		*((unsigned int*) (h->data + c + 0x10) ) = w;
		w = U32( h->data + c + 0x14 );
		*((unsigned int*) (h->data + c + 0x14) ) = w;
		w = U32( h->data + c + 0x18 );
		*((unsigned int*) (h->data + c + 0x18) ) = w;
		w = U32( h->data + c + 0x1C );
		*((unsigned int*) (h->data + c + 0x1C) ) = w;
		w = U32( h->data + c + 0x20 );
		*((unsigned int*) (h->data + c + 0x20) ) = w;
		w = U32( h->data + c + 0x24 );
		*((unsigned int*) (h->data + c + 0x24) ) = w;
		w = U32( h->data + c + 0x28 );
		*((unsigned int*) (h->data + c + 0x28) ) = w;
		w = U32( h->data + c + 0x2C );
		*((unsigned int*) (h->data + c + 0x2C) ) = w;
		w = U32( h->data + c + 0x30 );
		*((unsigned int*) (h->data + c + 0x30) ) = w;
		w = U32( h->data + c + 0x34 );
		*((unsigned int*) (h->data + c + 0x34) ) = w;
		w = U32( h->data + c + 0x38 );
		*((unsigned int*) (h->data + c + 0x38) ) = w;
		w = U32( h->data + c + 0x3C );
		*((unsigned int*) (h->data + c + 0x3C) ) = w;
	}
	
	/* Done */
	goto finish;
	
	/* ~~ ~~~ ~~~~ ~~~ ~~ ~~~ ~~~~ ~~~ ~~ ~~~ ~~~~ ~~~ ~~ ~~~ ~~~~ ~~~ ~~ */
	
	/* Operation complete */
	finish: 
	
	GZRTD_MESG( "%u, %u, %u, %u: %08X", fws, fmode, tws, tmode, U32( h->data ) );
	GZRTD_MESG( "Swapped at %.4fMB/s.",
		((double)h->filesize / 1024.0 / 1024.0) / ((double)(clock() - time_start) / (double)CLOCKS_PER_SEC) );
	return clock() - time_start;
}
