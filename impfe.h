/*
 * $Id: impfe.h,v 1.2 2000/05/17 22:51:46 marisa Exp $
 *
 * Contains definitions and prototypes unrelated to the GUI
 *
 * $Log: impfe.h,v $
 * Revision 1.2  2000/05/17 22:51:46  marisa
 * More work to support autoconf - now builds clean on Linux
 *
 * Revision 1.1.1.1  2000/05/17 19:22:13  marisa
 * First CVS checkin
 *
 * Revision 1.1.1.10  1997/05/03 09:42:39  marisag
 * patch1: Daily checkin
 *
 * Revision 1.1.1.9  1997/04/27 16:32:38  marisag
 * patch1: Daily checkin
 *
 * Revision 1.1.1.8  1997/04/25 06:39:16  marisag
 * patch1: Code for the planet & ship detail forms
 *
 * Revision 1.1.1.7  1997/04/24 04:47:23  marisag
 * patch1: Many changes
 *
 * Revision 1.1.1.6  1997/04/21 00:02:58  marisag
 * patch1: Now uses MAX_SECT_PLAN and MAX_SECT_SHIP in the FeSector_t struct
 *
 * Revision 1.1.1.5  1997/04/16 03:07:22  marisag
 * patch1: Daily checkin
 *
 * Revision 1.1.1.4  1997/04/14 16:05:11  marisag
 * patch1: Daily checkin
 *
 * Revision 1.1.1.3  1997/04/14 02:52:15  marisag
 * patch1: Many changes
 *
 * Revision 1.1.1.2  1997/04/12 16:53:51  marisag
 * patch1: Many changes, started filling out definition of the
 * patch1: FeShip_t and FeItem_t structs, added prototypes for
 * patch1: many functions.
 *
 * Revision 1.1.1.1  1997/04/10 06:31:47  marisag
 * patch1: Yawn....
 *
 * Revision 1.1  1997/04/09 03:21:58  marisag
 * Initial revision
 *
 *
 */

#include <stdbool.h>
#ifndef BOOL
#define BOOL bool
#endif
#ifndef TRUE
#define TRUE	1
#endif
#ifndef FALSE
#define FALSE	0
#endif
#define UBYTE unsigned char
#define USHORT unsigned int
#define ULONG unsigned long
#define IT_NONE 0xFFFFFFFE  /* Returned for item/planet/ship number unknown */

#define DEF_PORT	3458

#ifndef HAVE_BZERO
#ifndef bzero
#define bzero(s,l) memset((s), 0, (1))
#endif
#endif

/*
 * Bitfields
 */
#define	BF_DIRTY	00000001 /* Should be rescanned */
#define	BF_OWNED	00000002 /* Player owns it      */
#define	BF_NO_OWNER	00000004 /* (planets) Server says nobody owns it */
#define	BF_XFER_KNOWN	00000010 /* (planets) "xfer" is real, not a default */
#define	BF_FOR_SALE	00000004 /* Item is for sale    */
#define	BF_HAS_BLAS	00000008
#define	BF_HAS_PHOT	00000010
#define	BF_ONPLAN	00000020 /* Set if item on planet */
#define	BF_INSTALL	00000040 /* Set if item installed */
#define	BF_DUMMY06	00000080

/* For queueing commands */
#define QUE_IMMEDIATE	4    /* Will be done immediately */
#define QUE_HIGH	3        /* Rest go high->medium->low */
#define QUE_MED		2
#define QUE_LOW		1
#define QUE_AVAIL   0

/*
 * Define our data types
 */
