/**********************************
* Zelda 64 Display List Processor *
**********************************/
#include <z64mv.h>
#include <glib.h>

/*
** Process display list
*/
void z64mv_dlist_process ( Z64MV *h, u8 *data, u32 size, u32 start )
{
	RDPINSTR instruction;
	u32 counter;
	
	u32 vtx_size;
	u32 vtx_number;
	VERTEX * vtx = 0;
	
	/* Step through each command */
	for( counter = start; counter < size; counter += 8 )
	{
		/* Load next instruction */
		instruction = U64( &data[counter] );
		
		/* Handle */
		switch( ZGFX_INSTR( instruction ) )
		{
			/* End of display list */
			case ZGFX_ENDDL:
			 /* Z64MV_DEBUG( "End of dlist 0x%06X reached.", start ); */
			 return;
			break;
			
			/* New display list */
			case ZGFX_DL:
				
			 /* Check bank */
			 if( ZGFX_BANK( instruction ) != 0x06 && ZGFX_BANK( instruction ) != 0x03 )
			 {
				 static int shown;
				 
				 if( !shown++ )
					 Z64MV_DEBUG( "Cannot load dlist from bank 0x%02X.", ZGFX_BANK( instruction ) );
				 goto endloop;
			 }
			 
			 /* Everything looks ok, process */
			 Z64MV_DEBUG( "Recursing into dlist 0x%06X.", ZGFX_ADDR( instruction ) ); 
			 z64mv_dlist_process( h, data, size, ZGFX_ADDR( instruction ) );
			break;

			/* Load vertexes */
			case ZGFX_VTX:
				
			 /* Set the pointer */
			 vtx = (VERTEX*)(data + ZGFX_ADDR( instruction ));
			 
			 /* Set attributes */
			 vtx_size   = ZGFX_VTX_VERTS( instruction ) * 16;
			 vtx_number = ZGFX_VTX_VERTS( instruction );
			break;
			
			/* Draw a single triangle */
			case ZGFX_TRI1:
				
			 /* Check that vertexes have actuallt been loaded */
			 if( !vtx )
				 continue;
			 
			 /* Call */
			 z64mv_dlist_tri1( vtx, ZGFX_TRI1_V1( instruction ),
			 ZGFX_TRI1_V2( instruction ),  ZGFX_TRI1_V3( instruction ) );
			break;
			
			/* Draw two triangles */
			case ZGFX_TRI2:
			
			 /* Check that vertexes have actually been loaded */
			 if( !vtx )
				 continue;
			 
			 /* First triangle */
			 z64mv_dlist_tri1( vtx, ZGFX_TRI2_T1V1( instruction ),
			 ZGFX_TRI2_T1V2( instruction ),  ZGFX_TRI2_T1V3( instruction ) );
			 
			 /* Second triangle */
			 z64mv_dlist_tri1( vtx, ZGFX_TRI2_T2V1( instruction ),
			 ZGFX_TRI2_T2V2( instruction ),  ZGFX_TRI2_T2V3( instruction ) );
			break;
			
			/* Unknown instruction 
			default:
			 printf("Unknown instruction: [%02llX]%014llX\n", 
			 ZGFX_INSTR( instruction ), instruction & 0xFFFFFFFFFFFFFF ); */
		}
		
		endloop:;
	}
}

/*
** Draw a single triangle
*/
void z64mv_dlist_tri1 ( VERTEX * vtx, u8 i1, u8 i2, u8 i3 )
{
	/*
		SWAP16 used due to endianness issues
	*/	
	
	glBegin( GL_TRIANGLES );
	
	 glColor4b(  vtx[i1].r, vtx[i1].g, vtx[i1].b, vtx[i1].a );
	 glVertex3s( SWAP16(vtx[i1].x), SWAP16(vtx[i1].y), SWAP16(vtx[i1].z) );
	 
	 glColor4b(  vtx[i2].r, vtx[i2].g, vtx[i2].b, vtx[i2].a );
	 glVertex3s( SWAP16(vtx[i2].x), SWAP16(vtx[i2].y), SWAP16(vtx[i2].z) );
	 
	 glColor4b(  vtx[i3].r, vtx[i3].g, vtx[i3].b, vtx[i3].a );
	 glVertex3s( SWAP16(vtx[i3].x), SWAP16(vtx[i3].y), SWAP16(vtx[i3].z) );
	
	glEnd();
}

