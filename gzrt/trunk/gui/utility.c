/*****************************
* GZRT GTK Utility Functions *
*****************************/
#include <gzrt.h>
#include <gui/utility.h>
#include <gtk/gtk.h>
#include <stdarg.h>

/* Load an icon from file */
GtkWidget * 
gzrt_gtk_icon_load ( char * name )
{
	GtkWidget * image;
	char buffer[128];
	
	snprintf( buffer, sizeof(buffer), "%s%s", GZRT_GUI_ICON_DIR, name );
	image = gtk_image_new_from_file( buffer );
	
	return image;
}

/* Return a horizontal box with a stock image and a label */
GtkWidget *
gzrt_gtk_image_label ( char * stock, GtkIconSize size, char * label )
{
	GtkWidget * align;
	GtkWidget * l;
	GtkWidget * hbox;
	GtkWidget * i;
	
	align = gtk_alignment_new( 0.5f, 0.5f, 0.0f, 0.0f );
	hbox = gtk_hbox_new( FALSE, 4 );
	
	gtk_container_add( GTK_CONTAINER(align), hbox );
	
	i = gtk_image_new_from_stock( stock, size );
	gtk_box_pack_start( GTK_BOX(hbox), i, FALSE, FALSE, 0 );
	
	l = gtk_label_new_with_mnemonic( label );
	gtk_box_pack_start( GTK_BOX(hbox), l, FALSE, FALSE, 0 );
	
	gtk_widget_show_all( align );
	return align;
}

/* Return a horizontal box with a stock image and a label (with formatting) */
GtkWidget *
gzrt_gtk_image_label_f ( char * stock, GtkIconSize size, char * fmt, ... )
{
	GtkWidget * align;
	GtkWidget * l;
	GtkWidget * hbox;
	GtkWidget * i;
	va_list     ap;
	gchar     * str;
	
	/* Prepare the string */
	va_start( ap, fmt );
	str = g_strdup_vprintf( fmt, ap );
	va_end( ap );
	
	/* Create the widget */
	align = gtk_alignment_new( 0.5f, 0.5f, 0.0f, 0.0f );
	hbox = gtk_hbox_new( FALSE, 4 );
	gtk_container_add( GTK_CONTAINER(align), hbox );
	i = gtk_image_new_from_stock( stock, GTK_ICON_SIZE_BUTTON );
	gtk_box_pack_start( GTK_BOX(hbox), i, FALSE, FALSE, 0 );
	l = gtk_label_new_with_mnemonic( str );
	gtk_box_pack_start( GTK_BOX(hbox), l, FALSE, FALSE, 0 );
	
	/* Free string */
	g_free( str );
	
	/* Show widget */
	gtk_widget_show_all( align );
	return align;
}

/* Create a button with an image and a label */
GtkWidget * 
gzrt_gtk_image_button_stock ( char * stock, char * label )
{
	GtkWidget * align;
	GtkWidget * l;
	GtkWidget * hbox;
	GtkWidget * button;
	GtkWidget * i;
	
	button = gtk_button_new();
	align = gtk_alignment_new( 0.5f, 0.5f, 0.0f, 0.0f );
	hbox = gtk_hbox_new( FALSE, 2 );
	
	gtk_container_add( GTK_CONTAINER(button), align);
	gtk_container_add( GTK_CONTAINER(align), hbox );
	
	i = gtk_image_new_from_stock( stock, GTK_ICON_SIZE_BUTTON );
	gtk_box_pack_start( GTK_BOX(hbox), i, FALSE, FALSE, 0 );
	
	l = gtk_label_new_with_mnemonic( label );
	gtk_box_pack_start( GTK_BOX(hbox), l, FALSE, FALSE, 0 );
	
	return button;
}

/* Create a button with an image and a label (with formatting) */
GtkWidget * 
gzrt_gtk_image_button_stock_f ( char * stock, char * fmt, ... )
{
	GtkWidget * align;
	GtkWidget * button;
	GtkWidget * hbox;
	GtkWidget * i;
	GtkWidget * l;
	va_list     ap;
	gchar     * str;
	
	/* Prepare the string */
	va_start( ap, fmt );
	str = g_strdup_vprintf( fmt, ap );
	va_end( ap );
	
	/* Create the widget */
	button = gtk_button_new();
	align = gtk_alignment_new( 0.5f, 0.5f, 0.0f, 0.0f );
	hbox = gtk_hbox_new( FALSE, 4 );
	gtk_container_add( GTK_CONTAINER(button), align );
	gtk_container_add( GTK_CONTAINER(align), hbox );
	i = gtk_image_new_from_stock( stock, GTK_ICON_SIZE_BUTTON );
	gtk_box_pack_start( GTK_BOX(hbox), i, FALSE, FALSE, 0 );
	l = gtk_label_new_with_mnemonic( str );
	gtk_box_pack_start( GTK_BOX(hbox), l, FALSE, FALSE, 0 );
	
	/* Free string */
	g_free( str );
	
	/* Show widget */
	gtk_widget_show_all( button );
	return button;
}

/* Create a menu item with an image */
GtkWidget * 
gzrt_gtk_image_menu_item_stock ( char * stock, GtkIconSize size, char * label )
{
	GtkWidget * image;
	GtkWidget * menu;
	
	menu = gtk_image_menu_item_new_with_mnemonic( label );
	image = gtk_image_new_from_stock( stock, size );
	gtk_image_menu_item_set_image( GTK_IMAGE_MENU_ITEM(menu), image );
	
	return menu;
}

/* Create a menu item with an icon */
GtkWidget * 
gzrt_gtk_image_menu_item ( char * icon, char * label )
{
	GtkWidget * image;
	GtkWidget * menu;
	
	menu = gtk_image_menu_item_new_with_mnemonic( label );
	image = gzrt_gtk_icon_load( icon );
	gtk_image_menu_item_set_image( GTK_IMAGE_MENU_ITEM(menu), image );
	
	return menu;
}

/* Create a button with an arbitrary image */
GtkWidget *
gzrt_gtk_toolbar_button ( char * image, char * label )
{
	GtkWidget * button;
	
	button = (GtkWidget*)gtk_tool_button_new( gzrt_gtk_icon_load( image ), label );
	
	return button; 
}

/* Set the font of a widget */
void gzrt_gtk_font_set ( GtkWidget * w, char * font )
{
	PangoFontDescription * font_desc =
		pango_font_description_from_string( font );
	gtk_widget_modify_font( w, font_desc );
}
