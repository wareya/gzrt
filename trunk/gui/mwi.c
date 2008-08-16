/*********************************
* GZRT Multiple Window Interface *
*********************************/
#include <gzrt.h>

/*
** This interface is created so that is easier to keep track of subwindows
** created during the running time of the program. It also ensures that no
** multiple instances of windows are created.
*/

/* Set up a new window interface */
MWI * gzrt_mwi_new ( void )
{
	/* Allocate memory */
	MWI * ret = calloc( sizeof(MWI), 1 );
	
	/* Allocate the sub-structures 
	for( int i = 0; i < GZRT_WMAIN_MAX; i++ )
		ret->entries[i] = calloc( sizeof(struct gzrt_mwi_entry), 1 );*/
	
	return ret;
}

/*
** Bind a window to list 
** A return value of zero indicates success
*/
int gzrt_mwi_bind ( MWI *mwi, MAINWIN *parent, GtkWidget *window )
{
	int i = 0, a;
	
	/* Find next unused entry */
	while( mwi->entries[i].parent && i < GZRT_WMAIN_MAX )
		i++;
	
	/* Did we overstep the bounds? */
	if( i == GZRT_WMAIN_MAX )
	{
		GZRTD_MESG( "No free entries to create MWI." );
		return GZRT_MWI_E_NO_FREE;
	}
	
	/* Check that this window isn't already created with this parent */
	for( a = 0; a < GZRT_WMAIN_MAX && mwi->entries[a].parent; a++ )
		if( mwi->entries[a].parent == parent )
		{
			/* Yep, already exists */
			GZRTD_MESG( "Parent #%u already has a window \"%s\" (0x%08X).",
			parent->id + 1, gtk_window_get_title(GTK_WINDOW(window)), window );
			return GZRT_MWI_E_EXISTS;
		}
		
	/* Ok, it doesn't - fill structure */
	mwi->entries[i].parent = parent;
	mwi->entries[i].window = window;
	
	/* Success */
	GZRTD_MESG( "Bound window \"%s\" (0x%08X) to parent #%u.", 
	gtk_window_get_title(GTK_WINDOW(window)), window, parent->id + 1 );
	return GZRT_MWI_E_NONE;
}

/*
** Remove a window from a list
** The GTK window is not needed because we can just go by the parent's address
** which is stored.
*/
int gzrt_mwi_remove ( MWI *mwi, MAINWIN *parent )
{
	int i;
	
	/* Identify parent */
	for( i = 0; i < GZRT_WMAIN_MAX && mwi->entries[i].parent != parent; i++ );
	
	/* Did we reach the max? */
	if( i == GZRT_WMAIN_MAX )
	{
		GZRTD_MESG( "Attempt at unbinding window linked to #%u failed.", parent->id + 1 );
		return GZRT_MWI_E_NOT_FOUND;
	}
	
	/* Success */
	GZRTD_MESG( "Removed window \"%s\" (0x%08X) from parent #%u.", 
	gtk_window_get_title(GTK_WINDOW(mwi->entries[i].window)), mwi->entries[i].window, parent->id + 1 );
	
	/* Clear the structure */
	memset( &mwi->entries[i], 0, sizeof( struct gzrt_mwi_entry ) );
	
	/* Return */
	return GZRT_MWI_E_NONE;
}
