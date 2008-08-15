/********************
* CRC Fixing Window *
********************/
#include <gzrt.h>

/* Create an instance */
void gzrt_wcrc_create ( MAINWIN *parent )
{
	/* Storage for CRCs */
	unsigned int crc_old[2] = {
		U32( parent->c->data + 0x10 ),
		U32( parent->c->data + 0x14 )
	};
	
	unsigned int crc_new[2];
	
	GZRTD_MESG( "%08X, %08X", crc_old[0], crc_old[1] );
	
	/* New CRCs */
	gen_table();
	N64CalcCRC( crc_new, parent->c->data );
	
	/* Create a new window */
	WINDOW *w = gzrt_window_create( "Fix CRC", 0, 0, 0 );
	
	/* GtkWidgets */
	GtkWidget *vbox = gtk_vbox_new( FALSE, 4 );
	
	/* Loop through */
	for( int i = 0; i < 2; i++ )
	{
		char buffer[128], *tag_s, *tag_e = "</span>";
		GtkWidget *frame;
		GtkWidget *mhbox = gtk_hbox_new( FALSE, 8 );
		GtkWidget *mvbox = gtk_vbox_new( FALSE, 8 );
		GtkWidget *old = gtk_label_new( "" );
		GtkWidget *new = gtk_label_new( "" );
		GtkWidget *a;
		
		gtk_box_pack_start( GTK_BOX(mhbox), mvbox, TRUE, TRUE, 8 );
		
		sprintf( buffer, "CRC #%u", i + 1 );
		frame = gtk_frame_new( buffer );
		gtk_container_add( GTK_CONTAINER(frame), mhbox );
		
		if( crc_old[i] == crc_new[i] )
			tag_s = "<span font_desc=\"Courier\" color=\"green\">";
		else
			tag_s = "<span font_desc=\"Courier\" color=\"red\">";
		
		snprintf( buffer, sizeof(buffer), "Old CRC: %s0x%08X%s", tag_s, crc_old[i], tag_e );
		gtk_label_set_markup( GTK_LABEL(old), buffer );
		a = gtk_alignment_new( 0.0, 1.0, 0.0, 1.0 );
		gtk_container_add( GTK_CONTAINER(a), old );
		gtk_box_pack_start( GTK_BOX(mvbox), a, TRUE, TRUE, 4 );
		snprintf( buffer, sizeof(buffer), "New CRC: %s0x%08X%s", "<span font_desc=\"Courier\" color=\"green\">", crc_new[i], tag_e );
		gtk_label_set_markup( GTK_LABEL(new), buffer );
		a = gtk_alignment_new( 0.0, 1.0, 0.0, 1.0 );
		gtk_container_add( GTK_CONTAINER(a), new );
		gtk_box_pack_start( GTK_BOX(mvbox), a, TRUE, TRUE, 4 );
		gtk_box_pack_start( GTK_BOX(vbox), frame, TRUE, TRUE, 8 );
	}
	
	gzrt_window_attach( w, vbox );
	gzrt_window_show( w );
}
	
