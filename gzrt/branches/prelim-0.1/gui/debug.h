/************************
* Debug Box GUI Element *
************************/
#ifndef __GZRT_WDEBUG_H
#define __GZRT_WDEBUG_H
 
/* Defines */
# define GZRT_WDEBUG_W      500
# define GZRT_WDEBUG_H      300
# define GZRT_WDEBUG_T      "Debug Console" 
# define GZRT_WDEBUG_ICO    GZRT_GUI_ICON_DIR "icodebug.png"
# define GZRT_WDEBUG_I      ((int)-34)
 
/* Data structure */
struct _gzrt_wdebug 
{
    GtkWidget * window;
    GtkWidget * scroll;
    GtkWidget * text;
    GtkWidget * buffer;
    
    /* Memory usage */
    GtkWidget * bar;;
    GError    * err;
    GThread   * thread;
};
 
/* Function declarations */
void gzrt_gui_debug_create ( void );
void gzrt_gui_debug_add ( char *file, int line, char *fmt, ... );
int gzrt_gui_debug_mem ( void );

/* Macros */
#ifdef GZRT_DEBUG
# define GZRTD_MESG(x, ...)             gzrt_gui_debug_add( __FILE__, __LINE__, x "\n", ##__VA_ARGS__ )
# define GZRTD_MESG_CONSOLE(x, ... )    { g_print( "[%s:%u] " x "\n", __FILE__, __LINE__,  ##__VA_ARGS__ ); fflush(stdout); }
# define GZRTD_MARKER                   g_print("[marker:%s:%u]\n", __FILE__, __LINE__ );
#else
# define GZRTD_MESG(x, ...)
# define GZRTD_MESG_CONSOLE(x, ... )    
# define GZRTD_MARKER
#endif

#endif /* __GZRT_WDEBUG_H */
