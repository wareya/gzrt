/*****************************
* GZRT GTK Utility Functions *
*****************************/
#ifndef __GZRT_GUI_UTILITY_H
#define __GZRT_GUI_UTILITY_H

typedef enum
{
	VBOX, HBOX
}
BOXTYPE;
                              
extern GtkWidget * gzrt_gtk_icon_load ( char * name );
extern GtkWidget * gzrt_gtk_image_label ( char * stock, GtkIconSize size, char * label );
extern GtkWidget * gzrt_gtk_image_label_f ( char * stock, GtkIconSize size, char * fmt, ... );
extern GtkWidget * gzrt_gtk_image_button_stock ( char * stock, char * label );
extern GtkWidget * gzrt_gtk_image_button_stock_f ( char * stock, char * fmt, ... );
extern GtkWidget * gzrt_gtk_image_menu_item_stock ( char * stock, GtkIconSize size, char * label );
extern GtkWidget * gzrt_gtk_image_menu_item ( char * icon, char * label );
extern GtkWidget * gzrt_gtk_toolbar_button ( char * image, char * label );
extern void gzrt_gtk_font_set ( GtkWidget * w, char * font );

#endif /* __GZRT_GUI_UTILITY_H */
