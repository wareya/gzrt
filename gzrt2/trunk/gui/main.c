/************************
* GZRT GUI: Main Window *
************************/
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "gzrt.h"
#include "msg.h"
#include "gui.h"

/* ----------------------------------------------
   Variables
   ---------------------------------------------- */

/* List of windows open */
static GList * windows;		/* Type is `GzrtWindow` */


/* ----------------------------------------------
   Local function declarations
   ---------------------------------------------- */
   
static gboolean gzrt_gui_main_close ( GzrtWindow * W );


/* ----------------------------------------------
   Menus
   ---------------------------------------------- */

struct Menu
{
	char * name;
	char * image;
	int flags;
	void * handler;
	void * arg;
};

struct MenuBar
{
	char * name;
	const struct Menu * submenu;
};

/* Flags */
#define SEP		(1 << 1)	/* Horizontal separator */

/* File menu */
static const struct Menu
menu_file[] = 
{
	{ "_Close", GTK_STOCK_CLOSE, 0,   gzrt_gui_main_close },
	{ "_Quit",  GTK_STOCK_QUIT,  0,   NULL                },
	{ NULL,     NULL,            SEP, NULL                },
	{ "_Debug", GTK_STOCK_ABOUT, 0,   NULL                },
	{ NULL }
};

/* Help menu */
static const struct Menu
menu_help[] = 
{
	{ "_About", GTK_STOCK_ABOUT, 0, gzrt_gui_about_show },
	{ NULL }
};

/* Menu bar */
static const struct MenuBar
menu_spec[] =
{
	{ "File", menu_file },
	{ "Help", menu_help },
	{ NULL },
};

/* Create the menu bar */
static GtkWidget *
gzrt_gui_main_create_menu_bar ( GzrtWindow * W )
{
	GtkWidget * menu_bar;
	GtkWidget * menu;
	GtkWidget * item;
	GtkWidget * image;
	int i, k;
	
	/* Create menu bar */
	menu_bar = gtk_menu_bar_new();
	
	/* Step through the specification... */
	for( i = 0; menu_spec[i].name; i++ )
	{
		/* Create menu launcher */
		item = gtk_menu_item_new_with_mnemonic( menu_spec[i].name );
		
		/* Add menu launcher to bar */
		gtk_container_add( GTK_CONTAINER(menu_bar), item );
		
		/* Create a new menu */
		menu = gtk_menu_new();
		
		/* Set the menu launcher's menu */
		gtk_menu_item_set_submenu( GTK_MENU_ITEM(item), menu );
		
		/* Create all the entries */
		for( k = 0; menu_spec[i].submenu[k].name || menu_spec[i].submenu[k].flags; k++ )
		{
			/* Shorthand */
			const struct Menu * submenu = &menu_spec[i].submenu[k];
			
			/* What kind of item is this? */
			if( submenu->flags & SEP )
			{
				/* Separator */
				item = gtk_separator_menu_item_new();
			}
			else
			{
				/* Get image */
				image = gtk_image_new_from_stock( submenu->image, GTK_ICON_SIZE_MENU );
				
				/* Create menu item */
				item = gtk_image_menu_item_new_with_mnemonic( submenu->name );
				
				/* Set image */
				gtk_image_menu_item_set_image( GTK_IMAGE_MENU_ITEM(item), image );
			}
			
			/* Handler? */
			if( submenu->handler )
				
				/* Connect */
				g_signal_connect_swapped( G_OBJECT(item), "activate", G_CALLBACK(submenu->handler), W );
			
			/* Add to menu */
			gtk_container_add( GTK_CONTAINER(menu), item );
		}
	}
	
	/* Show the menu */
	gtk_widget_show_all( menu_bar );
	
	/* Return it */
	return menu_bar;
}


/* ----------------------------------------------
   List generators
   ---------------------------------------------- */

/* Create the list for entire file table */
static GtkWidget *
gzrt_gui_main_make_list_file_table ( GzrtWindow * w )
{
}


/* ----------------------------------------------
   Other local functions
   ---------------------------------------------- */
   
/* Make a vbox with some text and stock image */
static GtkWidget *
gzrt_gui_make_image_combo ( char * stock, char * text )
{
	GtkWidget * hbox;
	GtkWidget * image;
	GtkWidget * label;
	
	hbox = gtk_hbox_new( FALSE, 4 );
	label = gtk_label_new( text );
	image = gtk_image_new_from_stock( stock, GTK_ICON_SIZE_LARGE_TOOLBAR );
	
	gtk_box_pack_start( GTK_BOX(hbox), image, FALSE, FALSE, 0 );
	gtk_box_pack_start( GTK_BOX(hbox), label, FALSE, FALSE, 0 );
	
	gtk_widget_show_all( hbox );
	
	return hbox;
}

