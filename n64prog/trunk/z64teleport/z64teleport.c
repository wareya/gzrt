/***********************************
* Teleporter for Zelda 64 v1.0 (U) *
***********************************/
#include <n64.h>
#include <z64teleport.h>

/*
** CONTROLS
**
**  DUP     -   Store current coords in slot
**  DLEFT   -   Decrease current slot by one
**  DRIGHT  -   Increase current slot by one
**  DDOWN   -   Restore current slot (load XYZ)
*/

/* Preserved vars */
THandle     status;                     /* Application status   */
CoordR      locations[Z64T_LOC_AMT];    /* Locations            */

/* Main routine */
void _start ( void )
{
    /* Data pointer */
    asm volatile("la $gp, _gp");
    
    /* Vars */
    CoordR      *link   = Z64T_CNST_LINKXYZ;
    Controller  *input  = Z64T_CNST_BUTTON;
    u16         *rupee  = Z64T_CNST_RUPEE;
    
    /* ~~~~~ ~~~~~ ~~~~~ ~~~~~ ~~~~~ ~~~~~ ~~~~~ ~~~~~ ~~~~~ ~~~~~ ~~~~~ */
    
    /*
    ** Increment slot position
    */
    if( (input->pad & BUTTON_D_RIGHT) && status.curslot < Z64T_LOC_AMT - 1 )
        status.curslot++;
    
    /*
    ** Decrement slot position
    */
    if( (input->pad & BUTTON_D_LEFT) && status.curslot  )
        status.curslot--;
    
    /*
    ** Store current coords
    */
    if( input->pad & BUTTON_D_UP )
    {
        locations[status.curslot].x = link->x;
        locations[status.curslot].y = link->y;
        locations[status.curslot].z = link->z;
    }
    
    /*
    ** Restore coordinates
    */
    if( input->pad & BUTTON_D_DOWN )
    {
        link->x = locations[status.curslot].x;
        link->y = locations[status.curslot].y;
        link->z = locations[status.curslot].z;
    }
    
    /* Store slot position in rupees */
    *rupee = status.curslot;
}
