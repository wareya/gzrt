/********************************
* GZRT Graphical User Interface *
********************************/
#ifndef __GZRT_GUI_H__
#define __GZRT_GUI_H__

#include <gtk/gtk.h>
#include "file.h"

/* Replacements for old GTKs */
#if GTK_MINOR_VERSION < 10
# define gtk_window_set_deletable
#endif

/* Constants */
#define GZRT_GUI_RES_PATH		"./res"
#define GZRT_GUI_RES_LOGO		GZRT_GUI_RES_PATH "/logo.png"
#define GZRT_GUI_WINDOW_TITLE	"GNU Zelda ROM Tool 2"

/* A GZRT main window */
typedef struct 
{
	/* Other elements */
	GzrtFile * file;
	
	/* GTK elements */
	GtkWidget * window;
	GtkTreeView  * tree_view;
	GtkTreeModel * tree_model;
}
GzrtWindow;

/* Functions -- gui.c */
extern void gzrt_gui_init ( void );
extern void gzrt_gui_show_startup_screen ( void );
extern void gzrt_gui_close_startup_screen ( void );

/* Functions -- gui/about.c */
extern void gzrt_gui_about_show ( void );

/* Functions -- gui/open.c */
extern void gzrt_gui_open_rom ( void );

#endif /* __GZRT_GUI_H__ */

