#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <GL/glew.h>

#define BYTE unsigned char
#define DWORD unsigned int
#define FLOAT float

#include "dlist.h"

#define DebugDL

static unsigned long Flip32 (unsigned long ToFlip) {
    return ((ToFlip&0xFF000000)>>24)|((ToFlip&0x00FF0000)>>8)|
        ((ToFlip&0x0000FF00)<<8)|((ToFlip&0x000000FF)<<24);
}


unsigned char *RDRAM;
int ProcessDisplayListSW(DWORD dl);

FILE *logfile=NULL;

void Log_Message (char * Message, ...) {
	DWORD dwWritten;
	char Msg[2048];
	va_list ap;

	va_start( ap, Message );
	vsnprintf( Msg, 2048, Message, ap );
	va_end( ap );
	
	strcat(Msg,"\r\n");

    if(logfile!=NULL)
        fwrite(Msg, strlen(Msg), 1, logfile);
}

void Start_Log (void) {
    if(logfile==NULL)
        logfile=fopen("dlist.log","w");    
}

void Stop_Log (void) {
    if(logfile!=NULL)
        fclose(logfile);
    logfile=NULL;
}

// THANKS TO GUYS BEHIND TR!
typedef struct
{
   DWORD id1;
   DWORD id2;
   DWORD id3;
} CachedTexture_t;

CachedTexture_t CachedTextures[1024];
DWORD nCachedTextures;
DWORD currenttexture=0;


#define IMMINC \
   dl+=8;      \
   break;

DWORD SEGMENT[16];
DWORD ALPHADITHER;
DWORD RGBDITHER;
DWORD COMBKEY;
DWORD TEXTCONV;
DWORD TEXTFILT;
DWORD TEXTLUT;
DWORD TEXTLOD;
DWORD TEXTDETAIL;
DWORD TEXTPERSP;
DWORD CYCLETYPE;
DWORD COLORDITHER;
DWORD PIPELINE;
DWORD ALPHACOMPARE;
DWORD ZSRCSEL;
DWORD RENDERMODE;
DWORD BLENDER;
// **********************************************************************
//                       --- COMBINE STUFF ---
// **********************************************************************

                  DWORD COMBINE0,COMBINE1;
                  DWORD cA0,cB0,cC0,cD0,aA0,aB0,aC0,aD0;
                  DWORD cA1,cB1,cC1,cD1,aA1,aB1,aC1,aD1;
                  char *ColorA[] = { "cCOMBINED","cTEXEL0",
                                     "cTEXEL1","cPRIM",
                                     "cSHADE","cENV",
                                     "1.0","cNOISE",
                                     "0.0","0.0",
                                     "0.0","0.0",
                                     "0.0","0.0",
                                     "0.0","0.0"};
                  char *ColorB[] = { "cCOMBINED","cTEXEL0",
                                     "cTEXEL1","cPRIM",
                                     "cSHADE","cENV",
                                     "cKEYCENTER","CONVK4",
                                     "0.0","0.0",
                                     "0.0","0.0",
                                     "0.0","0.0",
                                     "0.0","0.0"};
                  char *ColorC[] = { "cCOMBINED","cTEXEL0",
                                     "cTEXEL1","cPRIM",
                                     "cSHADE","cENV",
                                     "cKEYSCALE","aCOMBINED",
                                     "aTEXEL0","aTEXEL1",
                                     "aPRIM","aSHADE",
                                     "aENV","LODFRAC",
                                     "PRIMLODFRAC","CONVK5",
                                     "0.0","0.0",
                                     "0.0","0.0",
                                     "0.0","0.0",
                                     "0.0","0.0",
                                     "0.0","0.0",
                                     "0.0","0.0",
                                     "0.0","0.0",
                                     "0.0","0.0"};
                  char *ColorD[] = { "cCOMBINED","cTEXEL0",
                                     "cTEXEL1","cPRIM",
                                     "cSHADE","cENV",
                                     "1.0","0.0"};
                  char *AlphaA[] = { "aCOMBINED","aTEXEL0",
                                     "aTEXEL1","aPRIM",
                                     "aSHADE","aENV",
                                     "1.0","0.0"};
                  char *AlphaB[] = { "aCOMBINED","aTEXEL0",
                                     "aTEXEL1","aPRIM",
                                     "aSHADE","aENV",
                                     "1.0","0.0"};
                  char *AlphaC[] = { "LODFRAC","aTEXEL0",
                                     "aTEXEL1","aPRIM",
                                     "aSHADE","aENV",
                                     "PRIMLODFRAC","0.0"};
                  char *AlphaD[] = { "aCOMBINED","aTEXEL0",
                                     "aTEXEL1","aPRIM",
                                     "aSHADE","aENV",
                                     "1.0","0.0"};



DWORD FILLCOLOR;
DWORD FOG_R,FOG_G,FOG_B,FOG_A;
DWORD BLEND_R,BLEND_G,BLEND_B,BLEND_A;
DWORD PRIM_M,PRIM_L,PRIM_R,PRIM_G,PRIM_B,PRIM_A;
DWORD ENV_R,ENV_G,ENV_B,ENV_A;

DWORD USE_ZBUFFER=0;
DWORD USE_SHADE=0;
DWORD USE_LIGHT=0;

DWORD TEXTURE_SSCALE,TEXTURE_TSCALE,TEXTURE_LEVEL,TEXTURE_TILE,TEXTURE_ENABLE;

DWORD CIMG_Format,CIMG_Size,CIMG_Width,CIMG_CFB,CIMG_SWAP;

DWORD SCISSOR_LX,SCISSOR_LY,SCISSOR_RX,SCISSOR_RY,SCISSOR_Mode;

DWORD FILLRECT_LX,FILLRECT_LY,FILLRECT_RX,FILLRECT_RY;

DWORD TILE_Format,TILE_Size,TILE_Width,TILE_CFB;

struct RDPTile
{
   DWORD Format;
   DWORD TexelSize;
   DWORD LineSize;
   DWORD TMemAddress;
   DWORD Palette;
   DWORD CMT;
   DWORD CMS;
   DWORD MaskT;
   DWORD MaskS;
   DWORD ShiftT; //signed?
   DWORD ShiftS; //signed?
   DWORD ClipS1;
   DWORD ClipT1;
   DWORD ClipS2;
   DWORD ClipT2;
   DWORD ULS;
   DWORD ULT;
   DWORD LRS;
   DWORD LRT;
   DWORD TILEULS;
   DWORD TILEULT;
   DWORD TILELRS;
   DWORD TILELRT;
   DWORD SizeX;
   DWORD SizeY;
   DWORD GLX;
   DWORD GLY;
   DWORD SetTileLo;
   DWORD SetTileHi;
}RDP_Tiles[8];

DWORD RDP_TN;

DWORD TLUT_DRAM,TLUT_TMEM,TLUT_COUNT;

unsigned char TMEM[4096];

DWORD RECT_TILE;
FLOAT RECT_X1,RECT_Y1,RECT_X2,RECT_Y2;
DWORD RECT_S,RECT_T,RECT_DSDX,RECT_DTDY;
FLOAT RECT_U1,RECT_V1,RECT_U2,RECT_V2;
DWORD RECT_XSIZE,RECT_YSIZE;

DWORD LOADBLOCKSIZE,LOADBLOCKLINE;

DWORD MOVEMEMSRC,MOVEMEMSIZE,MOVEMEMDST;

DWORD ZIMG;

DWORD MATRIXSRC,MATRIXSIZE,MATRIXFLAG;

DWORD VTXSRC,VTXSIZE,VTXNUMBER,VTXSTART;


DWORD TRI1FLAG,TRI1V0,TRI1V1,TRI1V2;




DWORD LoadedCFB;

void LoadTile(int Tile)
{
   int TextureSizeX;
   int TextureSizeY;
   int DRAMPitch;
   int TMEMPitch;
   int /*S,*/T;
   unsigned char * SrcPtr;
   unsigned char * DstPtr;
   int i;
   DWORD DstPtr2;
   DWORD SrcPtr2;

   switch(TILE_Size&0x3)
   {
      case 0: // 4bit
         DRAMPitch=(TILE_Width+1)>>1; 
         break;
      case 1: // 8bit
         DRAMPitch=TILE_Width;
         break;
      case 2: // 16bit
         DRAMPitch=TILE_Width*2;
         break;
      case 3: // 32bit
         DRAMPitch=TILE_Width*4;
         break;
   }      

   TextureSizeX=RDP_Tiles[Tile].LRS-RDP_Tiles[Tile].ULS+10;
   TextureSizeY=RDP_Tiles[Tile].LRT-RDP_Tiles[Tile].ULT+1;
   TMEMPitch=RDP_Tiles[Tile].LineSize*8;
  
   DstPtr=TMEM; //+RDP_Tiles[Tile].TMemAddress*8;
   SrcPtr=RDRAM; //+(0x1FFFFFFF&TILE_CFB)+RDP_Tiles[Tile].ULS+RDP_Tiles[Tile].ULT*DRAMPitch;

   #ifdef DebugDL
   Log_Message("Loading tile : SizeX:%d SizeY:%d TMEMPitch:%d DRAMPitch:%d",TextureSizeX,TextureSizeY,TMEMPitch,DRAMPitch);
   #endif

   DstPtr2=0;
   SrcPtr2=0;
   for(T=0;T<TextureSizeY;T++)
   {
      for(i=0;i<DRAMPitch;i++)
      {
         DstPtr[RDP_Tiles[Tile].TMemAddress*8+i+DstPtr2]=SrcPtr[(i+SrcPtr2+(((0x1FFFFFFF&TILE_CFB)+RDP_Tiles[Tile].ULS+RDP_Tiles[Tile].ULT*DRAMPitch)))^3];
      }
//      memcpy(DstPtr,SrcPtr,DRAMPitch);
      DstPtr2+=TMEMPitch;
      SrcPtr2+=DRAMPitch;
   }
   LoadedCFB=0;
}

void LoadBlock(int Tile)
{
   int DRAMPitch;
   int TMEMPitch;
   int LINESIZE;
   int i;

   unsigned char * SrcPtr;
   unsigned char * DstPtr;

   DstPtr=TMEM; //RDP_Tiles[Tile].TMemAddress*8;
   SrcPtr=RDRAM; //+(0x1FFFFFFF&TILE_CFB);


   if(LOADBLOCKLINE!=0)
   {
      LINESIZE=2048/LOADBLOCKLINE;
   }
   else
   {
      LINESIZE=0;
   }

   #ifdef DebugDL
   Log_Message("Loading block : SIZE : %d  LINESIZE:%d",LOADBLOCKSIZE+1,LINESIZE);
   #endif
   
   for(i=0;i<(LOADBLOCKSIZE+1)*2;i++)
   {
      DstPtr[RDP_Tiles[Tile].TMemAddress*8+i]=SrcPtr[((0x1FFFFFFF&TILE_CFB)+i)]; // ^3
   }
//   memcpy(DstPtr,SrcPtr,(LOADBLOCKSIZE+1)*2);
}



int pow2(int i)
{
  int c;
  for(c=1; c <= i; c <<= 1);
  return( c );
}

unsigned int TextureMem[4096*4];

