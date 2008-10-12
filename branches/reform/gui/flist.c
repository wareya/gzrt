/************************
* Zelda 64 File Listing *
************************/
#include <gui/gui.h>

struct _gzrt_wflist
{
	GtkWidget		*window;
	GtkWidget		*view;
	GtkListStore	*store;
	GtkTreeIter		iter;
	GtkCellRenderer *renderer;
	GtkTreeModel	*model;
};

enum
{
	GZRT_COL_NAME,
	GZRT_COL_TYPE,
	GZRT_COL_COUNT
};

struct _gzrt_list_cols
{
	char *name;
	int   id;
	int   type;
};

struct _gzrt_wflist DDDDDD;
struct _gzrt_wflist *w = &DDDDDD;

void gzrt_wflist_init ( void )
{
	w->window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	w->view = gtk_tree_view_new();
	w->renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes( w->view,
		-1, "Filename", w->renderer, "text", GZRT_COL_NAME, NULL );
	
	w->renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes( w->view,
		-1, "Filetype", w->renderer, "text", GZRT_COL_TYPE, NULL );
	
	w->store = gtk_list_store_new( GZRT_COL_COUNT,
		G_TYPE_STRING, G_TYPE_STRING );
	
	/* Append & fill */
	gtk_list_store_append( w->store, &w->iter );
	gtk_list_store_set( w->store, &w->iter, GZRT_COL_NAME, "JEW", 
		GZRT_COL_TYPE, "ZMAP" );
	
	gtk_tree_view_set_model( w->view, w->store );
	g_object_unref( w->model );
	
	gtk_container_add( w->window, w->view );
	gtk_widget_show_all( w->window );	
}
	