#define st_unknown    0
#define st_a    1
#define st_b    2
#define st_c    3
#define st_d    4
#define st_e    5
#define st_m    6
#ifndef BUILDING_MAIN
extern char SHIP_CHAR[];
#else
char SHIP_CHAR[] = "?abcdem\0";
#endif
#define NAME_LEN 32
#define RACE_MAX 7
#define PLAYER_MAX 254
#define SHIP_NAME_LEN 20
typedef struct {
	unsigned char
		flags;      /* Bitfield - See above for meanings */
	long	last_seen;
	UBYTE	ShipType;   /* class letter: a-e or m, ? if unknown */
	ULONG	planet;     /* What planet it is on, if any */
	USHORT
		fuelLeft,   /* fuel left */
		cargo,      /* current cargo amount */
		armourLeft, /* Armor left on hull */
		sh_row,     /* What row it is in  */
		sh_col,     /* What col it is in  */
		shields,    /* energy in shields  */
		shieldsKeep, /* energy to maintain in shields */
		airLeft,    /* Amount of air left */
		energy,     /* Energy left        */
		sh_tf;      /* Global tech factor */
	UBYTE	fleet,      /* Fleet it is in     */
		efficiency, /* Efficiency         */
		owner,      /* Who owns it        */
		plagueStage, /* What stage of the plague is it in */
		hullTF,
		engTF,
		engEff;
	char	name[SHIP_NAME_LEN]; /* Ship name, if any */
	USHORT	num_torp,   /* Number of torps on board */
		num_ore,    /* Amount of ore on board */
		num_gold,   /* Amount of gold bars on board */
		num_civ,
		num_sci,
		num_mil,
		num_ofc,
		num_misl,
		num_airt,
		num_ftnk,
		num_comp,
		num_eng,
		num_life,
		num_wpn,
		num_elect;
	} FeShip_t;

#define PL_NAME_LEN 16
#define PL_PSWD_LEN 8
#define PT_TRADE	0 /* Peacefully */
#define PT_PEACEFULL	1 /* Newly taken planet */
#define PT_HOME		2 /* Home planet */
#define PT_HOSTILE	3 /* Taken by force */
#define PPROD_FIRST	0
#define PPROD_LAST	16
#ifndef BUILDING_MAIN
extern const char *PPROD_NAME[];
#else
const char *PPROD_NAME[]=
    {
        "civilians",
        "military",
        "technology",
        "research",
        "education",
        "ocs",
        "ore mining",
        "gold mining",
        "air tanks",
        "fuel tanks",
        "weapons",
        "engines",
        "hull",
        "missile",
        "planes",
        "electronics",
        "cash"
    };
#endif
    /* item (commodity) types */
#define it_civilians    0
#define it_scientists   1
#define it_military     2
#define it_officers     3
#define it_missiles     4
#define it_planes       5
#define it_ore          6
#define it_bars         7
#define it_airTanks     8
#define it_fuelTanks    9
#define it_computers    10
#define it_engines      11
#define it_lifeSupp     12
#define it_elect        13
#define it_weapons      14
typedef UBYTE ItemType_t;
#define IT_FIRST        0
#define IT_LAST_SMALL   9
#define IT_LAST         14
#ifndef BUILDING_MAIN
extern const char ITEM_CHAR[];
#else
const char ITEM_CHAR[] = "csmOMpobafCelEw\0";
#endif
typedef struct {
	unsigned char
		flags;	/* Bitfield - See above for meanings */
	long	last_seen;
	UBYTE	class,      /* What planet class             */
		size,       /* Planet size                   */
		eff,        /* Efficiency                    */
		polut,      /* Polution level                */
		minr,	    /* Minerals                      */
		gold,       /* Richness of gold              */
		water,      /* Pct water                     */
		gas,        /* Pct gas                       */
		mobil,      /* Mobility units                */
		PopPct;     /* Population percent            */
	USHORT	PF;         /* Plague factor                 */
	USHORT	TF;         /* Tech factor                   */
	UBYTE	xfer,       /* How the planet was xfered     */
		owner,      /* Planet owner                  */
		lstOwn,     /* Previous owner                */
		plgStg;     /* Plague stage                  */
	USHORT	RF;               /* Tech factor                    */
	ULONG	ResLv,        /* Research level                 */
		TechLv;           /* Technology level               */
	USHORT	pl_row,       /* Location (row)                 */
		pl_col,           /* Location (col)                 */
		pl_btu,           /* BTUs                           */
		prod[PPROD_LAST + 1], /* Production points/type     */
		quant[IT_LAST + 1];   /* Items on planet            */
	UBYTE	workPer[PPROD_LAST + 1]; /* 0-100% of prodct    */
	char	name[PL_NAME_LEN],
		chkpoint[PL_PSWD_LEN];
	} FePlanet_t;

#define bp_computer     0
#define bp_engines      1
#define bp_hull         2
#define bp_lifeSupp     3
#define bp_sensors      4
#define bp_teleport     5
#define bp_photon       6
#define bp_blaser       7
#define bp_shield       8
#define bp_tractor      9
typedef UBYTE BigPart_t;
#define BP_FIRST    0
#define BP_LAST     9
#ifndef BUILDING_MAIN
extern const char BIG_PART_CHAR[];
#else
const char BIG_PART_CHAR[] = "cehlstpbST\0";
#endif