GtkWidget*
create_CRC_Checker ( MAINWIN *parent )
{
  /* GTK Elements */
  GtkWidget *CRC_Checker;
  GtkWidget *window_seperator;
  GtkWidget *Frame_1_alignment;
  GtkWidget *Frame_1;
  GtkWidget *Frame_1_label_align;
  GtkWidget *Frame_1_v_sep;
  GtkWidget *F1_CRC_Old;
  GtkWidget *F1_CRC_New;
  GtkWidget *Frame_1_label;
  GtkWidget *Frame_2_alignment;
  GtkWidget *Frame_2;
  GtkWidget *Frame_2_label_align;
  GtkWidget *Frame_2_v_sep;
  GtkWidget *F2_CRC_Old;
  GtkWidget *F2_CRC_New;
  GtkWidget *Frame_2_label;
  GtkWidget *Button_seperator;
  GtkWidget *Button_close;
  GtkWidget *Button_apply;
	
	gzrt_wmain_status_addmsg( parent, "Fixing CRCs" );
  
  /* Other elements */
  char buffer[128];
  unsigned int crc_old[2] = {
	  U32( parent->c->data + 0x10 ),
 	  U32( parent->c->data + 0x14 )
  };
  static unsigned int crc_new[2], color;
  static struct crcarg d;
  d.parent = parent;
  d.crc = (unsigned int*)&crc_new;
  
  /* Calculate new CRCs */
  gen_table();
  N64CalcCRC( crc_new, parent->c->data );
  
  /* Debug */
  GZRTD_MESG( "Old CRCs: %08X, %08X", crc_old[0], crc_old[1] );
  GZRTD_MESG( "New CRCs: %08X, %08X", crc_new[0], crc_new[1] );

  /* Set up GTK window */
  CRC_Checker = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_size_request (CRC_Checker, 180, 190);
  gtk_container_set_border_width (GTK_CONTAINER (CRC_Checker), 1);
  gtk_window_set_title (GTK_WINDOW (CRC_Checker), _("CRC Checker"));
  gtk_window_set_default_size (GTK_WINDOW (CRC_Checker), 180, 190);

  window_seperator = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (window_seperator);
  gtk_container_add (GTK_CONTAINER (CRC_Checker), window_seperator);

  Frame_1_alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (Frame_1_alignment);
  gtk_box_pack_start (GTK_BOX (window_seperator), Frame_1_alignment, TRUE, TRUE, 0);
  gtk_alignment_set_padding (GTK_ALIGNMENT (Frame_1_alignment), 12, 6, 12, 12);

  Frame_1 = gtk_frame_new (NULL);
  gtk_widget_show (Frame_1);
  gtk_container_add (GTK_CONTAINER (Frame_1_alignment), Frame_1);

  Frame_1_label_align = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (Frame_1_label_align);
  gtk_container_add (GTK_CONTAINER (Frame_1), Frame_1_label_align);
  gtk_alignment_set_padding (GTK_ALIGNMENT (Frame_1_label_align), 0, 0, 12, 12);

  Frame_1_v_sep = gtk_vbox_new (FALSE, 2);
  gtk_widget_show (Frame_1_v_sep);
  gtk_container_add (GTK_CONTAINER (Frame_1_label_align), Frame_1_v_sep);

  F1_CRC_Old = gtk_label_new (_("Old: 0x80740000"));
  gtk_widget_show (F1_CRC_Old);
  gtk_box_pack_start (GTK_BOX (Frame_1_v_sep), F1_CRC_Old, TRUE, TRUE, 0);
  gtk_misc_set_alignment (GTK_MISC (F1_CRC_Old), 0, 0.5);

  F1_CRC_New = gtk_label_new (_("New: 0x80740000"));
  gtk_widget_show (F1_CRC_New);
  gtk_box_pack_start (GTK_BOX (Frame_1_v_sep), F1_CRC_New, TRUE, TRUE, 0);
  gtk_misc_set_alignment (GTK_MISC (F1_CRC_New), 0, 0.5);
  
  /* Set markup */
  if( crc_old[0] == crc_new[0] )
	  color = 0x00DD00;
  else
	  color = 0xFF0000;
  
  /* Do it faggot */
  sprintf( buffer, "Old: <span font_desc=\"Courier\" color=\"#%06X\">0x%08X</span>", color, crc_old[0] );
  gtk_label_set_markup( GTK_LABEL(F1_CRC_Old), buffer );
  sprintf( buffer, "New: <span font_desc=\"Courier\" color=\"#00DD00\">0x%08X</span>", crc_new[0] );
  gtk_label_set_markup( GTK_LABEL(F1_CRC_New), buffer );

  Frame_1_label = gtk_label_new (_("<b>CRC 1</b>"));
  gtk_widget_show (Frame_1_label);
  gtk_frame_set_label_widget (GTK_FRAME (Frame_1), Frame_1_label);
  gtk_label_set_use_markup (GTK_LABEL (Frame_1_label), TRUE);

  Frame_2_alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (Frame_2_alignment);
  gtk_box_pack_start (GTK_BOX (window_seperator), Frame_2_alignment, TRUE, TRUE, 0);
  gtk_alignment_set_padding (GTK_ALIGNMENT (Frame_2_alignment), 6, 12, 12, 12);

  Frame_2 = gtk_frame_new (NULL);
  gtk_widget_show (Frame_2);
  gtk_container_add (GTK_CONTAINER (Frame_2_alignment), Frame_2);

  Frame_2_label_align = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (Frame_2_label_align);
  gtk_container_add (GTK_CONTAINER (Frame_2), Frame_2_label_align);
  gtk_alignment_set_padding (GTK_ALIGNMENT (Frame_2_label_align), 0, 0, 12, 12);

  Frame_2_v_sep = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (Frame_2_v_sep);
  gtk_container_add (GTK_CONTAINER (Frame_2_label_align), Frame_2_v_sep);

  F2_CRC_Old = gtk_label_new (_("Old: 0x80740000"));
  gtk_widget_show (F2_CRC_Old);
  gtk_box_pack_start (GTK_BOX (Frame_2_v_sep), F2_CRC_Old, TRUE, TRUE, 0);
  gtk_misc_set_alignment (GTK_MISC (F2_CRC_Old), 0, 0.5);

  F2_CRC_New = gtk_label_new (_("New: 0x80740000"));
  gtk_widget_show (F2_CRC_New);
  gtk_box_pack_start (GTK_BOX (Frame_2_v_sep), F2_CRC_New, TRUE, TRUE, 0);
  gtk_misc_set_alignment (GTK_MISC (F2_CRC_New), 0, 0.5);
  
  /* Set markup */
  if( crc_old[1] == crc_new[1] )
	  color = 0x00DD00;
  else
	  color = 0xFF0000;
  
  /* Do it */
  sprintf( buffer, "Old: <span font_desc=\"Courier\" color=\"#%06X\">0x%08X</span>", color, crc_old[1] );
  gtk_label_set_markup( GTK_LABEL(F2_CRC_Old), buffer );
  sprintf( buffer, "New: <span font_desc=\"Courier\" color=\"#00DD00\">0x%08X</span>", crc_new[1] );
  gtk_label_set_markup( GTK_LABEL(F2_CRC_New), buffer );

  Frame_2_label = gtk_label_new (_("<b>CRC 2</b>"));
  gtk_widget_show (Frame_2_label);
  gtk_frame_set_label_widget (GTK_FRAME (Frame_2), Frame_2_label);
  gtk_label_set_use_markup (GTK_LABEL (Frame_2_label), TRUE);

  Button_seperator = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (Button_seperator);
  gtk_box_pack_start (GTK_BOX (window_seperator), Button_seperator, TRUE, FALSE, 0);

  Button_close = gtk_button_new_with_mnemonic (_("Close"));
  gtk_widget_show (Button_close);
  gtk_box_pack_start (GTK_BOX (Button_seperator), Button_close, TRUE, FALSE, 0);

  Button_apply = gtk_button_new_with_mnemonic (_("Apply"));
  gtk_widget_show (Button_apply);
  gtk_box_pack_start (GTK_BOX (Button_seperator), Button_apply, TRUE, FALSE, 0);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (CRC_Checker, CRC_Checker, "CRC_Checker");
  GLADE_HOOKUP_OBJECT (CRC_Checker, window_seperator, "window_seperator");
  GLADE_HOOKUP_OBJECT (CRC_Checker, Frame_1_alignment, "Frame_1_alignment");
  GLADE_HOOKUP_OBJECT (CRC_Checker, Frame_1, "Frame_1");
  GLADE_HOOKUP_OBJECT (CRC_Checker, Frame_1_label_align, "Frame_1_label_align");
  GLADE_HOOKUP_OBJECT (CRC_Checker, Frame_1_v_sep, "Frame_1_v_sep");
  GLADE_HOOKUP_OBJECT (CRC_Checker, F1_CRC_Old, "F1_CRC_Old");
  GLADE_HOOKUP_OBJECT (CRC_Checker, F1_CRC_New, "F1_CRC_New");
  GLADE_HOOKUP_OBJECT (CRC_Checker, Frame_1_label, "Frame_1_label");
  GLADE_HOOKUP_OBJECT (CRC_Checker, Frame_2_alignment, "Frame_2_alignment");
  GLADE_HOOKUP_OBJECT (CRC_Checker, Frame_2, "Frame_2");
  GLADE_HOOKUP_OBJECT (CRC_Checker, Frame_2_label_align, "Frame_2_label_align");
  GLADE_HOOKUP_OBJECT (CRC_Checker, Frame_2_v_sep, "Frame_2_v_sep");
  GLADE_HOOKUP_OBJECT (CRC_Checker, F2_CRC_Old, "F2_CRC_Old");
  GLADE_HOOKUP_OBJECT (CRC_Checker, F2_CRC_New, "F2_CRC_New");
  GLADE_HOOKUP_OBJECT (CRC_Checker, Frame_2_label, "Frame_2_label");
  GLADE_HOOKUP_OBJECT (CRC_Checker, Button_seperator, "Button_seperator");
  GLADE_HOOKUP_OBJECT (CRC_Checker, Button_close, "Button_close");
  GLADE_HOOKUP_OBJECT (CRC_Checker, Button_apply, "Button_apply");
  
  /* On OK close window */
  g_signal_connect_swapped( Button_close, "clicked", G_CALLBACK(gtk_widget_destroy), (gpointer)CRC_Checker );
  g_signal_connect_swapped( Button_apply, "clicked", G_CALLBACK(gzrt_wcrc_write), (gpointer)&d );
  g_signal_connect_swapped( CRC_Checker, "destroy", G_CALLBACK(gzrt_wcrc_closed), (gpointer)parent );
  
  /* Disable button? */
  if( crc_old[0] == crc_new[0] && crc_old[1] == crc_new[1] )
	  gtk_widget_set_sensitive( Button_apply, FALSE );
  
  /* Show window */
  gtk_widget_show_all( CRC_Checker );

  return CRC_Checker;
}

