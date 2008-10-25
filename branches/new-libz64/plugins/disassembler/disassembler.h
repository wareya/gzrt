#ifndef __DISASSEMBLER_H
#define __DISASSEMBLER_H
#include <gtk/gtk.h>

/* Constants */
#define ASM_ROWS 20
#define ASM_COLS 3
#define ASM_FONT "Lucida Console 8"
    
/* Text buffer container */
struct DASMRowTextBuffers
{
    GtkTextBuffer * address;
    GtkTextBuffer * raw;
    GtkTextBuffer * disassembly;
};

/* Disassembler window context */
typedef struct
{
    /* GUI stuff */
    GtkWidget                  * window;
    struct DASMRowTextBuffers    Text[ASM_ROWS];
    
    /* Varying disassembler stuff */
    unsigned int                 flags;
    unsigned int                 pc;
    unsigned int                 pc_start;
    
    /* Disassembler stuff */
    int                          filesize;
    char                       * filename;
    unsigned char              * data;
	
	/* Plugin stuff */
	const struct PluginTransac * transac;
} 
DASM;

/* Functions not contained within main file */
extern void dasm_save_show ( DASM * h );

#endif /* __DISASSEMBLER_H */
