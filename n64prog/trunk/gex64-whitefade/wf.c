/*************************************
* Gex 64: Fade out to white on death *
*************************************/
#include "mips.h"
#include "gex.h"

/* Base box sepcification */
gexMakeBox( box, 320, 240, 0, 0, 255, 255, 255, 0 );

/* Flags... */
#define WHITE_IN_PROGRESS   (1 << 0)

/* Status! */
static u32 status;
static int count;
static int wait;
static float inc;

/* Gex's health */
static u8 * health = (void*)0x800C56BB;

/* Entry point */
void n64start ( void )
{
    /* Set up hook */
    *((u32*)0xA003BDB4) = JAL(hookFunc);
    
    /* No GP overwriting */
    *((u32*)0xA005DC10) = 0;
}

/* Our main() */
void gexMain ( void )
{
    /* Are we whiting yet? */
    if( !(status & WHITE_IN_PROGRESS) )
    {
        /* Is our health zero? */
        if( !*health )
        {
            /* Time to start */
            status |= WHITE_IN_PROGRESS;
            count = 0;
            box.a = 0;
            wait = 0;
            inc = 3.0f;
        }
        else
        {
            /* Just return */
            return;
        }
    }
    
    /* Are we done whiting? */
    if( *health )
    {
        /* Yep */
        status &= ~WHITE_IN_PROGRESS;
        return;
    }
    
    /* Wait? */
    if( wait < 30 )
    {
        wait++;
        return;
    }
    
    /* Draw white box */
    gexDrawBox( &box );
    
    /* Not maxed yet? */
    if( count * inc > 255 )
        box.a = 255;
    else
        box.a = count * inc;
    
    /* Update inc 
    inc += 0.25f;*/
    
    /* Update counter */
    count++;
}

        