/* Write CRCs to ROM */
void gzrt_wcrc_write ( struct crcarg *c, GtkWidget *w )
{
	fseek( c->parent->c->handle, 0x10, SEEK_SET );
	
	/* CRC 1 */
	fputc( c->crc[0] >> 24,		   c->parent->c->handle );
	fputc( c->crc[0] >> 16 & 0xFF, c->parent->c->handle );
	fputc( c->crc[0] >>  8 & 0xFF, c->parent->c->handle );
	fputc( c->crc[0] & 0xFF,       c->parent->c->handle );
    GZRTD_MESG( "Wrote new CRC1.", c->crc[0] );
	fputc( c->crc[1] >> 24,        c->parent->c->handle );
	fputc( c->crc[1] >> 16 & 0xFF, c->parent->c->handle );
	fputc( c->crc[1] >>  8 & 0xFF, c->parent->c->handle );
	fputc( c->crc[1] & 0xFF,       c->parent->c->handle );
    GZRTD_MESG( "Wrote new CRC2.", c->crc[1] );
	
	/* Also copy to ROM storage */
	WU32( c->parent->c->data + 0x10, c->crc[0] );
	WU32( c->parent->c->data + 0x14, c->crc[1] );
	memcpy( c->parent->c->data + 0x10, c->parent->c->data + 0x14, sizeof(int) * 2 );
	
	/* Show notice box */
	gzrt_werror_show( "Notice", "Done!", 0 );
	gtk_widget_destroy( w );
}

/* Destroy handler */
void gzrt_wcrc_closed ( MAINWIN *w )
{
	gzrt_wmain_status_rmmsg( w );
}
