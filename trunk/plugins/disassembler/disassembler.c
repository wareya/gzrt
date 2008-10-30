#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <r4kdis.h>
#include <disassembler.h>

#include "gzrtplugin.h"

/* Function declarations */
int init     ( const struct Functions * f );
int menu_bar ( const struct Functions *, const struct RomSpec * );
int dasm     ( const struct Functions *, const struct RomSpec *, const struct PluginTransac * t );

/* GZRT Inherited functions */
static const struct Functions * func;

/* Plugin information */
struct PluginMeta gzrt_plugin_info =
{
	"Disassembler",
	"MIPS R4300 Disassembler",
	"ZZT32",
	"64.vg",
	"prelim",
	
	/* Description */
	NULL,
	
	/* Init, menu & file action funcs */
	init, menu_bar, dasm
};

/* Macros */
#define U32(x) ((x)[0] << 24 | (x)[1] << 16 | (x)[2] << 8 | (x)[3])

/* Connect data to a widget */
#define HOOKUP( component, widget, name )                       \
    g_object_set_data_full( G_OBJECT(component), name,          \
    gtk_widget_ref(widget), (GDestroyNotify)gtk_widget_unref )

/* Lookup connected data */
#define LOOKUP( component, name )   \
    g_object_get_data( G_OBJECT(component), name )
    
/* Functions */
void dasm_jump_handle ( GtkWidget * w, DASM * h );

/* Set font of a widget */
void dasm_set_font ( GtkWidget * w, char * font )
{
    PangoFontDescription * font_desc;
    
    font_desc = pango_font_description_from_string( font );
    gtk_widget_modify_font( w, font_desc );
}

/* Pad a widget */
GtkWidget * dasm_pad_widget ( GtkWidget * w, int t, int b, int l, int r )
{
    GtkWidget * a = gtk_alignment_new( 0.5f, 0.5f, 1.0f, 1.0f );
    gtk_alignment_set_padding( GTK_ALIGNMENT(a), t, b, l, r );
    gtk_container_add( GTK_CONTAINER(a), w );
    
    return a;
}

/* Set the status bar message */
void dasm_status_set_message ( DASM * h, char * fmt, ... )
{
    va_list      ap;
    static int   id;
    GtkWidget  * bar = LOOKUP( h->window, "window-status" );
    char         buffer[256];
    
    /* Prepare the message */
    va_start( ap, fmt );
    vsnprintf( buffer, sizeof(buffer) - 1, fmt, ap );
    va_end( ap );
    
    /* Push the new message */
    if( id )
    {
        gtk_statusbar_pop( GTK_STATUSBAR(bar), id );  
        gtk_statusbar_push( GTK_STATUSBAR(bar), (id = 0), buffer );
    }
    else
        gtk_statusbar_push( GTK_STATUSBAR(bar), id++, buffer );
}

/* Populate the text buffers */
void dasm_buffer_fill ( DASM * h )
{
    GtkTextIter     st, en;
    unsigned int    i, len;
    char            buffer[128];
    
    /* Go through each row */
    for( i = 0; i < ASM_ROWS; i++ )
    {
        unsigned int pc    = (h->pc | h->pc_start) + i * 4;
        unsigned int instr = U32( h->data + h->pc + i * 4 );
        
        
        /* Address - prepare buffer */
        gtk_text_buffer_get_bounds( GTK_TEXT_BUFFER(h->Text[i].address), &st, &en );
        gtk_text_buffer_delete(     GTK_TEXT_BUFFER(h->Text[i].address), &st, &en );
        
        /* Insert text */
        len = snprintf( buffer, sizeof(buffer), "%08X", pc );
        gtk_text_buffer_insert_at_cursor( GTK_TEXT_BUFFER(h->Text[i].address), buffer, len );
        
        
        /* Raw hex - prepare buffer */
        gtk_text_buffer_get_bounds( GTK_TEXT_BUFFER(h->Text[i].raw), &st, &en );
        gtk_text_buffer_delete(     GTK_TEXT_BUFFER(h->Text[i].raw), &st, &en );
        
        /* Insert text */
        len = snprintf( buffer, sizeof(buffer), "%08X", instr );
        gtk_text_buffer_insert_at_cursor( GTK_TEXT_BUFFER(h->Text[i].raw), buffer, len );
        
        
        /* Disassembly */
        gtk_text_buffer_get_bounds( GTK_TEXT_BUFFER(h->Text[i].disassembly), &st, &en );
        gtk_text_buffer_delete(     GTK_TEXT_BUFFER(h->Text[i].disassembly), &st, &en );
        
        /* Insert text */
        mr4kd_disassemble( instr, pc, buffer );
        gtk_text_buffer_insert_at_cursor( GTK_TEXT_BUFFER(h->Text[i].disassembly), buffer, -1 );
    }
}

