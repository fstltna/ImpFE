#ifndef LINT
static char rcsid[]="$Id: ship.c,v 1.1.1.1 2000/05/17 19:22:13 marisa Exp $";
#endif
/*
 * ImpFE
 *
 * $Id: ship.c,v 1.1.1.1 2000/05/17 19:22:13 marisa Exp $
 *
 * This file contains routines for handling ship data manipulation
 *
 * $Log: ship.c,v $
 * Revision 1.1.1.1  2000/05/17 19:22:13  marisa
 * First CVS checkin
 *
 * Revision 1.1.1.2  1997/04/27 16:34:22  marisag
 * patch1: Fix browser bug
 *
 * Revision 1.1.1.1  1997/04/25 06:40:30  marisag
 * patch1: Code for ship detail forms
 *
 * Revision 1.1  1997/04/24 04:46:28  marisag
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
 * buildShCensus - Clears the existing contents of the ship census
 *                 window and redraws it based on current known
 *                 ships.
 */
void buildShCensus(void)
{
	ULONG shNum, crew;
	USHORT eff;
	FeShip_t tmpShip;
	char workBuf[160], nameBuf[10], shType, location[15], crewBuf[5],
		tBuff[28], shName[10];

	/* Clear the existing contents, if any */
	ui_browser_clear(fd_ShipCensusForm->ShBrowse);
	/* Loop for each known ship */
fprintf(stderr, "*** start of loop\n");
	for (shNum=0; shNum < next_ship; shNum++)
	{
fprintf(stderr, "*** shNum is %u!\n", shNum);
fprintf(stderr, "*** next_ship is %u!\n", next_ship);
		/* Make sure the ship has been seen */
		if (readShip(&tmpShip, shNum))
		{
fprintf(stderr, "*** readShip ok %u!\n", shNum);
			if(tmpShip.last_seen != 0)
			{
fprintf(stderr, "*** tempship ok %u!\n", shNum);
				if (tmpShip.owner != 0)
				{
fprintf(stderr, "*** owner %u!\n", tmpShip.owner);
					strncpy(nameBuf, Player[tmpShip.owner].name, 9);
					nameBuf[9] = '\0';
				}
				else
				{
fprintf(stderr, "*** unknown owner\n");
					strcpy(nameBuf, "???");
				}
				strncpy(shName, tmpShip.name, 9);
				shName[9]='\0';
				if (tmpShip.ShipType != 0)
				{
fprintf(stderr, "*** ship type %u!\n", tmpShip.ShipType);
					shType = tmpShip.ShipType;
				}
				else
				{
fprintf(stderr, "*** ship type unknown!\n");
					shType = '?';
				}
				crew = tmpShip.num_civ + tmpShip.num_mil + tmpShip.num_sci +
					tmpShip.num_ofc;
fprintf(stderr, "*** ship crew %u!\n", crew);
				if (crew < 1000)
				{
					sprintf(crewBuf, "%3u", crew);
				}
				else if (crew < 10000)
				{
					sprintf(crewBuf, "%2uX", crew / 100);
				}
				else
				{
					sprintf(crewBuf, "%2uK", crew / 1000);
				}
fprintf(stderr, "*** about to set ship location\n");
				sprintf(location, "%4u, %4u", tmpShip.sh_row, tmpShip.sh_col);
				strcpy(tBuff, ctime(&tmpShip.last_seen));
				tBuff[19]='\0';
				sprintf(workBuf, "%8u  %8s   %8s    %c   %s  %s %3u %s", shNum,
						shName, nameBuf, shType, location, crewBuf, tmpShip.efficiency,
						&tBuff[4]);
fprintf(stderr, "*** about to add browser line\n");
				ui_browser_add_line(fd_ShipCensusForm->ShBrowse, workBuf);
			}
		}
	}
fprintf(stderr, "*** about to unfreeze form\n");
}

/*
 * buildShDetDisp - Clears the existing contents of the ship detail
 *                 window and redraws it based on current known
 *                 ships.
 */