void LoadTileTMEM(int Tile)
{
   unsigned int i,j,TexColor,TexColor2;
   unsigned short *PalIndex;
   unsigned char * TexIndex;
   unsigned short * TexData;
   unsigned int * TexData2;
   unsigned int I;
   int TileScale;
   int TileY;
   int ValidPixels=0;
   int texturetouse;
   DWORD *TMEMDW;
   DWORD cur_id1,cur_id2,cur_id3;
   DWORD releasecount;

     if(RDP_Tiles[Tile].SizeX>4)
     {
      TileScale=pow2(RDP_Tiles[Tile].SizeX-1);
     }
     else
     {
      TileScale=pow2(RDP_Tiles[Tile].SizeX);
     }

     if(RDP_Tiles[Tile].SizeY>4)
     {
      TileY=pow2(RDP_Tiles[Tile].SizeY-1);
     }
     else
     {
      TileY=pow2(RDP_Tiles[Tile].SizeY);
     }

   RDP_Tiles[Tile].GLX=TileScale;
   RDP_Tiles[Tile].GLY=TileY;
  
   cur_id1=RDP_Tiles[Tile].SetTileLo;
   cur_id2=RDP_Tiles[Tile].SetTileHi;

   TMEMDW=(DWORD *) &TMEM[0];
   cur_id3=0;
   for(i=RDP_Tiles[Tile].TMemAddress*2;i<1024;i++)
   {      
      cur_id3+=TMEMDW[i];
   }

   for(i=0;i<1024;i++)
   {
      if( (CachedTextures[i].id1 == cur_id1) && (CachedTextures[i].id2 == cur_id2) && (CachedTextures[i].id3 == cur_id3) )
      {
         texturetouse=i;
         goto SetTexture;
      }
   }


   currenttexture+=1;
   currenttexture&=0x3FF;
   CachedTextures[currenttexture].id1=cur_id1;
   CachedTextures[currenttexture].id2=cur_id2;
   CachedTextures[currenttexture].id3=cur_id3;
//   Log_Message("Texture[%X] ID1:%X ID2:%X",currenttexture,CachedTextures[currenttexture].id1,CachedTextures[currenttexture].id2);


   #ifdef DebugDL
   Log_Message("Loading tile %d format:%d texelsize:%d",Tile,RDP_Tiles[Tile].Format,RDP_Tiles[Tile].TexelSize);
   #endif
   RDP_Tiles[Tile].GLX=TileScale;
   RDP_Tiles[Tile].GLY=TileY;

   #ifdef DebugDL
   Log_Message("LoadTILETMEM SizeX:%d SizeY:%d GLX:%d GLY:%d",RDP_Tiles[Tile].SizeX,RDP_Tiles[Tile].SizeY,RDP_Tiles[Tile].GLX,RDP_Tiles[Tile].GLY);
   #endif

   switch(RDP_Tiles[Tile].Format)
      {
      // RGBA
      case 0:  switch(RDP_Tiles[Tile].TexelSize)
                  {
                  case 2:
                     Log_Message("RGBA 16bit ");
                     TexData=(unsigned short *)(TMEM+RDP_Tiles[Tile].TMemAddress*8);
                     for(j=0;j<RDP_Tiles[Tile].SizeY;j++)
                     {
                        for(i=0;i<RDP_Tiles[Tile].SizeX;i++)
                        {
                           TexColor=TexData[0];
                           TexColor=((TexColor&0xFF)<<8)+((TexColor&0xFF00)>>8);
                           TexColor2=((TexColor&0xF800)>>8)|
                                     ((TexColor&0x07C0)<<5)|
                                     ((TexColor&0x003E)<<18);
                           if(TexColor&1) TexColor2|=0xFF000000;
                           TextureMem[(TileY-1-j)*TileScale+i]=TexColor2;
                           TexData+=1;
//                           Log_Message("X:%d Y:%d : %X",i,j,TexColor2);
                        }
                     TexData+=RDP_Tiles[Tile].LineSize*4-RDP_Tiles[Tile].SizeX;
                     }
                     break;
                  case 3:
                     Log_Message("RGBA 32bit ");
                     TexData2=(unsigned int *)(TMEM+RDP_Tiles[Tile].TMemAddress*8);
                     for(j=0;j<RDP_Tiles[Tile].SizeY;j++)
                     {
                        for(i=0;i<RDP_Tiles[Tile].SizeX;i++)
                        {
                           TexColor=(TexData2[0]);                            //Flip32REAL
                           TexColor2=(TexColor>>8)|((TexColor&0xFF)<<24);
                           TextureMem[(TileY-1-j)*TileScale+i]=TexColor2;
                           TexData2+=1;
                        }
                     TexData2+=RDP_Tiles[Tile].LineSize*2-RDP_Tiles[Tile].SizeX;
                     }
                     break;
                  default:
                     Log_Message("LoadTileTMEM() : %d size %d not supported",RDP_Tiles[Tile].Format,RDP_Tiles[Tile].TexelSize);
                  }
               break;
      // YUV
      case 1:  Log_Message("LoadTileTMEM() : Format YUV not implemented");
               break;
      // CI
      case 2:
               switch(RDP_Tiles[Tile].TexelSize)
                  {
                  case 0:
                     TexIndex=(unsigned char *)(TMEM+RDP_Tiles[Tile].TMemAddress*8); //+RDP_Tiles[Tile].ULS/2+RDP_Tiles[Tile].ULT*RDP_Tiles[Tile].LineSize*8;
                     PalIndex=(unsigned short *)(TMEM + (RDP_Tiles[Tile].Palette*16*2)+0x800);
//                     Log_Message("4bit Pal : %d  PalIndex : %X ",RDP_Tiles[Tile].Palette,RDP_Tiles[Tile].Palette*16*2+0x800);
                     for(j=0;j<RDP_Tiles[Tile].SizeY;j++)
                     {
                        for(i=0;i<RDP_Tiles[Tile].SizeX/2;i++)
                        {
                           TexColor=PalIndex[(TexIndex[0]&0xF0)>>4];
                           TexColor=((TexColor&0xFF)<<8)+((TexColor&0xFF00)>>8);
                           TexColor2=((TexColor&0xF800)>>8)|
                                     ((TexColor&0x07C0)<<5)|
                                     ((TexColor&0x003E)<<18);
                           if(TexColor&1) TexColor2|=0xFF000000;
                           TextureMem[(TileY-1-j)*TileScale+i*2]=TexColor2;

                           TexColor=PalIndex[(TexIndex[0]&0x0F)];
                           TexColor=((TexColor&0xFF)<<8)+((TexColor&0xFF00)>>8);
                           TexColor2=((TexColor&0xF800)>>8)|
                                     ((TexColor&0x07C0)<<5)|
                                     ((TexColor&0x003E)<<18);
                           if(TexColor&1) TexColor2|=0xFF000000;

                           TextureMem[(TileY-1-j)*TileScale+i*2+1]=TexColor2;
                           TexIndex+=1;
                        }
                     TexIndex+=RDP_Tiles[Tile].LineSize*8-((RDP_Tiles[Tile].SizeX)/2);
                     }
                     break;
                  case 1:
                     TexIndex=(unsigned char *)(TMEM+RDP_Tiles[Tile].TMemAddress*8);//+RDP_Tiles[Tile].ULS+RDP_Tiles[Tile].ULT*RDP_Tiles[Tile].LineSize*8;
//                     PalIndex=(unsigned short *)(TMEM + (RDP_Tiles[Tile].Palette*16*2)+0x800);
                     PalIndex=(unsigned short *)(TMEM +0x800);
//                     Log_Message("8bit Pal : %d  PalIndex : %X ",RDP_Tiles[Tile].Palette,RDP_Tiles[Tile].Palette*16*2+0x800);

                     for(j=0;j<RDP_Tiles[Tile].SizeY;j++)
                     {
                        for(i=0;i<RDP_Tiles[Tile].SizeX;i++)
                        {
                           TexColor=PalIndex[TexIndex[0]];
                           TexColor=((TexColor&0xFF)<<8)+((TexColor&0xFF00)>>8);
                           TexColor2=((TexColor&0xF800)>>8)|
                                     ((TexColor&0x07C0)<<5)|
                                     ((TexColor&0x003E)<<18);
                           if(TexColor&1) TexColor2|=0xFF000000;
                           TextureMem[(TileY-1-j)*TileScale+i]=TexColor2;
                           TexIndex+=1;                     
                        }
                     TexIndex+=RDP_Tiles[Tile].LineSize*8-RDP_Tiles[Tile].SizeX;
                     }
                     break;
                  default:
                     Log_Message("LoadTileTMEM() : %d size %d not supported",RDP_Tiles[Tile].Format,RDP_Tiles[Tile].TexelSize);
                  }
               break;
      // IA
      case 3:
               switch(RDP_Tiles[Tile].TexelSize)
                  {
                  case 0:
                     Log_Message("IA 4bit ");
                     TexIndex=(unsigned char *)(TMEM+RDP_Tiles[Tile].TMemAddress*8); 
                     for(j=0;j<RDP_Tiles[Tile].SizeY;j++)
                     {
                        for(i=0;i<RDP_Tiles[Tile].SizeX/2;i++) // 2 texel/byte
                        {
                           TexColor=(TexIndex[0]&0xF0)>>4;
                           I=(TexColor&0xE)<<4;
                           
                           TexColor2=(I<<16)|(I<<8)|I;
                           if(TexColor&1) TexColor2|=0xFF000000;
                           TextureMem[(TileY-1-j)*TileScale+i*2]=TexColor2;

                           TexColor=(TexIndex[0]&0x0F);
                           I=(TexColor&0xE)<<4;                           
                           TexColor2=(I<<16)|(I<<8)|I;
                           if(TexColor&1) TexColor2|=0xFF000000;
                           TextureMem[(TileY-1-j)*TileScale+i*2+1]=TexColor2;
                           TexIndex+=1;
                        }
                     TexIndex+=RDP_Tiles[Tile].LineSize*8-((RDP_Tiles[Tile].SizeX)/2);
                     }
                     break;
                  case 1:
                     Log_Message("IA 8bit ");
                     TexIndex=(unsigned char *)(TMEM+RDP_Tiles[Tile].TMemAddress*8);
                     for(j=0;j<RDP_Tiles[Tile].SizeY;j++)
                     {
                        for(i=0;i<RDP_Tiles[Tile].SizeX;i++)
                        {
                           TexColor=TexIndex[0];
                           I=(TexColor&0xF0);
                           TexColor2=(I<<16)|(I<<8)|I;
                           TexColor2|=(TexColor&0xF)<<27;
                           TextureMem[(TileY-1-j)*TileScale+i]=TexColor2;
                           TexIndex+=1;                     
                        }
                     TexIndex+=RDP_Tiles[Tile].LineSize*8-RDP_Tiles[Tile].SizeX;
                     }
                     break;
                  case 2:
                     Log_Message("IA 16bit ");
                     TexData=(unsigned short *)(TMEM+RDP_Tiles[Tile].TMemAddress*8);
                     for(j=0;j<RDP_Tiles[Tile].SizeY;j++)
                     {
                        for(i=0;i<RDP_Tiles[Tile].SizeX;i++)
                        {
                           TexColor=TexData[0];
                           TexColor=((TexColor&0xFF)<<8)+((TexColor&0xFF00)>>8);
                           I=(TexColor&0xFF00)>>8;
                           TexColor2=(I<<16)|(I<<8)|I;
                           TexColor2|=(TexColor&0xFF)<<24;                           
                           TextureMem[(TileY-1-j)*TileScale+i]=TexColor2;
                           TexData+=1;
                        }
                     TexData+=RDP_Tiles[Tile].LineSize*4-RDP_Tiles[Tile].SizeX;
                     }
                     break;
                  default:
                     Log_Message("LoadTileTMEM() : %d size %d not supported",RDP_Tiles[Tile].Format,RDP_Tiles[Tile].TexelSize);
                  }
               break;
      // I
      case 4:
               switch(RDP_Tiles[Tile].TexelSize)
                  {
                  case 0:
                     Log_Message("I 4bit ");
                     TexIndex=(unsigned char *)(TMEM+RDP_Tiles[Tile].TMemAddress*8); 
                     for(j=0;j<RDP_Tiles[Tile].SizeY;j++)
                     {
                        for(i=0;i<RDP_Tiles[Tile].SizeX/2;i++) // 2 texel/byte
                        {
                           TexColor=(TexIndex[0]&0xF0)>>4;
                           I=(TexColor&0xF)<<4;
                           TexColor2=(I<<24)|(I<<16)|(I<<8)|I;
//                           TexColor2|=0xFF000000;
                           TextureMem[(TileY-1-j)*TileScale+i*2]=TexColor2;

                           TexColor=(TexIndex[0]&0x0F);
                           I=(TexColor&0xF)<<4;                           
                           TexColor2=(I<<24)|(I<<16)|(I<<8)|I;
//                           TexColor2|=0xFF000000;
                           TextureMem[(TileY-1-j)*TileScale+i*2+1]=TexColor2;
                           TexIndex+=1;
                        }
                     TexIndex+=RDP_Tiles[Tile].LineSize*8-((RDP_Tiles[Tile].SizeX)/2);
                     }
                     break;
                  case 1:
                     Log_Message("I 8bit ");
                     TexIndex=(unsigned char *)(TMEM+RDP_Tiles[Tile].TMemAddress*8);
                     for(j=0;j<RDP_Tiles[Tile].SizeY;j++)
                     {
                        for(i=0;i<RDP_Tiles[Tile].SizeX;i++)
                        {
                           TexColor=TexIndex[0];
                           I=(TexColor&0xFF);                           
                           TexColor2=(I<<24)|(I<<16)|(I<<8)|I;
//                           TexColor2|=0xFF000000;
                           TextureMem[(TileY-1-j)*TileScale+i]=TexColor2;
                           TexIndex+=1;                     
                        }
                     TexIndex+=RDP_Tiles[Tile].LineSize*8-RDP_Tiles[Tile].SizeX;
                     }
                     break;
                  default:
                     Log_Message("LoadTileTMEM() : %d size %d not supported",RDP_Tiles[Tile].Format,RDP_Tiles[Tile].TexelSize);
                  }
               break;
      default: Log_Message("LoadTileTMEM() : %d size %d not supported",RDP_Tiles[Tile].Format,RDP_Tiles[Tile].TexelSize);
               break;
      }

   #ifdef DebugDL
   Log_Message("LoadTILETMEM valid : %d total : %d",ValidPixels,(RDP_Tiles[Tile].LRT+1)*(RDP_Tiles[Tile].LRS+1));
   #endif

   RDP_Tiles[Tile].GLX=TileScale;
   RDP_Tiles[Tile].GLY=TileY;
   #ifdef DebugDL
   Log_Message("LoadTILETMEM SizeX:%d SizeY:%d GLX:%d GLY:%d",RDP_Tiles[Tile].SizeX,RDP_Tiles[Tile].SizeY,RDP_Tiles[Tile].GLX,RDP_Tiles[Tile].GLY);
   #endif
   
      // TODO: fix that to use an array of preallocated textures!!!
      glBindTexture(GL_TEXTURE_2D, currenttexture+1024);


     // if (RDP_Tiles[Tile].MaskS == 0)
      //   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
     //  else
         //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT_ARB);
         
             glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT_ARB );

