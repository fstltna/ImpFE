#ifndef LINT
static char rcsid[]="$Id: parse.c,v 1.2 2000/05/25 23:48:19 marisa Exp $";
#endif
/*
 * ImpFE
 *
 * $Id: parse.c,v 1.2 2000/05/25 23:48:19 marisa Exp $
 *
 * This file contains routines for parsing Imperium<->FE codes
 *
 * $Log: parse.c,v $
 * Revision 1.2  2000/05/25 23:48:19  marisa
 * Update parsing for 4 digit rows in SRS
 *
 * Revision 1.1.1.1  2000/05/17 19:22:13  marisa
 * First CVS checkin
 *
 * Revision 1.1.1.11  1997/09/03 19:05:35  marisag
 * patch1: Add handleShipScan
 *
 * Revision 1.1.1.10  1997/05/03 09:42:52  marisag
 * patch1: Daily checkin
 *
 * Revision 1.1.1.9  1997/04/27 23:24:02  marisag
 * patch1: Daily checkin
 *
 * Revision 1.1.1.8  1997/04/27 16:34:11  marisag
 * patch1: Add code to handle planet dumps
 *
 * Revision 1.1.1.7  1997/04/25 06:39:32  marisag
 * patch1: Code for the planet & ship detail forms
 *
 * Revision 1.1.1.6  1997/04/24 04:46:13  marisag
 * patch1: Many changes
 *
 * Revision 1.1.1.5  1997/04/17 09:26:33  marisag
 * patch1: Added support for lines 1 & 2 of planet production report
 *
 * Revision 1.1.1.4  1997/04/16 03:07:39  marisag
 * patch1: Daily checkin
 *
 * Revision 1.1.1.3  1997/04/14 16:05:16  marisag
 * patch1: Daily checkin
 *
 * Revision 1.1.1.2  1997/04/14 02:52:47  marisag
 * patch1: Many MANY changes
 *
 * Revision 1.1.1.1  1997/04/12 16:51:40  marisag
 * patch1: Many changes - added functions to parse out almost all
 * patch1: ship data, player & race data, added stubs for more functions.
 *
 * Revision 1.1  1997/04/10 06:30:51  marisag
 * Initial revision
 *
 */

#define CreateStatic 1

#include "config.h"
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#ifdef I_STRING
#include <string.h>
#else
#include <strings.h>
#endif
#include "impfe.h"
#include "ImpFeGui.h"
#include "ImpFeMess.h"
#include "gui.h"

/* These set the various warning levels */
#define BTU_WARN 15
#define BTU_CRIT 5
#define TIME_WARN 10
#define TIME_CRIT 3

static char ParseBuf[2048];
static UBYTE LastRace=0;	/* Last race seen (for handling race rept 2nd line) */
#ifdef ZZZ
ULONG LastPlanet=0; /* Last planet seen (for handling planet rpt 2nd line) */
#endif

void handleComment(const char *string)
{
	sprintf(ParseBuf, "_%s\n", string);
	ui_browser_append(fd_ImpFeMain->MainBrowser, ParseBuf);
}

void handlePrompt(const char *string)
{
    char locBuf[25];
    int tmpBtu, tmpTime;

	/* Check for status type prompts */
	if (string[0] == '!')
	{
	    ui_set_text(fd_ImpFeMain->MainPrompt, &string[1]);
	    AtMainPrompt=FALSE;
	    return;
	}
	else
	{
	    ui_set_text(fd_ImpFeMain->MainPrompt, string);
	}
	/* See if at the main prompt */
	if (sscanf(string, "[%d:%d] Command", &tmpBtu, &tmpTime) == 2)
	{
	    AtMainPrompt=TRUE;
	    sprintf(locBuf, "%d", tmpBtu);
	    ui_set_text(fd_ImpFeMain->BtuDisp, locBuf);
	    if (tmpBtu < BTU_WARN)
	    {
			if (tmpBtu < BTU_CRIT)
			{
				ui_set_level(fd_ImpFeMain->BtuDisp, UI_LEVEL_CRIT);
			}
			else
			{
				ui_set_level(fd_ImpFeMain->BtuDisp, UI_LEVEL_WARN);
			}
	    }
	    else
	    {
			if (game_btus < BTU_WARN)
			{
				ui_set_level(fd_ImpFeMain->BtuDisp, UI_LEVEL_NORMAL);
			}
	    }
	    game_btus=tmpBtu;
	    sprintf(locBuf, "%d", tmpTime);
	    ui_set_text(fd_ImpFeMain->TimeDisp, locBuf);
	    if (tmpTime < TIME_WARN)
	    {
			if (tmpTime < TIME_CRIT)
			{
				ui_set_level(fd_ImpFeMain->TimeDisp, UI_LEVEL_CRIT);
			}
			else
			{
				ui_set_level(fd_ImpFeMain->TimeDisp, UI_LEVEL_WARN);
			}
	    }
	    else
	    {
			if (game_time < TIME_WARN)
			{
				ui_set_level(fd_ImpFeMain->TimeDisp, UI_LEVEL_NORMAL);
			}
	    }
	    game_time=tmpTime;
	    /* Update the maps, if needed */
	    updateMaps();
	}
	else
	{
	    AtMainPrompt=FALSE;
	}
}

void handleRace1(const char *line)
{
	unsigned int raceNum, techLev, numPlan;
	char raceName[NAME_LEN], raceStatus[NAME_LEN];

	/* Make sure this gets saved */
	DirtyData=TRUE;
	/* Show the line */
	ui_browser_append(fd_ImpFeMain->MainBrowser, line);
	/* Race # | Race Name | Tech Lev | # Planets | Status */
	if (sscanf(line, "| %u | %s | %u | %u | %s |", &raceNum, raceName,
			   &techLev, &numPlan, raceStatus) != 5)
	{
	    fputs("*** Invalid race report line 1 format\n", stderr);
	}
	else
	{
	    strcpy(Race[raceNum].name, raceName);
		LastRace=raceNum;
	}
}

void handleRace2(const char *line)
{
	/* just dump it out, for now  since we don't save any of this data */
	/* Make sure this gets saved */
	/*DirtyData=TRUE;*/
	/* Home planet name | Res Lev | # Players */
	/* "| | %s | %u | %u | |" */
	ui_browser_append(fd_ImpFeMain->MainBrowser, line);
}

/* The race we belong to, once the player list has told us */
static int OurRace = -1;

int ourRace(void)
{
	if ((OurRace < 0) && (home_planet != 0))
	{
		/* Known from an earlier session: the race whose home we share */
		return(planetHomeRace(home_planet));
	}
	return(OurRace);
}

/*
 * setRaceHome - records which planet is a race's home planet. Home
 *               planets belong to a race rather than to a player.
 */
static void setRaceHome(int race, ULONG plNum)
{
	if ((race < 0) || (race >= RACE_MAX))
	{
		return;
	}
	if (!Race[race].homeKnown || (Race[race].home != plNum))
	{
		Race[race].home = plNum;
		Race[race].homeKnown = TRUE;
		DirtyData = TRUE;
	}
}

