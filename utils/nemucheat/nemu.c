/*************************
* Nemu Cheat File Editor *
*************************/
#include <glib.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>

struct GSCode
{
	guint32	address;
	guint32	value;
};

struct Cheat
{
	char          * name;
	struct GSCode * codes;
	int             count;
};

struct Game
{
	char * name;
	GList * cheats;
};

/* Globals */
static GList         * games;
static GKeyFile      * config;
static GtkWidget     * combo;
static GtkTreeSelection * _sel;
static GtkWidget     * view;
static GtkListStore  * store;
static GtkWidget     * cname;
static GtkTextBuffer * cdata;

static void
SIG ( void )
{
	g_print( "SIG\n" );
}

/* Add a game to the list */
static int
nc_add_game ( char * name )
{
	char ** keys;
	int amount, i;
	struct Game * g = malloc( sizeof(struct Game) );
	g->name = strdup( name );
	g->cheats = NULL;
	
	/* Append to list */
	games = g_list_append( games, g );
	
	/* Load codes */
	amount = atoi( g_key_file_get_value( config, name, "NumCheats", NULL ) );
	
	/* Load each one */
	for( i = 0; i < amount; i++ )
	{
		char buffer[64];
		struct Cheat * cheat = malloc( sizeof(struct Cheat) );
		int count, j;
		
		sprintf( buffer, "CheatName%uCount", i );
		count = atoi( g_key_file_get_value( config, name, buffer, NULL ) );
		cheat->count = count;
		
		/* Allocate mem for codes */
		cheat->codes = malloc( sizeof(struct GSCode) * count );
		
		/* Set the name */
		sprintf( buffer, "CheatName%u", i );
		cheat->name = g_key_file_get_value( config, name, buffer, NULL );
		
		/* Read 'em */
		for( j = 0; j < count; j++ )
		{
			char buffer[64];
			char * result;
			struct GSCode * cur = &cheat->codes[j];
			
			sprintf( buffer, "CheatName%uCode%u", i, j );
			
			if( !(result = g_key_file_get_value( config, name, buffer, NULL )) )
			{
				continue;
			}
			
			sscanf( result, "%08X %04X", &cur->address, &cur->value );
			free( result );
		}
		
		/* Append the cheat */
		g->cheats = g_list_append( g->cheats, cheat );
		
	cont:;
	}
	
	/* Return the ID */
	return g_list_length( games ) - 1;
}

/* Load in the cheat code file */
static void
nc_load_codes ( char * file )
{
	GError * error = NULL;
	gchar ** r;
	
	/* Create new keyfile context */
	config = g_key_file_new();
	
	/* Open the file */
	if( !g_key_file_load_from_file( config, file, 0, &error ) )
	{
		printf("Error while loading configuration: %s\n", error->message );
		exit( -1 );
	}
	
	r = g_key_file_get_groups( config, NULL );
	
	while( *r )
	{
		nc_add_game( r[0] );
		g_print( "%s\n", *r );
		r++;
	}
}

/* Generate the menu bar */
static GtkWidget * 
nc_menu_bar_gen ( void )
{
	struct MenuBarEntry
	{
		const char * name;
		const char * image;
		void (*handler)();
	};
	
	struct MenuBar
	{
		const char                * name;
		const struct MenuBarEntry * entries;
		int 						count;
	};
	
	static const struct MenuBarEntry
	menu_file[] =
	{
		{ "_New",     GTK_STOCK_NEW,     NULL },
		{ "_Open",    GTK_STOCK_OPEN,    NULL },
		{ "_Save",    GTK_STOCK_SAVE,    NULL },
		{ "Save _As", GTK_STOCK_SAVE_AS, NULL },
		{ NULL,       NULL,              NULL },
		{ "_Quit",    GTK_STOCK_QUIT,    NULL }
	};
	
	static const struct MenuBarEntry
	menu_help[] =
	{
		{ "About", GTK_STOCK_ABOUT, NULL }
	};
	
	static const struct MenuBar
	menu[] =
	{
		{ "_File", menu_file, sizeof(menu_file) / sizeof(struct MenuBarEntry) },
		{ "_Help", menu_help, sizeof(menu_help) / sizeof(struct MenuBarEntry) }
	};
	
	
	GtkWidget * menubar;
	GtkWidget * item;
	GtkWidget * submenu;
	GtkWidget * image;
	int i, j;
	
	/* Create menu */
	menubar = gtk_menu_bar_new();
	
	/* Loop through each menu */
	for( i = 0; i < sizeof(menu) / sizeof(struct MenuBar); i++ )
	{
		/* Create the item */
		item = gtk_menu_item_new_with_mnemonic( menu[i].name );
		gtk_container_add( GTK_CONTAINER(menubar), item );
		
		/* Create submenu */
		submenu = gtk_menu_new();
		gtk_menu_item_set_submenu( GTK_MENU_ITEM(item), submenu );
		
		/* Create items */
		for( j = 0; j < menu[i].count; j++ )
		{
			const struct MenuBarEntry * f = &menu[i].entries[j];
			
			/* Is it NULL? If so, it's a separator */
			if( !f->name && !f->image )
			{
				item = gtk_separator_menu_item_new();
				gtk_widget_set_sensitive( item, FALSE );
				gtk_container_add( GTK_CONTAINER(submenu), item );
				continue;
			}
			
			/* Create item */http://www.64.vg/wiki/index.php?title=Main_Page
			item = gtk_image_menu_item_new_with_mnemonic( f->name );
			gtk_container_add( GTK_CONTAINER(submenu), item );
			
			/* Create image (if applicable) */
			if( f->image )
			{
				image = gtk_image_new_from_stock( f->image, GTK_ICON_SIZE_MENU );
				gtk_image_menu_item_set_image( GTK_IMAGE_MENU_ITEM(item), image );
			}
			
			/* Set handler (if applicable) */
			if( f->handler )
				g_signal_connect( G_OBJECT(item), "activate", G_CALLBACK(f->handler), NULL );
		}
	}
	
	/* Show all */
	gtk_widget_show_all( menubar );
	
	/* Return it */
	return menubar;
}

