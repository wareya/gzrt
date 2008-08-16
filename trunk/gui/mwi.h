/*********************************
* GZRT Multiple Window Interface *
*********************************/
#ifndef __GZRT_MWI_H
#define __GZRT_MWI_H

/* Status messages */
enum
{
	/* All around */
	GZRT_MWI_E_NONE,
	
	/* For binding */
	GZRT_MWI_E_NO_FREE,
	GZRT_MWI_E_EXISTS,
	
	/* For removing */
	GZRT_MWI_E_NOT_FOUND,
};

/* The structure for a single entry */
struct gzrt_mwi_entry
{
	/* The parent window */
	MAINWIN *parent;
	
	/* The window itself (from which we can lookup information) */
	GtkWidget *window;
};

/* The structure containing all the windows creatable */
typedef struct gzrt_mwi
{
	/* The entries */
	struct gzrt_mwi_entry entries[GZRT_WMAIN_MAX];
} MWI;

/* Functions */
MWI * gzrt_mwi_new    ( void );
int   gzrt_mwi_bind   ( MWI *mwi, MAINWIN *parent, GtkWidget *window );
int   gzrt_mwi_remove ( MWI *mwi, MAINWIN *parent );

#endif /* __GZRT_MWI_H */
