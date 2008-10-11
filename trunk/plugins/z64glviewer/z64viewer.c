#ifdef WIN32
#include <windows.h>
#endif
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include "SDL.h"
#include "mesamatrix.h"
#include <dlist.h>

#define true 1
#define false 0

int __quit = 0;
int __quited = 0;
static GLint T0 = 0;
static GLint Frames = 0;

int entrypoint;
int datasize=8*1024*1024;
unsigned char *databuffer;
int distance=4000;

#define DWORD unsigned int
extern unsigned char *RDRAM;
extern int ProcessDisplayListSW(DWORD dl);
extern DWORD SEGMENT[16];

static GLfloat camera[16];

extern void Start_Log(void);
extern void Stop_Log(void);
int mode=0;

unsigned long Flip32 (unsigned long ToFlip) {
    return ((ToFlip&0xFF000000)>>24)|((ToFlip&0x00FF0000)>>8)|
        ((ToFlip&0x0000FF00)<<8)|((ToFlip&0x000000FF)<<24);
}

void waitTime(int ms)
{
#ifdef WIN32
  Sleep(ms);
#else
  usleep(ms*1000);
#endif
}

DrawStructure(int LinksOffset, int position, int X1, int Y1, int Z1, int level)
{
    DWORD StructurePtr;
    DWORD DlistPtr,DlistPtr2=0;
    int i;
    StructurePtr = Flip32(*(DWORD *)(RDRAM+LinksOffset+position*4));
    for(i=0;i<level;i++) printf(" ");
    printf("StructurePtr %X\n",StructurePtr);
    StructurePtr&=0xFFFFFF;
    short X,Y,Z;
    unsigned char A,B;
    X=((RDRAM[StructurePtr+0]<<8)+RDRAM[StructurePtr+1]);
    Y=((RDRAM[StructurePtr+2]<<8)+RDRAM[StructurePtr+3]);
    Z=((RDRAM[StructurePtr+4]<<8)+RDRAM[StructurePtr+5]);
    A=RDRAM[StructurePtr+6];
    B=RDRAM[StructurePtr+7];
    DlistPtr=Flip32(*(DWORD *)(RDRAM+StructurePtr+8));
    DlistPtr&=0xFFFFFF;
    if(mode==2)
    {
        DlistPtr2=Flip32(*(DWORD *)(RDRAM+StructurePtr+12));
        DlistPtr2&=0xFFFFFF;
    }
    if(DlistPtr!=0)
    {
        glPushMatrix();
        for(i=0;i<level;i++) printf(" ");
        printf("[%d] X %d Y %d Z %d A: %d B: %d DL:%X DL:%X\n",position, X,Y,Z,A,B,DlistPtr,DlistPtr2);
        glTranslatef(X1,Y1,Z1);
        for(i=0;i<level;i++) printf(" ");
        printf("[%d] X1 %d Y1 %d Z1 %d DL: %08X DL: %08X\n",position, X1,Y1,Z1,DlistPtr,DlistPtr2);
        ProcessDisplayListSW(DlistPtr);
        glPopMatrix();
        
        glPushMatrix();
        glTranslatef(X1,Y1,Z1);
        if(mode==2 && DlistPtr2!=0)
        {
            ProcessDisplayListSW(DlistPtr2);
        }
        glPopMatrix();
    }
    if(A!=255)
    {
        DrawStructure(LinksOffset, A, X1+X, Y1+Y, Z1+Z, level+1);
    }
    if(B!=255)
    {
        DrawStructure(LinksOffset, B, X1+X, Y1+Y, Z1+Z, level+1);
    }
}

DrawLinks()
{
  DWORD LinksOffset;
  DWORD LinksCount;
  int i;
  
  
  
  LinksOffset=Flip32( *(DWORD *)(RDRAM+entrypoint) );
  LinksOffset&=0xFFFFFF;
  LinksCount=RDRAM[entrypoint+4];
  printf("LinksOffset %X LinksCount %d\n",LinksOffset,LinksCount);
  
  DrawStructure(LinksOffset, 0, 0, 0, 0, 0);
  
}

LoadResourceZ(char *name, int offset, int segment)
{
  int resourcesize;
  printf("Loading %s to segment %d (rdram %08X)\n",name,segment,offset);
  FILE *infile=fopen(name, "rb");
  fseek(infile, 0, SEEK_END);
  resourcesize=ftell(infile);
  fseek(infile, 0, SEEK_SET);
  fread(&databuffer[offset], resourcesize, 1, infile);
  fclose(infile);
  SEGMENT[segment]=offset;
}

