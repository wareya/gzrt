/************************
* ROM Extraction Window *
************************/
#include <gzrt.h>

/* Structure */
static struct _gzrt_wextract gzrt_wextract;

/* Pointer shorthand */
static struct _gzrt_wextract *w = &gzrt_wextract;

/* Last window */
static MAINWIN *last = NULL;

/* Initialize the window */
void gzrt_wextract_init ( void )
{
	/* Create a new window */
	w->window = gtk_window_new(  GTK_WINDOW_TOPLEVEL                                       );
	/* gtk_widget_set_size_request( GTK_WINDOW(w->window), GZRT_WEXTRACT_W,   GZRT_WEXTRACT_H ); */
	gtk_window_set_title(        GTK_WINDOW(w->window), GZRT_WEXTRACT_T                    );
	gtk_window_set_position(     GTK_WINDOW(w->window), GTK_WIN_POS_CENTER                 );
	
	/* Create border vbox */
	w->wvbox = gtk_vbox_new( FALSE, 12 );
	
	/* Create border hbox */
	w->whbox = gtk_hbox_new( FALSE, 12 );
	
	/* Pack 'em */
	gtk_box_pack_start( GTK_BOX(w->wvbox),       w->whbox, TRUE, TRUE, 12 );
	gtk_container_add( GTK_CONTAINER(w->window), w->wvbox );
	
	/* Create vbox for elements */
	w->vbox = gtk_vbox_new( FALSE, 4 );
	
	/* Create a frame for options */
	GtkWidget *h = gtk_hbox_new( FALSE, 12 ), *v = gtk_vbox_new( FALSE, 12 );
	w->frame = gtk_frame_new( "Options" );
	gtk_container_add( GTK_CONTAINER(w->frame), h );
	
	/* Check boxes */
	w->b_strip_prefixes = gtk_check_button_new_with_label( GZRT_WEXTRACT_C_1 );
	w->b_scene_table = gtk_check_button_new_with_label( GZRT_WEXTRACT_C_2 );
	w->b_actor_table = gtk_check_button_new_with_label( GZRT_WEXTRACT_C_3 );
	
	/* Pack */
	GtkWidget *vv = gtk_vbox_new( FALSE, 4 );
	gtk_box_pack_start( GTK_BOX(w->whbox), w->vbox,  TRUE, TRUE,  10 );
	gtk_box_pack_start( GTK_BOX(w->vbox),  w->frame, TRUE, TRUE, 0  );
	gtk_box_pack_start( GTK_BOX(h),        v,        TRUE, TRUE,  12  );
	gtk_box_pack_start( GTK_BOX(v),        vv,        TRUE, TRUE,  12  );
	gtk_box_pack_start( GTK_BOX(vv), w->b_strip_prefixes, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(vv), w->b_scene_table, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(vv), w->b_actor_table, TRUE, TRUE, 0 );
	
	#ifdef GZRT_DEBUG
	w->dontwrite = gtk_check_button_new_with_label( GZRT_WEXTRACT_CD1 );
	gtk_box_pack_start( GTK_BOX(vv), w->dontwrite, TRUE, TRUE, 0 );
	#endif
	
	GtkWidget *l = gtk_label_new( "Output directory:" );
	GtkWidget *a = gtk_alignment_new( 0.0, 1.0, 0.0, 1.0 );
	gtk_container_add( GTK_CONTAINER(a), l );
	gtk_box_pack_start( GTK_BOX(vv), a, TRUE, TRUE, 0 );
	w->entry = gtk_entry_new();
	w->browse = gtk_button_new_with_label( "Browse..." );
	GtkWidget *hh = gtk_hbox_new( FALSE, 4 );
	gtk_box_pack_start( GTK_BOX(hh), w->entry, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(hh), w->browse, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(vv), hh, TRUE, TRUE, 0 );
	
	#ifdef GZRT_DEBUG
	a = gtk_alignment_new( 0.0, 1.0, 0.0, 1.0 );
	l = gtk_label_new( "Filename format:" );
	gtk_container_add( GTK_CONTAINER(a), l );
	w->fmt = gtk_entry_new();
	gtk_entry_set_text( GTK_ENTRY(w->fmt), GZRT_WEXTRACT_FMT_NT);
	gtk_box_pack_start( GTK_BOX(vv), a, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(vv), w->fmt, TRUE, TRUE, 0 );
	#endif
	
	w->button = gtk_button_new_with_label( GZRT_WEXTRACT_OK );
	a = gtk_alignment_new( 0.5, 1.0, 0.25, 1.0 );
	gtk_container_add( GTK_CONTAINER(a), w->button ); 
	gtk_box_pack_start( GTK_BOX(w->vbox), a, TRUE, TRUE, 0  );
	
	/* Show everything 
	gtk_widget_show_all( w->window ); */
	
	/* Extract */
	g_signal_connect_swapped( G_OBJECT(w->button), "clicked", gzrt_wextract_begin, NULL );
	
	/* On destroy, regenerate */
	g_signal_connect_swapped( G_OBJECT(w->window), "destroy", gzrt_wextract_regen, NULL );
	
	/* Debug */
	GZRTD_MESG( "Extractor window created." );
}

/* Show extractor window */
void gzrt_wextract_show ( MAINWIN *h )
{
	last = h;
	
	/* Set main window status */
	gzrt_wmain_status_addmsg( h, "Extracting ROM contents" );
	GZRTD_MESG( "Extractor window shown; fed WINMAIN * 0x%08X (#%u).", h, h->id );
	gtk_widget_show_all( w->window );
}

/* Regenerate window */
void gzrt_wextract_regen ( void )
{
	gzrt_wmain_status_rmmsg( last );
	GZRTD_MESG( "Extractor window destroyed - recreating." );
	gzrt_wextract_init();
}

/* Extraction procedure */
void gzrt_wextract_begin ( void )
{
}

/* Name format string parser */
int gzrt_wextract_soffsetf ( char *dest, struct _gzrt_wextract_stats * stats, char *fmt )
{
}
