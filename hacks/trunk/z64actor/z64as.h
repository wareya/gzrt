/************************
* OoT 1.0 Actor Spawner *
************************/
#ifndef __Z64AS_H
#define __Z64AS_H

/* OoT controller layout */
typedef struct
{
	/* All the other buttons; described above */
	u16 pad;
	
	/* Joystick */
	signed x : 8;
	signed y : 8;
} Controller;

union FloatU32
{
	float 	f;
	u32 	u;
};

/* Coord triad */
typedef struct
{
	float x;
	float y;
	float z;
} Coord;

/* Coord triad (w/ RAW) */
typedef struct
{
	union FloatU32 x;
	union FloatU32 y;
	union FloatU32 z;
} CoordU;

/* Actor ID */
typedef u16 Z64AID;

/* Actor variable */
typedef u16 Z64VAR;

/* Amount of rupees */
typedef u16 RUPEES;

/* Amount of arrows */
typedef u8 ARROWS;

/* Amount of bombs */
typedef u8 BOMBS;

/* Application status word */
typedef struct
{
	/* Free use */
	u16 i;
	
	/* Flags */
	u16 flags;
} 
STATUS;

/* Macros - read */
#define GET_DW(x)	(*((u64*)(x)))
#define GET_W(x)	(*((u32*)(x)))
#define GET_HW(x)	(*((u16*)(x)))
#define GET_B(x)	(*((u8*) (x)))

