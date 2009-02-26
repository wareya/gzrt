/**************************
* OoT Debug print routine *
**************************/

/* Credit to sakura89 for info used in this file	*/

#ifndef __Z64TEXT_H
#define __Z64TEXT_H

/* Set text RGB		*/
extern int SetTextRGB(u32 a0, u16 red, u16 green, u16 blue, u16 alpha);

/* Set text coordinites	*/
extern int SetTextXY ( u32 a0, u16 xpos, u16 ypos);

/* Draw text		*/
extern int DrawText (u32 a0, char* text, u32 texvar);

/* Argument Constants	*/
#define Z64TEXT_CONST_ARG	0x80168408 	/* Constant, global pointer used as first arg for each function	*/

#endif

