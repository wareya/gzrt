#ifndef __GZRT_GUI_H
#define __GZRT_GUI_H

/* Globals */
#define SHOW(x)				gtk_widget_show(    GTK_WIDGET(x) )
#define PRESENT(x)			gtk_window_present( GTK_WINDOW(x) )

/* Resource dir */
#define GZRT_GUI_ICON_DIR	"resources" GZRT_SLASH

/* Defines */
#define GZRT_GUI_ICON_1		GZRT_GUI_ICON_DIR "ico16x16.png"
#define GZRT_GUI_ICON_2		GZRT_GUI_ICON_DIR "ico32x32.png"
#define GZRT_GUI_ICON_3		GZRT_GUI_ICON_DIR "ico64x64.png"
#define GZRT_GUI_ICON_4		GZRT_GUI_ICON_DIR "ico128x128.png"

/* Macros */
#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)
  
#define _(x)	x


/* Headers */
#include <gtk/gtk.h>

/* Structure */
typedef struct _gzrt_padding
{
	GtkWidget *hbox;
	GtkWidget *vbox;
	GtkWidget *main;
} 
PADDING;

/* Window structure */
typedef struct _gzrt_window
{
	GtkWidget *window;
	GtkWidget *hbox;
	GtkWidget *vbox;
	GtkWidget *main;
} 
WINDOW;


/* Functions */
void      gzrt_gui_init        ( int argc, char **argv );
void      gzrt_gui_quit        ( void );
PADDING * gzrt_wpadding_create ( GtkWidget *window );
void 	  gzrt_wpadding_close  ( PADDING * h );
WINDOW *  gzrt_window_create   ( char *title, int w, int h, int flags );
void      gzrt_window_attach   ( WINDOW *w, GtkWidget *o );
void      gzrt_window_show     ( WINDOW *w );
void      gzrt_window_hide     ( WINDOW *w );
void      gzrt_window_close    ( WINDOW *w );

/* Global variables */
extern int window_amount;

/* Local headers */
#include <gui/debug.h>
#include <gui/pbar.h>
#include <gui/main.h>
#include <gui/splash.h>
#include <gui/error.h>
#include <gui/filesel.h>
#include <gui/extract.h>
#include <gui/about.h>
#include <gui/crc.h>
#include <gui/decomp.h>
#include <gui/byteswap.h>
#include <gui/replace.h>

#endif
