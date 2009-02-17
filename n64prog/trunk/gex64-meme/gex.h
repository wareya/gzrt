/******************************
* Gex 64 Function Definitions *
******************************/
#ifndef __GEX64_H__
#define __GEX64_H__

/* C stuff */
#ifndef __LANGUAGE_ASSEMBLY__

#include <stdint.h>

typedef uint64_t    u64;
typedef uint32_t    u32;
typedef uint16_t    u16;
typedef int16_t     s16;
typedef uint8_t     u8;

/* Data types */
struct GexBox
{
    /* Coordinates */
    s16 tx, ty, bx, by;
    
    /* Colors */
    u8 r, g, b, a;
    
    /* Other information */
    u16 boxtype;
    u16 _1; /* 0004 */
};

struct GexText
{
	/* Coordinates */
	u16 x, y;
	
	/* Text itself */
	char * text;
};
	

typedef int (*__func_gexDrawText)
(
    char *,  /* String to be printed */
    int,     /* X location of text   */
    int,     /* Y location of text   */
    int      /* Unknown              */
);

typedef int (*__func_gexDrawBox)
(
    struct GexBox *
);

typedef int (*__func_gexSprintf)
(
    char *, /* Target        */
    char *, /* Format string */
    ...     /* Arguments     */
);

typedef int (*__func_gexDrawTextBox)
(
    int,	/* Horizontal offset (from 0; signed) */
	int,	/* Vertical offset (from 0; signed)   */
	int,	/* Width							  */
	int,	/* Height 							  */
	int,	/* Number of objects                  */
	void *  /* Pointer to objects (GexText)		  */
);

typedef int (*__func_osGetCount)( void );



/* Macros for neatly calling Gex functions */
#define CALL_GEX_FUNC(func)    		((__func_##func)FUNC_PTR(__func_addr_##func))
#define FUNC_PTR(ptr)           	((void*)(((u32)ptr)&0x807FFFFF))

/* Functions */
#define gexDrawText(text, x, y, u)  (CALL_GEX_FUNC(gexDrawText)(text, x, y, u))
#define gexDrawBox(box)             (CALL_GEX_FUNC(gexDrawBox)(box))
#define osGetCount()				(CALL_GEX_FUNC(osGetCount)())
#define gexDrawTextBox(x,y,w,h,n,s)	(CALL_GEX_FUNC(gexDrawTextBox)(x,y,w,h,n,s))
/* #define gexSprintf(dest, fmt, ...)  (CALL_GEX_FUNC(gexSprintf)(dest, fmt, ##__VA_ARGS__)) */
extern int gexSprintf ( char *, char *, ... );

/* Pseudo, custom functions */
#define gexMakeBox(name,w,h,x,y,r,g,b,a)        \
struct GexBox name = {                          \
    (x) - (320/2), (y) - (240/2),               \
    (x) - (320/2) + (w), (y) - (240/2) + (h),   \
    (r), (g), (b), (a),                         \
    0, 0                                        \
}

/* Global functions */
extern void gexMain ( void );
extern void hookFunc ( void );
/* Address of controller input */
#define GEX_CONTROLLER		(void*)0x800E5DB0

#endif /* !__LANGUAGE_ASSEMBLY__ */

/* Function addresses */
#define __func_addr_gexDrawText     0x80030DD8
#define __func_addr_gexDrawBox      0x8003F334
#define __func_addr_gexSprintf      0x8005C290
#define __func_addr_gexDrawText3D   0x800387F0
#define __func_addr_osGetCount		0x80066340
#define __func_addr_gexDrawTextBox	0x8003F6CC

/* Box functions */
#define __func_addr_gexBoxSetLoc    0x8003F6CC


/* Constants - related to arguments */
#define __type_value_GexBox_ptr_1   (void*)0x80078110
#define __type_value_GexBox_ptr_2   (void*)0x8025E8F4




#endif /* __GEX64_H__ */