//      if (RDP_Tiles[Tile].MaskT == 0)
 //        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
 //     else
 //        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);



      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
          
    // if (RDP_Tiles[RDP_TN].CMS == 1) 
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP	);
    
     //else 
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT_ARB	);   
    
  //  if (RDP_Tiles[RDP_TN].CMS == 1) 
  //  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GLCLAMP_TO_BORDER_ARB	);
    
  //    else 
  //  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT_ARB	);   
       
               
      glTexImage2D(GL_TEXTURE_2D,
                   0, 4,
                   RDP_Tiles[Tile].GLX, RDP_Tiles[Tile].GLY,
                   0, GL_RGBA,
                   GL_UNSIGNED_BYTE, (BYTE *)TextureMem);
   
SetTexture:
      RDP_Tiles[Tile].GLX=TileScale;
      RDP_Tiles[Tile].GLY=TileY;
      glBindTexture(GL_TEXTURE_2D, texturetouse+1024);
      return;
}

void MOVEMEM(DWORD SRC,DWORD SIZE,DWORD DST)
{
   switch(DST)
   {
      case G_MV_LIGHT:
      case G_MV_POINT:
      case G_MV_MATRIX:
      case G_MV_VIEWPORT:
      case G_MVO_LOOKATX:
      case G_MVO_LOOKATY:
      case G_MVO_L0:
      case G_MVO_L1:
      case G_MVO_L2:
      case G_MVO_L3:
      case G_MVO_L4:
      case G_MVO_L5:
      case G_MVO_L6:
      case G_MVO_L7:
         #ifdef DebugDL
         Log_Message("MOVEMEM NOT IMPLEMENTED %X",DST);
         #endif
         break;
      default:
         Log_Message("UNKNOWN MOVEMEM");
   }
}

void MultMatrix(float res[4][4],float mf[4][4],float nf[4][4])
{
	int	i, j, k;
	float	temp[4][4];
	for (i=0; i<4; i++) {
	    for (j=0; j<4; j++) {
		temp[i][j] = 0.0;
		for (k=0; k<4; k++) {
		    temp[i][j] += mf[i][k] * nf[k][j];
		}
	    }
	}
	for (i=0; i<4; i++) {
	    for (j=0; j<4; j++) {
		res[i][j] = temp[i][j];
	    }
	}
}


void CopyMatrix(float dest[4][4],float source[4][4])
{
dest[0][0]=source[0][0];
dest[0][1]=source[0][1];
dest[0][2]=source[0][2];
dest[0][3]=source[0][3];
dest[1][0]=source[1][0];
dest[1][1]=source[1][1];
dest[1][2]=source[1][2];
dest[1][3]=source[1][3];
dest[2][0]=source[2][0];
dest[2][1]=source[2][1];
dest[2][2]=source[2][2];
dest[2][3]=source[2][3];
dest[3][0]=source[3][0];
dest[3][1]=source[3][1];
dest[3][2]=source[3][2];
dest[3][3]=source[3][3];
}

typedef struct {
unsigned short Mat[16];
unsigned short MatFrac[16];
}MatrixRSP1_t;

typedef struct {
unsigned short Mat[32];
}MatrixRSP2_t;

typedef union {
MatrixRSP1_t mat;
MatrixRSP2_t matflip;
}MatrixRSP_t;

typedef union {
float mat[16];
float mat2[4][4];
}Matrix_t;

void MATRIX(DWORD SRC,DWORD SIZE,DWORD FLAG)
{
   int i,j;
   MatrixRSP_t TempMatrixRSP;
   Matrix_t TempMatrix;
   Matrix_t TempMatrix2;
   unsigned int MatValue;
   int * MatValue2;
   short TempValue;

   memcpy(&TempMatrixRSP.mat.Mat[0],RDRAM+(0x1FFFFFFF&SRC),64);
   for(i=0;i<16;i++)
   {
      TempValue=TempMatrixRSP.matflip.Mat[i*2+1];
      TempMatrixRSP.matflip.Mat[i*2+1]=TempMatrixRSP.matflip.Mat[i*2];
      TempMatrixRSP.matflip.Mat[i*2]=TempValue;
   }

   for(i=0;i<32;i++)
      {
      TempMatrixRSP.matflip.Mat[i]=((TempMatrixRSP.matflip.Mat[i]));
      }

   for(j=0;j<4;j++)
   {
      for(i=0;i<4;i++)
      {
         MatValue=(TempMatrixRSP.mat.Mat[i+j*4]<<16)+TempMatrixRSP.mat.MatFrac[i+j*4];
         MatValue2=(int *)&MatValue;
         TempMatrix.mat[i+j*4]=(float) MatValue2[0]/65536;
      }
   }

   if(FLAG&G_MTX_PROJECTION)
   {
      glMatrixMode(GL_PROJECTION);
      if(FLAG&G_MTX_LOAD)
      {
         #ifdef DebugDL
         Log_Message("LOAD Projection Matrix");
         #endif
         glLoadMatrixf(TempMatrix.mat);
      }
      else
      {
         #ifdef DebugDL
         Log_Message("MULT Projection Matrix ");
         #endif
         glMultMatrixf(TempMatrix.mat);
         // do mul and put result in TempMatrix....
      }
   }
   else
   {
      glMatrixMode(GL_MODELVIEW);
      if(FLAG&G_MTX_PUSH)
      {
         #ifdef DebugDL
         Log_Message("PUSH Model Matrix ");
         #endif
         // push then put matrix
         glPushMatrix();
      }
      else
      {
         #ifdef DebugDL
         Log_Message("NOPUSH Model Matrix");
         #endif
         // put model matrix
      }
      if(FLAG&G_MTX_LOAD)
      {
         #ifdef DebugDL
         Log_Message("LOAD Model Matrix");
         #endif
         glLoadMatrixf(TempMatrix.mat);
      }
      else
      {
         #ifdef DebugDL
         Log_Message("MULT Model Matrix ");
         #endif
         glMultMatrixf(TempMatrix.mat);
      }
   }
/*      Log_Message("Matrix %8f %8f %8f %8f",TempMatrix.mat[0],TempMatrix.mat[1],TempMatrix.mat[2],TempMatrix.mat[3]);
      Log_Message("...    %8f %8f %8f %8f",TempMatrix.mat[4],TempMatrix.mat[5],TempMatrix.mat[6],TempMatrix.mat[7]);
      Log_Message("...    %8f %8f %8f %8f",TempMatrix.mat[8],TempMatrix.mat[9],TempMatrix.mat[10],TempMatrix.mat[11]);
      Log_Message("...    %8f %8f %8f %8f",TempMatrix.mat[12],TempMatrix.mat[13],TempMatrix.mat[14],TempMatrix.mat[15]);

      glGetFloatv(GL_PROJECTION_MATRIX, &TempMatrix.mat[0]);
      Log_Message("Proj M %8f %8f %8f %8f",TempMatrix.mat[0],TempMatrix.mat[1],TempMatrix.mat[2],TempMatrix.mat[3]);
      Log_Message("...    %8f %8f %8f %8f",TempMatrix.mat[4],TempMatrix.mat[5],TempMatrix.mat[6],TempMatrix.mat[7]);
      Log_Message("...    %8f %8f %8f %8f",TempMatrix.mat[8],TempMatrix.mat[9],TempMatrix.mat[10],TempMatrix.mat[11]);
      Log_Message("...    %8f %8f %8f %8f",TempMatrix.mat[12],TempMatrix.mat[13],TempMatrix.mat[14],TempMatrix.mat[15]);

      glGetFloatv(GL_MODELVIEW_MATRIX, &TempMatrix.mat[0]);
      Log_Message("ModelM %8f %8f %8f %8f",TempMatrix.mat[0],TempMatrix.mat[1],TempMatrix.mat[2],TempMatrix.mat[3]);
      Log_Message("...    %8f %8f %8f %8f",TempMatrix.mat[4],TempMatrix.mat[5],TempMatrix.mat[6],TempMatrix.mat[7]);
      Log_Message("...    %8f %8f %8f %8f",TempMatrix.mat[8],TempMatrix.mat[9],TempMatrix.mat[10],TempMatrix.mat[11]);
      Log_Message("...    %8f %8f %8f %8f",TempMatrix.mat[12],TempMatrix.mat[13],TempMatrix.mat[14],TempMatrix.mat[15]); */

      glMatrixMode(GL_MODELVIEW);
}



Vtx VtxBuffer[128]; //64 vertex buffer