/* Update code view */
static void
nc_code_update ( void )
{
	GtkTreeModel * m = gtk_tree_view_get_model( view );
	GList * l = gtk_tree_selection_get_selected_rows( _sel, &m );
	if( !l )
		return;
	char * b = gtk_tree_path_to_string(l->data);
	if( !b )
		return;
	int id = atoi(b);
	g_free(b);
	struct Game * game;
	struct Cheat * cheat;
	GtkTextIter st, en;
	char buffer[64];
	char * p = buffer;
	int i;
	
	game = g_list_nth( games, gtk_combo_box_get_active( GTK_COMBO_BOX(combo) ) )->data;
	cheat = g_list_nth( game->cheats, id )->data;
	
	/* Set code name */
	gtk_entry_set_text( GTK_ENTRY(cname), cheat->name );
	
	/* Display code */
	gtk_text_buffer_get_bounds( GTK_TEXT_BUFFER(cdata), &st, &en );
	gtk_text_buffer_delete( GTK_TEXT_BUFFER(cdata), &st, &en );
	
	for( i = 0; i < cheat->count; i++ )
	{
		snprintf( buffer, sizeof(buffer), "%08X %04X\n", 
		cheat->codes[i].address, cheat->codes[i].value );
		gtk_text_buffer_insert_at_cursor( GTK_TEXT_BUFFER(cdata), buffer, -1 );
	}
}

/* Populate the tree view */
static void
nv_tree_view_populate ( void )
{
	int id = gtk_combo_box_get_active( GTK_COMBO_BOX(combo) );
	GList * e;
	struct Game * g;
	int i;
	
	if( id < 0 )
		return;
	
	store = gtk_list_store_new( 1, G_TYPE_STRING, -1 );
	
	/* Select game */
	id = gtk_combo_box_get_active( GTK_COMBO_BOX(combo) );
	e = g_list_nth( games, id );
	g = e->data;
	
	/* Add some data... */
	for( i = 0; i < g_list_length(g->cheats); i++ )
	{
		GtkTreeIter j;
		char buffer[64];
		char * name = ((struct Cheat*)(g_list_nth(g->cheats, i)->data))->name;
		
		gtk_list_store_append( GTK_LIST_STORE(store), &j );
		
		gtk_list_store_set( GTK_LIST_STORE(store), &j, 0, name, -1 );
	}
	
	/* Set model */
	if( view )
		gtk_tree_view_set_model( GTK_TREE_VIEW(view), GTK_TREE_MODEL(store) );
}

/* Create the tree view */
static GtkWidget * 
nc_tree_view_create ( void )
{
	GtkWidget    * scroll;
	int i, id;
	
	/* Create tree view & store */
	view = gtk_tree_view_new();
	
	/* Create col */
	GtkCellRenderer * r = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes( GTK_TREE_VIEW(view),
		-1, "Cheat name", r, "text", 0, NULL );
	
	nv_tree_view_populate();
	
	/* Set model */
	gtk_tree_view_set_model( GTK_TREE_VIEW(view), GTK_TREE_MODEL(store) );
	
	_sel = gtk_tree_view_get_selection( GTK_TREE_VIEW(view) );
	g_signal_connect( G_OBJECT(_sel), "changed", G_CALLBACK(nc_code_update), NULL );
	
	/* Return it */
	return view;
}
	
	

