/******************************
* Gex 64 Function Definitions *
******************************/
#ifndef __GEX64_H__
#define __GEX64_H__

#include <stdint.h>

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
    
    /* Unknown pointers */
    void * _2;
    void * _3;
};

typedef int (*__func_gexDrawText)
(
    char * text,
    int x,
    int y,
    int unkn
);

typedef int (*__func_gexDrawBox)
(
    struct GexBox * b
);

#define CALL_GEX_FUNC(func)     ((__func_##func)FUNC_PTR(__func_addr_##func))
#define FUNC_PTR(ptr)           ((void*)(((u32)ptr)&0x807FFFFF))

/* Functions */
#define gexDrawText(text, x, y, u)  (CALL_GEX_FUNC(gexDrawText)(text, x, y, u))
#define gexDrawBox(box)             (CALL_GEX_FUNC(gexDrawBox)(box))

/* Pseudo, custom functions */
#define gexMakeBox(name,w,h,x,y,r,g,b,a)        \
struct GexBox name = {                          \
    (x) - (320/2), (y) - (240/2),               \
    (x) - (320/2) + (w), (y) - (240/2) + (h),   \
    (r), (g), (b), (a),                         \
    1, 4,                                       \
    __type_value_GexBox_ptr_1,                  \
    __type_value_GexBox_ptr_2                   \
}

/* Function addresses */
#define __func_addr_gexDrawText     0x80030DD8
#define __func_addr_gexDrawBox      0x8003F334
#define __func_addr_gexDrawText3D   0x800387F0

/* Constants - related to arguments */
#define __type_value_GexBox_ptr_1   (void*)0x80078110
#define __type_value_GexBox_ptr_2   (void*)0x8025E8F4

#endif /* __GEX64_H__ */
