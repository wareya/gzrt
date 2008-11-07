/*******************
* Zelda 64 Library *
*******************/
#include <z64.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>

/* Open a Zelda 64 ROM */
Z64 * z64_open ( char * filename )
{
	Z64 * ret;
	
	/* Allocate memory for struct */
	if( !(ret = calloc( sizeof(Z64), 1 )) )
		return NULL;
	
	/* Open the ROM */
	if( !(ret->rom = n64rom_load( filename )) )
	{
		free( ret );
		return NULL;
	}
	
	/* Copy file handle for ROM */
	ret->handle = ret->rom->handle;
	
	/* Read filesystem */
	if( !(ret->fs = z64fs_open( ret->handle )) )
	{
		n64rom_close( ret->rom );
		free( ret );
		return NULL;
	}
	ret->status |= Z64_LOADED_FS;
	
	/* Read the name table */
	if( (ret->nt = z64nt_open( ret->handle )) )
		ret->status |= Z64_LOADED_NT;
	
	/* Read the actor table */
	if( (ret->at = z64at_open( ret )) )
		ret->status |= Z64_LOADED_AT;
	
	/* Store filename */
	ret->filename = strdup( filename );
	
	return ret;
}

/* Close a Zelda 64 ROM */
void z64_close ( Z64 * h )
{
	z64fs_close( h->fs );
	z64nt_close( h->nt );
	free( h->filename );
	free( h->f_code_data );
	n64rom_close( h->rom );
	free( h );
}

/* Read a file (and decompress it if required) */
void z64_read_file ( Z64 * h, int id, unsigned char * dest )
{
	/* Read the file */
    fseek( h->handle, ZFileRealStart(h->fs, id), SEEK_SET );
    fread( dest, ZFileRealSize(h->fs, id), 1, h->handle );
    
    /* Do we need to decompress it? */
    if( ZFileIsCompressed(h->fs, id) && !strncmp("Yaz0", (char*)dest, 4) )
    {
        unsigned char * tmp = malloc(ZFileVirtSize(h->fs, id));
        
        /* Decode it */
        z64yaz0_decode( dest + 16, tmp, ZFileVirtSize(h->fs, id) );
        
        /* Copy it to destination */
        memcpy( dest, tmp, ZFileVirtSize(h->fs, id) );
        
        /* Free temp */
        free(tmp);
    }
}

/* Discover the code block in a Zelda ROM */
gboolean
z64_discover_code ( Z64 * h )
{
	int i;
	unsigned char * tmp;
	
	/* Does this context already have one? */
	if( h->f_code )
		return TRUE;
	
	/* Have we already tried and failed? */
	if( (h->status & Z64_NO_CODE) )
		return FALSE;
	
	/*
		The criteria for the code file are the following:
		 - One of the first 10 - 40 files
		 - Less than 2MB but greater than 400KB
		 - Contains the above chunk (just some debug text)
	*/
	
	/* Loop through the first 40 FS elements */
	for( i = 10; i < 40; i++ )
	{
		/* Size a match? */
		if( ZFileVirtSize(h->fs, i) > 400 * 1024 && 
			ZFileVirtSize(h->fs, i) < 2 * 1024 * 1024 )
		{
			goto file_found;
			
		continue_loop:
			continue;
		}
	}
	
	/* The file hasn't been found, no good */
	h->status |= Z64_NO_CODE;
	return FALSE;
	
file_found: ;
	
	/* Read it in */
	tmp = malloc( ZFileVirtSize(h->fs, i) );
	z64_read_file( h, i, tmp );
	
	/* Scan for block */
	if( z64detect_raw( tmp, ZFileVirtSize(h->fs, i) ) == Z64_ASM )
	{
		/* We got it! */
		h->f_code = z64fs_file( h->fs, i );
		h->f_code_data = tmp;
		h->status |= Z64_LOADED_CODE;
			
		return TRUE;
	}
	
	/* Not found? Keep going... */
	free( tmp );
	goto continue_loop;
}

