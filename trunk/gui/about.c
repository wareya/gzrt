/*****************************
* GZRT About Window (credit) *
*****************************/
#include <gzrt.h>
#include <gui/about.h>

/* Show the window */
void gzrt_wabout_show ( MAINWIN * c )
{
    const gchar * license =
        "This program is free software; you can redistribute it and/or modify\n"
        "it under the terms of the GNU General Public License as published by\n"
        "the Free Software Foundation; either version 2 of the License, or\n"
        "(at your option) any later version.\n"
         "\n"
        "This program is distributed in the hope that it will be useful,\n"
        "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
        "GNU General Public License for more details.\n"
        "\n"
        "You should have received a copy of the GNU General Public License\n"
        "along with this program; if not, write to the\n"
        "Free Software Foundation, Inc.,\n"
        "51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.\n";
	
	const gchar * authors[] = 
	{
		"ZZT32",
		"Cendamos",
		"_Demo_ (Z64Viewer plugin)",
		"Wareya (ZEV plugin)"
	};
	
	GdkPixbuf * pixbuf = gdk_pixbuf_new_from_file( GZRT_GUI_ICON_DIR "ico128x128.png", NULL );
	
	gtk_show_about_dialog
	( 
		GTK_WINDOW(c->window), 
		"version", GZRT_VERSION,
		"copyright", "(c) 2008 ZZT32 (http://64.vg/)",
		"license", license,
		"website", "http://gzrt.googlecode.com/",
		"comments", "GZRT is an open source ROM toolkit for N64 Zeldas.",
		"authors", authors,
		"logo", pixbuf,
		"title", "About GZRT",
		"program-name", "GZRT",
		NULL
	);
}