/* Close window handler */
static gboolean
gzrt_gui_main_close ( GzrtWindow * W )
{
	/* Close window */
	gtk_widget_destroy( W->window );
	
	/* Close file */
	gzrt_file_close( W->file );
	
	/* Last window? */
	if( g_list_length(windows) == 1 )
	{
		/* Yep */
		g_list_free( windows );
		windows = NULL;
		
		/* Show startup window */
		gzrt_gui_show_startup_screen();
	}
	else
	{
		/* just remove reference */
		windows = g_list_remove( windows, W );
	}
	
	/* Free window memory */
	free( W );
}

/* Create a main window based on file */
static void
gzrt_gui_main_window_create_internal ( GzrtFile * file,  GzrtWindow * W )
{
	GtkWidget * menubar;
	GtkWidget * vbox;
	GtkWidget * align;
	static char buffer[512];
	
	/* Create the window */
	W->window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	
	/* Prepare window title */
	snprintf( buffer, sizeof(buffer), "%s %s - \"%s\"", GZRT_GUI_WINDOW_TITLE,
	GZRT_VERSION, file->name );
	
	/* Set window attributes */
	gtk_window_set_title( GTK_WINDOW(W->window), buffer );
	
	/* Create master vbox */
	vbox = gtk_vbox_new( FALSE, 0 );
	gtk_container_add( GTK_CONTAINER(W->window), vbox );
	
	/* Create menu bar */
	menubar = gzrt_gui_main_create_menu_bar( W );
	
	/* Append it */
	gtk_box_pack_start( GTK_BOX(vbox), menubar, FALSE, TRUE, 0 );
	
	/* Create alignment */
	align = gtk_alignment_new( 0.5, 0.5, 1.0, 1.0 );
	gtk_alignment_set_padding( GTK_ALIGNMENT(align), 12, 12, 12, 12 );
	
	/* Append it */
	gtk_box_pack_start( GTK_BOX(vbox), align, TRUE, TRUE, 0 );
	
	/* Create a notebook */
	W->notebook = gtk_notebook_new();
	gtk_container_add( GTK_CONTAINER(align), W->notebook );
	
	/* Create pages */
	gtk_notebook_append_page( GTK_NOTEBOOK(W->notebook), 
		gtk_label_new("SOON"),
		gzrt_gui_make_image_combo( GTK_STOCK_DND_MULTIPLE, "File list" )
	);
	gtk_notebook_append_page( GTK_NOTEBOOK(W->notebook), 
		gtk_label_new("SOON"),
		gzrt_gui_make_image_combo( GTK_STOCK_EXECUTE, "Actor list" )
	);
	gtk_notebook_append_page( GTK_NOTEBOOK(W->notebook), 
		gtk_label_new("SOON"),
		gzrt_gui_make_image_combo( GTK_STOCK_SELECT_COLOR, "Scene list" )
	);
	gtk_notebook_append_page( GTK_NOTEBOOK(W->notebook), 
		gtk_label_new("SOON"),
		gzrt_gui_make_image_combo( GTK_STOCK_SELECT_FONT, "Object list" )
	);
	gtk_notebook_append_page( GTK_NOTEBOOK(W->notebook), 
		gtk_label_new("SOON"),
		gzrt_gui_make_image_combo( GTK_STOCK_INFO, "ROM info" )
	);
	
	/* Show window */
	gtk_widget_show_all( W->window );
}
	


/* ----------------------------------------------
   Global functions
   ---------------------------------------------- */

/* Create a main window */
void gzrt_gui_main_window_create ( void )
{
	GzrtFile * file;
	GzrtWindow * window;
	
	/* Get a file */
	if( !(file = gzrt_gui_open_rom( GZRT_FILE_Z64 )) )
		return;
	
	/* Allocate memory for new window */
	window = calloc( sizeof(GzrtWindow), 1 );
	window->file = file;
	
	/* We have a file, close the startup window */
	gzrt_gui_close_startup_screen();
	
	/* Create the window */
	gzrt_gui_main_window_create_internal( file, window );
	
	/* Add to list */
	windows = g_list_append( windows, window );
	
	DEBUG( "Now managing window 0x%08X.", window );
}