/* Create the main display */
static GtkWidget *
nc_display_create ( void )
{
	GtkWidget * vbox;
	GtkWidget * vbox2;
	GtkWidget * vbox3;
	GtkWidget * scroll;
	GtkWidget * tree;
	GtkWidget * text;
	GtkWidget * align;
	GtkWidget * hbox;
	GtkWidget * hbox2;
	GtkWidget * button;
	GtkWidget * entry;
	GtkWidget * label;
	int i;
	
	/* Create main vbox */
	vbox = gtk_vbox_new( FALSE, 0 );
	
	/* Append the menu bar */
	gtk_box_pack_start( GTK_BOX(vbox), nc_menu_bar_gen(), FALSE, TRUE, 0 );
	
	/* Append alignment */
	align = gtk_alignment_new( 0.5f, 0.5f, 1.0f, 1.0f );
	gtk_alignment_set_padding( GTK_ALIGNMENT(align), 12, 12, 12, 12 );
	gtk_box_pack_start( GTK_BOX(vbox), align, TRUE, TRUE, 0 );
	
	/* Create second vbox */
	vbox2 = gtk_vbox_new( FALSE, 12 );
	gtk_container_add( GTK_CONTAINER(align), vbox2 );
	
	/* Create game selector */
	combo = gtk_combo_box_entry_new_text();
	g_signal_connect( G_OBJECT(combo), "changed", G_CALLBACK(nv_tree_view_populate), NULL );
	
	/* Fill it */
	for( i = 0; i < g_list_length( games ); i++ )
	{
		char buffer[128];
		int k = 0;
		char * g = ((struct Game*)(g_list_nth(games, i)->data))->name;
		
		while( *g && strcmp( g, ".Cheats" ) )
			buffer[k++] = *g++;
		
		gtk_combo_box_append_text( GTK_COMBO_BOX(combo), buffer );
		
		if( !i )
			gtk_combo_box_set_active( GTK_COMBO_BOX(combo), i );
	}
	
	/* Add it */
	gtk_box_pack_start( GTK_BOX(vbox2), combo, FALSE, TRUE, 0 );
		
	/* Create middle pane */
	hbox = gtk_hbox_new( TRUE, 12 );
	gtk_box_pack_start( GTK_BOX(vbox2), hbox, TRUE, TRUE, 0 );
	
	/* Create tree */
	tree = gtk_tree_view_new();
	scroll = gtk_scrolled_window_new( NULL, NULL );
	gtk_scrolled_window_set_shadow_type( GTK_SCROLLED_WINDOW(scroll), GTK_SHADOW_IN );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC );
	gtk_container_add( GTK_CONTAINER(scroll), nc_tree_view_create() );
	gtk_box_pack_start( GTK_BOX(hbox), scroll, TRUE, TRUE, 0 );	
	
	/* Create code */
	vbox3 = gtk_vbox_new( FALSE, 8 );
	entry = gtk_entry_new( );
	cname = entry;
	gtk_entry_set_text( GTK_ENTRY(entry), "Code name" );
	gtk_box_pack_start( GTK_BOX(vbox3), entry, FALSE, TRUE, 0 );
	scroll = gtk_scrolled_window_new( NULL, NULL );
	text = gtk_text_view_new();
	cdata = gtk_text_view_get_buffer( GTK_TEXT_VIEW(text) );
	PangoFontDescription *font_desc;
	font_desc = pango_font_description_from_string( "Courier 10" );
	gtk_widget_modify_font( text, font_desc );
	gtk_text_view_set_wrap_mode( GTK_TEXT_VIEW(text), GTK_WRAP_NONE );
	gtk_scrolled_window_set_shadow_type( GTK_SCROLLED_WINDOW(scroll), GTK_SHADOW_IN );
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
	gtk_container_add( GTK_CONTAINER(scroll), text );
	gtk_box_pack_start( GTK_BOX(vbox3), scroll, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(hbox), vbox3, TRUE, TRUE, 0 );	
	
	
	/* Create buttons */
	align = gtk_alignment_new( 0.0f, 0.5f, 0.2f, 1.0f );
	hbox = gtk_hbox_new( TRUE, 8 );
	gtk_container_add( GTK_CONTAINER(align), hbox );
	button = gtk_button_new_with_label( "Close" );
	gtk_box_pack_start( GTK_BOX(hbox), button, FALSE, TRUE, 0 );
	button = gtk_button_new_with_label( "Save" );
	gtk_box_pack_start( GTK_BOX(hbox), button, FALSE, TRUE, 0 );
	button = gtk_button_new_with_label( "Add" );
	gtk_box_pack_start( GTK_BOX(hbox), button, FALSE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX(vbox2), align, FALSE, TRUE, 0 );
	
	return vbox;
}

int main ( int argc, char ** argv )
{
	
	
	nc_load_codes( argv[1] );
	
	gtk_init( &argc, &argv );
	
	GtkWidget * w = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	gtk_container_add( GTK_CONTAINER(w), nc_display_create() );
	gtk_widget_show_all( w );
	
	gtk_main();
	
	return 0;
}