/* Seeking functions */
void dasm_seek ( DASM * h, unsigned int pc )
{
    /* Check validity */
    if( !((int)pc >= 0 && pc < h->filesize) )
        return;
    
    h->pc = pc;
    dasm_buffer_fill( h );
    dasm_status_set_message( h, "PC: 0x%08X", h->pc | h->pc_start );
}
void dasm_seek_page_up ( DASM * h )
{
    dasm_seek( h, h->pc - ASM_ROWS * 4 );
}
void dasm_seek_page_down ( DASM * h )
{
    dasm_seek( h, h->pc + ASM_ROWS * 4 );
}
void dasm_seek_start ( DASM * h )
{
    dasm_seek( h, 0 );
}
void dasm_seek_end ( DASM * h )
{
    dasm_seek( h, h->filesize - (ASM_ROWS * 4) );
}

/* Create a text box */
GtkWidget * dasm_window_text_box_create ( GtkTextBuffer * buffer )
{
    GtkWidget * scroll;
    GtkWidget * text;
    
    /* Set scrolled window attributes */
    scroll = gtk_scrolled_window_new( NULL, NULL );
    gtk_scrolled_window_set_policy(
        GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_NEVER, GTK_POLICY_NEVER
    );
    gtk_scrolled_window_set_shadow_type(
        GTK_SCROLLED_WINDOW(scroll), GTK_SHADOW_IN
    );
    
    /* Create a text field */
    text = gtk_text_view_new_with_buffer( buffer );
    
    /* Set text field preferences */
    gtk_text_view_set_pixels_above_lines( GTK_TEXT_VIEW(text), 2     );
    gtk_text_view_set_pixels_below_lines( GTK_TEXT_VIEW(text), 2     );
    gtk_text_view_set_left_margin(        GTK_TEXT_VIEW(text), 2     );
    gtk_text_view_set_right_margin(       GTK_TEXT_VIEW(text), 2     );
    gtk_text_view_set_editable(           GTK_TEXT_VIEW(text), FALSE );
    
    /* Set font */
    dasm_set_font( text, ASM_FONT );
    
    /* Attach text to scroll */
    gtk_container_add( GTK_CONTAINER(scroll), text );
    
    return scroll;
}

/* Create a jump to address dialog */
static int jta_status;
void dasm_window_jump_to_address ( DASM * h )
{
    GtkWidget *Jump_to_address;
    GtkWidget *Element_separator;
    GtkWidget *Address_selector;
    GtkWidget *Address_confirmation;
    extern void dasm_window_close ( GtkWidget * w );
    
    /* Check invocation status */
    if( jta_status )
        return;
    else
        jta_status = 1;
    
    /* Create window */
    Jump_to_address = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    gtk_container_set_border_width( GTK_CONTAINER(Jump_to_address), 12 );
    gtk_window_set_title( GTK_WINDOW (Jump_to_address), "Jump to address" );
    gtk_window_set_resizable( GTK_WINDOW(Jump_to_address), FALSE );
    
    /* Create separator */
    Element_separator = gtk_hbox_new( FALSE, 12 );
    gtk_widget_show( Element_separator );
    gtk_container_add( GTK_CONTAINER(Jump_to_address), Element_separator );
    
    /* Address selection box */
    Address_selector = gtk_combo_box_entry_new_text();
    gtk_widget_show( Address_selector );
    gtk_box_pack_start( GTK_BOX(Element_separator), Address_selector, TRUE, TRUE, 0 );
    gtk_combo_box_append_text( GTK_COMBO_BOX(Address_selector), "0x80000400" );
    
    /* Set font */
    dasm_set_font( Address_selector, ASM_FONT );
    
    /* Address confirmation button */
    Address_confirmation = gtk_button_new_with_mnemonic( "Go" );
    gtk_widget_show( Address_confirmation );
    gtk_box_pack_start( GTK_BOX(Element_separator), Address_confirmation, FALSE, FALSE, 0 );
    
    /* Hookup text box */
    HOOKUP( Address_confirmation, Address_selector, "address" );
    HOOKUP( Address_confirmation, Jump_to_address, "window" );
    
    /* Set signal */
    g_signal_connect( G_OBJECT(Address_confirmation), "clicked", G_CALLBACK(dasm_jump_handle), h );
    g_signal_connect( G_OBJECT(Jump_to_address), "destroy", G_CALLBACK(dasm_window_close), NULL );
    
    /* Show the window */
    gtk_widget_show_all( Jump_to_address );
}
void dasm_window_close ( GtkWidget * w )
{
    jta_status = 0;
}
    

