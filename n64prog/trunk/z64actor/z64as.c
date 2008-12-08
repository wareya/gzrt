/************************
* OoT 1.0 Actor Spawner *
************************/
/*
    This kickass code snippet brought to you by:
     * ZZT32 [mbr@64.vg]
     * Sakura89
    
    NOTES TO DEVVERS:
     In order to  make gcc create code that will call OoT's
     functions correctly you MUST specify -mabi=32 on the
     command line while compiling.
     
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    CONTROLS
     d-up:      increase actor id
     d-down:    decrease actor id
     d-left:    decrease actor var
     d-right:   increase actor var
     l+r+z:     spawn actor
     z:         holding Z while changing ID or var will inc by 10 instead of 1
    
    In the future, the ability to change the XYZ of the spawned
    actors will be available. For now they just use Link's current
    coordinates.
    
    Rotation as well will (hopefully) be added if people want it.
    But it's not exactly that useful.
    
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    ON SCREEN INDICATORS:
     Since figuring out how the game prints text is actually
     more difficult than getting it to spawn actors, this
     app uses the rupee indicator and the arrows & bombs for
     the first and second byte of the actor variable respectively.
     
     Nothing hexadecimal, so that's kind of a pain in the balls.
     This is probably the most important issue.
     
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    IDEAS:
     Clone z-targetted actors, maybe?
*/
#include <n64.h>
#include <z64as.h>

/* Preserved variables */
STATUS  status;             /* Application run information  */
Z64AID  current_id;         /* Current actor number         */
Z64VAR  current_var;        /* Current actor variable       */

/* Main routine */
void _start ( void )
{    
    /* Actor spawning routine */
    ASPAWN spawn_actor = (void*)Z64AS_ROUTINE_ADDR;
    
    /* Game variables */
    Controller  *input  = (void*)Z64NC_CNST_BUTTON;
    CoordU      *link   = (void*)Z64NC_CNST_LINKXYZ;
    RUPEES      *rupees = (void*)Z64NC_CNST_RUPEES;
    BOMBS       *bombs  = (void*)Z64NC_CNST_BOMBS;
    ARROWS      *arrows = (void*)Z64NC_CNST_ARROWS;
    int          inc;
    
    /* ~~~~ Handle counter changing input ~~~~ */
    
    /* Increment amount */
    if( input->pad & BUTTON_Z )
        inc = 10; else inc = 1;
    
    /* d-pad up: increase actor id */
    if( (input->pad & BUTTON_D_UP) && current_id < Z64AS_ACTOR_LIST_MAX )
        current_id += inc;
    
    /* d-pad down: decrease actor id */
    if( (input->pad & BUTTON_D_DOWN) && current_id >= inc )
        current_id -= inc;
    
    /* d-pad right: increase actor var */
    if( input->pad & BUTTON_D_RIGHT )
        current_var += inc;
    
    /* d-pad left: decrease actor var */
    if( (input->pad & BUTTON_D_LEFT) && current_var >= inc )
        current_var -= inc;
    
    /* ~~~~ Store the values so the game can display them ~~~~ */
    
    /* Actor ID */
    *rupees = current_id;
    
    /* Actor variable */
    PUT_B( Z64NC_CNST_ARROWS, current_var >> 8  );  /* Part 1 */
    PUT_B( Z64NC_CNST_BOMBS, current_var & 0xFF );  /* Part 2 */
    
    /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
    
    /* Check invoke counter - don't want too many actors! */
    if( status.flags & Z64AS_F_WAITING )
        if( status.i )
        {
            status.i--; /* Decrement */ 
            return;
        }
        else
            status.flags &= ~(Z64AS_F_WAITING);
    
    /* On L + R + Z, spawn chosen actor */
    if( CHECK_BUTTONS( &input->pad, BUTTON_L | BUTTON_R | BUTTON_Z) )
    {
        /* Call OoT's actor spawning function */
        spawn_actor
        (
            Z64AS_CONST_SPAWN_ARG1,
            Z64AS_CONST_SPAWN_ARG2,
            current_id,
            link->x.u,
            link->y.u,
            link->z.u,
            0,
            0,
            0,
            current_var
        );
        
        /* Wait a bit for spawning the next actor */
        status.flags |= Z64AS_F_WAITING;
        status.i = 4;
    }
}


