#ifndef LINT
static char rcsid[]="$Id: planet.c,v 1.2 2000/05/24 20:40:39 marisa Exp $";
#endif
/*
 * ImpFE
 *
 * $Id: planet.c,v 1.2 2000/05/24 20:40:39 marisa Exp $
 *
 * This file contains routines for handling planet data manipulation
 *
 * $Log: planet.c,v $
 * Revision 1.2  2000/05/24 20:40:39  marisa
 * Disable some menu options until config file created or opened.
 *
 * Revision 1.1.1.1  2000/05/17 19:22:13  marisa
 * First CVS checkin
 *
 * Revision 1.1.1.1  1997/04/27 16:34:28  marisag
 * patch1: Fix browser bug
 *
 * Revision 1.1  1997/04/25 06:41:04  marisag
 * Initial revision
 *
 *
 */

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
#include "gui.h"

/*
 * planetHomeRace - returns the race whose home planet this is, or -1
 */
int planetHomeRace(ULONG plNum)
{
	int race;

	for (race = 0; race < RACE_MAX; race++)
	{
		if (Race[race].homeKnown && (Race[race].home == plNum))
		{
			return(race);
		}
	}
	return(-1);
}

/*
 * planetOwnerName - who owns a planet, for display. Home planets belong
 *                   to a race rather than a player. brief is for the
 *                   narrow census column.
 */
const char *planetOwnerName(const FePlanet_t *pl, ULONG plNum, BOOL brief)
{
	static char buf[NAME_LEN + 16];
	int race;

	race = planetHomeRace(plNum);
	if (race >= 0)
	{
		if (Race[race].name[0] != '\0')
		{
			g_snprintf(buf, sizeof(buf), brief ? "R:%s" : "%s (race)",
				Race[race].name);
		}
		else
		{
			g_snprintf(buf, sizeof(buf), "Race %d", race);
		}
		return(buf);
	}
	if ((pl->owner != 0) && (pl->owner < PLAYER_MAX) &&
		(Player[pl->owner].name[0] != '\0'))
	{
		return(Player[pl->owner].name);
	}
	return("???");
}

/*
 * buildPlCensus - Clears the existing contents of the planet census
 *                 window and redraws it based on current known
 *                 planets.
 */
void buildPlCensus(void)
{
	ULONG plNum, crew;
	USHORT eff;
	FePlanet_t tmpPlanet;
	char workBuf[160], nameBuf[10], plClass, location[24], crewBuf[24],
		tBuff[28], plName[10];

	/* Clear the existing contents, if any */
	ui_browser_clear(fd_PlanetCensusForm->PlBrowse);
	/* Loop for each known planet */
	for (plNum=0; plNum < next_planet; plNum++)
	{
		/* Make sure the ship has been seen */
		if (readPlanet(&tmpPlanet, plNum))
		{
			if (tmpPlanet.last_seen != 0)
			{
			strncpy(nameBuf, planetOwnerName(&tmpPlanet, plNum, TRUE), 8);
			nameBuf[8] = '\0';
			strncpy(plName, tmpPlanet.name, 9);
			plName[9]='\0';
			if (tmpPlanet.class != 0)
			{
				plClass = tmpPlanet.class;
			}
			else
			{
				plClass = '?';
			}
			crew = tmpPlanet.quant[it_civilians] +
				tmpPlanet.quant[it_military] +
				tmpPlanet.quant[it_scientists] +
				tmpPlanet.quant[it_officers];
			if (crew < 1000)
			{
				sprintf(crewBuf, "%3lu", crew);
			}
			else if (crew < 10000)
			{
				sprintf(crewBuf, "%2luX", crew / 100);
			}
			else
			{
				sprintf(crewBuf, "%2luK", crew / 1000);
			}
			sprintf(location, "%4u, %4u", tmpPlanet.pl_row, tmpPlanet.pl_col);
			strcpy(tBuff, ctime(&tmpPlanet.last_seen));
			tBuff[19]='\0';
			sprintf(workBuf, "%8lu  %8s   %8s    %c   %s  %s  %3u  %3u %3u %3u %3u %3u  %s", plNum,
					plName, nameBuf, plClass, location, crewBuf, tmpPlanet.eff,
					tmpPlanet.gas, tmpPlanet.water, tmpPlanet.minr, tmpPlanet.gold, tmpPlanet.polut, &tBuff[4]);
			ui_browser_add_line(fd_PlanetCensusForm->PlBrowse, workBuf);
		}
		}
	}
}

/*
 * raceRelation - describes our relation to a race (prefix is "Def/"
 *                when it applies because the player relation is default)
 */
static void raceRelation(char *buf, int race, const char *prefix)
{
	const char *rel;

	if ((race < 0) || (race >= RACE_MAX))
	{
		strcpy(buf, "???");
		return;
	}
	switch (Race[race].relation)
	{
		case relation_allied:
			rel = "Allied";
			break;
		case relation_war:
			rel = "*WAR*";
			break;
		case relation_default:
			rel = "Neutral";
			break;
		default:
			rel = "???";
			break;
	}
	sprintf(buf, "%s%s", prefix, rel);
}

