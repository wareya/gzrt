
struct Data
{
    unsigned char * data;
    unsigned int    size;
    unsigned int    ep;
};

void Check_RSP ( void );

// ********************************* RDP **********************************

#define  G_SETCIMG               0xFF
#define  G_SETZIMG               0xFE
#define  G_SETTIMG               0xFD
#define  G_SETCOMBINE            0xFC
#define  G_SETENVCOLOR           0xFB
#define  G_SETPRIMCOLOR          0xFA
#define  G_SETBLENDCOLOR         0xF9
#define  G_SETFOGCOLOR           0xF8
#define  G_SETFILLCOLOR          0xF7
#define  G_FILLRECT              0xF6
#define  G_SETTILE               0xF5
#define  G_LOADTILE              0xF4
#define  G_LOADBLOCK             0xF3
#define  G_SETTILESIZE           0xF2
#define  G_LOADTLUT              0xF0
#define  G_RDPSETOTHERMODE       0xEF
#define  G_SETPRIMDEPTH          0xEE
#define  G_SETSCISSOR            0xED
#define  G_SETCONVERT            0xEC
#define  G_SETKEYR               0xEB
#define  G_SETKEYGB              0xEA
#define  G_RDPFULLSYNC           0xE9
#define  G_RDPTILESYNC           0xE8
#define  G_RDPPIPESYNC           0xE7
#define  G_RDPLOADSYNC           0xE6
#define  G_TEXRECTFLIP           0xE5
#define  G_TEXRECT               0xE4
#define  G_TRI_SHADE_TXTR_ZBUFF  0xCF
#define  G_TRI_SHADE_TXTR        0xCE
#define  G_TRI_SHADE_ZBUFF       0xCD
#define  G_TRI_SHADE             0xCC
#define  G_TRI_TXTR_ZBUFF        0xCB
#define  G_TRI_TXTR              0xCA
#define  G_TRI_FILL_ZBUFF        0xC9
#define  G_TRI_FILL              0xC8


// G_SETIMG fmt: set image formats
#define G_IM_FMT_RGBA   0
#define G_IM_FMT_YUV    1
#define G_IM_FMT_CI     2
#define G_IM_FMT_IA     3
#define G_IM_FMT_I      4
// G_SETIMG siz: set image pixel size
#define G_IM_SIZ_4b     0
#define G_IM_SIZ_8b     1
#define G_IM_SIZ_16b    2
#define G_IM_SIZ_32b    3

// G_SETCOMBINE: color combine modes
#define G_CCMUX_COMBINED          0
#define G_CCMUX_TEXEL0            1
#define G_CCMUX_TEXEL1            2
#define G_CCMUX_PRIMITIVE         3
#define G_CCMUX_SHADE             4
#define G_CCMUX_ENVIRONMENT       5
#define G_CCMUX_CENTER            6
#define G_CCMUX_SCALE             6
#define G_CCMUX_COMBINED_ALPHA    7
#define G_CCMUX_TEXEL0_ALPHA      8
#define G_CCMUX_TEXEL1_ALPHA      9
#define G_CCMUX_PRIMITIVE_ALPHA 10
#define G_CCMUX_SHADE_ALPHA      11
#define G_CCMUX_ENV_ALPHA        12
#define G_CCMUX_LOD_FRACTION     13
#define G_CCMUX_PRIM_LOD_FRAC    14
#define G_CCMUX_NOISE             7
#define G_CCMUX_K4                7
#define G_CCMUX_K5               15
#define G_CCMUX_1                 6
#define G_CCMUX_0                31

// G_SETCOMBINE: alpha combine modes
#define G_ACMUX_COMBINED         0
#define G_ACMUX_TEXEL0           1
#define G_ACMUX_TEXEL1           2
#define G_ACMUX_PRIMITIVE        3
#define G_ACMUX_SHADE            4
#define G_ACMUX_ENVIRONMENT      5
#define G_ACMUX_LOD_FRACTION     0
#define G_ACMUX_PRIM_LOD_FRAC    6
#define G_ACMUX_1                6
#define G_ACMUX_0                7



// G_SETOTHERMODE_H shift count
#define G_MDSFT_ALPHADITHER     4
#define  G_MDSFT_RGBDITHER       6
#define G_MDSFT_COMBKEY         8
#define  G_MDSFT_TEXTCONV        9
#define  G_MDSFT_TEXTFILT        12
#define G_MDSFT_TEXTLUT         14
#define G_MDSFT_TEXTLOD         16
#define G_MDSFT_TEXTDETAIL      17
#define  G_MDSFT_TEXTPERSP       19
#define  G_MDSFT_CYCLETYPE       20
#define  G_MDSFT_COLORDITHER     22
#define  G_MDSFT_PIPELINE        23