typedef struct {
	unsigned char
		flags;	          /* Bitfield - See above for meanings */
	time_t	last_seen;
	UBYTE	type,         /* What type of item              */
	        efficiency,   /* efficiency                     */
		owner;            /* Item owner                     */
	ULONG	where;        /* What planet/ship it is on      */
	USHORT  it_tf,        /* Tech factor                    */
	        weight;       /* How much does it weigh?        */
	} FeItem_t;

#define BF_IS_BH	    1 /* Is a black hole                */
#define BF_IS_SN	    2 /* Is a super nova                */
#define BF_IS_NORMAL	4 /* Has at least 1 planet/star     */
#define BF_IS_EMPTY	    8 /* Has no planets/stars           */

typedef struct {
	unsigned char
	        flags;	  /* Bitfield - See above for meanings  */
	USHORT  num_plan, /* Also stars                         */
		num_ship; /* These speed up access              */
	ULONG   plan[MAX_SECT_PLAN], /* Planet array            */
		ship[MAX_SECT_SHIP]; /* Ship array              */
	} FeSector_t;

#define BM_UNKNOWN	0
#define BM_PLAN		1 /* Unknown planet type */
#define BM_STAR		2
#define BM_BH		3
#define BM_SN		4
#define BM_PC_A		5 /* Rocky, lifeless, no minerals */
#define BM_PC_B		6 /* entirely water, no land masses */
#define BM_PC_C		7 /* Gaseous */
#define BM_PC_D		8 /* generic planet class, but no intelligent life */
#define BM_PC_M		9 /* earth-type, likely to support int. life */
#define BM_PC_N		10 /* rocky, no native life, good min */
#define BM_PC_O		11 /* about the same as N, but good gold */
#define BM_PC_Q		12 /* Very rare, maybe one or two per game */
#define BM_PC_H		13 /* Home planet */
#define BM_EMPTY	14
typedef struct {
	UBYTE type:4;
	} FeBitMapType_t;
typedef struct {
	FeBitMapType_t coord[2560][2560];
	} FeBitmap_t;

#ifndef BUILDING_MAIN
extern
#endif
FeBitmap_t *BitMap;		/* Pointer to the memmap'ed bitmap file */
                                /* Reference as BitMap->coord[row][col] */

#define relation_unknown	0
#define relation_default	1
#define relation_allied		2
#define relation_war		3

#define ps_notseen  0
#define ps_deity    1
#define ps_active   2
#define ps_quit     3
#define ps_visitor  4
#define ps_idle     5
typedef struct {
	time_t	last_seen,
		last_on;
	ULONG	planets;	/* # planets owned */
	long	money;		/* How much $$$    */
	UBYTE	status,		/* ps_deity -> ps_idle */
		number,		/* Player #        */
		race,		/* What race are they */
		relation;	/* Players relation to them */
	USHORT	civ,
		mil,
		sh,		/* Ships... */
		bar;
	short	power;
		
	char	name[NAME_LEN];	/* Player name     */
	} Player_t;

typedef struct {
	char	name[NAME_LEN];	/* Races name               */
	UBYTE	relation;	/* Players relation to race */
	UBYTE	homeKnown;	/* TRUE once "home" is set  */
				/* (fits in what was padding, so the
				   races database layout is unchanged) */
	ULONG	home;		/* Home planet #            */
	} Race_t;
/*
 * prototypes
 */

extern void impfeCleanup(void); /* called before exiting - can use GTK calls */
extern void impfeQuit(void);    /* saves everything and leaves the main loop */

extern void eShowInMain(); /* Displays an error in the main window text area */
extern void ShowInMain(); /* Displays normal text in the main window text area */

extern void fe_puts(const char *string);
extern void fe_parse(const char *string);

/* feconfig.c */
extern void handleOpenCfg(void);
extern void handleSaveCfg(void);
extern void handleCloseCfg(void);
extern void handleNewCfg(void);

