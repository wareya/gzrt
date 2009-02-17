/*
    Gex 64: Memory modifier
    ~~~~~~~~~~~~~~~~~~~~~~~
    By: ZZT32 [mbr@64.vg]
    
    Controls:
     
     Take note: the "L" button is the modifier for doing anything at all with
     the memory editor.
     
     Z:       Set editing mode (see enum)
     R:       Hold to double whatever increment/decremet (incl. addr)
     C-Up:    Increase value by 1
     C-Down:  Decrease value by 1
     C-Right: Increase value by 16
     C-Left:  Decrease value by 16
     D-Up:    Increase addr by 1 element
     D-Down:  Decrease addr by 1 element
     D-Right: Increase addr by 16 bytes
     D-Left:  Decrease addr by 16 bytes
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gex.h"
#include "n64.h"
#include "mips.h"
#include "time.h"


/* ----------------------------------------------
   Constants
   ---------------------------------------------- */

#define ADDRESS_MIN     0x80000000
#define ADDRESS_MAX     0x807FFFF0


/* ----------------------------------------------
   Local variables
   ---------------------------------------------- */

/* Current multiplier */
static int multiplier = 1;

/* Current address */
static void * volatile address = (void*)ADDRESS_MIN;

/* Current value modifier */
static int val_modifier;

/* Current address modifier */
static int addr_modifier;


/* ----------------------------------------------
   Local function declarations
   ---------------------------------------------- */

static void   getCurrentData    ( void );
static void   setCurrentData    ( void );
static void   printCurrentData  ( void );
static void   incDataType       ( void*);
static char * strCurrentData    ( void );
static void   processController ( void );
static void   sanitizeAddress   ( void );


/* ----------------------------------------------
   Data management
   ---------------------------------------------- */
   
/* Function declarations */
static void getByte       ( void *      );
static void getHalfword   ( void *      );
static void getWord       ( void *      );
static void getSingle     ( void *      );
static void getDouble     ( void *      );
static void printByte     ( void *      );
static void printHalfword ( void *      );
static void printWord     ( void *      );
static void printSingle   ( void *      );
static void printDouble   ( void *      );
static void modByte       ( int, void * );
static void modHalfword   ( int, void * );
static void modWord       ( int, void * );
static void modSingle     ( int, void * );
static void modDouble     ( int, void * );

/* Supported data types specification */
struct DataTypes
{
    int size;               /* Size of type - bytes         */
    void (*Get)(void*);     /* Get the value function       */
    void (*Print)(void*);   /* sprint value function        */
    void (*Mod)(int,void*); /* Modify the value */
    void * arg;             /* Argument to pass to function */
    char * name;            /* Human readable name of type  */
};

static struct DataTypes
types[] =
{
    { 1, getByte,     printByte,     modByte,     NULL, "BYTE"     },
    { 2, getHalfword, printHalfword, modHalfword, NULL, "HALFWORD" },
    { 4, getWord,     printWord,     modWord,     NULL, "WORD"     },
    { 4, getSingle,   printSingle,   modSingle,   NULL, "SINGLE"   },
    { 8, getDouble,   printDouble,   modDouble,   NULL, "DOUBLE"   }
};

/* Current type */
static struct DataTypes * data_current = &types[0];

/* Data buffer */
static u8 data_value[16];
static char data_representation[64];

/*
 *  Handlers - get
 */

static void getByte ( void * arg )
{
    memcpy( data_value, address, 1 );
}
static void getHalfword ( void * arg )
{
    memcpy( data_value, address, 2 );
}
static void getWord ( void * arg )
{
    memcpy( data_value, address, 4 );
}
static void getSingle ( void * arg )
{
    getWord( arg );
}
static void getDouble ( void * arg )
{
    memcpy( data_value, address, 8 );
}

/*
 *  Handlers - print
 */

static void printByte ( void * arg )
{
    gexSprintf( data_representation, "%02X", *(u8*)address );
}
static void printHalfword ( void * arg )
{
    gexSprintf( data_representation, "%04X", *(u16*)address );
}
static void printWord ( void * arg )
{
    gexSprintf( data_representation, "%08X", *(u32*)address );
}
static void printSingle ( void * arg )
{
    gexSprintf( data_representation, "%.2f", *(float*)address );
}
static void printDouble ( void * arg )
{
    gexSprintf( data_representation, "%.2f", *(double*)address );
}

/*
 *  Handlers - modify
 */

static void modByte ( int v, void * a )
{
    *(u8*)address = v;
}

static void modHalfword ( int v, void * a )
{
    *(u16*)address = v;
}

static void modWord ( int v, void * a )
{
    *(u32*)address = v;
}

static void modSingle ( int v, void * a )
{
    *(float*)address = (float)v;
}

static void modDouble ( int v, void * a )
{
    *(double*)address = (double)v;
}

/*
 *  Wrappers
 */

/* Write prepared data */
static void setCurrentData ( void )
{
    /* Call handler */
    data_current->Mod( val_modifier * multiplier, data_current->arg );
}

/* Get the current data */
static void getCurrentData ( void )
{
    /* Call handler */
    data_current->Get( data_current->arg );
}