// G_SETOTHERMODE_L shift count
#define G_MDSFT_ALPHACOMPARE        0
#define G_MDSFT_ZSRCSEL         2
#define G_MDSFT_RENDERMODE      3
#define G_MDSFT_BLENDER         16


// G_DL flags
#define G_DL_PUSH                0x00
#define G_DL_NOPUSH              0x01

// Structures

typedef struct {
   short          x,y,z;
    unsigned short  flag;
   short          u,v;
   unsigned char  r,g,b,a;
} Vtx_t;

typedef struct {
   short          x,y,z;
    unsigned short  flag;
   short          u,v;
   signed char    nx,ny,nz; 
   unsigned char  a;
} Vtx_tn;

typedef struct {
   unsigned short x,y,z;
    unsigned short  flag;
   unsigned short u,v;
   unsigned char  r,g,b,a;
} Vtx_flip;


typedef union {
   Vtx_t          v;
   Vtx_tn         n;
   Vtx_flip       f;
} Vtx;

typedef struct {
  void *SourceImagePointer;
  void *TlutPointer;
  short Stride;
  short SubImageWidth;
  short SubImageHeight;
  char  SourceImageType;
  char  SourceImageBitSize;
  short SourceImageOffsetS;
  short SourceImageOffsetT;
  char dummy[4]; 
} uSprite_t;

typedef struct {
    unsigned char   flag;
    unsigned char   v[3];
} Tri;

typedef struct {
   short vscale[4];  
   short vtrans[4];  
} Vp_t;

typedef struct {
  unsigned char   r,g,b;
  char            pad1;
  unsigned char   rc,gc,bc;         /* copy of diffuse light value (rgba) */
  char            pad2;
  signed char     dirx,diry,dirz;   /* direction of light (normalized) */
  char            pad3;
} Light_t;

typedef struct {
  unsigned char   r,g,b;            /* ambient light value (rgba) */
  char            pad1;
  unsigned char   rc,rg,rb;         /* copy of ambient light value (rgba) */
  char            pad2;
} Ambient_t;

typedef struct {
  int    x1,y1,x2,y2;               /* texture offsets for highlight 1/2 */
} Hilite_t;



// ******************************** F3DEX2  ********************************

#define G_NOOP          0x00
#define G_RDPHALF_2     0xf1
#define G_SETOTHERMODE_H    0xe3
#define G_SETOTHERMODE_L    0xe2
#define G_RDPHALF_1     0xe1
#define G_SPNOOP        0xe0
#define G_ENDDL         0xdf
#define G_DL            0xde
#define G_LOAD_UCODE        0xdd
#define G_MOVEMEM       0xdc
#define G_MOVEWORD      0xdb
#define G_MTX           0xda
#define  G_GEOMETRYMODE     0xd9
#define G_POPMTX        0xd8
#define G_TEXTURE       0xd7
#define G_SUBMODULE     0xd6

#define G_VTX           0x01
#define G_MODIFYVTX     0x02
#define G_CULLDL        0x03
#define G_BRANCH_Z      0x04
#define G_TRI1          0x05
#define  G_TRI2       0x06
#define  G_LINE3D     0x07


// G_MTX: parameter flags
# define G_MTX_MODELVIEW    0x00    /* matrix types */
# define G_MTX_PROJECTION   0x04
# define G_MTX_MUL      0x00    /* concat or load */
# define G_MTX_LOAD     0x02
# define G_MTX_NOPUSH       0x00    /* push or not */
# define G_MTX_PUSH     0x01

// G_SETGEOMETRYMODE: flags
#define  G_ZBUFFER            0x00000001
#define  G_TEXTURE_ENABLE     0x00000002  /* Microcode use only */
#define  G_SHADE              0x00000004  /* enable Gouraud interp */
                              /* rest of low byte reserved for setup ucode */
#define  G_SHADING_SMOOTH     0x00000200  /* flat or smooth shaded */
#define  G_CULL_FRONT         0x00001000
#define  G_CULL_BACK          0x00002000
#define  G_CULL_BOTH          0x00003000  /* To make code cleaner */
#define  G_FOG                0x00010000
#define  G_LIGHTING           0x00020000
#define  G_TEXTURE_GEN        0x00040000
#define  G_TEXTURE_GEN_LINEAR 0x00080000
#define  G_LOD                0x00100000  /* NOT IMPLEMENTED */
#define  G_CLIPPING           0x00000000
                           