void buildShDetDisp(void)
{
	ULONG itNum;
	USHORT num_comp=0, num_eng=0, num_life=0, num_wpn=0, num_elect=0;
	FeShip_t tmpShip;
	FeItem_t tmpItem;
	int compChar, engChar, lifeChar, photChar, blasChar, sensChar,
		shldChar, telepChar, tractChar;
	char workBuf[160], nameBuf[10], itType, location[15], tBuff[28], inst;

	/* Clear the existing contents, if any */
	ui_browser_clear(fd_ShipDetailForm->ShDetItem);
	compChar = (int) BIG_PART_CHAR[bp_computer];
	engChar = (int) BIG_PART_CHAR[bp_engines];
	lifeChar = (int) BIG_PART_CHAR[bp_lifeSupp];
	photChar = (int) BIG_PART_CHAR[bp_photon];
	blasChar = (int) BIG_PART_CHAR[bp_blaser];
    sensChar = (int) BIG_PART_CHAR[bp_sensors];
	shldChar = (int) BIG_PART_CHAR[bp_shield];
	telepChar = (int) BIG_PART_CHAR[bp_teleport];
	tractChar = (int) BIG_PART_CHAR[bp_tractor];
	/* Loop for each known big item */
	for (itNum=0; itNum < next_item; itNum++)
	{
		/* Make sure the item has been seen and is on the ship */
		if (!readItem(&tmpItem, itNum))
		{
			continue;
		}
		if ((tmpItem.last_seen != 0) && (tmpItem.where == LastShip) &&
			!(tmpItem.flags & BF_ONPLAN))
		{
			if (tmpItem.owner != 0)
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
			/* Check if it is installed, or just cargo */
			if (tmpItem.flags & BF_INSTALL)
			{
				/* Installed */
				inst='Y';
				/* Increment our running counts */
				if (itType == tractChar)
				{
					num_comp++;
				}
				else if (itType == engChar)
				{
					num_eng++;
				}
				else if (itType == lifeChar)
				{
					num_life++;
				}
				else if ((itType == photChar) || (itType == blasChar))
				{
					num_wpn++;
				}
				else if ((itType == sensChar) || (itType == shldChar) ||
						 (itType == telepChar) || (itType == tractChar))
				{
					num_elect++;
				}
			}
			else
			{
				/* Not installed, just being carried */
				inst=' ';
			}
			sprintf(workBuf, "%8u  %c  %3u  %3u  %5u %c", itNum,
					itType, tmpItem.it_tf, tmpItem.efficiency,
					tmpItem.weight, inst);
			ui_browser_add_line(fd_ShipDetailForm->ShDetItem, workBuf);
		}
	}
	/* read the ship in */
	(void) readShip(&tmpShip, LastShip);
	/* Update using the counts we saw before */
	tmpShip.num_comp = num_comp;
	tmpShip.num_eng = num_eng;
	tmpShip.num_life = num_life;
	tmpShip.num_wpn = num_wpn;
	tmpShip.num_elect = num_elect;
	/* Write ship back */
	(void) writeShip(&tmpShip, LastShip);
	sprintf(workBuf, "%u", tmpShip.fuelLeft);
	ui_set_text(fd_ShipDetailForm->Fuel, workBuf);
	sprintf(workBuf, "%u", tmpShip.energy);
	ui_set_text(fd_ShipDetailForm->Energy, workBuf);
	sprintf(workBuf, "%u", tmpShip.armourLeft);
	ui_set_text(fd_ShipDetailForm->Armor, workBuf);
	sprintf(workBuf, "%u", tmpShip.shields);
	ui_set_text(fd_ShipDetailForm->Shields, workBuf);
	sprintf(workBuf, "%u", tmpShip.shieldsKeep);
	ui_set_text(fd_ShipDetailForm->ShKeep, workBuf);
	sprintf(workBuf, "%u", tmpShip.airLeft);
	ui_set_text(fd_ShipDetailForm->Air, workBuf);
	sprintf(workBuf, "%u", tmpShip.efficiency);
	ui_set_text(fd_ShipDetailForm->Effic, workBuf);
	sprintf(workBuf, "%u", tmpShip.sh_tf);
	ui_set_text(fd_ShipDetailForm->TF, workBuf);
	sprintf(workBuf, "%u", tmpShip.hullTF);
	ui_set_text(fd_ShipDetailForm->HullTF, workBuf);
	sprintf(workBuf, "%u", tmpShip.engTF);
	ui_set_text(fd_ShipDetailForm->EngTF, workBuf);
	sprintf(workBuf, "%u", tmpShip.engEff);
	ui_set_text(fd_ShipDetailForm->EngEff, workBuf);
	if (tmpShip.plagueStage != 0)
	{
		ui_set_text(fd_ShipDetailForm->Plague, "YES");
	}
	else
	{
		ui_set_text(fd_ShipDetailForm->Plague, "NO");
	}
	sprintf(workBuf, "%u", tmpShip.num_civ);
	ui_set_text(fd_ShipDetailForm->Civ, workBuf);
	sprintf(workBuf, "%u", tmpShip.num_sci);
	ui_set_text(fd_ShipDetailForm->Sci, workBuf);
	sprintf(workBuf, "%u", tmpShip.num_mil);
	ui_set_text(fd_ShipDetailForm->Mil, workBuf);
	sprintf(workBuf, "%u", tmpShip.num_ofc);
	ui_set_text(fd_ShipDetailForm->Ofc, workBuf);
	sprintf(workBuf, "%u", tmpShip.num_torp);
	ui_set_text(fd_ShipDetailForm->Torp, workBuf);
	sprintf(workBuf, "%u", tmpShip.num_ore);
	ui_set_text(fd_ShipDetailForm->Ore, workBuf);
	sprintf(workBuf, "%u", tmpShip.num_gold);
	ui_set_text(fd_ShipDetailForm->Bars, workBuf);
	sprintf(workBuf, "%u", tmpShip.num_airt);
	ui_set_text(fd_ShipDetailForm->AirTnk, workBuf);
	sprintf(workBuf, "%u", tmpShip.num_ftnk);
	ui_set_text(fd_ShipDetailForm->FuelTnk, workBuf);
	sprintf(workBuf, "%u", tmpShip.cargo);
	ui_set_text(fd_ShipDetailForm->Cargo, workBuf);
	strcpy(workBuf, ctime(&tmpShip.last_seen));
	workBuf[24]='\0'; /* strip off NL */
	ui_set_text(fd_ShipDetailForm->Seen, workBuf);
	sprintf(workBuf, "%u", LastShip);
	ui_set_text(fd_ShipDetailForm->ShNum, workBuf);
	sprintf(workBuf, "%u,%u", tmpShip.sh_row, tmpShip.sh_col);
	ui_set_text(fd_ShipDetailForm->RowCol, workBuf);
	if (tmpShip.owner != 0)
	{
		strcpy(workBuf, Player[tmpShip.owner].name);
	}
	else
	{
		strcpy(workBuf, "???");
	}
	ui_set_text(fd_ShipDetailForm->Owner, workBuf);
	if (tmpShip.flags & BF_ONPLAN)
	{
		sprintf(workBuf, "Ship is on planet %u", tmpShip.planet);
		ui_set_text(fd_ShipDetailForm->Where, workBuf);
	}
	else
	{
		ui_set_text(fd_ShipDetailForm->Where, "Ship is in space");
	}

}