/* Print current data */
static void printCurrentData ( void )
{
    /* Call handler */
    data_current->Print( data_current->arg );
}

/* Return address to buffer */
static char * strCurrentData  ( void )
{
    return data_representation;
}

/* Change the data type */
static void incDataType ( void * J )
{
    static int cur;
    
    if( cur + 1 == sizeof(types) / sizeof(struct DataTypes) )
    {
        cur = 0;
    }
    else
    {
        cur++;
    }
    
    data_current = &types[cur];
}


/* ----------------------------------------------
   Controller input
   ---------------------------------------------- */

/* Function declarations */
static void doubleMod       ( void * );
static void incAddrByEntity ( void * );
static void incAddr         ( void * );
static void incVal          ( void * );

/* Flags */
#define INPUT_ABSOLUTE      (1 << 0)
   
/* Controller combinations and their handlers */
struct InputHandler
{
    int mask;               /* Button mask               */
    int flags;              /* Flags                     */
    void (*handler)(void*); /* Function to call on match */
    void * arg;             /* Pass to handler           */
};

static struct InputHandler
inputs[] =
{
    { BUTTON_R,       0, doubleMod,       NULL       },
    { BUTTON_Z,       0, incDataType,     NULL       },
    { BUTTON_C_UP,    0, incAddrByEntity, (void*)1   },
    { BUTTON_C_DOWN,  0, incAddrByEntity, (void*)-1  },
    { BUTTON_C_RIGHT, 0, incAddr,         (void*)16  },
    { BUTTON_C_LEFT,  0, incAddr,         (void*)-16 },
    { BUTTON_D_UP,    0, incVal,          (void*)1   },
    { BUTTON_D_DOWN,  0, incVal,          (void*)-1  },
    { BUTTON_D_RIGHT, 0, incVal,          (void*)10  },
    { BUTTON_D_LEFT,  0, incVal,          (void*)-10 }
};

/* Step through the controller input handlers */
void processController ( void )
{
    int test;
    int i, l = sizeof(inputs) / sizeof(struct InputHandler);
    u16 * controller = GEX_CONTROLLER;
    
    for( i = 0; i < l; i++ )
    {
        /* Lenient? */
        if( !(inputs[i].flags & INPUT_ABSOLUTE) )
        {
            test = (*controller & inputs[i].mask);
        }
        else
        {
            test = (*controller == inputs[i].mask);
        }
        
        /* Check result */
        if( test )
        {
            /* Matched, call function */
            inputs[i].handler( inputs[i].arg );
        }
    }
}

/*
 *  Input handlers
 */

static void doubleMod ( void * arg )
{
    multiplier = 2;
}

static void incAddrByEntity ( void * arg )
{
    int v = (int)arg;
    
    addr_modifier = v * data_current->size;
}

static void incAddr ( void * arg )
{
    int v = (int)arg;
    
    addr_modifier = v;
}

static void incVal ( void * arg )
{
    int v = (int)arg;
    
    val_modifier = v;
}


/* ----------------------------------------------
   Local function definitions
   ---------------------------------------------- */

/* Sanitize the working address */
static void sanitizeAddress ( void )
{
    /* Align the address */
    address -= (u32)address % data_current->size;;
    
    /* Check range */
    if( !((u32)address >= ADDRESS_MIN && (u32)address < ADDRESS_MAX) )
        address = (void*)ADDRESS_MIN;
}


/* ----------------------------------------------
   Entry point
   ---------------------------------------------- */

void n64start ( void )
{
    /* Set up hook */
    *((u32*)0xA003BDB4) = JAL(hookFunc);
    
    /* No GP overwriting */
    *((u32*)0xA005DC10) = 0;
}


/* ----------------------------------------------
   Main
   ---------------------------------------------- */

void gexMain ( void )
{
    static float last, fps;
    static char buffer[128];
    
    /* Reset variables */
    multiplier    = 1;
    val_modifier  = 0;
    addr_modifier = 0;
    
    /* Modifier on? */
    if( CHECK_BUTTONS_LENIENT(GEX_CONTROLLER, BUTTON_L) )
    {
        /* Process controller input */
        processController();
    }
    
    /* Update address */
    address += multiplier * addr_modifier;
    
    /* Sanitize the address */
    sanitizeAddress();
    
    /* Read the current value */
    getCurrentData();
    
    /* Set the new data 
    setCurrentData();*/
    
    /* Print it */
    printCurrentData();
    
    /* Prepare text */
    gexSprintf( buffer, "%08X: %s", (u32)address, strCurrentData() );
    
    /* Draw it */
    gexDrawText( buffer, 0, 12, 1 );
    
    /* FPS */
    if( !last )
    {
        fps = 0.0f;
    }
    else
    {
        fps = 1.0f / (getTimeNow() - last);
    }
    
    /* Prepare */
    gexSprintf( buffer, "FPS: %2.2f", fps );
    
    /* Draw */
    gexDrawText( buffer, 0, 230, 1 );
    
    /* Set last */
    last = getTimeNow();
}