void handlePlayerList(const char *line)
{
	unsigned int playNum, playTime, playBtu;
	char playName[NAME_LEN], playStatus[NAME_LEN], playRace[NAME_LEN],
		lastDay[8], lastMon[8], lastDate[8], lastTime[15];
	int tmp;

	/* Make sure this gets saved */
	DirtyData=TRUE;
	ui_browser_append(fd_ImpFeMain->MainBrowser, line);
	/*  Player # | Day Mon Date Time | Game Time | BTU | Status | Race | Name */
	if (sscanf(line, "%u  %s  %s %s %s [%u] [%u] %s %s %s", &playNum,
			   lastDay, lastMon, lastDate, lastTime, &playTime,
			   &playBtu, playStatus, playRace, playName) != 10)
	{
	    fputs("*** Invalid player list format\n", stderr);
	}
	else
	{
	    strcpy(Player[playNum].name, playName);
	    /* See if this is us - the server ignores case in names, so we must too */
	    if ((game_player[0] != '\0') && (strcasecmp(playName, game_player) == 0))
	    {
			/* Yep!! */
			if (player_num != playNum)
			{
				if (player_num == 0)
				{
					fe_puts(">>> Ah! Found out your player number.\n");
				}
				else
				{
					fe_puts(">>> Hmm, your player number seems to have changed!\n");
				}
				player_num = playNum;
				/* Ships we already know about may have been saved without
				   an owner (shown as ???) - refresh them */
				(void) addCmd("ship status\n", QUE_HIGH);
				(void) addCmd("ship big\n", QUE_HIGH);
			}
	    }
	    /* The player list only shows the first few letters of the race */
	    for (tmp=0; tmp < RACE_MAX; tmp++)
	    {
			if ((Race[tmp].name[0] != '\0') &&
				(strncasecmp(Race[tmp].name, playRace, strlen(playRace)) == 0))
			{
				Player[playNum].race = tmp;
				if (playNum == player_num)
				{
					OurRace = tmp;
					if (home_planet != 0)
					{
						setRaceHome(tmp, home_planet);
					}
				}
				break;
			}
	    }
	    if (strcmp(playStatus, "DEITY") == 0)
	    {
			Player[playNum].status = ps_deity;
	    }
	    else if (strcmp(playStatus, "Active") == 0)
	    {
			Player[playNum].status = ps_active;
	    }
	    else if (strcmp(playStatus, "Idle") == 0)
	    {
			Player[playNum].status = ps_idle;
	    }
	    else if (strcmp(playStatus, "Quit") == 0)
	    {
			Player[playNum].status = ps_quit;
	    }
	    else if (strcmp(playStatus, "Visitor") == 0)
	    {
			Player[playNum].status = ps_visitor;
	    }
	}
}

void handlePowerRpt(const char *line)
{
	ULONG plan, civ, mil, sh, gun, pl, bar, pct, ship;
	long cash;
	int power;
	char playName[NAME_LEN];
	int tmp;

	/* Make sure this gets saved */
	DirtyData=TRUE;
	ui_browser_append(fd_ImpFeMain->MainBrowser, line);
	/*  # Plan | # civ | # mil | sh | gun | pl | bars | % | ships | $$$ | power | Name */
	if ((tmp=sscanf(line, "%lu %lu %lu %lu %lu %lu %lu %lu %lu %ld %d %s",
			   &plan, &civ, &mil, &sh, &gun, &pl, &bar, &pct, &ship,
			   &cash, &power, playName)) != 12)
	{
	    fprintf(stderr, "*** Invalid power report format '%d'\n", tmp);
	}
	else
	{
		for (tmp = 0; tmp < PLAYER_MAX; tmp++)
		{
			if (Player[tmp].status != ps_notseen)
			{
				if (strcmp(Player[tmp].name, playName) == 0)
				{
					Player[tmp].planets = plan;
					Player[tmp].civ = civ;
					Player[tmp].mil = mil;
					Player[tmp].sh = ship;
					Player[tmp].bar = bar;
					Player[tmp].power = power;
					/* Don't assign "cash" if the line is for the player
					 * because the power report may be out of date - player
					 * status would be better...
					 */
					if (tmp != player_num)
					{
						Player[tmp].money = cash;
					}
					break;
				}
			}
		}
	}
}

void zapEol(char *buf)
{
	while (*buf != '\0')
	{
	    if (*buf == '\n')
	    {
			*buf = '\0';
			return;
	    }
	    buf++;
	}
}

void handleShipStatus(const char *line)
{
	FeShip_t tmpShip;
	ULONG shipNum;
	USHORT oRow, oCol;
	char shipName[NAME_LEN], shipCoords[15];
	char *endPtr;
	int tmp;
	struct timeval tp;
	char work[80];

	/* Make sure this gets saved */
	DirtyData=TRUE;
	ui_browser_append(fd_ImpFeMain->MainBrowser, line);
	/*  Ship # | Type | Row,Col | Fleet | Eff | TF | Energ | Fuel |Armor | Price | Planet | Name */
	if ((line[12] != '|') || (line[22] != '|') || (line[24] != '|') ||
		(line[28] != '%'))
	{
	    fprintf(stderr, "*** Invalid ship status format\n");
	}
	else
	{
		endPtr=strrchr(line, '|');
		strcpy(shipName, (char *)(endPtr + sizeof(char)));
		zapEol(shipName);
		strncpy(work, line, 9 * sizeof(char));
		shipNum=(unsigned long) atoi(work);
		/* This will create an initialized ship struct, if needed */
		(void) readShip(&tmpShip, shipNum);

		strcpy(tmpShip.name, shipName);
		(void)gettimeofday(&tp, NULL);
		tmpShip.last_seen=tp.tv_sec;
		tmpShip.ShipType=line[11];
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (13 * sizeof(char)),
				4 * sizeof(char));
		oRow = tmpShip.sh_row;
		tmpShip.sh_row=atoi(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (18 * sizeof(char)),
				4 * sizeof(char));
		oCol = tmpShip.sh_col;
		tmpShip.sh_col=atoi(work);
		if ((oRow != tmpShip.sh_row) || (oCol != tmpShip.sh_col))
		{
			delSectShip(oRow, oCol, shipNum);
			addSectShip(tmpShip.sh_row, tmpShip.sh_col, shipNum);
		}
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (58 * sizeof(char)),
				8 * sizeof(char));
		tmpShip.planet = (unsigned long) atoi(work);
		if (tmpShip.planet != 0)
		{
			tmpShip.flags |= BF_ONPLAN;
		}
		else
		{
			tmpShip.flags &= ~BF_ONPLAN;
		}
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (40 * sizeof(char)),
				5 * sizeof(char));
		tmpShip.fuelLeft = atoi(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (46 * sizeof(char)),
				5 * sizeof(char));
		tmpShip.armourLeft = atoi(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (34 * sizeof(char)),
				5 * sizeof(char));
		tmpShip.energy = atoi(work);
		tmpShip.fleet = line[23];
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (25 * sizeof(char)),
				3 * sizeof(char));
		tmpShip.efficiency = atoi(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (30 * sizeof(char)),
				3 * sizeof(char));
		tmpShip.sh_tf = atoi(work);
		/* Since only the owner can use the ships command */
		tmpShip.owner = player_num;
		tmpShip.flags |= BF_OWNED;

		/* Write the ship */
		writeShip(&tmpShip, shipNum);
	}
}

void handleSRS(const char *line)
{
	const char *workPtr;
	int tmp, left, right, row, numCol;
	char work[80], type;

	/* Make sure this gets saved */
	DirtyData=TRUE;
	workPtr=&line[9];
	ui_browser_append(fd_ImpFeMain->MainBrowser, workPtr);
	/* Get the left coord */
	strncpy(work, line, 4 * sizeof(char));
	work[4]='\0';
	left=atoi(work);
	/* Get the right coord */
	strncpy(work, &line[5], 4 * sizeof(char));
	work[4]='\0';
	right=atoi(work);
	/* Get the row we are scanning */
	strncpy(work, &line[9], 4 * sizeof(char));
	work[4]='\0';
	row=atoi(work);
	/* Set workPtr to the start of the actual data */
	workPtr = &line[13];
	numCol = (right - left) + 1;
	for (tmp=0; tmp < numCol; tmp++)
	{
		type = BitMap->coord[row][left+tmp].type;
		switch (workPtr[tmp])
		{
			case '?': /* Out of range */
				/* Do nothing */
				break;
			case ' ': /* Empty space */
				/* Make sure we don't overwrite another */
				/* item we scanned before */
				if (type == BM_UNKNOWN)
				{
				    BitMap->coord[row][left+tmp].type = BM_EMPTY;
				}
				break;
			case '0': /* Unknown planet */
				/* Make sure we don't overwrite another */
				/* item we scanned before */
				if ((type == BM_UNKNOWN)
				    || (type == BM_EMPTY))
				{
				    BitMap->coord[row][left+tmp].type = BM_PLAN;
				}
				break;
			case '*': /* Star */
				BitMap->coord[row][left+tmp].type = BM_STAR;
				break;
			case 'A': /* Class A Planet */
				BitMap->coord[row][left+tmp].type = BM_PC_A;
				break;
			case 'B': /* Class B Planet */
				BitMap->coord[row][left+tmp].type = BM_PC_B;
				break;
			case 'C': /* Class C Planet */
				BitMap->coord[row][left+tmp].type = BM_PC_C;
				break;
			case 'D': /* Class D Planet */
				BitMap->coord[row][left+tmp].type = BM_PC_D;
				break;
			case 'M': /* Class M Planet */
				BitMap->coord[row][left+tmp].type = BM_PC_M;
				break;
			case 'N': /* Class N Planet */
				BitMap->coord[row][left+tmp].type = BM_PC_N;
				break;
			case 'O': /* Class O Planet */
				BitMap->coord[row][left+tmp].type = BM_PC_O;
				break;
			case 'Q': /* Class Q Planet */
				BitMap->coord[row][left+tmp].type = BM_PC_Q;
				break;
			case 'H': /* Class H Planet */
				BitMap->coord[row][left+tmp].type = BM_PC_H;
				break;
			case '$': /* Supernova */
				BitMap->coord[row][left+tmp].type = BM_SN;
				break;
			case '@': /* Black hole */
				BitMap->coord[row][left+tmp].type = BM_BH;
				break;
			default:
				fprintf(stderr, "Unknown sector type! '%c'\n",
						workPtr[tmp]);
				break;	
		}
	}
}

