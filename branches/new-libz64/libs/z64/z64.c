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
	
	/* Create file handle for ROM */
	ret->handle = fopen( filename, "rb" );
	
	/* Read filesystem */
	if( !(ret->fs = z64fs_open( ret->handle )) )
		return NULL;
	ret->status |= Z64_LOADED_FS;
	
	/* Read the name table */
	if( (ret->nt = z64nt_open( ret->handle )) )
		ret->status |= Z64_LOADED_NT;
	
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
	fclose( h->handle );
	free( h );
}

/* Read a file (and decompress it if required) */
void z64_read_file ( Z64 * h, int id, unsigned char * dest )
{
	/* Read the file */
    fseek( h->handle, ZFileRealStart(h->fs, id), SEEK_SET );
    fread( dest, ZFileRealSize(h->fs, id), 1, h->handle );
    
    /* Do we need to decompress it? */
    if( ZFileIsCompressed(h->fs, id) && !strncmp("Yaz0", dest, 4) )
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
	
	/* Some unique bytes with which to identify it */
	const guint8 code_ident[] = 
	{
		0x48, 0x20, 0x41, 0x32, 0x3A, 0x25, 0x30, 0x38,
		0x78, 0x48, 0x0A, 0x00, 0x41, 0x33, 0x3A, 0x25,
		0x30, 0x38, 0x78, 0x48, 0x20, 0x54, 0x30, 0x3A,
		0x25, 0x30, 0x38, 0x78, 0x48, 0x20, 0x54, 0x31,
		0x3A, 0x25, 0x30, 0x38, 0x78, 0x48, 0x0A, 0x00,
		0x54, 0x32, 0x3A, 0x25, 0x30, 0x38, 0x78, 0x48 
	};
	
	/* Does this context already have one? */
	if( h->f_code )
		return TRUE;
	
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
	return FALSE;
	
file_found: ;
	
	/* Read it in */
	tmp = malloc( ZFileVirtSize(h->fs, i) );
	z64_read_file( h, i, tmp );
	
	/* Scan for block */
	for( i = 0; i < ZFileVirtSize(h->fs, i); i += 16 )
		if( !memcmp( tmp + i, code_ident, sizeof(code_ident) ) )
		{
			/* We got it! */
			h->f_code = z64fs_file( h->fs, i );
			h->f_code_data = tmp;
			
			return TRUE;
		}
	
	/* Not found? Keep going... */
	free( tmp );
	goto continue_loop;
}