void VTX(DWORD SRC,DWORD SIZE,DWORD NUMBER,DWORD START)
{
   DWORD i;
   #ifdef DebugDL
   Log_Message("Loading %X vertex in buffer starting at %X from n64 mem %X",NUMBER,START,SRC);
   #endif
   memcpy(&VtxBuffer[START],RDRAM+(0x1FFFFFFF&SRC),0x10*NUMBER);
   for(i=START;i<(START+NUMBER);i++)
   {
       VtxBuffer[i].f.x=((VtxBuffer[i].f.x&0xFF)<<8)+(VtxBuffer[i].f.x>>8);
       VtxBuffer[i].f.y=((VtxBuffer[i].f.y&0xFF)<<8)+(VtxBuffer[i].f.y>>8);
       VtxBuffer[i].f.z=((VtxBuffer[i].f.z&0xFF)<<8)+(VtxBuffer[i].f.z>>8);
       VtxBuffer[i].f.u=((VtxBuffer[i].f.u&0xFF)<<8)+(VtxBuffer[i].f.u>>8);
       VtxBuffer[i].f.v=((VtxBuffer[i].f.v&0xFF)<<8)+(VtxBuffer[i].f.v>>8);
   #ifdef DebugDL
   Log_Message("Vtx %d  X:%d Y:%d Z:%d U:%d V:%d R:%d G:%d B:%d A:%d",i,VtxBuffer[i].v.x,
                VtxBuffer[i].v.y,VtxBuffer[i].v.z,VtxBuffer[i].v.u,
                VtxBuffer[i].v.v,VtxBuffer[i].v.r,VtxBuffer[i].v.g,
                VtxBuffer[i].v.b,VtxBuffer[i].v.a);
   #endif
   }

}

void MtxUse(float mf[4][4], float x, float y, float z, float *ox, float *oy, float *oz)
{
        *ox = mf[0][0]*x + mf[1][0]*y + mf[2][0]*z + mf[3][0];
        *oy = mf[0][1]*x + mf[1][1]*y + mf[2][1]*z + mf[3][1];
        *oz = mf[0][2]*x + mf[1][2]*y + mf[2][2]*z + mf[3][2];
}


void TRI1(DWORD FLAG, DWORD V0, DWORD V1, DWORD V2)
{
   float TempU,TempV;
   float TempR,TempG,TempB,TempA;

   if(TEXTURE_ENABLE==1)
   {
      #ifdef DebugDL
      Log_Message("Before LoadTileTMEM");
      #endif
      glEnable(GL_TEXTURE_2D);
      LoadTileTMEM(TEXTURE_TILE);
   }
   else
   {
     glDisable(GL_TEXTURE_2D);
   }
      
   float envcolor[4]={(float)ENV_R/256, (float)ENV_G/256, (float)ENV_B/256, (float)ENV_A/256};
   float primcolor[4]={(float)PRIM_R/256, (float)PRIM_G/256, (float)PRIM_B/256, (float)PRIM_A/256};
   
   glProgramEnvParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, 0, envcolor);
   glProgramEnvParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB, 1, primcolor);
   
   glBegin(GL_TRIANGLES);

   if(TEXTURE_ENABLE==1)
   {
      if(USE_LIGHT==1)
      {
         TempR=(float) 1.0;
         TempG=(float) 1.0;
         TempB=(float) 1.0;
         TempA=(float) VtxBuffer[V0].v.a/256;
      }   
      else
      {
         TempR=(float) VtxBuffer[V0].v.r/256;
         TempG=(float) VtxBuffer[V0].v.g/256;
         TempB=(float) VtxBuffer[V0].v.b/256;
         TempA=(float) VtxBuffer[V0].v.a/256;
      }
   }
   else
   {
      TempR=(float)PRIM_R/256;
      TempG=(float)PRIM_G/256;
      TempB=(float)PRIM_B/256;
      TempA=(float)PRIM_A/256;
   }

   TempU=(float)VtxBuffer[V0].v.u*TEXTURE_SSCALE/65536/32/RDP_Tiles[TEXTURE_TILE].GLX;
   TempV=(float)VtxBuffer[V0].v.v*TEXTURE_TSCALE/65536/32/RDP_Tiles[TEXTURE_TILE].GLY;
   //TempU/=(1<<RDP_Tiles[TEXTURE_TILE].ShiftS);
   //TempV/=(1<<RDP_Tiles[TEXTURE_TILE].ShiftT);
   
   #ifdef DebugDL
   Log_Message("X:%d Y:%d Z:%d R:%f G:%f B:%f A:%f U:%f V:%f",
                VtxBuffer[V0].v.x,VtxBuffer[V0].v.y,VtxBuffer[V0].v.z,
                TempR,TempG,TempB,TempA,
                TempU,TempV);
   #endif

   glTexCoord2f(TempU,TempV);
   glColor4f(TempR,TempG,TempB,TempA);
   glVertex3s(VtxBuffer[V0].v.x,VtxBuffer[V0].v.y,VtxBuffer[V0].v.z);   


   if(TEXTURE_ENABLE==1)
   {
      if(USE_LIGHT==1)
      {
         TempR=(float) 1.0;
         TempG=(float) 1.0;
         TempB=(float) 1.0;
         TempA=(float) VtxBuffer[V1].v.a/256;
      }   
      else
      {
         TempR=(float) VtxBuffer[V1].v.r/256;
         TempG=(float) VtxBuffer[V1].v.g/256;
         TempB=(float) VtxBuffer[V1].v.b/256;
         TempA=(float) VtxBuffer[V1].v.a/256;
      }
   }
   else
   {
      TempR=(float)PRIM_R/256;
      TempG=(float)PRIM_G/256;
      TempB=(float)PRIM_B/256;
      TempA=(float)PRIM_A/256;
   }

   TempU=(float)VtxBuffer[V1].v.u*TEXTURE_SSCALE/65536/32/RDP_Tiles[TEXTURE_TILE].GLX;
   TempV=(float)VtxBuffer[V1].v.v*TEXTURE_TSCALE/65536/32/RDP_Tiles[TEXTURE_TILE].GLY;
   //TempU/=(1<<RDP_Tiles[TEXTURE_TILE].ShiftS);
   //TempV/=(1<<RDP_Tiles[TEXTURE_TILE].ShiftT);

   #ifdef DebugDL
   Log_Message("X:%d Y:%d Z:%d R:%f G:%f B:%f A:%f U:%f V:%f",
                VtxBuffer[V1].v.x,VtxBuffer[V1].v.y,VtxBuffer[V1].v.z,
                TempR,TempG,TempB,TempA,
                TempU,TempV);
   #endif

   glTexCoord2f(TempU,TempV);
   glColor4f(TempR,TempG,TempB,TempA);
   glVertex3s(VtxBuffer[V1].v.x,VtxBuffer[V1].v.y,VtxBuffer[V1].v.z);   




   if(TEXTURE_ENABLE==1)
   {
      if(USE_LIGHT==1)
      {
         TempR=(float) 1.0;
         TempG=(float) 1.0;
         TempB=(float) 1.0;
         TempA=(float) VtxBuffer[V2].v.a/256;
      }   
      else
      {
         TempR=(float) VtxBuffer[V2].v.r/256;
         TempG=(float) VtxBuffer[V2].v.g/256;
         TempB=(float) VtxBuffer[V2].v.b/256;
         TempA=(float) VtxBuffer[V2].v.a/256;
      }
   }
   else
   {
      TempR=(float)PRIM_R/256;
      TempG=(float)PRIM_G/256;
      TempB=(float)PRIM_B/256;
      TempA=(float)PRIM_A/256;
   }

   TempU=(float)VtxBuffer[V2].v.u*TEXTURE_SSCALE/65536/32/RDP_Tiles[TEXTURE_TILE].GLX;
   TempV=(float)VtxBuffer[V2].v.v*TEXTURE_TSCALE/65536/32/RDP_Tiles[TEXTURE_TILE].GLY; 
   //TempU/=(1<<RDP_Tiles[TEXTURE_TILE].ShiftS);
   //TempV/=(1<<RDP_Tiles[TEXTURE_TILE].ShiftT);

   #ifdef DebugDL
   Log_Message("X:%d Y:%d Z:%d R:%f G:%f B:%f A:%f U:%f V:%f",
                VtxBuffer[V2].v.x,VtxBuffer[V2].v.y,VtxBuffer[V2].v.z,
                TempR,TempG,TempB,TempA,
                TempU,TempV);
   #endif

   glTexCoord2f(TempU,TempV);
   glColor4f(TempR,TempG,TempB,TempA);
   glVertex3s(VtxBuffer[V2].v.x,VtxBuffer[V2].v.y,VtxBuffer[V2].v.z);   
   glEnd();


/*   glBegin(GL_LINES);
      glVertex3s(VtxBuffer[V0].v.x,VtxBuffer[V0].v.y,VtxBuffer[V0].v.z);   
      glVertex3s(VtxBuffer[V1].v.x,VtxBuffer[V1].v.y,VtxBuffer[V1].v.z);   
      glVertex3s(VtxBuffer[V1].v.x,VtxBuffer[V1].v.y,VtxBuffer[V1].v.z);   
      glVertex3s(VtxBuffer[V2].v.x,VtxBuffer[V2].v.y,VtxBuffer[V2].v.z);   
      glVertex3s(VtxBuffer[V2].v.x,VtxBuffer[V2].v.y,VtxBuffer[V2].v.z);   
      glVertex3s(VtxBuffer[V0].v.x,VtxBuffer[V0].v.y,VtxBuffer[V0].v.z);   
   glEnd();*/
   CIMG_SWAP=1;
}

DWORD CCTexel0=0; // If texture is used
DWORD CCShade=0; // If shade color is used
DWORD CCPrim=0; // If primitive is used
DWORD CCEnv=0; // If environment is used

void UpdateStates()
{
/*   if(RENDERMODE&0x10)
   {
      g_pd3dDevice->SetRenderState( D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);
   }
   else
   {
      g_pd3dDevice->SetRenderState( D3DRENDERSTATE_ZENABLE, D3DZB_FALSE);
   }   


   if(RENDERMODE&0x20) // Z_UPDATE
   {
      g_pd3dDevice->SetRenderState( D3DRENDERSTATE_ZWRITEENABLE, TRUE);
   }
   else
   {
      g_pd3dDevice->SetRenderState( D3DRENDERSTATE_ZWRITEENABLE, FALSE);
   }

   
   if(RENDERMODE&0x800&&RENDERMODE&0x400) // DECAL
   {
//      Log_Message("DECAL MODE %X",RENDERMODE);
      g_pd3dDevice->SetRenderState( D3DRENDERSTATE_ZBIAS, 0);
//      g_pd3dDevice->SetRenderState( D3DRENDERSTATE_ZENABLE, D3DZB_FALSE);
   }
   else
   {
      g_pd3dDevice->SetRenderState( D3DRENDERSTATE_ZBIAS, 8);
   }*/

//   "cCOMBINED","cTEXEL0","cTEXEL1","cPRIM",
//   "cSHADE","cENV","cKEYSCALE","aCOMBINED",
//   "aTEXEL0","aTEXEL1","aPRIM","aSHADE",
//   "aENV","LODFRAC","PRIMLODFRAC","CONVK5",
//   "0.0","0.0","0.0","0.0",
//   "0.0","0.0","0.0","0.0",
//   "0.0","0.0","0.0","0.0",
//   "0.0","0.0","0.0","0.0",
/*
   switch(cC0)
   {
      case 0: // cCOMBINED
         Log_Message("Combined in cycle 0!?!");
         break;
      case 1: // cTEXEL0
         g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

         break;
      case 2:  // cTEXEL1
         g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

         break;
      case 3:  // cPRIM
//         g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
         
         break;
      case 4:  // cSHADE
         break;
      case 5:  // cENV
         break;
      case 6:  // cKEYSCALE
         break;
      case 7:  // aCOMBINED
         break;
      case 8:  // aTEXEL0
         break;
      case 9:  // aTEXEL1
         break;
      case 10: // aPRIM
         break;
      case 11: // aSHADE
         break;
      case 12: // aENV
         break;
      case 13: // LODFRAC
         break;
      case 14: // PRIMLODFRAC
         break;
      case 15: // CONVK5
         break;
   }
*/
/*   if(TEXTURE_ENABLE==1)
   {
      g_pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
      //   g_pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
      g_pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
      g_pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);
      g_pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
      g_pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);
      g_pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
   }
   else
   {
      g_pd3dDevice->SetTexture(0,NULL);
   }

   g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
   g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
   g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
   g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

   if(TEXTURE_ENABLE==0)
   {
      g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
      g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
   }
   else
   {
      g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
      g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
   }*/

}

