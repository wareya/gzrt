/*****************************
* GZRT GTK Utility Functions *
*****************************/
#ifndef __GZRT_GUI_UTILITY_H
#define __GZRT_GUI_UTILITY_H
                              
extern GtkWidget * gzrt_gtk_image_label ( char * stock, char * label );
extern GtkWidget * gzrt_gtk_image_label_f ( char * stock, char * fmt, ... );
extern GtkWidget * gzrt_gtk_image_button ( char * stock, char * label );
extern GtkWidget * gzrt_gtk_image_button_f ( char * stock, char * fmt, ... );
extern GtkWidget * gzrt_gtk_image_menu_item ( char * stock, GtkIconSize size, char * label );

#endif /* __GZRT_GUI_UTILITY_H */
