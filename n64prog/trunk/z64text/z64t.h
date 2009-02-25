/**************************
* OoT Debug print routine *
**************************/

/* Credit to sakura89 for info used in this file	*/

#ifndef __Z64TEXT_H
#define __Z64TEXT_H

/* Set text RGB		*/
typedef int ( *SETRGB )
(
	u32,		/* Not sure - Stack Pointer + ???	*/
	u16 red,	/* red 					*/
	u16 green,	/* green				*/
	u16 blue,	/* blue					*/
	u16 alpha	/* alpha				*/
);

/* Set text coordinites	*/
typedef int ( *SETXY )
(
	u32,		/* Not sure - Stack Pointer + ???	*/
	u16 xpos,	/* Text X position (X Pos >> 3)		*/
	u16 ypos	/* Text Y position (Y Pos >> 3)		*/
);

/* Draw text		*/
typedef int ( *DRAWTEXT )
(
	u32,		/* Not sure - Stack Pointer + ???	*/
	char* text,	/* String to be printed			*/
	u32 texvar	/* Variables for format string		*/
);

/* Function pointers	*/
#define Z64TEXT_SET_RGB		0x800FB3AC	/* Set RGB routine	*/
#define Z64TEXT_SET_XY		0x800FB41C	/* Set XY routine	*/
#define Z64TEXT_DRAW		0x800FBCB4	/* Draw text routine	*/

/* Argument Constants	*/
#define Z64TEXT_CONST_ARG	0x80168408 	/* Constant, global pointer used as first arg for each function	*/

#endif