/*
** Find entry point
*/
u64 z64mv_ep_find ( u8 *data, u32 size )
{
	u32 i;
	
	for( i = 0; i < size; i += 8 )
		if( ZGFX_INSTR(U64( &data[i] )) == 0xE7 && !(U64( &data[i] ) << 8) )
			return i;
	
	return -1;
}

/*
** Preprocess a model
*/

struct ModelTree
{
	GList * dlists;
};

struct DisplayList
{
	GList * triangles;
	GList * vertices;
};

struct Vertex
{
	/* Coordinates */
	short x, y, z;
	
	/* Color information */
	u8	r, g, b, a;
	
	/* Word-align */
	int _;
};

struct Triangle
{
	struct Vertex * parts[3];
};

/*
** Preload a display list
*/
GList * z64mv_dlist_preload ( GList * dlists, u8 * data, u32 size, u32 entry_point )
{
	u32 		counter;
	u32			vtx_size;
	u32			vtx_number;
	RDPINSTR	instruction;
	VERTEX    * vtx = 0;
	
	/* Create new model tree */
	struct DisplayList * ret = CALLOC( sizeof(struct DisplayList) );
	dlists = g_list_append( dlists, ret );
	
	/* Step through each command */
	for( counter = entry_point; counter < size; counter += 8 )
	{
		/* Load next instruction */
		instruction = U64( &data[counter] );
		
		/* Handle */
		switch( ZGFX_INSTR( instruction ) )
		{
			/* End of display list */
			case ZGFX_ENDDL:  return;  break;
			
			/* New display list */
			case ZGFX_DL:
				
			 /* Check bank */
			 if( ZGFX_BANK( instruction ) != 0x06 && ZGFX_BANK( instruction ) != 0x03 )
			 {
				 static int shown;
				 
				 if( !shown++ )
					 Z64MV_DEBUG( "Cannot load dlist from bank 0x%02X.", ZGFX_BANK( instruction ) );
				 goto endloop;
			 }
			 
			 /* Everything looks ok, process */
			 Z64MV_DEBUG( "Recursing into dlist 0x%06X.", ZGFX_ADDR( instruction ) );
			 dlists = z64mv_dlist_preload( dlists, data, size, ZGFX_ADDR( instruction ) );
			break;

			/* Load vertexes */
			case ZGFX_VTX:
				
			 /* Set the pointer */
			 vtx = (VERTEX*)(data + ZGFX_ADDR( instruction ));
			 
			 /* Set attributes */
			 vtx_size   = ZGFX_VTX_VERTS( instruction ) * 16;
			 vtx_number = ZGFX_VTX_VERTS( instruction );
			 
			{
				struct Vertex * v = MALLOC( sizeof(struct Vertex) * vtx_number );
			}
			break;
			
			/* Draw a single triangle */
			case ZGFX_TRI1:
				
			 /* Check that vertexes have actuallt been loaded */
			 if( !vtx )
				 continue;
			 
			 /* Call */
			 z64mv_dlist_tri1( vtx, ZGFX_TRI1_V1( instruction ),
			 ZGFX_TRI1_V2( instruction ),  ZGFX_TRI1_V3( instruction ) );
			break;
			
			/* Draw two triangles */
			case ZGFX_TRI2:
			
			 /* Check that vertexes have actually been loaded */
			 if( !vtx )
				 continue;
			 
			 /* First triangle */
			 z64mv_dlist_tri1( vtx, ZGFX_TRI2_T1V1( instruction ),
			 ZGFX_TRI2_T1V2( instruction ),  ZGFX_TRI2_T1V3( instruction ) );
			 
			 /* Second triangle */
			 z64mv_dlist_tri1( vtx, ZGFX_TRI2_T2V1( instruction ),
			 ZGFX_TRI2_T2V2( instruction ),  ZGFX_TRI2_T2V3( instruction ) );
			break;
		}
		
		endloop:;
	}
}

/* Load vertices */
GList * z64mv_load_vertices ( u8 * data, u32 length, u32 vaddr, u32 vcount )
{
	
}