/*
 * buildPlDetDisp - Clears the existing contents of the planet detail
 *                 window and redraws it based on current known
 *                 planets.
 */
void buildPlDetDisp(void)
{
	ULONG itNum;
	FePlanet_t tmpPlanet;
	FeItem_t tmpItem;
	char workBuf[160], nameBuf[10], itType, location[15], tBuff[28];
	int homeRace;

	/* Clear the existing contents, if any */
	ui_browser_clear(fd_PlanetDetailForm->PlProdBrowse);
	ui_browser_clear(fd_PlanetDetailForm->PlDetItem);
	/* read the planet in */
	(void) readPlanet(&tmpPlanet, LastPlanet);
	/* Loop for each production type */
	for (itNum=0; itNum < (PPROD_LAST + 1); itNum++)
	{
		sprintf(workBuf, "%11s   %5u   %3u", PPROD_NAME[itNum],
				tmpPlanet.prod[itNum], tmpPlanet.workPer[itNum]);
		ui_browser_add_line(fd_PlanetDetailForm->PlProdBrowse, workBuf);
	}
	/* Loop for each known big item */
	for (itNum=0; itNum < next_item; itNum++)
	{
		/* Make sure the item has been seen and is on the planet */
		if (!readItem(&tmpItem, itNum))
		{
			continue;
		}
		if ((tmpItem.last_seen != 0) && (tmpItem.where == LastPlanet) &&
			(tmpItem.flags & BF_ONPLAN))
		{
			if ((tmpItem.owner != 0) && (tmpItem.owner < PLAYER_MAX))
			{
				strncpy(nameBuf, Player[tmpItem.owner].name, 9);
				nameBuf[9] = '\0';
			}
			else
			{
				strcpy(nameBuf, "???");
			}
			if (tmpItem.type != 0)
			{
				itType=tmpItem.type;
			}
			else
			{
				itType='?';
			}
			sprintf(workBuf, "%8lu  %c  %3u  %3u  %5u", itNum,
					itType, tmpItem.it_tf, tmpItem.efficiency,
					tmpItem.weight);
			ui_browser_add_line(fd_PlanetDetailForm->PlDetItem, workBuf);
		}
	}
	sprintf(workBuf, "%u", tmpPlanet.size);
	ui_set_text(fd_PlanetDetailForm->Size, workBuf);
	sprintf(workBuf, "%u", tmpPlanet.eff);
	ui_set_text(fd_PlanetDetailForm->Effic, workBuf);
	sprintf(workBuf, "%u", tmpPlanet.polut);
	ui_set_text(fd_PlanetDetailForm->Polut, workBuf);
	sprintf(workBuf, "%u", tmpPlanet.minr);
	ui_set_text(fd_PlanetDetailForm->Minerals, workBuf);
	sprintf(workBuf, "%u", tmpPlanet.gold);
	ui_set_text(fd_PlanetDetailForm->Gold, workBuf);
	sprintf(workBuf, "%u", tmpPlanet.water);
	ui_set_text(fd_PlanetDetailForm->Water, workBuf);
	sprintf(workBuf, "%u", tmpPlanet.gas);
	ui_set_text(fd_PlanetDetailForm->Gas, workBuf);
	sprintf(workBuf, "%u", tmpPlanet.mobil);
	ui_set_text(fd_PlanetDetailForm->Mobil, workBuf);
	sprintf(workBuf, "%u", tmpPlanet.TF);
	ui_set_text(fd_PlanetDetailForm->TF, workBuf);
	switch(tmpPlanet.xfer)
	{
		case PT_PEACEFULL:
			ui_set_text(fd_PlanetDetailForm->Transfer, "New Owner");
			break;
		case PT_TRADE:
			ui_set_text(fd_PlanetDetailForm->Transfer, "Trade");
			break;
		case PT_HOME:
			ui_set_text(fd_PlanetDetailForm->Transfer, "Home Planet");
			break;
		case PT_HOSTILE:
			ui_set_text(fd_PlanetDetailForm->Transfer, "HOSTILE");
			break;
		default:
			ui_set_text(fd_PlanetDetailForm->Transfer, "IMPOSSIBLE");
			break;
	}
	if ((tmpPlanet.lstOwn != 0) && (tmpPlanet.lstOwn < PLAYER_MAX))
	{
		strcpy(workBuf, Player[tmpPlanet.lstOwn].name);
	}
	else
	{
		strcpy(workBuf, "NONE");
	}
	ui_set_text(fd_PlanetDetailForm->PrevOwn, workBuf);
	if (tmpPlanet.plgStg != 0)
	{
		ui_set_text(fd_PlanetDetailForm->Plague, "YES");
	}
	else
	{
		ui_set_text(fd_PlanetDetailForm->Plague, "NO");
	}
	sprintf(workBuf, "%u", tmpPlanet.RF);
	ui_set_text(fd_PlanetDetailForm->RF, workBuf);
	sprintf(workBuf, "%lu", tmpPlanet.ResLv);
	ui_set_text(fd_PlanetDetailForm->Research, workBuf);
	sprintf(workBuf, "%lu", tmpPlanet.TechLv);
	ui_set_text(fd_PlanetDetailForm->Tech, workBuf);
	sprintf(workBuf, "%u", tmpPlanet.pl_btu);
	ui_set_text(fd_PlanetDetailForm->BTU, workBuf);
	sprintf(workBuf, "%u", tmpPlanet.quant[it_civilians]);
	ui_set_text(fd_PlanetDetailForm->Civ, workBuf);
	sprintf(workBuf, "%u", tmpPlanet.quant[it_military]);
	ui_set_text(fd_PlanetDetailForm->Mil, workBuf);
	sprintf(workBuf, "%u", tmpPlanet.quant[it_scientists]);
	ui_set_text(fd_PlanetDetailForm->Sci, workBuf);
	sprintf(workBuf, "%u", tmpPlanet.quant[it_officers]);
	ui_set_text(fd_PlanetDetailForm->Ofc, workBuf);
	sprintf(workBuf, "%u", tmpPlanet.quant[it_missiles]);
	ui_set_text(fd_PlanetDetailForm->Misl, workBuf);
	sprintf(workBuf, "%u", tmpPlanet.quant[it_planes]);
	ui_set_text(fd_PlanetDetailForm->Plane, workBuf);
	sprintf(workBuf, "%u", tmpPlanet.quant[it_ore]);
	ui_set_text(fd_PlanetDetailForm->Ore, workBuf);
	sprintf(workBuf, "%u", tmpPlanet.quant[it_bars]);
	ui_set_text(fd_PlanetDetailForm->Bars, workBuf);
	sprintf(workBuf, "%u", tmpPlanet.quant[it_airTanks]);
	ui_set_text(fd_PlanetDetailForm->AirTnk, workBuf);
	sprintf(workBuf, "%u", tmpPlanet.quant[it_fuelTanks]);
	ui_set_text(fd_PlanetDetailForm->FuelTnk, workBuf);
	sprintf(workBuf, "%u", tmpPlanet.PF);
	ui_set_text(fd_PlanetDetailForm->PF, workBuf);
	sprintf(workBuf, "%u", tmpPlanet.PopPct);
	ui_set_text(fd_PlanetDetailForm->PopPct, workBuf);
	strcpy(workBuf, ctime(&tmpPlanet.last_seen));
	workBuf[24]='\0'; /* strip off NL */
	ui_set_text(fd_PlanetDetailForm->Seen, workBuf);
	sprintf(workBuf, "%lu", LastPlanet);
	ui_set_text(fd_PlanetDetailForm->PlNum, workBuf);
	sprintf(workBuf, "%u,%u", tmpPlanet.pl_row, tmpPlanet.pl_col);
	ui_set_text(fd_PlanetDetailForm->RowCol, workBuf);
	if (tmpPlanet.class != 0)
	{
		sprintf(workBuf, "%c", tmpPlanet.class);
		ui_set_text(fd_PlanetDetailForm->Class, workBuf);
	}
	else
	{
		ui_set_text(fd_PlanetDetailForm->Class, "?");
	}
	/* Owner - a player, or a race for home planets */
	ui_set_text(fd_PlanetDetailForm->Owner,
		planetOwnerName(&tmpPlanet, LastPlanet, FALSE));
	/* Our relation to the owner */
	homeRace = planetHomeRace(LastPlanet);
	if (homeRace >= 0)
	{
		if (homeRace == ourRace())
		{
			strcpy(workBuf, "Our race");
		}
		else
		{
			raceRelation(workBuf, homeRace, "");
		}
	}
	else if (tmpPlanet.owner == player_num)
	{
		strcpy(workBuf, "N/A");
	}
	else if ((tmpPlanet.owner != 0) && (tmpPlanet.owner < PLAYER_MAX))
	{
		switch (Player[tmpPlanet.owner].relation)
		{
			case relation_default:
				/* Check the race relation */
				raceRelation(workBuf, Player[tmpPlanet.owner].race, "Def/");
				break;
			case relation_allied:
				strcpy(workBuf, "Allied");
				break;
			case relation_war:
				strcpy(workBuf, "*WAR*");
				break;
			default:
				strcpy(workBuf, "???");
				break;
		}
	}
	else
	{
		strcpy(workBuf, "???");
	}
	ui_set_text(fd_PlanetDetailForm->OwnerRel, workBuf);
	ui_entry_set(fd_PlanetDetailForm->Name, tmpPlanet.name);
	ui_entry_set(fd_PlanetDetailForm->Checkpoint, tmpPlanet.chkpoint);

}