/* Macros - write */
#define PUT_DW(x, b)	{ *((u64*)(x)) = (b); }
#define PUT_W(x, b)		{ *((u32*)(x)) = (b); }
#define PUT_HW(x, b)	{ *((u16*)(x)) = (b); }
#define PUT_B(x, b)		{ *((u8 *)(x)) = (b); }

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Application status flags */
enum
{
	Z64AS_F_USABLE		= 0x0001, /* In an area where we can spawn actors */
	Z64AS_F_WAITING		= 0X0002, /* Waiting for counter				  */
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Actor spawner function */
typedef int (*ASPAWN)
(
	u32,			/* Not sure - constant pointer	*/
	u32,			/* Not sure - constant pointer	*/
	int actor,		/* Actor number					*/
	u32 fx, 		/* Spawn X position (float)		*/
	u32 fy, 		/* Spawn Y position (float)		*/
	u32 fz,			/* Spawn Z position (float)		*/
	u32 rx, 		/* X Rotation					*/
	u32 ry, 		/* Y Rotation					*/
	u32 rz,			/* Z Rotation					*/
	int variable	/* Actor variable				*/
);

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Function pointers	*/
#define Z64AS_ROUTINE_ADDR		0x80025110	/* Address to actor spawner	*/

/* Argument constants	*/
#define Z64AS_CONST_SPAWN_ARG1	0x801CA0C4	/* First argument			*/
#define Z64AS_CONST_SPAWN_ARG2	0x801C84A0	/* Second argument			*/

/* Constants			*/
#define Z64AS_INTERFACE_DEF		0xC080

/* Other pointers		*/
#define Z64NC_CNST_LINKXYZ		0x801DAA54	/* Link's current coords 	*/
#define Z64NC_CNST_BUTTON		0x801C84B4	/* Controller input 		*/
#define Z64NC_CNST_RUPEES		0x8011A604	/* Game rupee counter		*/
#define Z64NC_CNST_INTERFACE	0x801DAA90	/* interface mode  		    */
#define Z64NC_CNST_BOMBS		0x8011A65E	/* Amount of bombs			*/
#define Z64NC_CNST_ARROWS		0x8011A65F	/* Amount of arrows 		*/

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/*
** I would like to personally thank everyone who has contributed to the current
** actor list. It's made my job a lot easier, and probably took a lot of time
** and effort to put together, so thanks for that!
** -ZZT32
*/
enum Z64ACTOR_ID
{
	Z64ACTOR_LINK__MAY_SOMETIMES_CAUSE_Z_BUFF        = 0x0000,
	Z64ACTOR_STALFOS                                 = 0x0002,
	Z64ACTOR_DISSIPATING_FLAMES                      = 0x0007,
	Z64ACTOR_GREAT_FAIRY_S_FLAMES                    = 0x0008,
	Z64ACTOR_WOODEN_DOOR__NOTE__IF_THE_FOURTH        = 0x0009,
	Z64ACTOR_TREASURE_CHEST                          = 0x000A,
	Z64ACTOR_GREAT_FAIRY_                            = 0x000B,
	Z64ACTOR_PROXIMITY_ACTIVATED_FLAME_WALL_         = 0x000C,
	Z64ACTOR_POE                                     = 0x000D,
	Z64ACTOR_OCTOROK_                                = 0x000E,
	Z64ACTOR_WEBS                                    = 0x000F,
	Z64ACTOR_BOMB_                                   = 0x0010,
	Z64ACTOR_CEILING_WALL_MASTER                     = 0x0011,
	Z64ACTOR_DODONGO                                 = 0x0012,
	Z64ACTOR_KEESE_                                  = 0x0013,
	Z64ACTOR_EPONA                                   = 0x0014,
	Z64ACTOR_COLLECTIBLES_                           = 0x0015,
	Z64ACTOR_ARROWS                                  = 0x0016,
	Z64ACTOR_FAIRIES_                                = 0x0018,
	Z64ACTOR_CUCCO_                                  = 0x0019,
	Z64ACTOR_RED_TEKTITE                             = 0x001B,
	Z64ACTOR_PEAHAT_                                 = 0x001D,
	Z64ACTOR_BUTTERFLY_                              = 0x001E,
	Z64ACTOR_BUGS                                    = 0x0020,
	Z64ACTOR_FISH_                                   = 0x0021,
	Z64ACTOR_ROOM_CHANGING_BLACK_PLANE_              = 0x0023,
	Z64ACTOR_0000___1                                = 0x0024,
	Z64ACTOR_LIZAFOS_                                = 0x0025,
	Z64ACTOR_WOODEN_POST_WITH_RED_CLOTH_             = 0x0026,
	Z64ACTOR_GHOMA_                                  = 0x0028,
	Z64ACTOR_PRINCESS_ZELDA_CHILD__AT_WINDOW_        = 0x0029,
	Z64ACTOR_GHOMA_LARVA_                            = 0x002B,
	Z64ACTOR_SHABOM__BOUNCING_BUBBLE_ENEMY_          = 0x002D,
	Z64ACTOR_METAL_STUDDED__LIFTING_WOODEN_DO        = 0x002E,
	Z64ACTOR_BABY_DODONGO                            = 0x002F,
	Z64ACTOR_RETURNING_BOOMERANG_                    = 0x0032,
	Z64ACTOR_DARK_LINK_                              = 0x0033,
	Z64ACTOR_BIRI_THING                              = 0x0034,
	Z64ACTOR_SKULLTULA_                              = 0x0037,
	Z64ACTOR_TORCH_SLUG                              = 0x0038,
	Z64ACTOR_0000_GAMEPLAY_KEEP_ITEMS                = 0x0039,
	Z64ACTOR_STINGER_                                = 0x003A,
	Z64ACTOR_AMBIENT_SOUND_EFFECTS                   = 0x003B,
	Z64ACTOR_NORMAL_HORSE_                           = 0x003C,
	Z64ACTOR_GREAT_DEKU_TREE_S_JAW_                  = 0x003E,
	Z64ACTOR_OUT_OF_BOUNDS_PLANE__TYP__              = 0x003F,
	Z64ACTOR_MEGATON_STATUE_                         = 0x0040,
	Z64ACTOR_HUGE_STONE_SPIKE_PLATFORM__FIRE_        = 0x0041,
	Z64ACTOR_GANONDORF_S_HORSE_                      = 0x0042,
	Z64ACTOR_STONE_BLOCKS__FIRE_TEMPLE_              = 0x0043,
	Z64ACTOR_SPINNING_FLAME_THROWER_STONE__FI        = 0x0044,
	Z64ACTOR_STATIONARY_FLAME_THROWER_STATUE_        = 0x0045,
	Z64ACTOR_STONE_PLATFORM__FIRE_TEMPLE_            = 0x0046,
	Z64ACTOR_STONE_ELEVATOR__FIRE_TEMPLE_            = 0x0047,
	Z64ACTOR_FLAME_CIRCLE_                           = 0x0049,
	Z64ACTOR_DRAWBRIDGE_OBJECTS                      = 0x004A,
	Z64ACTOR_MOBLINS                                 = 0x004B,
	Z64ACTOR_BOMBFLOWER_BOMB_                        = 0x004C,
	Z64ACTOR_LOCK_ON_BLUE_SPOT_                      = 0x004F,
	Z64ACTOR_DEKU_TREE_PUZZLE_ELEMENTS               = 0x0050,
	Z64ACTOR_ROTATING_SPIKE_CYLINDER__DEKU_TR        = 0x0051,
	Z64ACTOR_PHANTOM_GANON_                          = 0x0052,
	Z64ACTOR_ARMOS_STATUE_                           = 0x0054,
	Z64ACTOR_DEKU_BABA__SNAPPING__                   = 0x0055,
	Z64ACTOR_RISING_STONE_PLATFORM__DODONGO_S        = 0x0058,
	Z64ACTOR_0000_LORD_JABU_JABU_                    = 0x005A,
	Z64ACTOR_ZELDA_S_HORSE_                          = 0x005B,
	Z64ACTOR_STONE_STAIRS_                           = 0x005C,
	Z64ACTOR_WARP_PORTALS                            = 0x005D,
	Z64ACTOR_GOLDEN_TORCH_STAND_                     = 0x005E,
	Z64ACTOR_HEART_CONTAINER_                        = 0x005F,
	Z64ACTOR_DEKU_SCRUB_                             = 0x0060,
	Z64ACTOR_LARGE_ROTATING_STONE_RING__FORES        = 0x0061,
	Z64ACTOR_LARGE_SQUARE_OF_WATER_                  = 0x0065,
	Z64ACTOR__BUBBLE__ENEMY                          = 0x0069,
	Z64ACTOR_TEMPLE_OF_TIME_WINDOWS_                 = 0x006A,
	Z64ACTOR_FLYING_FLOOR_TILE_                      = 0x006B,
	Z64ACTOR_MASTER_SWORD___CINEMA_                  = 0x006C,
	Z64ACTOR_WARP_PAD_                               = 0x006E,
	Z64ACTOR_OCARINA_WARP_PAD__ALPHA__               = 0x006E,
	Z64ACTOR_METAL_GATE_                             = 0x006F,
	Z64ACTOR_INVISIBLE_WALL__UNAFFECTED_BY_LE        = 0x0070,
	Z64ACTOR_STONE_STEPS_AND_PLATFORMS__FIRE_        = 0x0071,
	Z64ACTOR_BROWN_BIRD__ALPHA__                     = 0x0072,
	Z64ACTOR_GREENERY_                               = 0x0077,
	Z64ACTOR_LARGE_NOISY_STONE__ALPHA__              = 0x007C,
	Z64ACTOR_STONE_CUBE__ALPHA_                      = 0x007D,
	Z64ACTOR_SLIDING_SPIKED_METAL_TRAP_              = 0x0080,
	Z64ACTOR_ORANGE_POT__ALPHA__                     = 0x0082,
	Z64ACTOR_TALON_                                  = 0x0084,
	Z64ACTOR_BEAMOS__DOES_NOT_ATTACK_AT_ALL_         = 0x008A,
	Z64ACTOR_0000_SPIRITUAL_STONES                   = 0x008B,
	Z64ACTOR_0000_TEMPLE_OF_TIME_OBJECTS_            = 0x008C,
	Z64ACTOR_LARGE_FLAME_WALL                        = 0x008D,
	Z64ACTOR_FLOORMASTER_                            = 0x008E,
	Z64ACTOR_REDEAD_                                 = 0x0090,
	Z64ACTOR_MEG__POE_SISTER_FROM_FOREST_TEMP        = 0x0091,
	Z64ACTOR_GOLDEN_GAUNTLETS_ROCK_                  = 0x0092,
	Z64ACTOR_POE_PUZZLE_BLOCK_                       = 0x0093,
	Z64ACTOR_BUTTERFLIES__DEFUNCT__                  = 0x0094,
	Z64ACTOR_SKULLWALLTULA_                          = 0x0095,
	Z64ACTOR_FLYING_VOLVAGIA_                        = 0x0096,
	Z64ACTOR_ENVIRONMENTAL_EFFECTS_ALL_SOME__        = 0x0097,
	Z64ACTOR_FLARE_DANCER_                           = 0x0099,
	Z64ACTOR_YOUNG_EPONA_                            = 0x009A,
	Z64ACTOR_HOLE_IN_GROUND_EXIT_                    = 0x009B,
	Z64ACTOR_LARGE_EYE_OF_TRUTH_ENGRAVING            = 0x009C,
	Z64ACTOR_0000_GRAVESTONE_                        = 0x009D,
	Z64ACTOR_FARORE_S_WIND_                          = 0x009E,
	Z64ACTOR_DIN_S_FIRE_                             = 0x009F,
	Z64ACTOR_CHILD_PRINCESS_RUTO_                    = 0x00A1,
	Z64ACTOR_HOLE_VOLVAGIA_                          = 0x00A2,
	Z64ACTOR_FIREBALL_                               = 0x00A3,
	Z64ACTOR_DEAD_HAND__DOES_NOT_APPEAR_UNLES        = 0x00A4,
	Z64ACTOR_DEAD_HAND_S_HAND                        = 0x00A5,
	Z64ACTOR_RAURU                                   = 0x00A6,
	Z64ACTOR_ENEMY_SPAWNER                           = 0x00A7,
	Z64ACTOR_0000_CORE_FLARE_DANCER_CREATURE_        = 0x00AB,
	Z64ACTOR_PLATFORMS__SHADOW_TEMPLE_               = 0x00AF,
	Z64ACTOR_SPIRIT_TEMPLE_SHIP_                     = 0x00B0,
	Z64ACTOR_SPINNING_SCYTHE_STATUES_                = 0x00B1,
	Z64ACTOR_HYRULIAN_GUARDS                         = 0x00B3,
	Z64ACTOR_FLYING_RUBBLE_                          = 0x00B5,
	Z64ACTOR_FLOBBERY_MUSCLE_BLOCK__JABU_JABU        = 0x00B6,
	Z64ACTOR_STATUE_FROM_DARUNIA_S_ROOM_             = 0x00B9,
	Z64ACTOR_BARINADE                                = 0x00BA,
	Z64ACTOR_GIANT_SKULL_JAR__BLUE_FLAMES_ON_        = 0x00BB,
	Z64ACTOR_SHADOW_TEMPLE_OBJECTS                   = 0x00BC,
	Z64ACTOR_COFFIN_LID_                             = 0x00BD,
	Z64ACTOR_STATUE_AND_WALL__SHADOW_TEMPLE_         = 0x00BE,
	Z64ACTOR_CRATER_SMOKE_CONE_                      = 0x00BF,
	Z64ACTOR_SHOOTING_GALLERY_MAN_                   = 0x00C1,
	Z64ACTOR_SHOP_SHELVES_                           = 0x00C2,
	Z64ACTOR_MORPHA_                                 = 0x00C4,
	Z64ACTOR_SHELL_BLADE_                            = 0x00C5,
	Z64ACTOR_BIG_OCTO                                = 0x00C6,
	Z64ACTOR_WITHERED_DEKU_BABA                      = 0x00C7,
	Z64ACTOR_LORD_JABU_JABU_OBJECTS_                 = 0x00C8,
	Z64ACTOR_DODONGO_CAVERN_ENTRANCE_ROCK            = 0x00CD,
	Z64ACTOR_BOMBABLE_WALLS__FIRE_TEMPLE_            = 0x00CF,
	Z64ACTOR_2D_BOMBABLE_WALL__DEFUNCT__             = 0x00D0,
	Z64ACTOR_ADULT_RUTO__DEFUNCT__                   = 0x00D2,
	Z64ACTOR_WATER_NOISE_                            = 0x00D4,
	Z64ACTOR_LAKE_HYLIA_OBJECTS                      = 0x00D5,
	Z64ACTOR_MOVABLE_ICE_BLOCK_                      = 0x00D6,
	Z64ACTOR_BOMBCHU_                                = 0x00DA,
	Z64ACTOR_TWINROVA_                               = 0x00DC,
	Z64ACTOR_LIKE_LIKE_                              = 0x00DD,
	Z64ACTOR_TENTACLE_FROM_INSIDE_LORD_JABU_J        = 0x00DE,
	Z64ACTOR_BLOCKING_TENTACLE_FROM_INSIDE_LO        = 0x00DF,
	Z64ACTOR_ANUBIS_STATUE_                          = 0x00E0,
	Z64ACTOR_FIRE_THING__POSSIBLY_FROM_VOLVAG        = 0x00E1,
	Z64ACTOR___0073___FOREST_TEMPLE_LADDER           = 0x00E2,
	Z64ACTOR___0073___FOREST_TEMPLE_ELEMENTS         = 0x00E3,
	Z64ACTOR_INVISIBLE_GEOMETRY__DEFUNCT__           = 0x00E4,
	Z64ACTOR_DEATH_MOUNTAIN_CLOUD_RING_              = 0x00E5,
	Z64ACTOR_SWITCH_FROM_INSIDE_LORD_JABU_JAB        = 0x00E6,
	Z64ACTOR_CHILD_MALON__DEFUNCT__                  = 0x00E7,
	Z64ACTOR___017C___017D___GANONDORF_BOSS          = 0x00E8,
	Z64ACTOR_BONGO_BONGO_S_HAND_                     = 0x00E9,
	Z64ACTOR_SPIKE_ENEMY_                            = 0x00EC,
	Z64ACTOR_DEKU_SHIELD_                            = 0x00EE,
	Z64ACTOR_RED_ICE                                 = 0x00EF,
	Z64ACTOR_BLUE_FLAME_                             = 0x00F0,
	Z64ACTOR_OCARINA_OF_TIME                         = 0x00F1,
	Z64ACTOR_NAYRU_S_LOVE_                           = 0x00F4,
	Z64ACTOR_SHADOW_TEMPLE_TRUTH_SPINNER_            = 0x00F7,
	Z64ACTOR_HYRULE_CASTLE_FENCE__LOOKS_LIKE_        = 0x00F8,
	Z64ACTOR_GIANT_ROLLING_BOULDER__MOVES_VER        = 0x00F9,
	Z64ACTOR_SLIDING__CLIMBABLE_BRICK_WALL__S        = 0x00FA,
	Z64ACTOR_ROTATING_COBRA_MIRROR_                  = 0x00FC,
	Z64ACTOR_CLIMBABLE_METAL_GRATING__FIRE_TE        = 0x00FD,
	Z64ACTOR_0000_FISHING_POND_MAN_AND_FISH_         = 0x00FE,
	Z64ACTOR_MOVABLE_BLOCK                           = 0x00FF,
	Z64ACTOR_DEATH_MOUNTAIN_TRAIL_GATE_              = 0x0100,
	Z64ACTOR_SPARKLIES_GATHERING_TOGETHER_           = 0x0101,
	Z64ACTOR_WINDMILL_SAILS_                         = 0x0102,
	Z64ACTOR_KAKARIKO_VILLAGE_WELL_CROSSBEAM_        = 0x0103,
	Z64ACTOR_KAKARIKO_VILLAGE_WELL_WATER_            = 0x0104,
	Z64ACTOR_GOLDEN_TORCH_STAND__GRANITE_BASE        = 0x0105,
	Z64ACTOR_MILK_CRATE_                             = 0x0107,
	Z64ACTOR_LON_LON_RANCH_OBSTACLE_FENCE            = 0x0108,
	Z64ACTOR_CRATES__CAN_ALSO_APPEAR_AS_A_GIA        = 0x0110,
	Z64ACTOR_BREAKABLE_POT__DEFUNCT__                = 0x0111,
	Z64ACTOR_INVISIBLE_COLLECTIBLE_                  = 0x0112,
	Z64ACTOR_0000_IRON_KNUCKLE                       = 0x0113,
	Z64ACTOR_SKULLKID_                               = 0x0115,
	Z64ACTOR_DEKU_SALESMAN__ALREADY_OUT__            = 0x011A,
	Z64ACTOR_NAVI_INFORMATION_SPOT                   = 0x011B,
	Z64ACTOR_STONE_EYE_FLAMETHROWER                  = 0x011C,
	Z64ACTOR_FLYING_POT                              = 0x011D,
	Z64ACTOR_ZORA_S_RIVER_WATERFALL_                 = 0x011F,
	Z64ACTOR_ZORA_S_DOMAIN_WATERFALL_                = 0x0120,
	Z64ACTOR_FREZZARD_                               = 0x0121,
	Z64ACTOR_DAMPE_S_GHOST__SPEAKS_OF_SHADOW_        = 0x0122,
	Z64ACTOR_ROTATING_WINDMILL_MECHANISM_            = 0x0123,
	Z64ACTOR_ZORA_DIVING_GAME_                       = 0x0124,
	Z64ACTOR_LIFTABLE_AND_CUTABLE__BASIC_BUSH        = 0x0125,
	Z64ACTOR_LIFTABLE_AND_CUTABLE__RESPAWNING        = 0x0125,
	Z64ACTOR_BEAN_PLANTING_SPOT_                     = 0x0126,
	Z64ACTOR_BROWN_BOULDER_                          = 0x0127,
	Z64ACTOR_SWITCHES__MAKE_PUZZLE_SOLVED_SOU        = 0x012A,
	Z64ACTOR_HUGE_STONE_ELEVATOR__ALPHA___           = 0x012B,
	Z64ACTOR_0000_SQUARE__COLLAPSING_PLATFORM        = 0x012C,
	Z64ACTOR_STONE_HOOKSHOT_TARGET_                  = 0x012D,
	Z64ACTOR_PLAY_OCARINA_HERE_SPOT_                 = 0x012E,
	Z64ACTOR_0000___2                                = 0x012F,
	Z64ACTOR_0000___3                                = 0x0130,
	Z64ACTOR_0000_SPARKLING_RUPEE_                   = 0x0131,
	Z64ACTOR_CARPENTER__LINK_FREEZES_IF_YOU_S        = 0x0133,
	Z64ACTOR_NON_SOLID_ROUND_CREATURES__ALPHA        = 0x0135,
	Z64ACTOR_DARK_LINK_S_ILLUSION_ROOM_              = 0x0136,
	Z64ACTOR_UNKNOWN                                 = 0x0137,
	Z64ACTOR_WHITE_CLOTHED_GERUDO_                   = 0x0138,
	Z64ACTOR_DYNAMIC_SHADOW_FOR_LINK__ALPHA__        = 0x013A,
	Z64ACTOR_ENEMY_ARWING__ALPHA__                   = 0x013B,
	Z64ACTOR_KAKARIKO_CUCCO_LADY_                    = 0x013C,
	Z64ACTOR_LOCKABLE_BLUE_ARROW_SPOT_               = 0x013D,
	Z64ACTOR_BEAN_SELLER_                            = 0x013E,
	Z64ACTOR_CARPENTER_S_SON__IGNORES_YOU_           = 0x013F,
	Z64ACTOR_SQUARE_SIGNPOST_                        = 0x0141,
	Z64ACTOR_HYRULE_CASTLE_GUARD                     = 0x0142,
	Z64ACTOR_HOPPING_CUCCO__NOT_SOLID_               = 0x0143,
	Z64ACTOR_ATTACKING_CUCCO__NOT_SOLID              = 0x0144,
	Z64ACTOR_SARIA_                                  = 0x0146,
	Z64ACTOR_CHECKABLE_SPOT__NAVI_HOVERS_OVER        = 0x0147,
	Z64ACTOR_POTION_SHOP_HAG_                        = 0x0149,
	Z64ACTOR_LAKESIDE_PROFESSOR_                     = 0x014A,
	Z64ACTOR_BOMBCHU_BOWLING_ALLEY_LADY_             = 0x014B,
	Z64ACTOR_KAEPORA_GAEBORA_                        = 0x014D,
	Z64ACTOR_SMALL_ROCK__MAY_CAUSE_GLITCHED_G        = 0x014E,
	Z64ACTOR_GRAVE_FLOWER_                           = 0x014F,
	Z64ACTOR_SUN_EMBLEM_TRIGGER__SPIRIT_TEMPL        = 0x0150,
	Z64ACTOR_GROUPS_OF_SMALL_DISPENSERS              = 0x0151,
	Z64ACTOR_WINDMILL_MAN_                           = 0x0153,
	Z64ACTOR_FREEZES_WHEN_TOUCH_                     = 0x0155,
	Z64ACTOR_LARGE_STONE_FACE__SPIRIT_TEMPLE_        = 0x0156,
	Z64ACTOR_LIGHT_BLOCKING_ROCK__BOMBABLE__T        = 0x0159,
	Z64ACTOR_CIRCULAR_METAL_GRATING_BLOCKADE_        = 0x015A,
	Z64ACTOR_BOMBABLE_ROCK_WALL                      = 0x015B,
	Z64ACTOR_GIANT_THREE_SIDED_GORON_STATUE_         = 0x015C,
	Z64ACTOR_GANON_S_ORGAN_AND_SURROUNDINGS_         = 0x015E,
	Z64ACTOR_WATER_SPOUT_                            = 0x015F,
	Z64ACTOR_WHIRLPOOL_EFFECT_                       = 0x0160,
	Z64ACTOR_RUNNING_MAN__CHILD_LINK_VERSION_        = 0x0162,
	Z64ACTOR_KOKIRI_                                 = 0x0163,
	Z64ACTOR_KING_ZORA_                              = 0x0164,
	Z64ACTOR_PROXIMITY_RAIN_WEATHER_EFFECTS          = 0x0165,
	Z64ACTOR_BONGO_BONGO_S_DRUM                      = 0x0166,
	Z64ACTOR_KAKARIKO_VILLAGE_ROOFTOP_MAN_           = 0x0167,
	Z64ACTOR_IRON_KNUCKLES_ROOM_STUFF__SPIRIT        = 0x0169,
	Z64ACTOR_MAGIC_CARPET_MAN_                       = 0x016A,
	Z64ACTOR_GRAVEYARD_BOY_                          = 0x016C,
	Z64ACTOR_MIDO                                    = 0x016D,
	Z64ACTOR_0000_MARKET_NPCS                        = 0x016E,
	Z64ACTOR_RAIN_AND_LIGHTNING_                     = 0x0170,
	Z64ACTOR_MAKES_MAP_BLUE_                         = 0x0171,
	Z64ACTOR_METAL_GRATE_DOOR__NAVI_SAYS_YOU_        = 0x0172,
	Z64ACTOR_TARGETABLE_NAVI_SPOT__GREEN__VAR        = 0x0173,
	Z64ACTOR_BIG_POE_                                = 0x0175,
	Z64ACTOR_LAVA_PARTICLE_FOUNTAIN__DEATH_MO        = 0x0176,
	Z64ACTOR_METAL_BARS__GANON_S_CASTLE_             = 0x0177,
	Z64ACTOR_HYRULE_GUARD                            = 0x0178,
	Z64ACTOR_PIERRE_THE_SCARECROW_                   = 0x017B,
	Z64ACTOR_TREASURE_CHEST_GAME_MAN_                = 0x017C,
	Z64ACTOR_CRASH_                                  = 0x017D,
	Z64ACTOR_SUN_S_SONG_EFFECT_                      = 0x017E,
	Z64ACTOR_THE_END_MESSAGE__DEFUNCT__              = 0x017F,
	Z64ACTOR_SONG_OF_STORMS_FAIRY_                   = 0x0183,
	Z64ACTOR_SPIRAL_BEAMS__GREAT_FAIRY_FOUNTA        = 0x0184,
	Z64ACTOR_CHECKABLE_SPOT__DISAPPEARS_AFTER        = 0x0185,
	Z64ACTOR_PATROLLING_GERUDO_                      = 0x0186,
	Z64ACTOR_TIMER                                   = 0x0187,
	Z64ACTOR_CURSED_SKULLTULA_PEOPLE                 = 0x0188,
	Z64ACTOR_ZELDA_S_LULLABY_EFFECT_                 = 0x018A,
	Z64ACTOR_SONG_OF_STORM_S_EFFECT_                 = 0x018B,
	Z64ACTOR_2D_SQUARE_TEXTURE_                      = 0x018D,
	Z64ACTOR_STONE_PLATFORM__DEFUNCT__               = 0x018E,
	Z64ACTOR_BLACK_PARTICLE_FOUNTAIN__LASTS_F        = 0x018F,
	Z64ACTOR_GERUDO_FORTRESS_WOODEN_GATE_            = 0x0190,
	Z64ACTOR_GERUDO_FORTRESS_TRAINING_AREA_GA        = 0x0191,
	Z64ACTOR_GREEN_DEKU_SCRUB__TEXT_SEEMS_TO_        = 0x0192,
	Z64ACTOR_GROUNDED_SALES_SCRUB_                   = 0x0195,
	Z64ACTOR_GERUDO_FIGHTER                          = 0x0197,
	Z64ACTOR_EPONA_S_SONG_EFFECT_                    = 0x0198,
	Z64ACTOR_SARIA_S_SONG_EFFECT_                    = 0x0199,
	Z64ACTOR_GIRL_CHASING_CUCCO__DEFUNCT__           = 0x019A,
	Z64ACTOR_DOG__NOTE__IT_IS_POSSIBLE_TO_MAK        = 0x019B,
	Z64ACTOR_GOLDEN_SKULLTULA_TOKEN_                 = 0x019C,
	Z64ACTOR_SEMI_BUILT_SHOOTING_GALLERY__DEF        = 0x019D,
	Z64ACTOR_BEEHIVE__GRAPHICS_VERY_GLITCHY_I        = 0x019E,
	Z64ACTOR_LARGE_CRATE_                            = 0x01A0,
	Z64ACTOR_TRIGGER_FOR_DEKU_MASK_PANEL__WEA        = 0x01A1,
	Z64ACTOR_DEKU_MASK_PANEL_HEAD_JUDGE_             = 0x01A2,
	Z64ACTOR_MAN_IN_PURPLE_PANTS_AND_WHITE_TO        = 0x01A4,
	Z64ACTOR_CARPENTER_S_SON__CHILD_LINK_VERS        = 0x01A6,
	Z64ACTOR_GANON_S_TOWER_MAGIC_BARRIER_            = 0x01A7,
	Z64ACTOR_ZORA_S_FOUNTAIN_BOMBABLE_WALL__G        = 0x01A8,
	Z64ACTOR_EXPLODABLE_CRATER_WALL__GRAPHICS        = 0x01A9,
	Z64ACTOR_TOWER_OF_RUPEES_                        = 0x01AB,
	Z64ACTOR_ENTWINED_LOVERS__HONEY_AND_DARLI        = 0x01AC,
	Z64ACTOR_HAGGLING_TOWNSPEOPLE_                   = 0x01AD,
	Z64ACTOR_GORONS                                  = 0x01AE,
	Z64ACTOR_WOLFOS_                                 = 0x01AF,
	Z64ACTOR_HYRULE_FIELD_STALCHILD                  = 0x01B0,
	Z64ACTOR_GANON_BATTLE_RUBBLE_                    = 0x01B1,
	Z64ACTOR_CHUNK_OF_STONE__NON_SOLID__POSSI        = 0x01B2,
	Z64ACTOR_SINKING_LAVA_PLATFORM__GANON_S_C        = 0x01B3,
	Z64ACTOR_ICE_BLOCK                               = 0x01B4,
	Z64ACTOR_WEB_BLOCKED_LIGHT_WINDOW_               = 0x01B5,
	Z64ACTOR_2D_STONE_WALL_                          = 0x01B6,
	Z64ACTOR_LARGE_SQUARE_ICE_BLOCK__DEFUNCT_        = 0x01B7,
	Z64ACTOR_POE_COLLECTOR_AND_SURROUNDINGS_         = 0x01B8,
	Z64ACTOR_GOSSIP_STONE                            = 0x01B9,
	Z64ACTOR_BOMBABLE_STONE_WALL_                    = 0x01BA,
	Z64ACTOR_METAL_GATE_2                            = 0x01BB,
	Z64ACTOR_CARPENTERS__DEFUNCT__                   = 0x01BC,
	Z64ACTOR_BOMBCHU_BOWLING_ALLEY_WALL_             = 0x01BD,
	Z64ACTOR_DESERT_WASTELAND_POE_GUIDE              = 0x01BF,
	Z64ACTOR_GUAY_                                   = 0x01C0,
	Z64ACTOR_FAKE_DOOR_                              = 0x01C1,
	Z64ACTOR_LID_TO_GORON_JAR                        = 0x01C3,
	Z64ACTOR_GORON_ROOM_DOOR_                        = 0x01C4,
	Z64ACTOR_COW_                                    = 0x01C6,
	Z64ACTOR_ICICLES                                 = 0x01C7,
	Z64ACTOR_2D_ICE_BARS__FOR_A_DOOR__               = 0x01C8,
	Z64ACTOR_GREEN_NAVI_SPOT                         = 0x01C9,
	Z64ACTOR_BONOORU_THE_SCARECROW_                  = 0x01CA,
	Z64ACTOR_SONG_OF_TIME_EFFECT                     = 0x01CB,
	Z64ACTOR_CIRCULAR_PIECE_OF_STONE_WALL_           = 0x01CD,
	Z64ACTOR_ZORA_                                   = 0x01CE,
	Z64ACTOR_GERUDO_GIVING_YOU_MEMBERSHIP_CAR        = 0x01D0,
	Z64ACTOR_TIME_BLOCK                              = 0x01D1,
	Z64ACTOR_BRONZE_BOULDER_                         = 0x01D2,
	Z64ACTOR_PRINCESS_ZELDA_CHILD_                   = 0x01D3,
	Z64ACTOR_NAVI_GREEN_SPOT__RELATED_TO_TIME        = 0x01D6
};

/* Max actor */
#define Z64AS_ACTOR_LIST_MAX	0x01E0

#endif /* __Z64AS_H */