/* database.c */
extern BOOL openFeDBs(BOOL create);
extern void closeFeDBs(void);
extern void flushCache(void);
extern BOOL readShip(FeShip_t *shBuf, unsigned long shNum);
extern BOOL writeShip(FeShip_t *shBuf, unsigned long shNum);
extern BOOL readPlanet(FePlanet_t *plBuf, unsigned long plNum);
extern BOOL writePlanet(FePlanet_t *plBuf, unsigned long plNum);
extern BOOL readItem(FeItem_t *itBuf, unsigned long itNum);
extern BOOL writeItem(FeItem_t *itBuf, unsigned long itNum);
extern BOOL readSector(FeSector_t *scBuf, USHORT row, USHORT col);
extern BOOL writeSector(FeSector_t *scBuf, USHORT row, USHORT col);
extern ULONG getSectPlanet(USHORT row, USHORT col);
extern ULONG getSectShip(USHORT row, USHORT col);
extern void getSectShList(USHORT row, USHORT col, char *buf);
extern void addSectShip(USHORT row, USHORT col, ULONG shNum);
extern void delSectShip(USHORT row, USHORT col, ULONG shNum);
extern void addSectPlan(USHORT row, USHORT col, ULONG plNum);

/* impcon.c */
extern BOOL cmd_connect(BOOL kill_it);
extern int write_data(int sock, const char *buf, int len);
extern void write_str(const char *buf);

/* gui_cb.c - what the next line typed into the main input box is for */
typedef enum
{
	INPUT_NORMAL,		/* a command or answer */
	INPUT_SECRET,		/* a password - hidden, not echoed */
	INPUT_PLAYER_NAME	/* the player name at login */
} InputMode_t;
extern void setInputMode(InputMode_t mode);
extern void showServerQuestion(const char *question);

/* planet.c - who owns a planet, for display */
extern const char *planetOwnerName(const FePlanet_t *pl, ULONG plNum,
	BOOL brief);
extern int planetHomeRace(ULONG plNum);
/* parse.c - the race we belong to (-1 if not known yet) */
extern int ourRace(void);

/* main.c */
extern void setDefaults(void);

/* cmnd.c */
extern int addCmd(const char *line, int priority);
extern BOOL modCmd(int pos, const char *line, int priority);
extern const char *getCmd(int pos);
extern int getPrior(int pos);
extern BOOL cancelCmd(int cmdKey);
extern void initCmdQueues(void);
extern void freeCmdQueues(void);
extern void runCmdQueue(void);
extern void loadCmdQueue(void);
extern void saveCmdQueue(void);
extern void buildCmdList(void);

/* femap.c */
extern void buildMapDisp(void);
extern void scrollFeMap(int stepX, int stepY);
extern void updateMaps(void);

/* help.c */
extern void cb_help_button(long chapter);

/* ship.c */
extern void buildShCensus(void);
extern void buildShDetDisp(void);

/* planet.c */
extern void buildPlCensus(void);
extern void buildPlDetDisp(void);

/*
 * Handle functions that may not be present
 */

#ifndef HAVE_STRCHR
#ifdef HAVE_INDEX
#define strchr index
#else
"BOGUS - You don't have strchr() OR index()"
#endif
#endif

#ifndef HAVE_BCOPY
# ifndef HAVE_MEMMOVE
#  ifndef HAVE_MEMCPY
"BOGUS - You don't have a safe bcopy(), memmove(), or memcpy()!"
#  else
#   define bcopy(s,d,l) memcpy((d), (s), (l))
#  endif
# else
#  define bcopy(s,d,l) memmove((d), (s), (l))
# endif
#endif

/*
 * Global variable definitions
 */
#ifndef BUILDING_MAIN
extern int game_time;
#else
int game_time=0;
#endif
#ifndef BUILDING_MAIN
extern int game_btus;
#else
int game_btus=0;
#endif
#ifndef BUILDING_MAIN
extern int GameSocket;
#else
int GameSocket=-1;
#endif

#define MAX_HOST_LEN	81
#define MAX_PORT_LEN	8
#define MAX_PLAYER_LEN	36
#define MAX_REP_LEN	36