LoadResourceZ_bin(unsigned char *data, int resourcesize, int offset, int segment)
{
  memcpy(&databuffer[offset], data, resourcesize );
  SEGMENT[segment]=offset;
}

static void
draw(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(camera[3*4+0], camera[3*4+1], camera[3*4+2],
                camera[3*4+0]+camera[0*4+0],
                camera[3*4+1]+camera[0*4+1],
                camera[3*4+2]+camera[0*4+2],
                camera[2*4+0],
                camera[2*4+1],
                camera[2*4+2]);

  glPushMatrix();
  // draw z64 stuff
  RDRAM=databuffer;
  if(mode==0)
  {
      ProcessDisplayListSW(entrypoint);
  }
  else if(mode>0)
  {
      DrawLinks();
  }
     
  glPopMatrix();

  SDL_GL_SwapBuffers();
  waitTime(20);

  Frames++;
}

static void
idle(void)
{
}

/* new window size or exposure */
static void
reshape(int width, int height)
{
  GLfloat h = (GLfloat) height / (GLfloat) width;

  glViewport(0, 0, (GLint) width, (GLint) height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1.0, 1.0, -h, h, 1.0, 320000.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  
  Start_Log();
  RDRAM=databuffer;
  if(mode==0)
  {
      ProcessDisplayListSW(entrypoint);
  }
  else if(mode>0)
  {
      DrawLinks();
  }
  Stop_Log();
}

static void
init(int argc, char *argv[])
{

  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
      exit(1);
  }

  if (argc < 3)
  {
      printf("Usage: z64viewer file entrypoint [-glinfo]\n");
      exit(1);
  }

  databuffer=malloc(8*1024*1024);
  if(databuffer==NULL)
  {
      printf("Could not allocate memory for data (8M)\n");
      exit(2);
  }
  memset(databuffer,0, 8*1024*1024);
      
  printf("argv 1: %s argv 2: %s\n",argv[1],argv[2]);
  sscanf(argv[2], "%x", &entrypoint);
  printf("entry point : %x\n",entrypoint);

  LoadResourceZ(argv[1],0,0);
  LoadResourceZ("gameplay_keep.zdata", 1024*1024, 4);
  
  if (argc > 3 && strcmp(argv[3], "-glinfo")==0) {
     printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
     printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
     printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
     printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
  }
  else
  {
      if(argc>3) LoadResourceZ(argv[3], 2*1024*1024, 2);
  }

    camera[0] = 1; camera[1]=0; camera[2]=0; camera[3]=0; // Forward
    camera[4] = 0; camera[5]=0; camera[6]=1; camera[7]=0; // Right
    camera[8] = 0; camera[9]=1; camera[10]=0; camera[11]=0; // Up
    camera[12] = -1000; camera[13]=0; camera[14]=0; camera[15]=0; // Position
}

