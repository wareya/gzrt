/***********************
* Byteswapper for GZRT *
***********************/
#include <gzrt.h>

/* Global, preserved variables */
static GtkWidget *Source_size;
static GtkWidget *Source_mode;
static GtkWidget *Destination_size;
static GtkWidget *Destination_mode;
  
GtkWidget * gzrt_wbyteswap_create ( MAINWIN * w )
{
  GtkWidget *Byteswap_window;
  GtkWidget *Main_window_separator;
  GtkWidget *Source_alignment;
  GtkWidget *Source_hbox;
  GtkWidget *To_label;
  GtkWidget *Destination_alignment;
  GtkWidget *Destination_hbox;
  GtkWidget *Button_alignment;
  GtkWidget *Button_start;
  GtkWidget *alignment4;
  GtkWidget *hbox3;
  GtkWidget *image1;
  GtkWidget *label2;

  Byteswap_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (Byteswap_window), _("Byteswapper"));
  gtk_window_set_position (GTK_WINDOW (Byteswap_window), GTK_WIN_POS_MOUSE);
  gtk_window_set_default_size (GTK_WINDOW (Byteswap_window), 400, 190);

  Main_window_separator = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (Main_window_separator);
  gtk_container_add (GTK_CONTAINER (Byteswap_window), Main_window_separator);
  gtk_container_set_border_width (GTK_CONTAINER (Main_window_separator), 12);

  Source_alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (Source_alignment);
  gtk_box_pack_start (GTK_BOX (Main_window_separator), Source_alignment, TRUE, FALSE, 0);

  Source_hbox = gtk_hbox_new (FALSE, 4);
  gtk_widget_show (Source_hbox);
  gtk_container_add (GTK_CONTAINER (Source_alignment), Source_hbox);

  Source_size = gtk_combo_box_entry_new_text ();
  gtk_widget_show (Source_size);
  gtk_box_pack_start (GTK_BOX (Source_hbox), Source_size, TRUE, TRUE, 0);
  gtk_combo_box_append_text (GTK_COMBO_BOX (Source_size), _("16-bit"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (Source_size), _("32-bit"));

  Source_mode = gtk_combo_box_entry_new_text ();
  gtk_widget_show (Source_mode);
  gtk_box_pack_start (GTK_BOX (Source_hbox), Source_mode, TRUE, TRUE, 0);
  gtk_combo_box_append_text (GTK_COMBO_BOX (Source_mode), _("Little endian (DCBA)"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (Source_mode), _("Big endian (ABCD)"));

  To_label = gtk_label_new (_("to"));
  gtk_widget_show (To_label);
  gtk_box_pack_start (GTK_BOX (Main_window_separator), To_label, TRUE, FALSE, 0);

  Destination_alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (Destination_alignment);
  gtk_box_pack_start (GTK_BOX (Main_window_separator), Destination_alignment, TRUE, FALSE, 0);

  Destination_hbox = gtk_hbox_new (FALSE, 4);
  gtk_widget_show (Destination_hbox);
  gtk_container_add (GTK_CONTAINER (Destination_alignment), Destination_hbox);

  Destination_size = gtk_combo_box_entry_new_text ();
  gtk_widget_show (Destination_size);
  gtk_box_pack_start (GTK_BOX (Destination_hbox), Destination_size, TRUE, TRUE, 0);
  gtk_combo_box_append_text (GTK_COMBO_BOX (Destination_size), _("16-bit"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (Destination_size), _("32-bit"));

  Destination_mode = gtk_combo_box_entry_new_text ();
  gtk_widget_show (Destination_mode);
  gtk_box_pack_start (GTK_BOX (Destination_hbox), Destination_mode, TRUE, TRUE, 0);
  gtk_combo_box_append_text (GTK_COMBO_BOX (Destination_mode), _("Little endian (DCBA)"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (Destination_mode), _("Big endian (ABCD)"));

  Button_alignment = gtk_alignment_new (0.5, 0.5, 0, 1);
  gtk_widget_show (Button_alignment);
  gtk_box_pack_start (GTK_BOX (Main_window_separator), Button_alignment, FALSE, FALSE, 0);

  Button_start = gtk_button_new ();
  gtk_widget_show (Button_start);
  gtk_container_add (GTK_CONTAINER (Button_alignment), Button_start);

  alignment4 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment4);
  gtk_container_add (GTK_CONTAINER (Button_start), alignment4);

  hbox3 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox3);
  gtk_container_add (GTK_CONTAINER (alignment4), hbox3);

  image1 = gtk_image_new_from_stock ("gtk-ok", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image1);
  gtk_box_pack_start (GTK_BOX (hbox3), image1, FALSE, FALSE, 0);

  label2 = gtk_label_new_with_mnemonic (_("Begin swap"));
  gtk_widget_show (label2);
  gtk_box_pack_start (GTK_BOX (hbox3), label2, FALSE, FALSE, 0);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (Byteswap_window, Byteswap_window, "Byteswap_window");
  GLADE_HOOKUP_OBJECT (Byteswap_window, Main_window_separator, "Main_window_separator");
  GLADE_HOOKUP_OBJECT (Byteswap_window, Source_alignment, "Source_alignment");
  GLADE_HOOKUP_OBJECT (Byteswap_window, Source_hbox, "Source_hbox");
  GLADE_HOOKUP_OBJECT (Byteswap_window, Source_size, "Source_size");
  GLADE_HOOKUP_OBJECT (Byteswap_window, Source_mode, "Source_mode");
  GLADE_HOOKUP_OBJECT (Byteswap_window, To_label, "To_label");
  GLADE_HOOKUP_OBJECT (Byteswap_window, Destination_alignment, "Destination_alignment");
  GLADE_HOOKUP_OBJECT (Byteswap_window, Destination_hbox, "Destination_hbox");
  GLADE_HOOKUP_OBJECT (Byteswap_window, Destination_size, "Destination_size");
  GLADE_HOOKUP_OBJECT (Byteswap_window, Destination_mode, "Destination_mode");
  GLADE_HOOKUP_OBJECT (Byteswap_window, Button_alignment, "Button_alignment");
  GLADE_HOOKUP_OBJECT (Byteswap_window, Button_start, "Button_start");
  GLADE_HOOKUP_OBJECT (Byteswap_window, alignment4, "alignment4");
  GLADE_HOOKUP_OBJECT (Byteswap_window, hbox3, "hbox3");
  GLADE_HOOKUP_OBJECT (Byteswap_window, image1, "image1");
  GLADE_HOOKUP_OBJECT (Byteswap_window, label2, "label2");
  
  /* Swap that bitch */
  g_signal_connect_swapped( G_OBJECT(Button_start), "clicked", G_CALLBACK(gzrt_wbyteswap_swap), (gpointer)w );
  
  /* Show window */
  gtk_widget_show_all( Byteswap_window );

  return Byteswap_window;
}

/* Swap handler */
void gzrt_wbyteswap_swap ( MAINWIN * w )
{
	char buffer[256];
	clock_t t = n64rom_swap( w->c, 
		(gtk_combo_box_get_active(GTK_COMBO_BOX(Source_size)) + 1) * 16,
		 gtk_combo_box_get_active(GTK_COMBO_BOX(Source_mode)),
		(gtk_combo_box_get_active(GTK_COMBO_BOX(Destination_size)) + 1) * 16,
		 gtk_combo_box_get_active(GTK_COMBO_BOX(Destination_mode)) ), e;
	if( t )
	{
		/* Write the ROM to disk */
		fseek( w->c->handle, 0, SEEK_SET );
		fwrite( w->c->data, w->c->filesize, 1, w->c->handle );
		
		snprintf( buffer, sizeof(buffer), "Swapping complete; speed: %.4fMB/s.\nYou might want to reload the ROM.",
			((double)w->c->filesize / 1024.0 / 1024.0) / ((double)(t) / (double)CLOCKS_PER_SEC));
		gzrt_werror_show( "Notice", buffer, 0 );
	}
}

