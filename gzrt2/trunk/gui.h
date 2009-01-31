/********************************
* GZRT Graphical User Interface *
********************************/
#ifndef __GZRT_GUI_H__
#define __GZRT_GUI_H__

#include <gtk/gtk.h>

/* Replacements for old GTKs */
#if GTK_MINOR_VERSION < 10
# define gtk_window_set_deletable
#endif

/* Constants */
#define GZRT_GUI_RES_PATH		"./res"
#define GZRT_GUI_RES_LOGO		GZRT_GUI_RES_PATH "/logo.svg"
#define GZRT_GUI_WINDOW_TITLE	"GNU Zelda ROM Tool"

/* Functions -- gui.c */
extern void gzrt_gui_init ( void );

#endif /* __GZRT_GUI_H__ */

