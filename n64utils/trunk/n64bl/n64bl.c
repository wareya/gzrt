/*
*   Nintendo 64 Binary Loader
*   Copyright (c) 2008  ZZT32 [mbr@64.vg]
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public
*   Licence along with this program; if not, write to the Free
*   Software Foundation, Inc., 51 Franklin Street, Fifth Floor, 
*   Boston, MA  02110-1301, USA
*/

/********************
* N64 Binary Loader *
********************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LOADER_ADDRESS  0x1000

#define notice(fmt, ...)        { printf( "[-] " fmt "\n", ##__VA_ARGS__ ); fflush( stdout ); }

#define W_U16(t,w)  { (t)[0] = (w) >> 8; (t)[1] = (w) & 0xFF; }
#define U16(a)      ((a)[0] << 8 | (a)[1])

#define CODE_SET_VALUE(code, i, w)          \
{                                           \
    W_U16( &(code)[i], (w) >> 16 );         \
    W_U16( &(code)[i + 4], (w) & 0xFFFF );  \
}

#define CODE_GET_VALUE(code, i) (U16(&(code)[i]) << 8 | U16(&(code)[i + 4]))

#define I_BC_ROM_ADDR   10  /* Copy source  */
#define I_BC_RAM_ADDR   26  /* Copy target  */
#define I_BC_SIZE       38  /* Copy size    */

unsigned char code_boot_copy[] =
{
	0x3C, 0x1D, 0x80, 0x3F,  0x37, 0xBD, 0xF0, 0x00, 
	0x3C, 0x02, 0xB0, 0x65,  0x34, 0x42, 0x43, 0x21, 
    0x3C, 0x03, 0xA4, 0x60,  0xAC, 0x62, 0x00, 0x04, 
	0x3C, 0x04, 0x12, 0x34,  0x34, 0x84, 0x56, 0x78, 
    0xAC, 0x64, 0x00, 0x00,  0x3C, 0x02, 0x00, 0x00, 
	0x34, 0x42, 0x46, 0x60,  0xAC, 0x62, 0x00, 0x0C, 
    0x8C, 0x62, 0x00, 0x10,  0x30, 0x42, 0x00, 0x01, 
	0x14, 0x40, 0xFF, 0xFD,  0x00, 0x00, 0x00, 0x00, 
    0x3C, 0x02, 0x80, 0x00,  0x00, 0x82, 0x10, 0x25, 
	0xBC, 0x55, 0x00, 0x00,  0x00, 0x40, 0x00, 0x08, 
    0x00, 0x00, 0x00, 0x00, 
} ;

};

#define I_CFB_SOURCE    10  /* Copy source  */
#define I_CFG_SIZE      18  /* Copy size    */
#define I_CFG_ENTRY     54  /* Function     */

unsigned char code_fix_boot[] =
{
	0x3C, 0x02, 0xA0, 0x00,  0x34, 0x42, 0x04, 0x00,
	0x3C, 0x03, 0x12, 0x34,  0x34, 0x63, 0x43, 0x21,
    0x3C, 0x04, 0x11, 0x11,  0x34, 0x84, 0x11, 0x11,
	0x00, 0x82, 0x20, 0x21,  0x8C, 0x65, 0x00, 0x00,
    0xAC, 0x45, 0x00, 0x00,  0x20, 0x42, 0x00, 0x04,
	0x20, 0x63, 0x00, 0x04,  0x14, 0x44, 0xFF, 0xFB,
    0xBC, 0x44, 0x00, 0x00,  0x3C, 0x04, 0x88, 0x88,
	0x34, 0x84, 0x88, 0x88,  0x00, 0x80, 0xF8, 0x09,
    0x00, 0x00, 0x00, 0x00,  0x08, 0x00, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00
};

/* Overwritten code */
unsigned char code_old[sizeof(code_boot_copy)];
unsigned code_old_size = sizeof(code_boot_copy);

/* Storage for binary file */
unsigned char * binary_file;
unsigned binary_size;

/* Application global configuration */
unsigned rom_source;
unsigned ram_target;

/* ROM file */
FILE * rom_h;
unsigned rom_size;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

void prototype ( int argc, char ** argv )
{
    printf
    ( 
        "Application prototype:\n"
        " %s [rom] [binary] [rom address] [ram entrypoint]\n",
        argv[0] 
    );
    fflush( stdout );
    exit( -1 );
}

int main ( int argc, char ** argv )
{
    int i;
    FILE * h;
    
    /* Check arguments */
    if( argc != 5 )
        prototype( argc, argv );
    
    /* Open ROM for reading */
    rom_h = fopen( argv[1], "rb+" );
    fseek( rom_h, 0, SEEK_END );
    rom_size = ftell(rom_h);
    fseek( rom_h, 0, SEEK_SET );
    
    /* Read the binary */
    h = fopen( argv[2], "rb" );
    fseek( h, 0, SEEK_END );
    binary_file = malloc( binary_size = ftell(h) );
    fseek( h, 0, SEEK_SET );
    fread( binary_file, 1, binary_size, h );
    fclose( h );
    
    /* Read other values */
    sscanf( argv[3], "%X", &rom_source );
    sscanf( argv[4], "%X", &ram_target );
    ram_target -= sizeof(code_fix_boot) + sizeof(code_old);
    
    /* Status info */
    notice( "0x%08X -> 0x%08X", rom_source, ram_target );
    
    /* Read the code to be overwritten */
    fseek( rom_h, LOADER_ADDRESS, SEEK_SET );
    fread( code_old, 1, sizeof(code_old), rom_h );
    
    /* Personalize code loader */
    int cs = binary_size + sizeof(code_fix_boot) + sizeof(code_old);
    CODE_SET_VALUE( code_boot_copy, I_BC_ROM_ADDR, rom_source | 0x10000000 );
    CODE_SET_VALUE( code_boot_copy, I_BC_RAM_ADDR, ram_target & 0x00FFFFFF );
    CODE_SET_VALUE( code_boot_copy, I_BC_SIZE, cs - (cs % 8) + 8 - 1 );
    
    /* Write code loader */
    fseek( rom_h, LOADER_ADDRESS, SEEK_SET );
    fwrite( code_boot_copy, 1, sizeof(code_boot_copy), rom_h );
    notice( "Wrote code loader @ 0x%08X.", LOADER_ADDRESS );
    
    /* Personalize tidiness code */
    CODE_SET_VALUE( code_fix_boot, I_CFB_SOURCE, (ram_target | 0xA0000000) + sizeof(code_fix_boot) );
    CODE_SET_VALUE( code_fix_boot, I_CFG_SIZE, sizeof(code_old) );
    CODE_SET_VALUE( code_fix_boot, I_CFG_ENTRY, (ram_target | 0x80000000) + sizeof(code_fix_boot) + sizeof(code_old) );
    
    /* Write tidiness code */
    fseek( rom_h, rom_source, SEEK_SET );
    fwrite( code_fix_boot, 1, sizeof(code_fix_boot), rom_h );
    notice( "Wrote tidiness code @ 0x%08X.", rom_source );
    
    /* Write original code */
    fwrite( code_old, 1, sizeof(code_old), rom_h );
    notice( "Wrote original code @ 0x%08X.", rom_source + sizeof(code_fix_boot) );
    
    /* Write binary */
    fwrite( binary_file, 1, binary_size, rom_h );
    notice( "Wrote binary @ 0x%08X.", rom_source + sizeof(code_fix_boot) + sizeof(code_fix_boot) );
    
    return 0;
}