int _main(int argc, char *argv[])
{
  SDL_Surface *screen;
  int done;
  Uint8 *keys;
  int mousebutton,mouseX,mouseY;
  int grabbed=false;
  SDL_Init(SDL_INIT_VIDEO);

  screen = SDL_SetVideoMode(640, 480, 32, SDL_OPENGL|SDL_RESIZABLE);
  if ( ! screen ) {
    fprintf(stderr, "Couldn't set 300x300 GL video mode: %s\n", SDL_GetError());
    SDL_Quit();
    exit(2);
  }
  SDL_WM_SetCaption("Z64Viewer", "z64viewer");
  SDL_ShowCursor(SDL_DISABLE);
  SDL_WM_GrabInput(SDL_GRAB_ON);
  grabbed=true;
  
  init(argc, argv);
  reshape(screen->w, screen->h);
  done = 0;
  while ( ! done ) {
    SDL_Event event;

    idle();
    while ( SDL_PollEvent(&event) ) {
      switch(event.type) {
        case SDL_VIDEORESIZE:
          screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 16,
                                    SDL_OPENGL|SDL_RESIZABLE);
          if ( screen ) {
            reshape(screen->w, screen->h);
          } else {
            printf("Problem...\n");
          }
          break;

        case SDL_QUIT:
          done = 1;
          break;
      }
    }
    keys = SDL_GetKeyState(NULL);

    if ( keys[SDLK_ESCAPE] ) {
      if(grabbed)
      {
          SDL_WM_GrabInput(SDL_GRAB_OFF);
          SDL_ShowCursor(SDL_ENABLE);
          grabbed=false;          
          waitTime(200); // Wait a bit so we don't exit by mistake
      }
      else
      {      
      done = 1;
      }
    }

    mousebutton=SDL_GetRelativeMouseState(&mouseX,&mouseY);
    
    GLfloat viewMatrix[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
    
    if(grabbed)
    {
        if((mousebutton&1) && (mousebutton&4))
        {
            matrixRotate(viewMatrix, mouseX/600.0f*360.0f, camera[0*4+0],camera[0*4+1],camera[0*4+2]);
            
            multM4Vect(&camera[0*4], viewMatrix, &camera[0*4]);
            multM4Vect(&camera[1*4], viewMatrix, &camera[1*4]);
            multM4Vect(&camera[2*4], viewMatrix, &camera[2*4]);
        }
        else
        {
            matrixRotate(viewMatrix, -mouseX/600.0f*360.0f, camera[2*4+0], camera[2*4+1],camera[2*4+2]);
            matrixRotate(viewMatrix, -mouseY/600.0f*360.0f, camera[1*4+0], camera[1*4+1],camera[1*4+2]);
            
            multM4Vect(&camera[0*4], viewMatrix, &camera[0*4]);
            multM4Vect(&camera[1*4], viewMatrix, &camera[1*4]);
        
            if(mousebutton&1)
            {
                camera[3*4+0]+=camera[0*4+0]*20;
                camera[3*4+1]+=camera[0*4+1]*20;
                camera[3*4+2]+=camera[0*4+2]*20;
                camera[3*4+3]+=camera[0*4+3]*20;
            }
    
          
            if(mousebutton&4)
            {
                camera[3*4+0]-=camera[0*4+0]*20;
                camera[3*4+1]-=camera[0*4+1]*20;
                camera[3*4+2]-=camera[0*4+2]*20;
                camera[3*4+3]-=camera[0*4+3]*20;
            }


            
            if(keys[SDLK_d])
            {
                camera[3*4+0]+=camera[1*4+0]*20;
                camera[3*4+1]+=camera[1*4+1]*20;
                camera[3*4+2]+=camera[1*4+2]*20;
                camera[3*4+3]+=camera[1*4+3]*20;
            }
            if(keys[SDLK_a])
            {
                camera[3*4+0]-=camera[1*4+0]*20;
                camera[3*4+1]-=camera[1*4+1]*20;
                camera[3*4+2]-=camera[1*4+2]*20;
                camera[3*4+3]-=camera[1*4+3]*20;
            }
            if(keys[SDLK_w])
            {
                camera[3*4+0]+=camera[2*4+0]*20;
                camera[3*4+1]+=camera[2*4+1]*20;
                camera[3*4+2]+=camera[2*4+2]*20;
                camera[3*4+3]+=camera[2*4+3]*20;
            }
            if(keys[SDLK_s])
            {
                camera[3*4+0]-=camera[2*4+0]*20;
                camera[3*4+1]-=camera[2*4+1]*20;
                camera[3*4+2]-=camera[2*4+2]*20;
                camera[3*4+3]-=camera[2*4+3]*20;
            }

//FASTER
            if(mousebutton&4 && mousebutton&2)
            {
                camera[3*4+0]-=camera[0*4+0]*200;
                camera[3*4+1]-=camera[0*4+1]*200;
                camera[3*4+2]-=camera[0*4+2]*200;
                camera[3*4+3]-=camera[0*4+3]*200;
            }
	    if(mousebutton&1 && mousebutton&2)
            {
                camera[3*4+0]+=camera[0*4+0]*200;
                camera[3*4+1]+=camera[0*4+1]*200;
                camera[3*4+2]+=camera[0*4+2]*200;
                camera[3*4+3]+=camera[0*4+3]*200;
            }

            if(keys[SDLK_i])
            {
                camera[3*4+0]+=camera[2*4+0]*200;
                camera[3*4+1]+=camera[2*4+1]*200;
                camera[3*4+2]+=camera[2*4+2]*200;
                camera[3*4+3]+=camera[2*4+3]*200;
            }
	    if(keys[SDLK_k])
            {
                camera[3*4+0]-=camera[2*4+0]*200;
                camera[3*4+1]-=camera[2*4+1]*200;
                camera[3*4+2]-=camera[2*4+2]*200;
                camera[3*4+3]-=camera[2*4+3]*200;
            }
 	    
            if(keys[SDLK_j])
            {
                camera[3*4+0]-=camera[1*4+0]*200;
                camera[3*4+1]-=camera[1*4+1]*200;
                camera[3*4+2]-=camera[1*4+2]*200;
                camera[3*4+3]-=camera[1*4+3]*200;
            }

            if(keys[SDLK_l])
            {
                camera[3*4+0]+=camera[1*4+0]*200;
                camera[3*4+1]+=camera[1*4+1]*200;
                camera[3*4+2]+=camera[1*4+2]*200;
                camera[3*4+3]+=camera[1*4+3]*200;
            }
//FASTER END 

        }            
    }
    else
    {
        if(mousebutton)
        {
            SDL_ShowCursor(SDL_DISABLE);
            SDL_WM_GrabInput(SDL_GRAB_ON);
            grabbed=true;
        }
    }    
    draw();
  }
  SDL_Quit();
  return 0;             /* ANSI C requires main to return int. */
}