/* These get set from our config files XXXX */
#ifndef BUILDING_MAIN
extern
#endif
Player_t Player[PLAYER_MAX];
#ifndef BUILDING_MAIN
extern
#endif
Race_t Race[RACE_MAX];
#ifndef BUILDING_MAIN
extern
#endif
char game_host[MAX_HOST_LEN];
#ifndef BUILDING_MAIN
extern
#endif
char game_port[11];
#ifndef BUILDING_MAIN
extern
#endif
char game_player[MAX_PLAYER_LEN];
#ifndef BUILDING_MAIN
extern
#endif
char game_player_pswd[MAX_PLAYER_LEN];
#ifndef BUILDING_MAIN
extern
#endif
char game_desc[81];
#ifndef BUILDING_MAIN
extern
#endif
char RawFile[1024];		/* File name of the active config */
#ifndef BUILDING_MAIN
extern
#endif
ULONG LastShip;			/* Last ship selected */
#ifndef BUILDING_MAIN
extern
#else
#ifdef CreateStatic
static
#endif
#endif
ULONG LastPlanet;		/* Last planet selected */
#ifndef BUILDING_MAIN
extern
#endif
unsigned short int HomeRow;	/* Home Planet Row */
#ifndef BUILDING_MAIN
extern
#endif
unsigned short int HomeCol;	/* Home Planet Col */
#ifndef BUILDING_MAIN
extern
#endif
unsigned short int LastX;	/* Last (top left) X coord */
#ifndef BUILDING_MAIN
extern
#endif
unsigned short int LastY;	/* Last (top left) Y coord */
#ifndef BUILDING_MAIN
extern
#endif
unsigned short int game_sizex;	/* Size of the world horizontally */
#ifndef BUILDING_MAIN
extern
#endif
unsigned short int game_sizey;	/* Size of the world vertically */
#ifndef BUILDING_MAIN
extern
#endif
unsigned short int game_version; /* The config file format version */
#ifndef BUILDING_MAIN
extern
#endif
unsigned long int next_ship;    /* Highest ship number seen */
#ifndef BUILDING_MAIN
extern
#endif
unsigned long int next_planet;  /* Highest ship number seen */
#ifndef BUILDING_MAIN
extern
#endif
unsigned long int next_item;    /* Highest ship number seen */
#ifndef BUILDING_MAIN
extern
#endif
unsigned short int player_num;	/* Player number */
#ifndef BUILDING_MAIN
extern
#endif
unsigned long int home_planet;  /* Home planet */
#ifndef BUILDING_MAIN
extern
#endif
BOOL DirtyData;			/* TRUE if we need to save data */
#ifndef BUILDING_MAIN
extern
#endif
BOOL AtMainPrompt;		/* TRUE if at the main prompt  */
#ifndef BUILDING_MAIN
extern
#endif
BOOL QueueOk;			/* TRUE if can queue commands */
#ifndef BUILDING_MAIN
extern
#endif
BOOL QueueEcho;			/* TRUE if want to echo queue commands */
#ifndef BUILDING_MAIN
extern
#endif
BOOL StopQueue;			/* TRUE if want to stop running  queued commands */
#ifndef BUILDING_MAIN
extern
#endif
USHORT NumCmds;			/* Number of queued commands */

/*
 * These are the keywords recognized by the config file parser
 * for "*.fe" files
 */
#define DEF_CFG_NAME "default.fe"
#define KEYWORD_GAME_VERSION "version"  /* Version of the .fe file */
#define KEYWORD_GAME_HOST "host"        /* Hostname              */
#define KEYWORD_GAME_PORT "port"        /* Port #                */
#define KEYWORD_GAME_PLAYER "player"    /* Player name           */
#define KEYWORD_GAME_PLAYER_PSWD "pswd" /* Player pswd           */
#define KEYWORD_GAME_DESC "desc"        /* Game description      */
#define KEYWORD_GAME_SIZEX "sizex"      /* Size of the game in X */
#define KEYWORD_GAME_SIZEY "sizey"      /* Size of the game in Y */
#define KEYWORD_HIGH_SHIP "next_ship"   /* Highest ship seen     */
#define KEYWORD_HIGH_PLANET "next_planet" /* Highest planet seen */
#define KEYWORD_HIGH_ITEM "next_item"   /* Highest item seen     */
#define KEYWORD_HOME_PLAN "home_planet" /* Home planet #         */
#define KEYWORD_PLAYER_NUM "player_num" /* What player # are we  */

/*
 * For the help file browser
 */
#ifndef BUILDING_MAIN
extern char *chapters[];
#else
char *chapters[] = {
	"%Start",               /* 0  */
	"%Overview",            /* 1  */
	"%Main",                /* 2  */
	"%Config",              /* 3 */
	NULL
};
#endif

#define CHAPTER_START    0 /* Used when calling help() to jump to */
#define CHAPTER_OVERVIEW 1 /* a given chapter                     */
#define CHAPTER_MAIN     2
#define CHAPTER_CONFIG   3
#define TOTAL_CHAPTERS   4
