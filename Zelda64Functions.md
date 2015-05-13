# Preface #

This article contains OoT functions with C definitions. These are for OoT 1.0 US.

# Functions #

```
/* Function addresses */
#define Z64_FUNC_YAZ0DEC      (void*)0x80008490
#define Z64_FUNC_ACTOR_SPAWN  (void*)0x80025110

/* Function declarations */
typedef int (*z64_yaz0dec)
(
    void * src, /* Source Yaz0 block */
    void * dst  /* Memory target     */
);
typedef int (*z64_actor_spawn)
(
    u32,	 /* Not sure - constant pointer	*/
    u32,	 /* Not sure - constant pointer	*/
    int actor,	 /* Actor number		*/
    u32 fx, 	 /* Spawn X position (float)	*/
    u32 fy, 	 /* Spawn Y position (float)	*/
    u32 fz,	 /* Spawn Z position (float)	*/
    u32 rx, 	 /* X Rotation			*/
    u32 ry, 	 /* Y Rotation			*/
    u32 rz,	 /* Z Rotation			*/
    int variable /* Actor variable		*/
);
/* Set text RGB	*/
typedef int ( *z64_set_text_rgb )
(
    u32,	/* Not sure - Stack Pointer + ?	*/
    u16 red,	/* red 				*/
    u16 green,	/* green			*/
    u16 blue,	/* blue				*/
    u16 alpha	/* alpha			*/
);

/* Set text coordinites	*/
typedef int ( *z64_set_text_xy )
(
    u32,	/* Not sure - Stack Pointer + ?	*/
    u16 xpos,	/* Text X position (X Pos >> 3)	*/
    u16 ypos	/* Text Y position (Y Pos >> 3)	*/
);

/* Draw text	*/
typedef int ( *z64_draw_text )
(
    u32,	/* Not sure - Stack Pointer + ?	*/
    char * text,/* Text				*/
    u32 texvar	/* Variables for format string	*/
);
```