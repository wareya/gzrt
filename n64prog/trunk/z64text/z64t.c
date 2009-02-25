/**************************
* OoT Debug print routine *
**************************/

/* Souylsin is clueless	*/

#include <n64.h>
#include <z64t.h>
#include <z64debug.h>

void _start ( void )
{
    /* Routines	*/
    SETRGB SetTextRGB	= (void*)Z64TEXT_SET_RGB;
    SETXY SetTextXY	= (void*)Z64TEXT_SET_XY;
    DRAWTEXT DrawText	= (void*)Z64TEXT_DRAW;

    /* ZTarget	*/
    Z64ZTARGET *ztarget	= (void*)Z64_CNST_ZTARGET;

    /* Set RGB	*/
    SetTextRGB
    (
        Z64TEXT_CONST_ARG,	/* Unknown	*/
        255,			/* Red		*/
        255,			/* Green	*/
        255,			/* Blue		*/
        255			/* Alpha	*/
    );

    /* Set XY	*/
    SetTextXY
    (
        Z64TEXT_CONST_ARG,	/* Unknown	*/
        12,			/* X position	*/
        26			/* Y position	*/
    );
    
    /* Draw that sucker */
    DrawText
    (
        Z64TEXT_CONST_ARG,		/* Unknown	*/
        "Souylsin is clueless (%08X)",	/* String	*/
        ztarget				/* Format string */
    );
}

	