/* Create the text fields containing info - returned in an alignment */
GtkWidget * dasm_window_fields_create ( DASM * h )
{
    GtkWidget * align;
    GtkWidget * disasm_and_buttons;
    GtkWidget * headsep;
    GtkWidget * headers;
    GtkWidget * cols;
    
    char *names[] = { "Address", "Raw", "Disassembly" };
    
    int i;
    
    /* Create alignment */
    align = gtk_alignment_new( 0.5f, 0.5f, 1.0f, 1.0f );
    gtk_alignment_set_padding( GTK_ALIGNMENT(align), 12, 12, 12, 12 );
    
    /* Create header & content separator */
    headsep = gtk_vbox_new( FALSE, 12 );
    
    /* Create hbox for each column */
    cols = gtk_hbox_new( FALSE, 4);
    
    /* Go through each column */
    for( i = 0; i < ASM_COLS; i++ )
    {
        GtkWidget * name = gtk_label_new( names[i] );
        GtkWidget * col  = gtk_vbox_new( FALSE, 4 );
        int         k;
        
        /* Set label alignment */
        gtk_misc_set_alignment( GTK_MISC(name), 0.0f, 0.5f );
        
        /* Append the col name to box */
        gtk_box_pack_start( GTK_BOX(col), name, TRUE, FALSE, 0 );
        
        /* Generate the text boxes */
        for( k = 0; k < ASM_ROWS; k++ )
        {
            gtk_box_pack_start
            ( 
                GTK_BOX(col), 
                dasm_window_text_box_create( GTK_TEXT_BUFFER( ((void**)&h->Text[k])[i]) ), 
                TRUE, FALSE, 0 
            );
        }
    
        /* Append the column to horizontal box */
        gtk_box_pack_start( GTK_BOX(cols), col, ( i + 1 == ASM_COLS ? TRUE : FALSE ), ( i + 1 == ASM_COLS ? TRUE : FALSE ), 0 );
    }
    
    /* Add the columns to the alignment */
    gtk_container_add( GTK_CONTAINER(align), cols );
    
    return align;
}

