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

typedef void (*CBACK)( MAINWIN * c );
struct MenuSpec
{
	char * name;
	char * image;
	CBACK  handler;
}
Menu[] =
{
	{ "_Open",           	   GTK_STOCK_OPEN,       NULL                         },
	{ NULL,              	   NULL,                 NULL                         }, /* H-sep */
	{ "_Default plugin", 	   GTK_STOCK_DISCONNECT, gzrt_wmain_plugin_action     },
	{ "_Plugins...",     	   GTK_STOCK_ZOOM_FIT,   NULL                         },
	{ NULL,              	   NULL,                 NULL                         }, /* H-sep */
	{ "_Extract",        	   GTK_STOCK_SAVE,       gzrt_wmain_extract           },
	{ "_Replace",        	   GTK_STOCK_JUMP_TO,    gzrt_wreplace_create         },
	{ NULL,				 	   NULL,				 NULL						  },
	{ "_Copy real address",    GTK_STOCK_COPY,	     gzrt_wmain_roffset_clipboard },
	{ "_Copy virtual address", GTK_STOCK_COPY,	     gzrt_wmain_voffset_clipboard }
};

static gboolean
click_handler ( GtkWidget * widget, GdkEventButton * event, gpointer data )
{
    if( event->type == GDK_BUTTON_PRESS )
		if( event->button == 3 )
		{
			gtk_menu_popup(GTK_MENU(data), NULL, NULL, NULL, NULL, event->button, event->time);
			return FALSE;
		}
	return FALSE;
}

/* Generate a menu */
static GtkWidget * 
menu_generate ( MAINWIN * c )
{
	GtkWidget * menu = gtk_menu_new();
	GtkWidget * item;
	gint		i;
	
	for( i = 0; i < sizeof(Menu) / sizeof(struct MenuSpec); i++ )
	{
		if( !Menu[i].name && !Menu[i].handler )
			item = gtk_separator_menu_item_new();
		else
		if( Menu[i].image )
			item = gzrt_gtk_image_menu_item_stock( Menu[i].image, GTK_ICON_SIZE_MENU, Menu[i].name );
		else
			item = gtk_image_menu_item_new_with_mnemonic( Menu[i].name );
		
		if( Menu[i].handler )
			g_signal_connect_swapped( G_OBJECT(item), "activate", G_CALLBACK(Menu[i].handler), c );
		
		gtk_menu_shell_append( GTK_MENU_SHELL(menu), item );
	}
	
	gtk_widget_show_all( menu );
	return menu;
}
	

/* Generate the tree view */
GtkWidget * 
gzrt_wmain_tree_generate ( MAINWIN * c )
{
    GtkWidget    * menu = menu_generate( c );
	GtkWidget    * tv = gtk_tree_view_new();
	GtkListStore * ls = gtk_list_store_new( sizeof(Cols) / sizeof(struct ColumnSpec), G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, -1 );
	
	/* Right click handler */
	g_signal_connect(tv, "button-press-event", G_CALLBACK(click_handler), (gpointer)menu);
	
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
		
		if( i == 1 )
		gtk_tree_view_column_set_expand( GTK_TREE_VIEW_COLUMN(col), TRUE );
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