// MOVEMEM indices
# define G_MV_VIEWPORT  8
# define G_MV_LIGHT 10
# define G_MV_POINT 12
# define G_MV_MATRIX    14      /* NOTE: this is in moveword table */
# define G_MVO_LOOKATX  (0*24)
# define G_MVO_LOOKATY  (1*24)
# define G_MVO_L0   (2*24)
# define G_MVO_L1   (3*24)
# define G_MVO_L2   (4*24)
# define G_MVO_L3   (5*24)
# define G_MVO_L4   (6*24)
# define G_MVO_L5   (7*24)
# define G_MVO_L6   (8*24)
# define G_MVO_L7   (9*24)
                           
// MOVEWORD indices
#define  G_MW_MATRIX       0x00  /* NOTE: also used by movemem */
#define  G_MW_NUMLIGHT     0x02
#define  G_MW_CLIP         0x04
#define  G_MW_SEGMENT      0x06
#define  G_MW_FOG          0x08
#define  G_MW_LIGHTCOL     0x0A
#define  G_MW_FORCEMTX     0x0C
#define  G_MW_PERSPNORM    0x0E

// offsets from the address in the dmem table
#define  G_MWO_NUMLIGHT          0x00
#define  G_MWO_CLIP_RNX          0x04
#define  G_MWO_CLIP_RNY          0x0C
#define  G_MWO_CLIP_RPX          0x14
#define  G_MWO_CLIP_RPY          0x1C
#define  G_MWO_SEGMENT_0         0x00
#define  G_MWO_SEGMENT_1         0x01
#define  G_MWO_SEGMENT_2         0x02
#define  G_MWO_SEGMENT_3         0x03
#define  G_MWO_SEGMENT_4         0x04
#define  G_MWO_SEGMENT_5         0x05
#define  G_MWO_SEGMENT_6         0x06
#define  G_MWO_SEGMENT_7         0x07
#define  G_MWO_SEGMENT_8         0x08
#define  G_MWO_SEGMENT_9         0x09
#define  G_MWO_SEGMENT_A         0x0A
#define  G_MWO_SEGMENT_B         0x0B
#define  G_MWO_SEGMENT_C         0x0C
#define  G_MWO_SEGMENT_D         0x0D
#define  G_MWO_SEGMENT_E         0x0E
#define  G_MWO_SEGMENT_F         0x0F
#define  G_MWO_FOG               0x00
#define  G_MWO_aLIGHT_1          0x00
#define  G_MWO_bLIGHT_1          0x04
#define  G_MWO_aLIGHT_2          0x20
#define  G_MWO_bLIGHT_2          0x24
#define  G_MWO_aLIGHT_3          0x40
#define  G_MWO_bLIGHT_3          0x44
#define  G_MWO_aLIGHT_4          0x60
#define  G_MWO_bLIGHT_4          0x64
#define  G_MWO_aLIGHT_5          0x80
#define  G_MWO_bLIGHT_5          0x84
#define  G_MWO_aLIGHT_6          0xA0
#define  G_MWO_bLIGHT_6          0xA4
#define  G_MWO_aLIGHT_7          0xC0
#define  G_MWO_bLIGHT_7          0xC4
#define  G_MWO_aLIGHT_8          0xE0
#define  G_MWO_bLIGHT_8          0xE4
#define  G_MWO_MATRIX_XX_XY_I    0x00
#define  G_MWO_MATRIX_XZ_XW_I    0x04
#define  G_MWO_MATRIX_YX_YY_I    0x08
#define  G_MWO_MATRIX_YZ_YW_I    0x0C
#define  G_MWO_MATRIX_ZX_ZY_I    0x10
#define  G_MWO_MATRIX_ZZ_ZW_I    0x14
#define  G_MWO_MATRIX_WX_WY_I    0x18
#define  G_MWO_MATRIX_WZ_WW_I    0x1C
#define  G_MWO_MATRIX_XX_XY_F    0x20
#define  G_MWO_MATRIX_XZ_XW_F    0x24
#define  G_MWO_MATRIX_YX_YY_F    0x28
#define  G_MWO_MATRIX_YZ_YW_F    0x2C
#define  G_MWO_MATRIX_ZX_ZY_F    0x30
#define  G_MWO_MATRIX_ZZ_ZW_F    0x34
#define  G_MWO_MATRIX_WX_WY_F    0x38
#define  G_MWO_MATRIX_WZ_WW_F    0x3C
#define  G_MWO_POINT_RGBA        0x10
#define  G_MWO_POINT_ST          0x14
#define  G_MWO_POINT_XYSCREEN    0x18
#define  G_MWO_POINT_ZSCREEN     0x1C