char shader[8192];
int fragProg;

int createGLShader()
{
    char *shaderout=&shader[0];
    shaderout+=sprintf(shaderout,"!!ARBfp1.0\n");
    shaderout+=sprintf(shaderout,"TEMP R0;\n");
    shaderout+=sprintf(shaderout,"TEMP R1;\n");
    shaderout+=sprintf(shaderout,"TEMP aR0;\n");
    shaderout+=sprintf(shaderout,"TEMP aR1;\n");
    shaderout+=sprintf(shaderout,"PARAM envcolor = program.env[0];\n");
    shaderout+=sprintf(shaderout,"PARAM primcolor = program.env[1];\n");
    switch(cA0)
    {
        case 1: // cTEXEL0
            shaderout+=sprintf(shaderout,"TEX R0, -fragment.texcoord[0], texture[0], 2D;\n");
            break;
        case 2: // cTEXEL1
            shaderout+=sprintf(shaderout,"TEX R0, -fragment.texcoord[0], texture[1], 2D;\n");
            break;
        case 3: // cPRIM
            shaderout+=sprintf(shaderout,"MOV R0, primcolor;\n");
            break;
        case 4: // cSHADE
            shaderout+=sprintf(shaderout,"MOV R0, fragment.color.primary;\n");
            break;
        case 5: // cENV
            shaderout+=sprintf(shaderout,"MOV R0, envcolor;\n");
            break;
        case 6: // 1.0
            shaderout+=sprintf(shaderout,"MOV R0, {1.0,1.0,1.0,1.0};\n");
            break;
        default:
            shaderout+=sprintf(shaderout,"MOV R0, {0,0,0,0};\n");
            break;
    }
    switch(cB0)
    {
        case 1: // cTEXEL0
            shaderout+=sprintf(shaderout,"TEX R1, -fragment.texcoord[0], texture[0], 2D;\n");
            break;
        case 2: // cTEXEL1
            shaderout+=sprintf(shaderout,"TEX R1, -fragment.texcoord[0], texture[1], 2D;\n");
            break;
        case 3: // cPRIM
            shaderout+=sprintf(shaderout,"MOV R1, primcolor;\n");
            break;
        case 4: // cSHADE
            shaderout+=sprintf(shaderout,"MOV R1, fragment.color.primary;\n");
            break;
        case 5: // cENV
            shaderout+=sprintf(shaderout,"MOV R1, envcolor;\n");
            break;
        default:
            shaderout+=sprintf(shaderout,"MOV R1, {0,0,0,0};\n");
            break;
    }
    shaderout+=sprintf(shaderout,"SUB R0, R0, R1;\n");
    switch(cC0)
    {
        case 1: // cTEXEL0
            shaderout+=sprintf(shaderout,"TEX R1, -fragment.texcoord[0], texture[0], 2D;\n");
            break;
        case 2: // cTEXEL1
            shaderout+=sprintf(shaderout,"TEX R1, -fragment.texcoord[0], texture[1], 2D;\n");
            break;
        case 3: // cPRIM
            shaderout+=sprintf(shaderout,"MOV R1, primcolor;\n");
            break;
        case 4: // cSHADE
            shaderout+=sprintf(shaderout,"MOV R1, fragment.color.primary;\n");
            break;
        case 5: // cENV
            shaderout+=sprintf(shaderout,"MOV R1, envcolor;\n");
            break;
        case 8: // aTEXEL0
            shaderout+=sprintf(shaderout,"TEX R1, -fragment.texcoord[0], texture[0], 2D;\n");
            shaderout+=sprintf(shaderout,"MOV R1, R1.aaaa;\n");
            break;
        case 9: // aTEXEL1
            shaderout+=sprintf(shaderout,"TEX R1, -fragment.texcoord[0], texture[1], 2D;\n");
            shaderout+=sprintf(shaderout,"MOV R1, R1.aaaa;\n");
            break;
        case 10: // aPRIM
            shaderout+=sprintf(shaderout,"MOV R1, primcolor;\n");
            shaderout+=sprintf(shaderout,"MOV R1, R1.aaaa;\n");
            break;
        case 11: // aSHADE
            shaderout+=sprintf(shaderout,"MOV R1, fragment.color.primary;\n");
            shaderout+=sprintf(shaderout,"MOV R1, R1.aaaa;\n");
            break;
        case 12: // aENV
            shaderout+=sprintf(shaderout,"MOV R1, envcolor;\n");
            shaderout+=sprintf(shaderout,"MOV R1, R1.aaaa;\n");
            break;
        default:
            shaderout+=sprintf(shaderout,"MOV R1, {0,0,0,0};\n");
            break;
    
    }
    shaderout+=sprintf(shaderout,"MUL R0, R0, R1;\n");
    switch(cD0)
    {
        case 1: // cTEXEL0
            shaderout+=sprintf(shaderout,"TEX R1, -fragment.texcoord[0], texture[0], 2D;\n");
            break;
        case 2: // cTEXEL1
            shaderout+=sprintf(shaderout,"TEX R1, -fragment.texcoord[0], texture[1], 2D;\n");
            break;
        case 3: // cPRIM
            shaderout+=sprintf(shaderout,"MOV R1, primcolor;\n");
            break;
        case 4: // cSHADE
            shaderout+=sprintf(shaderout,"MOV R1, fragment.color.primary;\n");
            break;
        case 5: // cENV
            shaderout+=sprintf(shaderout,"MOV R1, envcolor;\n");
            break;
        case 6: // 1.0
            shaderout+=sprintf(shaderout,"MOV R1, {1.0,1.0,1.0,1.0};\n");
            break;
        default:
            shaderout+=sprintf(shaderout,"MOV R1, {0,0,0,0};\n");
            break;
    }
    shaderout+=sprintf(shaderout,"ADD R0, R0, R1;\n");
    
    switch(aA0)
    {
        case 1: // aTEXEL0
            shaderout+=sprintf(shaderout,"TEX aR0, -fragment.texcoord[0], texture[0], 2D;\n");
            break;
        case 2: // aTEXEL1
            shaderout+=sprintf(shaderout,"TEX aR0, -fragment.texcoord[0], texture[1], 2D;\n");
            break;
        case 3: // aPRIM
            shaderout+=sprintf(shaderout,"MOV aR0, primcolor;\n");
            break;
        case 4: // aSHADE
            shaderout+=sprintf(shaderout,"MOV aR0, fragment.color.primary;\n");
            break;
        case 5: // aENV
            shaderout+=sprintf(shaderout,"MOV aR0, envcolor;\n");
            break;
        case 6: // 1.0
            shaderout+=sprintf(shaderout,"MOV aR0, {1.0,1.0,1.0,1.0};\n");
            break;
        default:
            shaderout+=sprintf(shaderout,"MOV aR0, {0,0,0,0};\n");
            break;
    }
    
    switch(aB0)
    {
        case 1: // aTEXEL0
            shaderout+=sprintf(shaderout,"TEX aR1, -fragment.texcoord[0], texture[0], 2D;\n");
            break;
        case 2: // aTEXEL1
            shaderout+=sprintf(shaderout,"TEX aR1, -fragment.texcoord[0], texture[1], 2D;\n");
            break;
        case 3: // aPRIM
            shaderout+=sprintf(shaderout,"MOV aR1, primcolor;\n");
            break;
        case 4: // aSHADE
            shaderout+=sprintf(shaderout,"MOV aR1, fragment.color.primary;\n");
            break;
        case 5: // aENV
            shaderout+=sprintf(shaderout,"MOV aR1, envcolor;\n");
            break;
        case 6: // 1.0
            shaderout+=sprintf(shaderout,"MOV aR1, {1.0,1.0,1.0,1.0};\n");
            break;
        default:
            shaderout+=sprintf(shaderout,"MOV aR1, {0,0,0,0};\n");
            break;
    }
    shaderout+=sprintf(shaderout,"SUB aR0, aR0, aR1;\n");
    switch(aC0)
    {
        case 1: // aTEXEL0
            shaderout+=sprintf(shaderout,"TEX aR1, -fragment.texcoord[0], texture[0], 2D;\n");
            break;
        case 2: // aTEXEL1
            shaderout+=sprintf(shaderout,"TEX aR1, -fragment.texcoord[0], texture[1], 2D;\n");
            break;
        case 3: // aPRIM
            shaderout+=sprintf(shaderout,"MOV aR1, primcolor;\n");
            break;
        case 4: // aSHADE
            shaderout+=sprintf(shaderout,"MOV aR1, fragment.color.primary;\n");
            break;
        case 5: // aENV
            shaderout+=sprintf(shaderout,"MOV aR1, envcolor;\n");
            break;
        default:
            shaderout+=sprintf(shaderout,"MOV aR1, {0,0,0,0};\n");
            break;
    }
    shaderout+=sprintf(shaderout,"MUL aR0, aR0, aR1;\n");
    switch(aD0)
    {
        case 1: // aTEXEL0
            shaderout+=sprintf(shaderout,"TEX aR1, -fragment.texcoord[0], texture[0], 2D;\n");
            break;
        case 2: // aTEXEL1
            shaderout+=sprintf(shaderout,"TEX aR1, -fragment.texcoord[0], texture[1], 2D;\n");
            break;
        case 3: // aPRIM
            shaderout+=sprintf(shaderout,"MOV aR1, primcolor;\n");
            break;
        case 4: // aSHADE
            shaderout+=sprintf(shaderout,"MOV aR1, fragment.color.primary;\n");
            break;
        case 5: // aENV
            shaderout+=sprintf(shaderout,"MOV aR1, envcolor;\n");
            break;
        case 6: // 1.0
            shaderout+=sprintf(shaderout,"MOV aR1, {1.0,1.0,1.0,1.0};\n");
            break;
        default:
            shaderout+=sprintf(shaderout,"MOV aR1, {0,0,0,0};\n");
            break;
    }
    shaderout+=sprintf(shaderout,"ADD aR0, aR0, aR1;\n");
    shaderout+=sprintf(shaderout,"MOV R0.a, aR0.a;\n");
    shaderout+=sprintf(shaderout,"MOV result.color, R0;\n");
    shaderout+=sprintf(shaderout,"END\n");
    
    Log_Message("Shader:\n%s",shader);
    
    glGenProgramsARB(1, &fragProg);
    glBindProgramARB (GL_FRAGMENT_PROGRAM_ARB, fragProg);
    
    glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB,
        strlen(shader), shader);
    
    int errPos;
    glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &errPos);
    if (errPos >= 0) 
    {
        printf("errPos %d\n",errPos);
        Log_Message("Fragment program failed to load, error at %d", errPos);
        shader[errPos]='*';
        Log_Message("Error in shader:\n%s",shader);
        
    }
    else
    {
        Log_Message("no error found in shader\n");
    }
    glEnable(GL_FRAGMENT_PROGRAM_ARB);
    glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, fragProg);
            
}

