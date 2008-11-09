/***********************
* Zelda 64 Scene Table *
***********************/
#ifndef __LZ64_SCENE_TABLE
#define __LZ64_SCENE_TABLE

typedef enum { GameOOT, GameMM } SceneTableGame;

struct SceneEntryOOT
{
	guint32		addr_start;		/* Address of scene       */
	guint32		addr_end;		/* Address of scene		  */
	guint32		image_start;	/* Address of level image */
	guint32		image_end;		/* Address of level image */
	guint32		unkn1;
};

struct SceneEntryMM
{
	guint32		addr_start;		/* Address of scene       */
	guint32		addr_end;		/* Address of scene		  */
	guint32		unkn1;
	guint32		unkn2;
};

typedef struct
{
	guint32			start;	/* Address, relative to code file */
	guint32			end;	/* Address, relative to code file */
	guint32			count;	/* Amount of scenes listed        */   
	SceneTableGame	game;	/* What game (OoT or MM) 		  */
	
	void * entries;                     
}
Z64ST;

#endif /* __LZ64_SCENE_TABLE */
