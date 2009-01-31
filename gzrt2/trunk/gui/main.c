/************************
* GZRT GUI: Main Window *
************************/
#include <stdio.h>
#include <gtk/gtk.h>
#include "gzrt.h"
#include "gui.h"

/* ----------------------------------------------
   Variables
   ---------------------------------------------- */

/* List of windows open */
static GList * windows;		/* Type is `GzrtWindow` */


/* ----------------------------------------------
   Menus
   ---------------------------------------------- */

struct Menu
{
	char * name;
	char * image;
	int flags;
	void * handler;
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
	{ "_Close", GTK_STOCK_CLOSE, 0,   NULL },
	{ "_Quit",  GTK_STOCK_CLOSE, 0,   NULL },
	{ NULL,     NULL,            SEP, NULL },
	{ "_Debug", GTK_STOCK_ABOUT, 0,   NULL },
	{ NULL }
};

/* Help menu */
static const struct Menu
menu_help[] = 
{
	{ "_About", GTK_STOCK_ABOUT, 0, NULL },
	{ NULL }
};

/* Menu bar */
static const struct MenuBar
menu[] =
{
	{ "File", menu_file },
	{ "Help", menu_help },
	{ NULL },
};

	
