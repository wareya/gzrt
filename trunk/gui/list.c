/***********************************
* File listing tree view generator *
***********************************/
#include <gzrt.h>

#define	FONT	"Courier 10"

static void gen_id ( char * dest, int id, MAINWIN * c )
{
	sprintf( dest, "%04u", id );
}

static void gen_vstart ( char * dest, int id, MAINWIN * c )
{
	sprintf( dest, "0x%08X", ZFileVirtStart(c->z, id) );
}

static void gen_vend ( char * dest, int id, MAINWIN * c )
{
	sprintf( dest, "0x%08X", ZFileVirtEnd(c->z, id) );
}

static void gen_name ( char * dest, int id, MAINWIN * c )
{
		if( !c->t )
		{
			strcpy( dest, "-" );
			return;
		}
		
		const char * s = z64nt_filename( c->t, id );
		if( !s )
			strcpy( dest, "-" );
		else
			sprintf( dest, "%s", s );
		return;
}
typedef void (*COLGEN)(char *, int, void *);
struct ColumnSpec
{
	char * title;
	char * font;
	COLGEN func;
}
Cols[] = 
{
	{ "ID",		  FONT, (COLGEN)gen_id     },
	{ "Filename", NULL, (COLGEN)gen_name   },
	{ "Start",	  FONT, (COLGEN)gen_vstart },
	{ "End",	  FONT, (COLGEN)gen_vend   }
};

/* Generate the tree view */
GtkWidget * gzrt_wmain_tree_generate ( MAINWIN * c )
{
	GtkWidget    * tv = gtk_tree_view_new();
	GtkListStore * ls = gtk_list_store_new( sizeof(Cols) / sizeof(struct ColumnSpec), G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, -1 );
	
	/* Create columns */
	for( int i = 0; i < sizeof(Cols) / sizeof(struct ColumnSpec); i++ )
	{
		GtkCellRenderer * r = gtk_cell_renderer_text_new();
		GtkTreeViewColumn * col;
		
		/* Font? */
		if( Cols[i].font )
			g_object_set( G_OBJECT(r), "font", FONT, NULL );
		
		gtk_tree_view_insert_column_with_attributes( GTK_TREE_VIEW(tv), 
		-1, Cols[i].title, r, "text", i, NULL );
		col = gtk_tree_view_get_column( GTK_TREE_VIEW(tv), i );
		gtk_tree_view_column_set_resizable( col, TRUE );
		g_object_set( r, "xpad", 12, NULL );
	}
	
	/* Generate data */
	for( int i = 0; i < z64fs_entries( c->z ); i++ )
	{
		GtkTreeIter j;
		
		gtk_list_store_append( GTK_LIST_STORE(ls), &j );
		 
		for( int k = 0; k < sizeof(Cols) / sizeof(struct ColumnSpec); k++ )
		{
			char buffer[64];
			Cols[k].func( buffer, i, c );
			gtk_list_store_set( GTK_LIST_STORE(ls), &j, k, buffer, -1 );
		}
	}
	
	/* Set model */
	gtk_tree_view_set_model( GTK_TREE_VIEW(tv), GTK_TREE_MODEL(ls) );
	
	/* Return it */
	return tv;
}
