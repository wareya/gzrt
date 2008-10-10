/**************************
* Display an error window *
**************************/
#ifndef __GZRT_WERROR_H
#define __GZRT_WERROR_H

struct _gzrt_werror 
{
	GtkWidget *win;
	GtkWidget *label;
	GtkWidget *okb;
	GtkWidget *vbx;
};

typedef enum _gzrt_errlevel
{
	GZRT_ERR_NORMAL,
	GZRT_ERR_CRITICAL
} ERRLEVEL;

/* Contants */
#define GZRT_ERROR_W	200
#define GZRT_ERROR_H	90
#define GZRT_ERROR_OKB	"Ok"
#define GZRT_ERROR_ICO	GZRT_GUI_ICON_DIR "err.png"

/* Functions */
void gzrt_werror_show    ( char *t, char *s, ERRLEVEL errlevel );
void gzrt_werror_destroy ( void );
void gzrt_notice         ( char * t, char * s );

#endif /* __GZRT_WERROR_H */