/*
 * cvtAbvAmt - converts a (possibly abreviated) amount to an
 *             unsigned short. Assumes input string is 3 characters!
 */
unsigned short cvtAbvAmt(const char *str)
{
	unsigned short val;
	unsigned short multip=1;

#ifdef DEBUG
	if (strlen(str) != 4)
	{
		fprintf(stderr, "cvtAbvAmt called with a string that is not 3 characters\n");
		return(0);
	}
#endif
	if (str[2] == 'K')
	{
		multip=1000;
	}
	else if (str[2] == 'x')
	{
		multip=100;
	}
	val=((unsigned short)atoi(str)) * multip;
	return(val);
}

void handlePlProd1(const char *line)
{
	FePlanet_t tmpPlanet;
	ULONG plNum;
	int tmp, civ, mil, tech, res, edu, ocs, om, gm, atn, ftn, wea, eng, hul,
		mis, pln, elc, csh;
	struct timeval tp;
	char work[80], pc[3], posit[15], *workPtr;

	/* Make sure this gets saved */
	DirtyData=TRUE;
	ui_browser_append(fd_ImpFeMain->MainBrowser, line);
	/* All of the below are scaled production amounts */
	/* Civ | Mil | Tech | Res | Edu | OCS | OM | GM | ATn | FTn | Wea | Eng | Hul | Misl | Pln | Elc | Cash | Pl # |*/
	if ((strlen(line) < 69) || (line[0] != '|') || (line[4] != '|') ||
		(line[8] != '|') || (line[68] != '|'))
	{
	    fprintf(stderr, "*** Invalid planet production1 format '%s'\n", line);
	}
	else
	{
		strncpy(work, &line[69], 8);
		work[8]='\0';
		plNum=(unsigned long) atoi(work);
		/* This will create an initialized planet struct, if needed */
		(void) readPlanet(&tmpPlanet, plNum);
		LastPlanet = plNum; /* So we can handle the 2nd line */

		(void)gettimeofday(&tp, NULL);
		tmpPlanet.last_seen=tp.tv_sec;

		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (1 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.prod[0] = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (5 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.prod[1] = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (9 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.prod[2] = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (13 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.prod[3] = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (17 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.prod[4] = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (21 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.prod[5] = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (25 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.prod[6] = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (29 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.prod[7] = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (33 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.prod[8] = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (37 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.prod[9] = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (41 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.prod[10] = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (45 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.prod[11] = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (49 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.prod[12] = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (53 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.prod[13] = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (57 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.prod[14] = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (61 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.prod[15] = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (65 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.prod[16] = cvtAbvAmt(work);
		/* Since only the owner can use the census command */
		tmpPlanet.owner = player_num;
		tmpPlanet.flags |= BF_OWNED;

		/* Write the planet */
		writePlanet(&tmpPlanet, plNum);
	}
}

void handlePlProd2(const char *line)
{
	FePlanet_t tmpPlanet;
	ULONG plNum;
	int tmp, civ, mil, tech, res, edu, ocs, om, gm, atn, ftn, wea, eng, hul,
		mis, pln, elc, csh;
	struct timeval tp;
	char work[80], pc[3], posit[15], *workPtr;

	/* Make sure this gets saved */
	DirtyData=TRUE;
	ui_browser_append(fd_ImpFeMain->MainBrowser, line);
	/* All of the below are production percentages (0-100) */
	/* Civ | Mil | Tech | Res | Edu | OCS | OM | GM | ATn | FTn | Wea | Eng | Hul | Misl | Pln | Elc | Cash | Pl # |*/
	if ((strlen(line) < 69) || (line[0] != '|') || (line[4] != '|') ||
		(line[8] != '|') || (line[68] != '|'))
	{
	    fprintf(stderr, "*** Invalid planet production2 format '%s'\n", line);
	}
	else
	{
		plNum=LastPlanet;
		/* This will create an initialized planet struct, if needed */
		(void) readPlanet(&tmpPlanet, plNum);

		(void)gettimeofday(&tp, NULL);
		tmpPlanet.last_seen=tp.tv_sec;

		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (1 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.workPer[0] = atoi(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (5 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.workPer[1] = atoi(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (9 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.workPer[2] = atoi(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (13 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.workPer[3] = atoi(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (17 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.workPer[4] = atoi(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (21 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.workPer[5] = atoi(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (25 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.workPer[6] = atoi(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (29 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.workPer[7] = atoi(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (33 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.workPer[8] = atoi(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (37 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.workPer[9] = atoi(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (41 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.workPer[10] = atoi(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (45 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.workPer[11] = atoi(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (49 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.workPer[12] = atoi(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (53 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.workPer[13] = atoi(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (57 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.workPer[14] = atoi(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (61 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.workPer[15] = atoi(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (65 * sizeof(char)),
				3 * sizeof(char));
		tmpPlanet.workPer[16] = atoi(work);
		/* Since only the owner can use the census command */
		tmpPlanet.owner = player_num;
		tmpPlanet.flags |= BF_OWNED;

		/* Write the planet */
		writePlanet(&tmpPlanet, plNum);
	}
}

void handlePlGeo(const char *line)
{
	FePlanet_t tmpPlanet;
	ULONG plNum;
	USHORT oRow, oCol;
	int tmp, size, eff, minr, gold, polut, gas, water, mobil, ore, bars,
		TF, ResLv;
	struct timeval tp;
	char work[80], pc[3], posit[15], *workPtr;

	/* Make sure this gets saved */
	DirtyData=TRUE;
	ui_browser_append(fd_ImpFeMain->MainBrowser, line);
	/* PC | Sz | Eff | Min | Gold | Pol | Gas | Wat | Mob | Ore | Bar | TF | ResL | Pl # | Position */
	if ((tmp=sscanf(line, "%s %u %u %u %u %u %u %u %u %u %u %u %u %lu %s",
					pc, &size, &eff, &minr, &gold, &polut, &gas, &water,
					&mobil, &ore, &bars, &TF, &ResLv, &plNum, posit)) != 15)
	{
	    fprintf(stderr, "*** Invalid planet geo format '%d'\n", tmp);
	}
	else
	{
		/* This will create an initialized planet struct, if needed */
		(void) readPlanet(&tmpPlanet, plNum);

		(void)gettimeofday(&tp, NULL);
		tmpPlanet.last_seen=tp.tv_sec;

		/* Check for the star indicating we own the planet */
		if (pc[0] == '*')
		{
			tmpPlanet.class = pc[1];
		}
		else
		{
			tmpPlanet.class = pc[0];
		}
		tmpPlanet.size = size;
		tmpPlanet.eff = eff;
		tmpPlanet.minr = minr;
		tmpPlanet.gold = gold;
		tmpPlanet.polut = polut;
		tmpPlanet.gas = gas;
		tmpPlanet.water = water;
		tmpPlanet.mobil = mobil;
		tmpPlanet.quant[it_ore] = ore;
		tmpPlanet.quant[it_bars] = bars;
		tmpPlanet.TF = TF;
		tmpPlanet.ResLv = ResLv;
		bzero(work, 80 * sizeof(char));
		workPtr=strchr(posit, ',');
		if (workPtr == NULL)
		{
			fprintf(stderr, "Unable to find the comma in the position!\n");
		}
		else
		{
			strncpy(work, posit,
					(workPtr - posit) * sizeof(char));
			oRow = tmpPlanet.pl_row;
			oCol = tmpPlanet.pl_col;
			tmpPlanet.pl_row=atoi(work);
			bzero(work, 80 * sizeof(char));
			strcpy(work, workPtr + sizeof(char));
			tmpPlanet.pl_col=atoi(work);
			if ((oRow != tmpPlanet.pl_row) ||
				(oCol != tmpPlanet.pl_col))
			{
				addSectPlan(tmpPlanet.pl_row, tmpPlanet.pl_col,
					plNum);
			}
		}
		/* Since only the owner can use the census command */
		tmpPlanet.owner = player_num;
		tmpPlanet.flags |= BF_OWNED;

		/* Write the planet */
		writePlanet(&tmpPlanet, plNum);
	}
}

void handlePlPop(const char *line)
{
	FePlanet_t tmpPlanet;
	ULONG plNum;
	int tmp, civil, scien, mil, ofc, BTU, PopPct, PF, race;
	struct timeval tp;
	char work[80], pc[3], name[PL_NAME_LEN], *workPtr;

	/* Make sure this gets saved */
	DirtyData=TRUE;
	ui_browser_append(fd_ImpFeMain->MainBrowser, line);
	/* PC | Civ | Sci | Mil | Ofc | BTU | Pop% | PF | Race | Pl # | Name */
	tmp=sscanf(line, "%s %u %u %u %u %u %u %u %u %lu %s",
					pc, &civil, &scien, &mil, &ofc, &BTU, &PopPct, &PF,
					&race, &plNum, name);
	/* Handle the fact they may not have a planet name... */
	if ((tmp != 10) && (tmp != 11))
	{
	    fprintf(stderr, "*** Invalid planet population format '%d'\n", tmp);
	}
	else
	{
		/* Handle no name specified */
		if (tmp == 10)
		{
		    name[0]='\0';
		}
		/* This will create an initialized planet struct, if needed */
		(void) readPlanet(&tmpPlanet, plNum);

		(void)gettimeofday(&tp, NULL);
		tmpPlanet.last_seen=tp.tv_sec;

		/* Check for the star indicating we own the planet */
		if (pc[0] == '*')
		{
			tmpPlanet.class = pc[1];
		}
		else
		{
			tmpPlanet.class = pc[0];
		}
		tmpPlanet.quant[it_civilians] = civil;
		tmpPlanet.quant[it_scientists] = scien;
		tmpPlanet.quant[it_military] = mil;
		tmpPlanet.quant[it_officers] = ofc;
		tmpPlanet.PopPct = PopPct;
		tmpPlanet.PF = PF;
		tmpPlanet.pl_btu = BTU;
		strcpy(tmpPlanet.name, name);

		/* Since only the owner can use the census command */
		tmpPlanet.owner = player_num;
		tmpPlanet.flags |= BF_OWNED;

		/* Write the planet */
		writePlanet(&tmpPlanet, plNum);
	}
}

void handleShipCargo(const char *line)
{
	FeShip_t tmpShip;
	ULONG shipNum;
	int tmp;
	struct timeval tp;
	char work[80];

	/* Make sure this gets saved */
	DirtyData=TRUE;
	ui_browser_append(fd_ImpFeMain->MainBrowser, line);
	/*  Ship # | Type | Avl | Civ | Sci | Mil | Ofc | Mis | Pln | Ore | Bar | Air | FTn | Cmp | Eng | Lif | Ele | Wpn*/
	if ((line[10] != '|') || (line[14] != '|') || (line[18] != '|') ||
		(line[22] != '|'))
	{
	    fprintf(stderr, "*** Invalid ship cargo format\n");
	}
	else
	{
		strncpy(work, line, 9 * sizeof(char));
		shipNum=(unsigned long) atoi(work);
		/* This will create an initialized ship struct, if needed */
		(void) readShip(&tmpShip, shipNum);

		(void)gettimeofday(&tp, NULL);
		tmpShip.last_seen=tp.tv_sec;
		tmpShip.ShipType=line[9];

		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (11 * sizeof(char)),
				3 * sizeof(char));
		tmpShip.cargo = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (15 * sizeof(char)),
				3 * sizeof(char));
		tmpShip.num_civ = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (19 * sizeof(char)),
				3 * sizeof(char));
		tmpShip.num_sci = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (23 * sizeof(char)),
				3 * sizeof(char));
		tmpShip.num_mil = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (27 * sizeof(char)),
				3 * sizeof(char));
		tmpShip.num_ofc = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (31 * sizeof(char)),
				3 * sizeof(char));
		tmpShip.num_misl = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (39 * sizeof(char)),
				3 * sizeof(char));
		tmpShip.num_ore = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (43 * sizeof(char)),
				3 * sizeof(char));
		tmpShip.num_gold = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (47 * sizeof(char)),
				3 * sizeof(char));
		tmpShip.num_airt = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (51 * sizeof(char)),
				3 * sizeof(char));
		tmpShip.num_ftnk = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (55 * sizeof(char)),
				3 * sizeof(char));
		tmpShip.num_comp = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (59 * sizeof(char)),
				3 * sizeof(char));
		tmpShip.num_eng = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (63 * sizeof(char)),
				3 * sizeof(char));
		tmpShip.num_life = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (67 * sizeof(char)),
				3 * sizeof(char));
		tmpShip.num_elect = cvtAbvAmt(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (71 * sizeof(char)),
				3 * sizeof(char));
		tmpShip.num_wpn = cvtAbvAmt(work);

		/* Since only the owner can use the ships command */
		tmpShip.owner = player_num;
		tmpShip.flags |= BF_OWNED;

		/* Write the ship */
		writeShip(&tmpShip, shipNum);
	}
}

void handleShipBig(const char *line)
{
	FeItem_t tmpItem;
	ULONG itemNum;
	int tmp;
	struct timeval tp;
	char work[80];

	/* Make sure this gets saved */
	DirtyData=TRUE;
	ui_browser_append(fd_ImpFeMain->MainBrowser, line);
	/*  Item # | Ship # | TF | T | Wgt | Eff | Ins */
	if ((line[8] != '|') || (line[17] != '|') || (line[22] != '|') ||
		(line[26] != '|'))
	{
	    fprintf(stderr, "*** Invalid ship big item format\n");
	}
	else
	{
		strncpy(work, line, 8 * sizeof(char));
		itemNum=(unsigned long) atoi(work);
		/* This will create an initialized item struct, if needed */
		(void) readItem(&tmpItem, itemNum);

		(void)gettimeofday(&tp, NULL);
		tmpItem.last_seen=tp.tv_sec;
		tmpItem.type=line[24];

		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (9 * sizeof(char)),
				9 * sizeof(char));
		tmpItem.where = atoi(work);
		tmpItem.flags &= ~BF_ONPLAN;
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (18 * sizeof(char)),
				4 * sizeof(char));
		tmpItem.it_tf = atoi(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (27 * sizeof(char)),
				6 * sizeof(char));
		tmpItem.weight = atoi(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (33 * sizeof(char)),
				5 * sizeof(char));
		tmpItem.efficiency = atoi(work);
		bzero(work, 80 * sizeof(char));
		if (line[40] == 'Y')
		{
			tmpItem.flags |= BF_INSTALL;
		}
		else
		{
			tmpItem.flags &= ~BF_INSTALL;
		}

		/* Since only the owner can use the ships command */
		tmpItem.owner = player_num;
		tmpItem.flags |= BF_OWNED;

		/* Write the item */
		writeItem(&tmpItem, itemNum);
	}
}

void handlePlanetBig(const char *line)
{
	FeItem_t tmpItem;
	ULONG itemNum;
	int tmp;
	struct timeval tp;
	char work[80];

	/* Make sure this gets saved */
	DirtyData=TRUE;
	ui_browser_append(fd_ImpFeMain->MainBrowser, line);
	/*  Item # | Plan # | TF | T | Wgt | Eff | Ins */
	if ((line[8] != '|') || (line[17] != '|') || (line[22] != '|') ||
		(line[26] != '|'))
	{
	    fprintf(stderr, "*** Invalid planet big item format\n");
	}
	else
	{
		strncpy(work, line, 8 * sizeof(char));
		itemNum=(unsigned long) atoi(work);
		/* This will create an initialized item struct, if needed */
		(void) readItem(&tmpItem, itemNum);

		(void)gettimeofday(&tp, NULL);
		tmpItem.last_seen=tp.tv_sec;
		tmpItem.type=line[24];

		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (9 * sizeof(char)),
				9 * sizeof(char));
		tmpItem.where = atoi(work);
		tmpItem.flags &= ~BF_ONPLAN;
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (18 * sizeof(char)),
				4 * sizeof(char));
		tmpItem.it_tf = atoi(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (27 * sizeof(char)),
				6 * sizeof(char));
		tmpItem.weight = atoi(work);
		bzero(work, 80 * sizeof(char));
		strncpy(work, (char *)line + (33 * sizeof(char)),
				5 * sizeof(char));
		tmpItem.efficiency = atoi(work);
		tmpItem.flags |= BF_ONPLAN;
		/* Since only the owner can use the census command */
		tmpItem.owner = player_num;
		tmpItem.flags |= BF_OWNED;

		/* Write the item */
		writeItem(&tmpItem, itemNum);
	}
}

#define ND_SHIP   1
#define ND_PLANET 2
#define ND_ITEM   3
/*
 * needDump - adds an entry to the various "dirty" lists
 */
void needDump(int what, unsigned long which)
{
	char cBuf[95];
	switch(what)
	{
		case ND_SHIP:
			/* Need to add code for this! XXX */
			break;
		case ND_PLANET:
			sprintf(cBuf, "dump %lu\n", which);
			addCmd(cBuf, QUE_MED);
			break;
		case ND_ITEM:
			break;
		default:
			fprintf(stderr, "Unknown needDump type '%d'\n", what);
			break;
	}
}


void handleWorldSize(const char *line)
{
	int tmp;
	unsigned int sizex, sizey;
	char work[80];

	ui_browser_append(fd_ImpFeMain->MainBrowser, line);

	if ((tmp=sscanf(line, "World size: %u rows by %u columns", &sizey,
					&sizex)) != 2)
	{
		fprintf(stderr, "*** invalid info world size line '%d'\n", tmp);
	}
	else
	{
		if (game_sizey != sizey)
		{
			sprintf(work, ">>> Old world size was %u rows, setting to new size\n", game_sizey);
			fe_puts(work);
			game_sizey = sizey;
			DirtyData=TRUE;
		}
		if (game_sizex != sizex)
		{
			sprintf(work, ">>> Old world size was %u columns, setting to new size\n", game_sizex);
			fe_puts(work);
			game_sizex = sizex;
			DirtyData=TRUE;
		}
	}
}

/*
 * handlePlDump - handles a planet dump line
 */
void handlePlDump(const char *line)
{
	ULONG plNum, row, col, val, tmp;
	FePlanet_t tmpPlan;
	char workBuf[80], tLine[231];
	const char *pos;
	struct timeval tp;

	/* Verify length is correct */
	if (strlen(line) != 229)
	{
		/* Nope! */
		fprintf(stderr, "Invalid dump line length: %zu\n", strlen(line));
		return;
	}
	/* Make sure this gets saved */
	DirtyData=TRUE;
	/* get planet # */
	pos=&line[0];
	memcpy(workBuf, pos, 8 * sizeof(char));
	workBuf[8]='\0';
	(void) sscanf(workBuf, "%lx", &plNum);
	pos += 8 * sizeof(char);
	memcpy(workBuf, pos, 4 * sizeof(char));
	workBuf[4]='\0';
	(void) sscanf(workBuf, "%lx", &row);
	pos += 4 * sizeof(char);
	memcpy(workBuf, pos, 4 * sizeof(char));
	workBuf[4]='\0';
	(void) sscanf(workBuf, "%lx", &col);
	pos += 4 * sizeof(char);
	(void) readPlanet(&tmpPlan, plNum);
	(void)gettimeofday(&tp, NULL);
	tmpPlan.last_seen=tp.tv_sec;
	if ((tmpPlan.pl_row != row) || (tmpPlan.pl_col != col))
	{
		addSectPlan(row, col, plNum);
		tmpPlan.pl_row = (USHORT) row;
		tmpPlan.pl_col = (USHORT) col;
	}
	memcpy(workBuf, pos, 8 * sizeof(char));
	workBuf[8]='\0';
	(void) sscanf(workBuf, "%lx", &tmpPlan.TechLv);
	pos += 8 * sizeof(char);
	memcpy(workBuf, pos, 8 * sizeof(char));
	workBuf[8]='\0';
	(void) sscanf(workBuf, "%lx", &tmpPlan.ResLv);
	pos += 8 * sizeof(char);
	/* ship over update time */
	pos += 8 * sizeof(char);
	if (*pos == '0')
	{
		tmpPlan.plgStg = 0;
	}
	else
	{
		tmpPlan.plgStg = 1;
	}
	pos += 1 * sizeof(char);
	if (*pos == '0')
	{
		tmpPlan.xfer = PT_PEACEFULL;
	}
	else
	{
		tmpPlan.xfer = PT_HOSTILE;
	}
	pos += 1 * sizeof(char);
	/* These come from the Imperium side */
	switch (*pos)
	{
		case '0':
			tmpPlan.class = '*';
			break;
		case '1':
			tmpPlan.class = 'H';
			break;
		case '2':
			tmpPlan.class = 'A';
			break;
		case '3':
			tmpPlan.class = 'B';
			break;
		case '4':
			tmpPlan.class = 'C';
			break;
		case '5':
			tmpPlan.class = 'D';
			break;
		case '6':
			tmpPlan.class = 'M';
			break;
		case '7':
			tmpPlan.class = 'N';
			break;
		case '8':
			tmpPlan.class = 'O';
			break;
		case '9':
			tmpPlan.class = 'Q';
			break;
		default:
			tmpPlan.class = '?';
			break;
	}
	if ((tmpPlan.class == 'H') && (tmpPlan.xfer == PT_PEACEFULL))
	{
		tmpPlan.xfer = PT_HOME;
	}
	pos += 1 * sizeof(char);
	memcpy(workBuf, pos, 4 * sizeof(char));
	workBuf[4]='\0';
	(void) sscanf(workBuf, "%lx", &val);
	tmpPlan.mobil = (UBYTE) val;
	pos += 4 * sizeof(char);
	memcpy(workBuf, pos, 2 * sizeof(char));
	workBuf[2]='\0';
	(void) sscanf(workBuf, "%lx", &val);
	tmpPlan.eff = (UBYTE) val;
	pos += 2 * sizeof(char);
	memcpy(workBuf, pos, 2 * sizeof(char));
	workBuf[2]='\0';
	(void) sscanf(workBuf, "%lx", &val);
	tmpPlan.minr = (UBYTE) val;
	pos += 2 * sizeof(char);
	memcpy(workBuf, pos, 2 * sizeof(char));
	workBuf[2]='\0';
	(void) sscanf(workBuf, "%lx", &val);
	tmpPlan.gold = (UBYTE) val;
	pos += 2 * sizeof(char);
	memcpy(workBuf, pos, 2 * sizeof(char));
	workBuf[2]='\0';
	(void) sscanf(workBuf, "%lx", &val);
	tmpPlan.polut = (UBYTE) val;
	pos += 2 * sizeof(char);
	memcpy(workBuf, pos, 2 * sizeof(char));
	workBuf[2]='\0';
	(void) sscanf(workBuf, "%lx", &val);
	tmpPlan.gas = (UBYTE) val;
	pos += 2 * sizeof(char);
	memcpy(workBuf, pos, 2 * sizeof(char));
	workBuf[2]='\0';
	(void) sscanf(workBuf, "%lx", &val);
	tmpPlan.water = (UBYTE) val;
	pos += 2 * sizeof(char);
	memcpy(workBuf, pos, 1 * sizeof(char));
	workBuf[1]='\0';
	(void) sscanf(workBuf, "%lx", &val);
	tmpPlan.size = (UBYTE) val;
	pos += 1 * sizeof(char);
	memcpy(workBuf, pos, 4 * sizeof(char));
	workBuf[4]='\0';
	(void) sscanf(workBuf, "%lx", &val);
	tmpPlan.pl_btu = (USHORT) val;
	pos += 4 * sizeof(char);
	/* Skip over checkpointed field */
	pos += sizeof(char);
	/* Make sure this is the item block type */
	if (*pos != '@')
	{
		printf("Error parsing planet dump line - invalid block type '%c'\n",
			   *pos);
		return;
	}
	pos += sizeof(char);
	for (tmp=0; tmp < IT_LAST + 1; tmp++)
	{
		memcpy(workBuf, pos, 4 * sizeof(char));
		workBuf[4]='\0';
		(void) sscanf(workBuf, "%lx", &val);
		tmpPlan.quant[tmp] = (USHORT) val;
		pos += 4 * sizeof(char);
	}
	for (tmp=0; tmp < PPROD_LAST + 1; tmp++)
	{
		memcpy(workBuf, pos, 4 * sizeof(char));
		workBuf[4]='\0';
		(void) sscanf(workBuf, "%lx", &val);
		tmpPlan.prod[tmp] = (USHORT) val;
		pos += 4 * sizeof(char);
		memcpy(workBuf, pos, 2 * sizeof(char));
		workBuf[2]='\0';
		(void) sscanf(workBuf, "%lx", &val);
		tmpPlan.workPer[tmp] = (UBYTE) val;
		pos += 2 * sizeof(char);
	}
	/* Since only the owner can use the dump command */
	tmpPlan.owner = player_num;
	tmpPlan.flags |= BF_OWNED;
	tmpPlan.flags &= ~BF_DIRTY;

	(void) writePlanet(&tmpPlan, plNum);
}

void handlePlScan(const char *line)
{
	ULONG plNum, row, col, val, tmp;
	FePlanet_t tmpPlan;
	char workBuf[80], tLine[231];
	const char *pos;
	struct timeval tp;

	/* Verify length is correct */
	if (strlen(line) != 65)
	{
		/* Nope! */
		fprintf(stderr, "Invalid planet scan line length: %zu\n", strlen(line));
		return;
	}
	/* Format of scan line: Any unknown field will be zero
	   XXXXXXXX - Planet number
	   XXXX     - Row
	   XXXX     - Col
	   X        - Planet Class
	   X        - Race (if home plan)
	   XX       - Owner (0 if none)
	   CCCCCCCCCCCC - Planet Name (left just)
	   XX       - Size
	   XX       - Polution
	   XX       - Minr
	   XX       - Gold
	   XX       - Gas
	   XX       - Water
	   XXXX     - Ore
	   XXXX     - Bars
	   XX       - Effic
	   XX       - TF
	   XXXX     - Civ
	   XXXX     - Mil
	   */
	   
	/* Make sure this gets saved */
	DirtyData=TRUE;
	/* get planet # */
	pos=&line[0];
	memcpy(workBuf, pos, 8 * sizeof(char));
	workBuf[8]='\0';
	(void) sscanf(workBuf, "%lx", &plNum);
	pos += 8 * sizeof(char);
	memcpy(workBuf, pos, 4 * sizeof(char));
	workBuf[4]='\0';
	(void) sscanf(workBuf, "%lx", &row);
	pos += 4 * sizeof(char);
	memcpy(workBuf, pos, 4 * sizeof(char));
	workBuf[4]='\0';
	(void) sscanf(workBuf, "%lx", &col);
	pos += 4 * sizeof(char);
	(void) readPlanet(&tmpPlan, plNum);
	(void)gettimeofday(&tp, NULL);
	tmpPlan.last_seen=tp.tv_sec;
	if ((tmpPlan.pl_row != row) || (tmpPlan.pl_col != col))
	{
		addSectPlan(row, col, plNum);
		tmpPlan.pl_row = (USHORT) row;
		tmpPlan.pl_col = (USHORT) col;
	}
	/* These come from the Imperium side */
	switch (*pos)
	{
		case '0':
			tmpPlan.class = '*';
			break;
		case '1':
			tmpPlan.class = 'H';
			break;
		case '2':
			tmpPlan.class = 'A';
			break;
		case '3':
			tmpPlan.class = 'B';
			break;
		case '4':
			tmpPlan.class = 'C';
			break;
		case '5':
			tmpPlan.class = 'D';
			break;
		case '6':
			tmpPlan.class = 'M';
			break;
		case '7':
			tmpPlan.class = 'N';
			break;
		case '8':
			tmpPlan.class = 'O';
			break;
		case '9':
			tmpPlan.class = 'Q';
			break;
		default:
			tmpPlan.class = '?';
			break;
	}
	pos += 1 * sizeof(char);
	/* The race that owns it, if it's a home planet */
	if (tmpPlan.class == 'H')
	{
		memcpy(workBuf, pos, 1 * sizeof(char));
		workBuf[1]='\0';
		(void) sscanf(workBuf, "%lx", &tmp);
		setRaceHome((int) tmp, plNum);
	}
	pos += 1 * sizeof(char);
	memcpy(workBuf, pos, 2 * sizeof(char));
	workBuf[2]='\0';
	(void) sscanf(workBuf, "%lx", &tmp);
	if (tmp != 0)
	{
		/* If player owns this planet, just force a dump */
		if (tmp == player_num)
		{
			sprintf(workBuf, "dump %lu\n", plNum);
			(void) addCmd(workBuf, QUE_IMMEDIATE);
			return;
		}
	}
	/* The server sends 0xff for "nobody"; we use 0 for that */
	tmpPlan.owner = (tmp < PLAYER_MAX) ? (UBYTE) tmp : 0;
	pos += 2 * sizeof(char);
	/* Skip over planet name */
	pos += 12 * sizeof(char);
	memcpy(workBuf, pos, 2 * sizeof(char));
	workBuf[2]='\0';
	(void) sscanf(workBuf, "%lx", &tmp);
	tmpPlan.size = tmp;
	pos += 2 * sizeof(char);
	memcpy(workBuf, pos, 2 * sizeof(char));
	workBuf[2]='\0';
	(void) sscanf(workBuf, "%lx", &tmp);
	tmpPlan.polut = tmp;
	pos += 2 * sizeof(char);
	memcpy(workBuf, pos, 2 * sizeof(char));
	workBuf[2]='\0';
	(void) sscanf(workBuf, "%lx", &tmp);
	tmpPlan.minr = tmp;
	pos += 2 * sizeof(char);
	memcpy(workBuf, pos, 2 * sizeof(char));
	workBuf[2]='\0';
	(void) sscanf(workBuf, "%lx", &tmp);
	tmpPlan.gold = tmp;
	pos += 2 * sizeof(char);
	memcpy(workBuf, pos, 2 * sizeof(char));
	workBuf[2]='\0';
	(void) sscanf(workBuf, "%lx", &tmp);
	tmpPlan.gas = tmp;
	pos += 2 * sizeof(char);
	memcpy(workBuf, pos, 2 * sizeof(char));
	workBuf[2]='\0';
	(void) sscanf(workBuf, "%lx", &tmp);
	tmpPlan.water = tmp;
	pos += 2 * sizeof(char);
	memcpy(workBuf, pos, 4 * sizeof(char));
	workBuf[4]='\0';
	(void) sscanf(workBuf, "%lx", &tmp);
	tmpPlan.quant[it_ore] = tmp;
	pos += 4 * sizeof(char);
	memcpy(workBuf, pos, 4 * sizeof(char));
	workBuf[4]='\0';
	(void) sscanf(workBuf, "%lx", &tmp);
	tmpPlan.quant[it_bars] = tmp;
	pos += 4 * sizeof(char);
	memcpy(workBuf, pos, 2 * sizeof(char));
	workBuf[2]='\0';
	(void) sscanf(workBuf, "%lx", &tmp);
	tmpPlan.eff = tmp;
	pos += 2 * sizeof(char);
	memcpy(workBuf, pos, 2 * sizeof(char));
	workBuf[2]='\0';
	(void) sscanf(workBuf, "%lx", &tmp);
	tmpPlan.TF = tmp;
	pos += 2 * sizeof(char);
	memcpy(workBuf, pos, 4 * sizeof(char));
	workBuf[4]='\0';
	(void) sscanf(workBuf, "%lx", &tmp);
	tmpPlan.quant[it_civilians] = tmp;
	pos += 4 * sizeof(char);
	memcpy(workBuf, pos, 4 * sizeof(char));
	workBuf[4]='\0';
	(void) sscanf(workBuf, "%lx", &tmp);
	tmpPlan.quant[it_military] = tmp;
	pos += 4 * sizeof(char);

	(void) writePlanet(&tmpPlan, plNum);
}

void handleShScan(const char *line)
{
	ULONG shNum, row, col, val, tmp;
	FeShip_t tmpShip;
	char workBuf[80], tLine[231];
	const char *pos;
	struct timeval tp;

	/* Verify length is correct */
	if (strlen(line) != 65)
	{
		/* Nope! */
		fprintf(stderr, "Invalid ship scan line length: %zu\n", strlen(line));
		return;
	}
	/* Format of scan line: Any unknown field will be zero
	   XXXXXXXX - Ship number
	   XXXX     - Row
	   XXXX     - Col
	   X        - Ship type
	   XX       - Owner (0 if none)
	   CCCCCCCCCCCC - Ship Name (left just)
	   XXX      - Number of engines
	   XXX      - Number of weapons
	   XXX      - Engine TF
	   XXX      - Weapon TF
	   XX       - Efficiency
	   XXXX     - Cargo
	   XXXX     - Civ
	   XXXX     - Mil
	   XXXX     - Shields
	   XXXX     - Energy
	   XXXX     - Armour
	   */
	   
	/* Make sure this gets saved */
	DirtyData=TRUE;
	/* get ship # */
	pos=&line[0];
	memcpy(workBuf, pos, 8 * sizeof(char));
	workBuf[8]='\0';
	(void) sscanf(workBuf, "%lx", &shNum);
	pos += 8 * sizeof(char);
	memcpy(workBuf, pos, 4 * sizeof(char));
	workBuf[4]='\0';
	(void) sscanf(workBuf, "%lx", &row);
	pos += 4 * sizeof(char);
	memcpy(workBuf, pos, 4 * sizeof(char));
	workBuf[4]='\0';
	(void) sscanf(workBuf, "%lx", &col);
	pos += 4 * sizeof(char);
	(void) readShip(&tmpShip, shNum);
	(void)gettimeofday(&tp, NULL);
	if (tmpShip.last_seen == 0)
	{
		addSectShip(row, col, shNum);
	}
	tmpShip.last_seen=tp.tv_sec;
	tmpShip.sh_row = (USHORT) row;
	tmpShip.sh_col = (USHORT) col;
	/* These come from the Imperium side */
	switch (*pos)
	{
		case '0':
			tmpShip.ShipType = 'a';
			break;
		case '1':
			tmpShip.ShipType = 'b';
			break;
		case '2':
			tmpShip.ShipType = 'c';
			break;
		case '3':
			tmpShip.ShipType = 'd';
			break;
		case '4':
			tmpShip.ShipType = 'e';
			break;
		case '5':
			tmpShip.ShipType = 'm';
			break;
		default:
			tmpShip.ShipType = '?';
			break;
	}
	pos += 1 * sizeof(char);
	memcpy(workBuf, pos, 2 * sizeof(char));
	workBuf[2]='\0';
	(void) sscanf(workBuf, "%lx", &tmp);
	if (tmp != 0)
	{
		/* If player owns this planet, just force a dump */
		if (tmp == player_num)
		{
			sprintf(workBuf, "dump %lup\n", shNum);
			(void) addCmd(workBuf, QUE_IMMEDIATE);
			return;
		}
	}
	/* The server sends 0xff for "nobody"; we use 0 for that */
	tmpShip.owner = (tmp < PLAYER_MAX) ? (UBYTE) tmp : 0;
	pos += 2 * sizeof(char);
	/* Skip over ship name */
	pos += 12 * sizeof(char);
	memcpy(workBuf, pos, 3 * sizeof(char));
	workBuf[3]='\0';
	(void) sscanf(workBuf, "%lx", &tmp);
	tmpShip.num_eng = tmp;
	pos += 3 * sizeof(char);
	memcpy(workBuf, pos, 3 * sizeof(char));
	workBuf[3]='\0';
	(void) sscanf(workBuf, "%lx", &tmp);
	tmpShip.num_wpn = tmp;
	pos += 3 * sizeof(char);
	memcpy(workBuf, pos, 3 * sizeof(char));
	workBuf[3]='\0';
	(void) sscanf(workBuf, "%lx", &tmp);
	tmpShip.engTF = tmp;
	pos += 3 * sizeof(char);
	/* Skip over weapon TF */
	pos += 3 * sizeof(char);
	memcpy(workBuf, pos, 2 * sizeof(char));
	workBuf[2]='\0';
	(void) sscanf(workBuf, "%lx", &tmp);
	tmpShip.efficiency = tmp;
	pos += 2 * sizeof(char);
	memcpy(workBuf, pos, 4 * sizeof(char));
	workBuf[4]='\0';
	(void) sscanf(workBuf, "%lx", &tmp);
	tmpShip.cargo = tmp;
	pos += 4 * sizeof(char);
	memcpy(workBuf, pos, 4 * sizeof(char));
	workBuf[4]='\0';
	(void) sscanf(workBuf, "%lx", &tmp);
	tmpShip.num_civ = tmp;
	pos += 4 * sizeof(char);
	memcpy(workBuf, pos, 4 * sizeof(char));
	workBuf[4]='\0';
	(void) sscanf(workBuf, "%lx", &tmp);
	tmpShip.num_mil = tmp;
	pos += 4 * sizeof(char);
	memcpy(workBuf, pos, 4 * sizeof(char));
	workBuf[4]='\0';
	(void) sscanf(workBuf, "%lx", &tmp);
	tmpShip.shields = tmp;
	pos += 4 * sizeof(char);
	memcpy(workBuf, pos, 4 * sizeof(char));
	workBuf[4]='\0';
	(void) sscanf(workBuf, "%lx", &tmp);
	tmpShip.energy = tmp;
	pos += 4 * sizeof(char);
	memcpy(workBuf, pos, 4 * sizeof(char));
	workBuf[4]='\0';
	(void) sscanf(workBuf, "%lx", &tmp);
	tmpShip.armourLeft = tmp;

	(void) writeShip(&tmpShip, shNum);
}

void handleShipScDet(const char *line)
{
	ULONG shNum, row, col, val, tmp, owner;
	FeShip_t tmpShip;
	char workBuf[80], tLine[231], class;
	const char *pos;
	struct timeval tp;

	/* Verify length is correct */
	if (strlen(line) != 22)
	{
		/* Nope! */
		fprintf(stderr, "Invalid ship detected via scan line length: %zu\n", strlen(line));
		return;
	}
	/* Format of scan line: Any unknown field will be zero
	   XXXX     - Owner
	   C        - Ship class
	   XXXXXXXX - Ship number
	   XXXX     - Row
	   XXXX     - Col
	   */
	   
	/* Make sure this gets saved */
	DirtyData=TRUE;
	/* get planet # */
	pos=&line[0];
	memcpy(workBuf, pos, 4 * sizeof(char));
	workBuf[4]='\0';
	(void) sscanf(workBuf, "%lx", &owner);
	pos += 4 * sizeof(char);
	switch(*pos)
	{
		case 'a':
			class = st_a;
			break;
		case 'b':
			class = st_b;
			break;
		case 'c':
			class = st_c;
			break;
		case 'd':
			class = st_d;
			break;
		case 'e':
			class = st_e;
			break;
		case 'm':
			class = st_m;
			break;
		default:
			fprintf(stderr, "*** unknown ship class in handleShipScDet %u\n",
					(int) *pos);
			class = st_unknown;
			break;
	}
	pos += 1 * sizeof(char);
	memcpy(workBuf, pos, 8 * sizeof(char));
	workBuf[8]='\0';
	(void) sscanf(workBuf, "%lx", &shNum);
	pos += 8 * sizeof(char);
	memcpy(workBuf, pos, 4 * sizeof(char));
	workBuf[4]='\0';
	(void) sscanf(workBuf, "%lx", &row);
	pos += 4 * sizeof(char);
	memcpy(workBuf, pos, 4 * sizeof(char));
	workBuf[4]='\0';
	(void) sscanf(workBuf, "%lx", &col);
	
	(void) readShip(&tmpShip, shNum);
	(void)gettimeofday(&tp, NULL);
	if ((tmpShip.sh_row != row) || (tmpShip.sh_col != col))
	{
		if (tmpShip.last_seen != 0)
		{
			delSectShip(tmpShip.sh_row, tmpShip.sh_col, shNum);
		}
		addSectShip(row, col, shNum);
		tmpShip.sh_row = (USHORT) row;
		tmpShip.sh_col = (USHORT) col;
	}
	/* The server sends 0xff for "nobody"; we use 0 for that */
	tmpShip.owner = (owner < PLAYER_MAX) ? (UBYTE) owner : 0;
	tmpShip.ShipType = class;
	tmpShip.last_seen=tp.tv_sec;

	(void) writeShip(&tmpShip, shNum);
}

/*
 * fe_parse - parse the given input line's FE tags
 */
void fe_parse(const char *buf)
{
	ULONG tmp;
	FeShip_t tmpShip;
	FePlanet_t tmpPlanet;
	char cmdBuf[120];

	/* If not an intro line, just return */
	if (buf[0] != '!')
	{
		/* Display it */
	    ui_browser_append(fd_ImpFeMain->MainBrowser, buf);
	    return;
	}
	/* Do it this way to speed things up */
	switch (buf[1])
	{
		case '<':	/* Comment */
			handleComment(&buf[2]);
			break;
		case '(':	/* Prompt setting */
			/* Chop of trailing NL */
			strcpy(ParseBuf, &buf[2]);
			ParseBuf[strlen(ParseBuf) - 1] = '\0';
			handlePrompt(ParseBuf);
			if (ParseBuf[0] != '!')
			{
			    ui_browser_append(fd_ImpFeMain->MainBrowser, ParseBuf);
			    ui_browser_append(fd_ImpFeMain->MainBrowser, ": ");
			}
			break;
		case '[':	/* Get line from user */
		case ']':	/* Cancel get line */
			/* Currently unused */
			/*ui_browser_append(fd_ImpFeMain->MainBrowser, buf);*/
			break;
		case '@':	/* Planet reports */
			switch (buf[2])
			{
				case 'D': /* Planet dump */
					handlePlDump(&buf[3]);
					break;
				case 'U': /* Dirty Planet */
					tmp=(unsigned long) atoi(&buf[3]);
					(void) readPlanet(&tmpPlanet, tmp);
					/* Set the dirty flag */
					tmpPlanet.flags |= BF_DIRTY;
					writePlanet(&tmpPlanet, tmp);
					/* Add to the needDump list */
					needDump(ND_PLANET, tmp);
					break;
				case '1': /* Geo report */
					handlePlGeo(&buf[3]);
					break;
				case '2': /* Pop report */
					handlePlPop(&buf[3]);
					break;
				case '3': /* Item report */
					ui_browser_append(fd_ImpFeMain->MainBrowser, buf);
					break;
				case '4': /* Planet big item rpt */
					handlePlanetBig(&buf[3]);
					break;
				case '5': /* Planet prod rpt */
					switch (buf[3])
					{
						case '1':
							handlePlProd1(&buf[4]);
							break;
						case '2':
							handlePlProd2(&buf[4]);
							break;
						default:
							fprintf(stderr, "*** unknown FE planet prod rpt type: '%c'\n", buf[3]);
							break;
					}
					break;
				case '6': /* Planet scan line */
					handlePlScan(&buf[3]);
					break;
				case '7': /* Planet ret scan line */
					ui_browser_append(fd_ImpFeMain->MainBrowser, buf);
					break;
				default:
					fprintf(stderr, "*** unknown FE planet code type: '%c'\n", buf[2]);
					break;
			}
			break;
		case '$':	/* Ship reports */
			switch (buf[2])
			{
				case 'D': /* Ship dump line */
					ui_browser_append(fd_ImpFeMain->MainBrowser, buf);
					break;
				case 'U': /* Dirty ship */
					tmp=(unsigned long) atoi(&buf[3]);
					(void) readShip(&tmpShip, tmp);
					/* Set the dirty flag */
					tmpShip.flags |= BF_DIRTY;
					writeShip(&tmpShip, tmp);
					/* Add to the needDump list */
					needDump(ND_SHIP, tmp);
					break;
				case '1': /* Ship status rept */
					handleShipStatus(&buf[3]);
					break;
				case '2': /* Ship cargo rept */
					handleShipCargo(&buf[3]);
					break;
				case '3': /* Ship big item rept */
					handleShipBig(&buf[3]);
					break;
				case '4': /* Ship config rept */
					ui_browser_append(fd_ImpFeMain->MainBrowser, buf);
					break;
				case '5': /* Ship scan line */
					ui_browser_append(fd_ImpFeMain->MainBrowser, buf);
					break;
				case '6': /* Ship detected via scan */
					handleShipScDet(&buf[3]);
					break;
				case '7': /* Ship retscan line */
					ui_browser_append(fd_ImpFeMain->MainBrowser, buf);
					break;
				default:
					fprintf(stderr, "*** unknown FE ship code type: '%c'\n", buf[2]);
					break;
			}
			break;
		case ')':	/* Scans */
			switch (buf[2])
			{
				case 'S': /* SRS */
					handleSRS(&buf[3]);
					break;
				case 'R': /* LRS */
					ui_browser_append(fd_ImpFeMain->MainBrowser, buf);
					break;
				case 'V': /* Visual */
					ui_browser_append(fd_ImpFeMain->MainBrowser, buf);
					break;
				default:
					fprintf(stderr, "*** unknown FE scan code type: '%c'\n", buf[2]);
					break;
			}
			break;
		case '*':	/* Misc */
			switch (buf[2])
			{
				case 'C': /* Change request */
					ui_browser_append(fd_ImpFeMain->MainBrowser, buf);
					break;
				case 'L': /* Player list */
					handlePlayerList(&buf[3]);
					break;
				case '\x50': /* Power report */
					handlePowerRpt(&buf[3]);
					break;
				case 'R': /* Race report */
					switch (buf[3])
					{
						case '1':
							handleRace1(&buf[4]);
							break;
						case '2':
							handleRace2(&buf[4]);
							break;
						default:
							fprintf(stderr,
									"*** unknown FE race report sub-type: '%c'\n",
									buf[2]);
							break;
					}
					break;
				case 'W': /* World size line */
					handleWorldSize(&buf[3]);
					break;
				case 'S': /* Player status report */
					switch (buf[3])
					{
						case 'H':
							ui_browser_append(fd_ImpFeMain->MainBrowser, &buf[4]);
							if (buf[15] != ':')
							{
								fprintf(stderr, "*** unknown player status home planet line\n");
							}
							else
							{
								/* Our race's home planet belongs to the race */
								setRaceHome(OurRace, (ULONG) atol(&buf[16]));
								if (atoi(&buf[16]) != home_planet)
								{
									home_planet=atoi(&buf[16]);
									sprintf(cmdBuf, "census geo %lu\n",
											home_planet);
									addCmd(cmdBuf, QUE_HIGH);
									sprintf(cmdBuf, "census pop %lu\n",
											home_planet);
									addCmd(cmdBuf, QUE_MED);
									DirtyData=TRUE;
								}
							}
							break;
						case 'S':
							ui_browser_append(fd_ImpFeMain->MainBrowser, &buf[4]);
							break;
						default:
							fprintf(stderr, "*** Unknown player status type '%c'\n", buf[3]);
							break;
					}
					break;
				case 'A': /* Realm follows */
					ui_browser_append(fd_ImpFeMain->MainBrowser, buf);
					break;
				default:
					fprintf(stderr, "*** unknown FE misc code type: '%c'\n", buf[2]);
					break;
			}
			break;
		case '&':	/* Combat results */
			switch (buf[2])
			{
				case 'L': /* Last attack failed */
					ui_browser_append(fd_ImpFeMain->MainBrowser, buf);
					break;
				case 'W': /* Last attack succeeded */
					ui_browser_append(fd_ImpFeMain->MainBrowser, buf);
					break;
				default:
					fprintf(stderr, "*** unknown FE combat result code type: '%c'\n", buf[2]);
					break;
			}
			break;
		default:	/* Shouldn't get here */
			fprintf(stderr, "*** unknown FE code type: '%c'\n", buf[1]);
			break;
	}
}