/* Create side buttons controlling everything */
GtkWidget * dasm_buttons_create ( DASM * h )
{
    /* Button specification */
    struct ButtonSpec
    {
        char * tooltip;
        char * imagename;
        void (*handler)( DASM * h );
    }
    Buttons[] =
    {
        { "Previous page",         "gtk-go-up",       dasm_seek_page_up           },
        { "Next page",             "gtk-go-down",     dasm_seek_page_down         },
        { "Jump to beginning",     "gtk-goto-top",    dasm_seek_start             },
        { "Jump to end",           "gtk-goto-bottom", dasm_seek_end               },
        { "Jump to address",       "gtk-jump-to",     dasm_window_jump_to_address },
        { "Disassembler settings", "gtk-properties",  NULL                        },
        { "Save disassembly",      "gtk-save",        dasm_save_show              }
    };
    GtkWidget * button;
    GtkWidget * image;
    GtkWidget * vbox;
    GtkTooltips * tooltips;
    
    int i;
    
    /* Set up tooltips */
    tooltips = gtk_tooltips_new();
    
    /* Create vertical box */
    vbox = gtk_vbox_new( FALSE, 12 );
    
    /* Create all the buttons */
    for( i = 0; i < sizeof(Buttons) / sizeof(struct ButtonSpec); i++ )
    {
        button = gtk_button_new();
        image = gtk_image_new_from_stock( Buttons[i].imagename, GTK_ICON_SIZE_BUTTON );
        gtk_container_add( GTK_CONTAINER(button), image );
        gtk_tooltips_set_tip( GTK_TOOLTIPS(tooltips), button, Buttons[i].tooltip, NULL );
        
        /* Connect handler */
        if( Buttons[i].handler )
            g_signal_connect_swapped( G_OBJECT(button), "clicked", G_CALLBACK(Buttons[i].handler), (void*)h );
        
        gtk_box_pack_start( GTK_BOX(vbox), button, TRUE, FALSE, 0 );
    }
    
    return vbox;
}

/* Handle jump */
void dasm_jump_handle ( GtkWidget * w, DASM * h )
{
    GtkWidget * field = LOOKUP( w, "address" );
    char * text = gtk_combo_box_get_active_text( GTK_COMBO_BOX(field) );
    unsigned addr;
    
    if( sscanf( text, "%X", &addr ) != 1 )
        return;
    else {
        dasm_seek( h, addr & 0x00FFFFFF );
        gtk_widget_destroy( LOOKUP(w, "window") );
    }
}

/* Populate the main window */
void dasm_window_populate ( DASM * h )
{
    GtkWidget * winrows;
    GtkWidget * winstatus;
    GtkWidget * panel;
	GdkPixbuf * img = gdk_pixbuf_new_from_file( "resources/dasm16x16.png", NULL );
	char        buffer[128];
	
	/* Prepare title */
	sprintf( buffer, "MIPS R4300 Disassembler - \"%s\"", h->filename );
    
    /* Create the main window */
    h->window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	gtk_window_set_icon( GTK_WINDOW(h->window), img );
    
    /* Set the title */
    gtk_window_set_title( GTK_WINDOW(h->window), buffer );
    
    /* Create window separator and add it to window */
    winrows = gtk_vbox_new( FALSE, 0 );
    gtk_container_add( GTK_CONTAINER(h->window), winrows );
    
    /* - create main panel - */
    
    panel = gtk_hbox_new( FALSE, 0 );
    
    gtk_box_pack_start( GTK_BOX(panel), dasm_window_fields_create( h ), TRUE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX(panel), dasm_pad_widget(dasm_buttons_create( h ), 0, 0, 0, 12), FALSE, FALSE, 0 );
    
    gtk_box_pack_start( GTK_BOX(winrows), panel, TRUE, TRUE, 0 );
    
    /* - attach main window - */
    
    /* Create status bar */
    winstatus = gtk_statusbar_new();
    dasm_set_font( winstatus, ASM_FONT );
    
    /* Attach it */
    gtk_box_pack_start( GTK_BOX(winrows), winstatus, FALSE, FALSE, 0 );
    
    /* Link up objects */
    HOOKUP( h->window, winrows,   "window-vbox"   );
    HOOKUP( h->window, winstatus, "window-status" );
    
    /* Show it */
    gtk_widget_show_all( h->window );
    
    return;
}

