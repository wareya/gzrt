/****************************
* Zelda 64 Filename Library *
****************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <z64nt.h>

/* Constants */
#define NAME_TABLE_START    0x0000BE80UL
#define NAME_TABLE_END      0x00012312UL
#define SIZE                (NAME_TABLE_END - NAME_TABLE_START)
#define COUNT               (SIZE / 16)

/* File entry */
struct Zelda64FileEntry
{
    unsigned    id;
    char        name[32];
};

/*
    This is -only- valid for the debug ROM.
*/

/* Create an instance from a file handle */
struct Zelda64NameTable *
z64nt_open ( FILE * handle )
{
    unsigned char * buffer, * seek;
    Z64NT         * ret;
    unsigned        i;
    
    /* Allocate memory for handle */
    ret = calloc( sizeof(Z64NT), 1 );
    
    /* Initialize the handle */
    ret->amount = COUNT;
    
    /* Allocate memory for buffer */
    buffer = malloc( SIZE );
    
    /* Seek to the start of the table */
    fseek( handle, NAME_TABLE_START, SEEK_SET );
    
    /* Read in the table */
    fread( buffer, 1, SIZE, handle );
    
    /* Check! */
    if( strncmp( buffer, "makerom", 7 ) )
    {
        free( buffer );
        free( ret );
        return NULL;
    }
    
    /* Copy each string to the hash table */
    for( seek = buffer, i = 0; i < ret->amount; i++ )
    {
        struct Zelda64FileEntry * t = malloc( sizeof(struct Zelda64FileEntry) );
        
        /* Set the entry data */
        t->id = i;
        seek += sprintf(t->name, "%s", seek );
        
        /* Store the string in the hash table */
        ret->names = g_list_append( ret->names, t );
        
        /* Seek past next null bytes */
        while( !*seek && seek < buffer + SIZE )
            seek++;
    }
    
    /* Free resources */
    free( buffer );
    
    /* Store start and end */
    ret->start = NAME_TABLE_START;
    ret->end   = NAME_TABLE_END;
    
    /* Return handle */
    return ret;
}

/* Read until newline or EOF */
static int
read_text ( unsigned char * store, size_t len, FILE * h )
{
    int next, count = 0;
    
    /* No newlines or EOF */
    while( (next = fgetc(h)) != EOF && next != '\n' && count < len - 1 )
    
        /* Store it */
        store[count++] = next;
    
    /* Null terminate */
    store[count] = 0;
    
    /* Return amount of characters read */
    return (next == EOF ? EOF : count);
}

/* Open a virtual name table */
struct Zelda64NameTable *
z64nt_open_virt ( char * filename )
{
    FILE * handle = fopen( filename, "r" );
    Z64NT * ret = calloc( sizeof(Z64NT), 1 );
    char name[256];
    
    if( !handle )
    {
        free( ret );
        return NULL;
    }
    
    if( !ret )
    {
        fclose( handle );
        return NULL;
    }
    
    /* Read from the input */
    while( read_text( name, sizeof(name), handle ) != EOF )
    {
        /* Comment? */
        if( name[0] == '#' )
            continue;
        
        /* Append the new filename to the list */
        ret->names = g_list_append( ret->names, name );
    }
    
    /* Close handle */
    fclose( handle );
    
    /* Set count */
    ret->amount = g_list_length( ret->names );
    
    /* Return it */
    return ret;
}

/* Close */
void z64nt_close ( Z64NT *  h )
{
    g_list_free( h->names );
    free( h );
}

/* For the below function */
static void print_entry ( gpointer * data, gpointer * user )
{
    struct Zelda64FileEntry * t = (struct Zelda64FileEntry *)data;
    
    printf( "[%4u] %s\n", t->id, t->name );
}

/* Dump a name table */
void z64nt_dump ( Z64NT * h )
{
    g_list_foreach( h->names, (GFunc)print_entry, NULL );
}

/* Get filename by id */
const char * z64nt_filename ( Z64NT * h, int id )
{
    GList * j = NULL;
    
    if( id >= 0 && id < COUNT )
        j = g_list_nth( h->names, id );
    else
        return NULL;
    
    return ((struct Zelda64FileEntry *)(j->data))->name;
}

/* Search for filename */
static int search_name ( gpointer a, gpointer b )
{
    struct Zelda64FileEntry * j = a;
    
    return strcmp( j->name, (char*)b );
}

/* Get id from filename */
int z64nt_id ( Z64NT * h, char * name )
{
    GList * result = g_list_find_custom( h->names, name, (GCompareFunc)search_name );
    
    return g_list_position( h->names, result );
}

/* Return start of name table */
unsigned z64nt_start ( Z64NT * h )
{
    return h->start;
}

/* Return end of name table */
unsigned z64nt_end ( Z64NT * h )
{
    return h->end;
}