void draw_from_data ( struct Data * k )
{
  SDL_Surface *screen;
  int done;
  Uint8 *keys;
  int mousebutton,mouseX,mouseY;
  int grabbed=false;
  SDL_Init(SDL_INIT_VIDEO);

  screen = SDL_SetVideoMode(640, 480, 32, SDL_OPENGL|SDL_RESIZABLE);
  if ( ! screen ) {
    fprintf(stderr, "Couldn't set 300x300 GL video mode: %s\n", SDL_GetError());
    SDL_Quit();
    exit(2);
  }
  SDL_WM_SetCaption("Z64Viewer", "z64viewer");
  SDL_ShowCursor(SDL_DISABLE);
  SDL_WM_GrabInput(SDL_GRAB_ON);
  grabbed=true;
  
  
  GLenum err = glewInit();
  
  databuffer=malloc(8*1024*1024);
  
  memset(databuffer,0, 8*1024*1024);
      
  entrypoint = k->ep;

  LoadResourceZ_bin(k->data, k->size, 0, 0);
  LoadResourceZ("C:\\gzrt-trunk\\plugins\\z64glviewer\\gameplay_keep.zdata", 1024*1024, 4);
  

    camera[0] = 1; camera[1]=0; camera[2]=0; camera[3]=0; // Forward
    camera[4] = 0; camera[5]=0; camera[6]=1; camera[7]=0; // Right
    camera[8] = 0; camera[9]=1; camera[10]=0; camera[11]=0; // Up
    camera[12] = -1000; camera[13]=0; camera[14]=0; camera[15]=0; // Position
  reshape(screen->w, screen->h);
  done = 0;
  while ( ! done ) {
    SDL_Event event;

    idle();
    while ( SDL_PollEvent(&event) ) {
      switch(event.type) {
        case SDL_VIDEORESIZE:
          screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 16,
                                    SDL_OPENGL|SDL_RESIZABLE);
          if ( screen ) {
            reshape(screen->w, screen->h);
          } else {
            printf("Problem...\n");
          }
          break;
      }
    }

    keys = SDL_GetKeyState(NULL);

    if ( keys[SDLK_ESCAPE] ) {
      if(grabbed)
      {
          SDL_WM_GrabInput(SDL_GRAB_OFF);
          SDL_ShowCursor(SDL_ENABLE);
          grabbed=false;          
          waitTime(200); // Wait a bit so we don't exit by mistake
      }
      else
      {      
      done = 1;
      }
    }

    mousebutton=SDL_GetRelativeMouseState(&mouseX,&mouseY);
    
    GLfloat viewMatrix[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
    
    if(grabbed)
    {
        if((mousebutton&1) && (mousebutton&4))
        {
            matrixRotate(viewMatrix, mouseX/600.0f*360.0f, camera[0*4+0],camera[0*4+1],camera[0*4+2]);
            
            multM4Vect(&camera[0*4], viewMatrix, &camera[0*4]);
            multM4Vect(&camera[1*4], viewMatrix, &camera[1*4]);
            multM4Vect(&camera[2*4], viewMatrix, &camera[2*4]);
        }
        else
        {
            matrixRotate(viewMatrix, -mouseX/600.0f*360.0f, camera[2*4+0], camera[2*4+1],camera[2*4+2]);
            matrixRotate(viewMatrix, -mouseY/600.0f*360.0f, camera[1*4+0], camera[1*4+1],camera[1*4+2]);
            
            multM4Vect(&camera[0*4], viewMatrix, &camera[0*4]);
            multM4Vect(&camera[1*4], viewMatrix, &camera[1*4]);
        
            if(mousebutton&1)
            {
                camera[3*4+0]+=camera[0*4+0]*20;
                camera[3*4+1]+=camera[0*4+1]*20;
                camera[3*4+2]+=camera[0*4+2]*20;
                camera[3*4+3]+=camera[0*4+3]*20;
            }
    
          
            if(mousebutton&4)
            {
                camera[3*4+0]-=camera[0*4+0]*20;
                camera[3*4+1]-=camera[0*4+1]*20;
                camera[3*4+2]-=camera[0*4+2]*20;
                camera[3*4+3]-=camera[0*4+3]*20;
            }


            
            if(keys[SDLK_d])
            {
                camera[3*4+0]+=camera[1*4+0]*20;
                camera[3*4+1]+=camera[1*4+1]*20;
                camera[3*4+2]+=camera[1*4+2]*20;
                camera[3*4+3]+=camera[1*4+3]*20;
            }
            if(keys[SDLK_a])
            {
                camera[3*4+0]-=camera[1*4+0]*20;
                camera[3*4+1]-=camera[1*4+1]*20;
                camera[3*4+2]-=camera[1*4+2]*20;
                camera[3*4+3]-=camera[1*4+3]*20;
            }
            if(keys[SDLK_w])
            {
                camera[3*4+0]+=camera[2*4+0]*20;
                camera[3*4+1]+=camera[2*4+1]*20;
                camera[3*4+2]+=camera[2*4+2]*20;
                camera[3*4+3]+=camera[2*4+3]*20;
            }
            if(keys[SDLK_s])
            {
                camera[3*4+0]-=camera[2*4+0]*20;
                camera[3*4+1]-=camera[2*4+1]*20;
                camera[3*4+2]-=camera[2*4+2]*20;
                camera[3*4+3]-=camera[2*4+3]*20;
            }

//FASTER
            if(mousebutton&4 && mousebutton&2)
            {
                camera[3*4+0]-=camera[0*4+0]*200;
                camera[3*4+1]-=camera[0*4+1]*200;
                camera[3*4+2]-=camera[0*4+2]*200;
                camera[3*4+3]-=camera[0*4+3]*200;
            }
	    if(mousebutton&1 && mousebutton&2)
            {
                camera[3*4+0]+=camera[0*4+0]*200;
                camera[3*4+1]+=camera[0*4+1]*200;
                camera[3*4+2]+=camera[0*4+2]*200;
                camera[3*4+3]+=camera[0*4+3]*200;
            }

            if(keys[SDLK_i])
            {
                camera[3*4+0]+=camera[2*4+0]*200;
                camera[3*4+1]+=camera[2*4+1]*200;
                camera[3*4+2]+=camera[2*4+2]*200;
                camera[3*4+3]+=camera[2*4+3]*200;
            }
	    if(keys[SDLK_k])
            {
                camera[3*4+0]-=camera[2*4+0]*200;
                camera[3*4+1]-=camera[2*4+1]*200;
                camera[3*4+2]-=camera[2*4+2]*200;
                camera[3*4+3]-=camera[2*4+3]*200;
            }
 	    
            if(keys[SDLK_j])
            {
                camera[3*4+0]-=camera[1*4+0]*200;
                camera[3*4+1]-=camera[1*4+1]*200;
                camera[3*4+2]-=camera[1*4+2]*200;
                camera[3*4+3]-=camera[1*4+3]*200;
            }

            if(keys[SDLK_l])
            {
                camera[3*4+0]+=camera[1*4+0]*200;
                camera[3*4+1]+=camera[1*4+1]*200;
                camera[3*4+2]+=camera[1*4+2]*200;
                camera[3*4+3]+=camera[1*4+3]*200;
            }
//FASTER END 

        }            
    }
    else
    {
        if(mousebutton)
        {
            SDL_ShowCursor(SDL_DISABLE);
            SDL_WM_GrabInput(SDL_GRAB_ON);
            grabbed=true;
        }
    }    
    draw();
	
	if(__quit)
		break;
  }
  SDL_Quit();
  __quited=1;
  return;             /* ANSI C requires main to return int. */
}