/* Create a new diassembler context */
DASM * dasm_new ( char * filename )
{
    DASM * ret;
    FILE * h;
    int i;
    
    /* Allocate space for struct */
    if( !(ret = func->calloc( sizeof(DASM) )) )
        return NULL;
    
    /* Read the file in */
    if( !(h = fopen(filename, "rb") ) )
        return NULL;
	
	/* Set filename */
	ret->filename = filename;
    
    /* Create text buffers */
    for( i = 0; i < ASM_ROWS; i++ )
    {
        ret->Text[i].address     = gtk_text_buffer_new( NULL );
        ret->Text[i].raw         = gtk_text_buffer_new( NULL );
        ret->Text[i].disassembly = gtk_text_buffer_new( NULL );
    }
    
    /* Set the program counter defaults */
    ret->pc_start = 0x80000000;
    
    /* Read in the file */
    fseek( h, 0, SEEK_END );
    ret->filesize = ftell(h);
    fseek( h, 0, SEEK_SET );
    ret->data = func->malloc( ret->filesize );
    fread( ret->data, ret->filesize, 1, h );
    fclose( h );
    
    /* Set disassembler defaults */
    ret->flags = mr4kd_flag_get( ~0 );
    
    /* Fill the text buffers */
    dasm_buffer_fill( ret );
    
    /* Populate the main window */
    dasm_window_populate( ret );
    
    /* Set the status bar message */
    dasm_status_set_message( ret, "PC: 0x%08X", ret->pc + ret->pc_start );
    
    /* Ok, all done! */
    return ret;
}

/* Create a new diassembler context from data */
DASM * dasm_new_from_raw ( char * filename, unsigned char * data, int len )
{
    DASM * ret;
    FILE * h;
    int i;
    
    /* Allocate space for struct */
    if( !(ret = func->calloc( sizeof(DASM) )) )
        return NULL;
    
	/* Set filename */
	ret->filename = filename;
	
    /* Create text buffers */
    for( i = 0; i < ASM_ROWS; i++ )
    {
        ret->Text[i].address     = gtk_text_buffer_new( NULL );
        ret->Text[i].raw         = gtk_text_buffer_new( NULL );
        ret->Text[i].disassembly = gtk_text_buffer_new( NULL );
    }
    
    /* Set the program counter defaults */
    ret->pc_start = 0x80000000;
    
    /* Read in the file */
    ret->filesize = len;
    ret->data = data;
    
    /* Set disassembler defaults */
    ret->flags = mr4kd_flag_get( ~0 );
    
    /* Fill the text buffers */
    dasm_buffer_fill( ret );
    
    /* Populate the main window */
    dasm_window_populate( ret );
    
    /* Set the status bar message */
    dasm_status_set_message( ret, "PC: 0x%08X", ret->pc + ret->pc_start );
    
    /* Ok, all done! */
    return ret;
}

void dasm_file_cleanup ( DASM * h )
{
	func->free( h->data );
	func->free( h       );
}

void dasm_cleanup ( DASM * h )
{
	/* Free the plugin handle */
	func->close( h->transac );
	
	/* Free handle */
	func->free( h );
}

/*
** Init
*/

int init ( const struct Functions * f )
{
	/* Update function pointer */
	func = f;
}

/*
** Disassemble a regular file
*/

int menu_bar ( const struct Functions * f, const struct RomSpec * r )
{
	static char fn[1024];
	DASM * h;
	GtkWidget * dialog = gtk_file_chooser_dialog_new
	( 
		"Choose a file to disassemble", NULL,
		GTK_FILE_CHOOSER_ACTION_OPEN, 
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, 
		NULL
	);
	gtk_widget_show_all( dialog );
	
	/* Wait for filename */
	switch( gtk_dialog_run( GTK_DIALOG(dialog) ) )
	{
		/* Yes */
		case GTK_RESPONSE_ACCEPT:
		 strcpy(fn, gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(dialog) ));
		 gtk_widget_destroy( dialog );
		 h = dasm_new( fn );
		 
		 /* Register cleanup handle */
	     g_signal_connect_swapped( G_OBJECT(h->window), "destroy", G_CALLBACK(dasm_file_cleanup), h );
		break;
		
		/* None given? Cancel */
		default:
		 gtk_widget_destroy( dialog );
		 return FALSE;
	}
	
	return TRUE;
}

/*
** Disassemble a file from OoT
*/

int dasm ( const struct Functions * f, const struct RomSpec * r, const struct PluginTransac * t )
{
	struct PluginFileSpec * k = t->file;
	DASM * h = dasm_new_from_raw( k->filename, k->file, k->filesize );
	h->transac = t;
	
	/* Register cleanup handle */
	g_signal_connect_swapped( G_OBJECT(h->window), "destroy", G_CALLBACK(dasm_cleanup), h );
}