int ProcessDisplayListSW(DWORD dl)
{
   DWORD CommandLow;
   DWORD CommandHigh;
   DWORD SavedHalf;
// !!!TEMPORARY TEST!!!
//   *GFXInfo.MI_INTR_REG |= R4300i_DP_Intr;
//   return(0);
// !!!TEMPORARY TEST!!!

//   CIMG_SWAP=1;
/*   if( FAILED( g_pd3dDevice->BeginScene() ) )
   {
      DisplayError("BeginScene error");
     return E_FAIL;
   } */  

   
//   printf("Starting display list processing\n");
   
   while (1==1)
   {
         CommandLow=Flip32( *(DWORD *)(RDRAM+dl) );
         CommandHigh=Flip32( *(DWORD *)(RDRAM+dl+4) );
         //printf("CMD %08X %08X\n",CommandLow, CommandHigh);
         switch (CommandLow>>24)
         {
            case G_DL:
                  if(((CommandHigh>>24)&0x0F)!=6 && ((CommandHigh>>24)&0x0F)!=3)
                  {
                      Log_Message("G_DL to bank %d not supported",((CommandHigh>>24)&0x0F));
                      IMMINC
                  }
                  
                  switch((CommandLow&0xFF0000)>>16)
                  {
                     case G_DL_NOPUSH:
                        #ifdef DebugDL
                           #ifdef DebugDL
                           Log_Message("G_DL BRANCH %X",SEGMENT[(CommandHigh>>24)&0x0F]+CommandHigh&0xFFFFFF);
                           #endif
                        #endif
                        ProcessDisplayListSW(SEGMENT[(CommandHigh>>24)&0x0F]+CommandHigh&0xFFFFFF);
                        return(0);
                        break;                     
                     case G_DL_PUSH:
                        #ifdef DebugDL
                           Log_Message("G_DL CALL %X",SEGMENT[(CommandHigh>>24)&0x0F]+CommandHigh&0xFFFFFF);
                        #endif
                        ProcessDisplayListSW(SEGMENT[(CommandHigh>>24)&0x0F]+CommandHigh&0xFFFFFF);
                        break;                     
                     default:
                        Log_Message("Unknown G_DL");
                  }
                  IMMINC
            case G_ENDDL:
                  #ifdef DebugDL
                  Log_Message("G_ENDDL");
                  #endif
                  return(0);
                  IMMINC
            case G_RDPFULLSYNC:
                  #ifdef DebugDL
                  Log_Message("RDP FULL SYNC!");
                  #endif
                  //*GFXInfo.MI_INTR_REG |= R4300i_DP_Intr;
                  //GFXInfo.CheckInterrupts();
                  IMMINC
            case G_RDPTILESYNC:
                  #ifdef DebugDL
                  Log_Message("G_RDPTILESYNC");
                  #endif
                  IMMINC
            case G_RDPPIPESYNC:
                  #ifdef DebugDL
                  Log_Message("G_RDPPIPESYNC");
                  #endif
                  IMMINC
            case G_RDPLOADSYNC:
                  #ifdef DebugDL
                  Log_Message("G_RDPLOADSYNC");
                  #endif
                  IMMINC
            case G_GEOMETRYMODE:
                  #ifdef DebugDL
                  Log_Message("G_GEOMETRYMODE CLEAR %X",~CommandLow);
                  Log_Message("G_GEOMETRYMODE SET %X",CommandHigh);
                  #endif
                           
                  if((~CommandLow)&G_ZBUFFER) 
                  {
                      Log_Message("G_GEOMETRYMODE disable zbuffer");
                      glDisable(GL_DEPTH_TEST);
                      USE_ZBUFFER=0;
                  }

                  if((~CommandLow)&G_SHADE) USE_SHADE=0;
                  if((~CommandLow)&G_LIGHTING) USE_LIGHT=0;
                  if(CommandHigh&G_ZBUFFER)
                  {
                      Log_Message("G_GEOMETRYMODE enable zbuffer");
                      glEnable(GL_DEPTH_TEST);
                      USE_ZBUFFER=0;
                  }
                  if(CommandHigh&G_SHADE) USE_SHADE=1;
                  if(CommandHigh&G_LIGHTING) USE_LIGHT=1;
                  UpdateStates();
                  IMMINC
            case G_SETOTHERMODE_L:
                  Log_Message("CMD %08X %08X",CommandLow, CommandHigh);
                  switch(32-((CommandLow>>8)&0xFF)-(CommandLow&0xFF)-1)
                  {
                     case G_MDSFT_ALPHACOMPARE:
                           ALPHACOMPARE=(CommandHigh);
                           #ifdef DebugDL
                           Log_Message("G_SETOTHERMODE_L G_MDSFT_ALPHACOMPARE %X",ALPHACOMPARE);
                           #endif
                           break;
                     case G_MDSFT_ZSRCSEL:
                           ZSRCSEL=(CommandHigh);
                           #ifdef DebugDL
                           Log_Message("G_SETOTHERMODE_L G_MDSFT_ZSRCSEL %X",ZSRCSEL);
                           #endif
                           break;
                     case G_MDSFT_RENDERMODE:
                           RENDERMODE=(CommandHigh);
                           #ifdef DebugDL
                           Log_Message("G_SETOTHERMODE_L G_MDSFT_RENDERMODE %X",RENDERMODE);                           
                           #endif
                           break;
                     case G_MDSFT_BLENDER:
                           BLENDER=(CommandHigh);
                           #ifdef DebugDL
                           Log_Message("G_SETOTHERMODE_L G_MDSFT_BLENDER %X",BLENDER);
                           #endif
                           break;
                     default:
                           #ifdef DebugDL
                           Log_Message("G_SETOTHERMODE_L ??? %X",CommandHigh);
                           #endif
                           break;
                  }
                  IMMINC
            case G_SETOTHERMODE_H:
                  Log_Message("CMD %08X %08X",CommandLow, CommandHigh);
                  switch(32-((CommandLow>>8)&0xFF)-(CommandLow&0xFF)-1)
                  {
                     case G_MDSFT_ALPHADITHER:
                           ALPHADITHER=(CommandHigh>>((CommandLow>>8)&0xFF))&((1<<(CommandLow&0xFF))-1);
                           #ifdef DebugDL
                           Log_Message("G_SETOTHERMODE_H G_MDSFT_ALPHADITHER %X",ALPHADITHER);
                           #endif
                           break;
                     case G_MDSFT_RGBDITHER:
                           RGBDITHER=(CommandHigh>>((CommandLow>>8)&0xFF))&((1<<(CommandLow&0xFF))-1);
                           #ifdef DebugDL
                           Log_Message("G_SETOTHERMODE_H G_MDSFT_RGBDITHER %X",RGBDITHER);
                           #endif
                           break;
                     case G_MDSFT_COMBKEY:
                           COMBKEY=(CommandHigh>>((CommandLow>>8)&0xFF))&((1<<(CommandLow&0xFF))-1);
                           #ifdef DebugDL
                           Log_Message("G_SETOTHERMODE_H G_MDSFT_COMBKEY %X",COMBKEY);
                           #endif
                           break;
                     case G_MDSFT_TEXTCONV:
                           TEXTCONV=(CommandHigh>>((CommandLow>>8)&0xFF))&((1<<(CommandLow&0xFF))-1);
                           #ifdef DebugDL
                           Log_Message("G_SETOTHERMODE_H G_MDSFT_TEXTCONV %X",TEXTCONV);
                           #endif
                           break;
                     case G_MDSFT_TEXTFILT:
                           TEXTFILT=(CommandHigh>>((CommandLow>>8)&0xFF))&((1<<(CommandLow&0xFF))-1);
                           #ifdef DebugDL
                           Log_Message("G_SETOTHERMODE_H G_MDSFT_TEXTFILT %X",TEXTFILT);
                           #endif
                           break;
                     case G_MDSFT_TEXTLUT:
                           TEXTLUT=(CommandHigh>>((CommandLow>>8)&0xFF))&((1<<(CommandLow&0xFF))-1);
                           #ifdef DebugDL
                           Log_Message("G_SETOTHERMODE_H G_MDSFT_TEXTLUT %X",TEXTLUT);
                           #endif
                           break;
                     case G_MDSFT_TEXTLOD:
                           TEXTLOD=(CommandHigh>>((CommandLow>>8)&0xFF))&((1<<(CommandLow&0xFF))-1);
                           #ifdef DebugDL
                           Log_Message("G_SETOTHERMODE_H G_MDSFT_TEXTLOD %X",TEXTLOD);
                           #endif
                           break;
                     case G_MDSFT_TEXTDETAIL:
                           TEXTDETAIL=(CommandHigh>>((CommandLow>>8)&0xFF))&((1<<(CommandLow&0xFF))-1);
                           #ifdef DebugDL
                           Log_Message("G_SETOTHERMODE_H G_MDSFT_TEXTDETAIL %X",TEXTDETAIL);
                           #endif
                           break;
                     case G_MDSFT_TEXTPERSP:
                           TEXTPERSP=(CommandHigh>>((CommandLow>>8)&0xFF))&((1<<(CommandLow&0xFF))-1);
                           #ifdef DebugDL
                           Log_Message("G_SETOTHERMODE_H G_MDSFT_TEXTPERSP %X",TEXTPERSP);
                           #endif
                           break;
                     case G_MDSFT_CYCLETYPE:
                           CYCLETYPE=(CommandHigh>>((CommandLow>>8)&0xFF))&((1<<(CommandLow&0xFF))-1);
                           #ifdef DebugDL
                           Log_Message("G_SETOTHERMODE_H G_MDSFT_CYCLETYPE %X",CYCLETYPE);
                           #endif
                           break;
                     case G_MDSFT_COLORDITHER:
                           COLORDITHER=(CommandHigh>>((CommandLow>>8)&0xFF))&((1<<(CommandLow&0xFF))-1);
                           #ifdef DebugDL
                           Log_Message("G_SETOTHERMODE_H G_MDSFT_COLORDITHER %X",COLORDITHER);
                           #endif
                           break;
                     case G_MDSFT_PIPELINE:
                           PIPELINE=(CommandHigh>>((CommandLow>>8)&0xFF))&((1<<(CommandLow&0xFF))-1);
                           #ifdef DebugDL
                           Log_Message("G_SETOTHERMODE_H G_MDSFT_PIPELINE %X",PIPELINE);
                           #endif
                           break;
                     default:
                           #ifdef DebugDL
                           Log_Message("G_SETOTHERMODE_H ??? %X",(CommandHigh>>((CommandLow>>8)&0xFF))&((1<<(CommandLow&0xFF))-1));
                           #endif
                           break;
                  }
                  IMMINC
            case G_SETCOMBINE:
                  #ifdef DebugDL
                  Log_Message("G_SETCOMBINE %X %X",CommandLow&0xFFFFFF,CommandHigh);
                  #endif
                  COMBINE0=CommandLow&0xFFFFFF;
                  COMBINE1=CommandHigh;
                  cA0=(COMBINE0>>20)&0xF;
                  cB0=(COMBINE1>>28)&0xF;
                  cC0=(COMBINE0>>15)&0x1F;
                  cD0=(COMBINE1>>15)&0x7;

                  aA0=(COMBINE0>>12)&0x7;
                  aB0=(COMBINE1>>12)&0x7;
                  aC0=(COMBINE0>>9)&0x7;
                  aD0=(COMBINE1>>9)&0x7;

                  cA1=(COMBINE0>>5)&0xF;
                  cB1=(COMBINE1>>24)&0xF;
                  cC1=(COMBINE0>>0)&0x1F;
                  cD1=(COMBINE1>>6)&0x7;

                  aA1=(COMBINE1>>21)&0x7;
                  aB1=(COMBINE1>>3)&0x7;
                  aC1=(COMBINE1>>18)&0x7;
                  aD1=(COMBINE1>>0)&0x7;

                  #ifdef DebugDL
                  if(CYCLETYPE==0||CYCLETYPE==1)
                  {
                     Log_Message("Cycle0 : C=(%s-%s)*%s+%s A=(%s-%s)*%s+%s",
                        ColorA[cA0],ColorB[cB0],ColorC[cC0],ColorD[cD0],
                        AlphaA[aA0],AlphaB[aB0],AlphaC[aC0],AlphaD[aD0]);
                  }
                  if(CYCLETYPE==1)
                  {
                     Log_Message("Cycle1 : C=(%s-%s)*%s+%s A=(%s-%s)*%s+%s",
                        ColorA[cA1],ColorB[cB1],ColorC[cC1],ColorD[cD1],
                        AlphaA[aA1],AlphaB[aB1],AlphaC[aC1],AlphaD[aD1]);
                  }
                  #endif
                  createGLShader();
                  IMMINC
            case G_SETFILLCOLOR:
                  FILLCOLOR=CommandHigh;
                  #ifdef DebugDL
                  Log_Message("G_SETFILLCOLOR %X",FILLCOLOR);
                  #endif
                  IMMINC
            case G_SETFOGCOLOR:
                  FOG_R=(CommandHigh>>24)&0xFF;
                  FOG_G=(CommandHigh>>16)&0xFF;
                  FOG_B=(CommandHigh>>8)&0xFF;
                  FOG_A=(CommandHigh)&0xFF;
                  #ifdef DebugDL
                  Log_Message("G_SETFOGCOLOR R:%d G:%d B:%d A:%d",FOG_R,FOG_G,FOG_B,FOG_A);
                  #endif
                  IMMINC
            case G_SETBLENDCOLOR:
                  BLEND_R=(CommandHigh>>24)&0xFF;
                  BLEND_G=(CommandHigh>>16)&0xFF;
                  BLEND_B=(CommandHigh>>8)&0xFF;
                  BLEND_A=(CommandHigh)&0xFF;
                  #ifdef DebugDL
                  Log_Message("G_SETBLENDCOLOR R:%d G:%d B:%d A:%d",BLEND_R,BLEND_G,BLEND_B,BLEND_A);
                  #endif
                  IMMINC
            case G_SETPRIMCOLOR:
                  PRIM_M=(CommandLow>>8)&0xFF;
                  PRIM_L=(CommandLow)&0xFF;
                  PRIM_R=(CommandHigh>>24)&0xFF;
                  PRIM_G=(CommandHigh>>16)&0xFF;
                  PRIM_B=(CommandHigh>>8)&0xFF;
                  PRIM_A=(CommandHigh)&0xFF;
                  #ifdef DebugDL
                  Log_Message("G_SETPRIMCOLOR M:%d L:%d R:%d G:%d B:%d A:%d",PRIM_M,PRIM_L,PRIM_R,PRIM_G,PRIM_B,PRIM_A);
                  #endif
                  IMMINC
            case G_SETENVCOLOR:
                  ENV_R=(CommandHigh>>24)&0xFF;
                  ENV_G=(CommandHigh>>16)&0xFF;
                  ENV_B=(CommandHigh>>8)&0xFF;
                  ENV_A=(CommandHigh)&0xFF;
                  #ifdef DebugDL
                  Log_Message("G_SETENVCOLOR R:%d G:%d B:%d A:%d",ENV_R,ENV_G,ENV_B,ENV_A);
                  #endif
                  IMMINC
            case G_TEXTURE:
                  TEXTURE_SSCALE=(CommandHigh>>16);
                  TEXTURE_TSCALE=CommandHigh&0xFFFF;
                  TEXTURE_LEVEL=(CommandLow>>11)&7;
                  TEXTURE_TILE=(CommandLow>>8)&7;
                  TEXTURE_ENABLE=CommandLow&1;
                  TEXTURE_ENABLE=1;
                  UpdateStates();
                  #ifdef DebugDL
                  Log_Message("G_TEXTURE SS:%X TS:%X LEVEL:%X TILE:%X ENABLE:%X",TEXTURE_SSCALE,TEXTURE_TSCALE,TEXTURE_LEVEL,TEXTURE_TILE,TEXTURE_ENABLE);
                  #endif
                  IMMINC
            case G_RDPHALF_1:
                  #ifdef DebugDL
                  Log_Message("RDPHALF ?!? %08X-%08X",CommandLow,CommandHigh);
                  #endif
                  SavedHalf=CommandHigh;
                  IMMINC
            case G_SETCIMG:
                  CIMG_Format=(CommandLow&0x00E00000)>>21;
                  CIMG_Size=(CommandLow&0x00180000)>>19;
                  CIMG_Width=(CommandLow&0x00000FFF)+1;
                  CIMG_CFB=CommandHigh;
                  if(CIMG_SWAP==1)
                     {
                     CIMG_SWAP=0;
                     }
                  #ifdef DebugDL
                  Log_Message("G_SETCIMG Format: %d Size: %d Width: %d CFB: %08X",CIMG_Format,CIMG_Size,CIMG_Width,CIMG_CFB);
                  #endif
                  IMMINC
            case G_SETSCISSOR:
                  SCISSOR_LX=(CommandLow&0x00FFF000)>>14;
                  SCISSOR_LY=(CommandLow&0x00000FFF)>>2;
                  SCISSOR_RX=(CommandHigh&0x00FFF000)>>14;
                  SCISSOR_RY=(CommandHigh&0x00000FFF)>>2;
                  SCISSOR_Mode=(CommandHigh&0x03000000);
                  #ifdef DebugDL
                  Log_Message("SCISSOR LX:%d LY:%d RX:%d RY:%d Mode:%d",SCISSOR_LX,SCISSOR_LY,SCISSOR_RX,SCISSOR_RY,SCISSOR_Mode);
                  #endif
                  IMMINC
            case G_FILLRECT:
                  FILLRECT_LX=(CommandHigh&0x00FFF000)>>14;
                  FILLRECT_LY=(CommandHigh&0x00000FFF)>>2;
                  FILLRECT_RX=(CommandLow&0x00FFF000)>>14;
                  FILLRECT_RY=(CommandLow&0x00000FFF)>>2;
                  #ifdef DebugDL
                  Log_Message("G_FILLRECT LX:%d LY:%d RX:%d RY:%d",FILLRECT_LX,FILLRECT_LY,FILLRECT_RX,FILLRECT_RY);
                  #endif

                  float FillRed,FillGreen,FillBlue,FillAlpha;

                  FillRed=((FILLCOLOR>>11)&0x1F);
                  FillGreen=((FILLCOLOR>>6)&0x1F);
                  FillBlue=((FILLCOLOR>>1)&0x1F);
                  FillAlpha=FILLCOLOR&1;                  


                  FillRed=FillRed/32;
                  FillGreen=FillGreen/32;
                  FillBlue=FillBlue/32;

                  #ifdef DebugDL
                  Log_Message("Fill red:%f green:%f blue:%f alpha:%f",FillRed,FillGreen,FillBlue,FillAlpha);
                  #endif

                  if(FILLCOLOR!=0xFFFCFFFC)
                  {
                     CIMG_SWAP=1;
                  }

                  IMMINC
            case G_SETTIMG:
                  TILE_Format=(CommandLow&0x00E00000)>>21;
                  TILE_Size=(CommandLow&0x00180000)>>19;
                  TILE_Width=(CommandLow&0x00000FFF)+1;
                  TILE_CFB=CommandHigh;
                  #ifdef DebugDL
                  Log_Message("G_SETTIMG : Format: %d Size: %d Width: %d CFB: %08X",TILE_Format,TILE_Size,TILE_Width,TILE_CFB);
                  #endif
                  TILE_CFB=SEGMENT[(CommandHigh>>24)&0x0F]+CommandHigh&0xFFFFFF;
                  #ifdef DebugDL
                  Log_Message("...         after segment CFB: %08X",TILE_CFB);
                  #endif

                  IMMINC
            case G_SETTILE:
                  RDP_TN=(CommandHigh&0x07000000)>>24;
                  RDP_Tiles[RDP_TN].Format=(CommandLow>>21)&0x7;
                  RDP_Tiles[RDP_TN].TexelSize=(CommandLow>>19)&0x3;
                  RDP_Tiles[RDP_TN].LineSize=(CommandLow>>9)&0x1FF;
                  RDP_Tiles[RDP_TN].TMemAddress=(CommandLow)&0x1FF;
                  RDP_Tiles[RDP_TN].Palette=(CommandHigh>>20)&0xF;
                  RDP_Tiles[RDP_TN].CMT=(CommandHigh>>18)&0x3;
                  RDP_Tiles[RDP_TN].MaskT=(CommandHigh>>14)&0xF;
                  RDP_Tiles[RDP_TN].ShiftT=(CommandHigh>>10)&0xF;
                  RDP_Tiles[RDP_TN].CMS=(CommandHigh>>8)&0x3;
                  RDP_Tiles[RDP_TN].MaskS=(CommandHigh>>4)&0xF;
                  RDP_Tiles[RDP_TN].ShiftS=(CommandHigh)&0xF;
                  RDP_Tiles[RDP_TN].SetTileLo=CommandLow;
                  RDP_Tiles[RDP_TN].SetTileHi=CommandHigh;
                  #ifdef DebugDL
                  Log_Message("G_SETTILE : Tile: %d Format: %d TexelSize:%d\n"
                              "            LineSize:%d TMemAddress:%d\n"
                              "            Palette:%d\n"
                              "            CMS:%d CMT:%d\n"
                              "            MaskS:%d MastT:%d\n"
                              "            ShiftS:%d ShiftT:%d",
                              RDP_TN,RDP_Tiles[RDP_TN].Format, RDP_Tiles[RDP_TN].TexelSize, 
                              RDP_Tiles[RDP_TN].LineSize, RDP_Tiles[RDP_TN].TMemAddress,
                              RDP_Tiles[RDP_TN].Palette,
                              RDP_Tiles[RDP_TN].CMS,RDP_Tiles[RDP_TN].CMT,
                              RDP_Tiles[RDP_TN].MaskS,RDP_Tiles[RDP_TN].MaskT,
                              RDP_Tiles[RDP_TN].ShiftS,RDP_Tiles[RDP_TN].ShiftT
                              );
                  #endif
                  IMMINC
            case G_SETTILESIZE:
                  RDP_TN=(CommandHigh&0x07000000)>>24;
                  RDP_Tiles[RDP_TN].ULS=(CommandLow&0x00FFF000)>>14;
                  RDP_Tiles[RDP_TN].ULT=(CommandLow&0x00000FFF)>>2;
                  RDP_Tiles[RDP_TN].LRS=(CommandHigh&0x00FFF000)>>14;
                  RDP_Tiles[RDP_TN].LRT=(CommandHigh&0x00000FFF)>>2;
                  RDP_Tiles[RDP_TN].SizeX=RDP_Tiles[RDP_TN].LRS-RDP_Tiles[RDP_TN].ULS+1;
                  RDP_Tiles[RDP_TN].SizeY=RDP_Tiles[RDP_TN].LRT-RDP_Tiles[RDP_TN].ULT+1;

                  #ifdef DebugDL
                  Log_Message("SETTILESIZE TILE:%d S1:%d T1:%d S2:%d T2:%d",RDP_TN,RDP_Tiles[RDP_TN].ULS,RDP_Tiles[RDP_TN].ULT,RDP_Tiles[RDP_TN].LRS,RDP_Tiles[RDP_TN].LRT);
                  #endif
                  IMMINC
            case G_LOADTLUT:
                  TLUT_DRAM=TILE_CFB;
                  TLUT_TMEM=RDP_Tiles[(CommandHigh&0x07000000)>>24].TMemAddress*8;
                  TLUT_COUNT=((CommandHigh>>14)&0x3FF)+1;
                  memcpy(&TMEM[TLUT_TMEM],RDRAM+(0x1FFFFFFF&TLUT_DRAM),TLUT_COUNT*2);
                  #ifdef DebugDL
                  Log_Message("LOADTLUT DRAM:%08X TMEM:%04X COUNT:%d",TLUT_DRAM,TLUT_TMEM,TLUT_COUNT);
                  #endif
                  IMMINC
            case G_LOADTILE:
                  RDP_TN=(CommandHigh&0x07000000)>>24;
                  RDP_Tiles[RDP_TN].ULS=(CommandLow&0x00FFF000)>>14;
                  RDP_Tiles[RDP_TN].ULT=(CommandLow&0x00000FFF)>>2;
                  RDP_Tiles[RDP_TN].LRS=(CommandHigh&0x00FFF000)>>14;
                  RDP_Tiles[RDP_TN].LRT=(CommandHigh&0x00000FFF)>>2;
                  RDP_Tiles[RDP_TN].TILEULS=(CommandLow&0x00FFF000)>>14;
                  RDP_Tiles[RDP_TN].TILEULT=(CommandLow&0x00000FFF)>>2;
                  RDP_Tiles[RDP_TN].TILELRS=(CommandHigh&0x00FFF000)>>14;
                  RDP_Tiles[RDP_TN].TILELRT=(CommandHigh&0x00000FFF)>>2;         
                  #ifdef DebugDL
                  Log_Message("LOADTILE TILE:%d S1:%d T1:%d S2:%d T2:%d",RDP_TN,RDP_Tiles[RDP_TN].ULS,RDP_Tiles[RDP_TN].ULT,RDP_Tiles[RDP_TN].LRS,RDP_Tiles[RDP_TN].LRT);
                  #endif
                  LoadTile(RDP_TN);
                  IMMINC
            case G_TEXRECT:
//                  CIMG_SWAP=1;
                  RECT_X1=(CommandHigh&0x00FFF000)>>12;
                  RECT_Y1=(CommandHigh&0x00000FFF);
                  RECT_X2=((CommandLow&0x00FFF000)>>12);
                  RECT_Y2=((CommandLow&0x00000FFF));
                  RECT_X1/=4;
                  RECT_X2/=4;
                  RECT_Y1/=4;
                  RECT_Y2/=4;

                  RECT_TILE=(CommandHigh>>24)&0x7;
                  dl+=8;
                  CommandLow=Flip32( *(DWORD *)(RDRAM+dl) );
                  CommandHigh=Flip32( *(DWORD *)(RDRAM+dl+4) );
                  RECT_S=(CommandHigh>>16)&0xFFFF;if(RECT_S&0x8000)RECT_S-=0x10000;
                  RECT_T=(CommandHigh)&0xFFFF;if(RECT_T&0x8000)RECT_T-=0x10000;
                  dl+=8;
                  CommandLow=Flip32( *(DWORD *)(RDRAM+dl) );
                  CommandHigh=Flip32( *(DWORD *)(RDRAM+dl+4) );
                  RECT_DSDX=(CommandHigh>>16)&0xFFFF;if(RECT_DSDX&0x8000)RECT_DSDX-=0x10000;
                  RECT_DTDY=(CommandHigh)&0xFFFF;if(RECT_DTDY&0x8000)RECT_DTDY-=0x10000;
                  #ifdef DebugDL
                  Log_Message("G_TEXRECT Tile:%d X1:%d Y1:%d X2:%d Y2:%d",RECT_TILE,RECT_X1,RECT_Y1,RECT_X2,RECT_Y2);
                  #endif
                  #ifdef DebugDL
                  Log_Message("...       S:%d T:%d DSDX:%d DTDY:%d",RECT_S,RECT_T,RECT_DSDX,RECT_DTDY);
                  #endif

                  RECT_YSIZE=RECT_Y2-RECT_Y1+1;
                  RECT_XSIZE=RECT_X2-RECT_X1+1;

                  LoadTileTMEM(RECT_TILE);

                  if(CYCLETYPE==2)
                  {
                  RECT_DSDX=RECT_DSDX/4;
                  }

                  RECT_U1=(RECT_S/1024);
                  RECT_U2=(RECT_S/1024+RECT_XSIZE*RECT_DSDX/1024)-1;
                  RECT_V1=(RECT_T/1024);
                  RECT_V2=(RECT_T/1024+RECT_YSIZE*RECT_DTDY/1024)-1;

                  #ifdef DebugDL
                  Log_Message("BEFORE SCALE U1:%f V1:%f U2:%f V2:%f",RECT_U1,RECT_V1,RECT_U2,RECT_V2);
                  #endif

                  RECT_U1=RECT_U1/RDP_Tiles[RECT_TILE].GLX;
                  RECT_U2=RECT_U2/RDP_Tiles[RECT_TILE].GLX;
                  RECT_V1=RECT_V1/RDP_Tiles[RECT_TILE].GLY;
                  RECT_V2=RECT_V2/RDP_Tiles[RECT_TILE].GLY;

                  #ifdef DebugDL
                  Log_Message("GLX:%d GLY:%d",RDP_Tiles[RECT_TILE].GLX,RDP_Tiles[RECT_TILE].GLY);
                  #endif

                  #ifdef DebugDL
                  Log_Message("GlTexRect: X1:%d Y1:%d X2:%d Y2:%d",RECT_X1,RECT_Y1,RECT_X2,RECT_Y2);
                  #endif
                  #ifdef DebugDL
                  Log_Message("           U1:%f V1:%f U2:%f V2:%f",RECT_U1,RECT_V1,RECT_U2,RECT_V2);
                  #endif

                  UpdateStates(); 
                  IMMINC
            case G_LOADBLOCK:
                  RDP_TN=(CommandHigh&0x07000000)>>24;
                  LOADBLOCKSIZE=(CommandHigh&0x00FFF000)>>12;
                  LOADBLOCKLINE=(CommandHigh&0x00000FFF)>>0;
                  #ifdef DebugDL
                  Log_Message("LOADBLOCK TILE:%d SIZE:%d LINE:%d",RDP_TN,LOADBLOCKSIZE,LOADBLOCKLINE);
                  #endif
                  LoadBlock(RDP_TN);
                  IMMINC         
            case G_SETZIMG:
                  ZIMG=CommandHigh;
                  #ifdef DebugDL
                  Log_Message("G_SETZIMG %X",ZIMG);
                  #endif
                  IMMINC
            case G_MOVEWORD:
                  switch((CommandLow>>16)&0xFF)
                  {
                     case G_MW_MATRIX:
                           #ifdef DebugDL
                           Log_Message("G_MOVEWORD MATRIX");
                           #endif
                           break;
                     case G_MW_NUMLIGHT:
                           #ifdef DebugDL
                           Log_Message("G_MOVEWORD NUMLIGHT");
                           #endif
                           break;
                     case G_MW_CLIP:
                           #ifdef DebugDL
                           Log_Message("G_MOVEWORD CLIP");
                           #endif
                           break;
                     case G_MW_SEGMENT:
                           #ifdef DebugDL
                           Log_Message("G_MOVEWORD SEGMENT");
                           #endif
                           #ifdef DebugDL
                           Log_Message("SEGMENT[%X]=%X",((CommandLow)&0xFFFF)/4,CommandHigh);
                           #endif
                           SEGMENT[((CommandLow)&0xFFFF)/4]=CommandHigh;
                           break;
                     case G_MW_FOG:
                           #ifdef DebugDL
                           Log_Message("G_MOVEWORD FOG");
                           #endif
                           break;
                     case G_MW_LIGHTCOL:
                           #ifdef DebugDL
                           Log_Message("G_MOVEWORD LIGHTCOL");
                           #endif
                           break;
                     case G_MW_FORCEMTX:
                           #ifdef DebugDL
                           Log_Message("G_MOVEWORD FORCEMTX");
                           #endif
                           break;
                     case G_MW_PERSPNORM:
                           #ifdef DebugDL
                           Log_Message("G_MOVEWORD PERSPNORM");
                           #endif
                           break;
                     default:
                           Log_Message("G_MOVEWORD ???");
                  }
                  IMMINC            
            case G_MTX:
                  MATRIXSRC=SEGMENT[(CommandHigh>>24)&0x0F]+CommandHigh&0xFFFFFF;
                  MATRIXSIZE=(((CommandLow>>8)&0xFF)+1)*8;
                  MATRIXFLAG=(CommandLow&0xFF)^G_MTX_PUSH;
                  
                  #ifdef DebugDL
                  Log_Message("G_MTX SRC:%X SIZE:%X FLAG:%X",MATRIXSRC,MATRIXSIZE,MATRIXFLAG);
                  #endif
                  MATRIX(MATRIXSRC,MATRIXSIZE,MATRIXFLAG);
                  IMMINC
            case G_VTX:

                  VTXSRC=SEGMENT[(CommandHigh>>24)&0x0F]+CommandHigh&0xFFFFFF;
//                  VTXSIZE=(CommandLow&0x03FF)+16;
//                  VTXNUMBER=((CommandLow&0xFC00)>>10);
//                  VTXSTART=((CommandLow&0xFF0000)>>17);
                  VTXSIZE=((CommandLow&0x0FF)>>1)*16;
                  VTXNUMBER=((CommandLow&0x00FF)>>1);
                  VTXSTART=0; //((CommandLow&0xFF0000)>>17);

                  #ifdef DebugDL
                  Log_Message("G_VTX SRC:%X SIZE:%X NUMBER:%X START:%X",VTXSRC,VTXSIZE,VTXNUMBER,VTXSTART);
                  Log_Message("VTX %08X-%08X",CommandLow,CommandHigh);
                  #endif
                  VTX(VTXSRC,VTXSIZE,VTXNUMBER,VTXSTART);
                  IMMINC
            case G_TRI1:
                  TRI1FLAG=0; //CommandHigh>>24;
                  TRI1V0=((CommandLow>>16)&0xFF)/2;
                  TRI1V1=((CommandLow>>8)&0xFF)/2;
                  TRI1V2=((CommandLow)&0xFF)/2;
                  #ifdef DebugDL
                  Log_Message("TRI1 %08X-%08X",CommandLow,CommandHigh);
                  Log_Message("G_TRI1 FLAG:%X V0:%X V1:%X V2:%X",TRI1FLAG,TRI1V0,TRI1V1,TRI1V2);
                  #endif
                  TRI1(TRI1FLAG,TRI1V0,TRI1V1,TRI1V2);
                  IMMINC
            case G_POPMTX:
                  glMatrixMode(GL_MODELVIEW);
                  glPopMatrix();
                  #ifdef DebugDL
                  Log_Message("G_POPMTX %X",CommandHigh);
                  #endif
                  IMMINC
            case G_TRI2:
                  TRI1FLAG=0;
                  TRI1V0=((CommandLow>>16)&0xFF)/2;
                  TRI1V1=((CommandLow>>8)&0xFF)/2;
                  TRI1V2=((CommandLow)&0xFF)/2;
                  #ifdef DebugDL
                  Log_Message("TRI2 %08X-%08X",CommandLow,CommandHigh);
                  Log_Message("G_TRI2 FLAG:%X V0:%X V1:%X V2:%X",TRI1FLAG,TRI1V0,TRI1V1,TRI1V2);
                  #endif
                  TRI1(TRI1FLAG,TRI1V0,TRI1V1,TRI1V2);
                  TRI1V0=((CommandHigh>>16)&0xFF)/2;
                  TRI1V1=((CommandHigh>>8)&0xFF)/2;
                  TRI1V2=((CommandHigh)&0xFF)/2;
                  #ifdef DebugDL
                  Log_Message("G_TRI2 FLAG:%X V0:%X V1:%X V2:%X",TRI1FLAG,TRI1V0,TRI1V1,TRI1V2);
                  #endif
                  TRI1(TRI1FLAG,TRI1V0,TRI1V1,TRI1V2);
                  IMMINC
            case G_BRANCH_Z:
                  if(((SavedHalf>>24)&0x0F)!=6 && ((SavedHalf>>24)&0x0F)!=3)
                  {
                      Log_Message("G_BRANCH_Z to bank %d not supported",((SavedHalf>>24)&0x0F));
                      IMMINC
                  }
                  
                  #ifdef DebugDL
                      #ifdef DebugDL
                      Log_Message("G_BRANCH_Z %X", 
                          SEGMENT[(SavedHalf>>24)&0x0F]+SavedHalf&0xFFFFFF);
                      #endif
                  #endif
                  dl=SEGMENT[(SavedHalf>>24)&0x0F]+SavedHalf&0xFFFFFF;
                  // Compensate for IMMINC...
                  dl-=8;
                  IMMINC
            default:
               Log_Message("Unknown DL%08X   %08X-%08X",dl,CommandLow,CommandHigh);
               dl += 8;
         }
   }
return(0);
}
