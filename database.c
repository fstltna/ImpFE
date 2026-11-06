#ifndef LINT
static char rcsid[]="$Id: database.c,v 1.2 2000/05/17 22:51:46 marisa Exp $";
#endif
/*
 * $Id: database.c,v 1.2 2000/05/17 22:51:46 marisa Exp $
 *
 * $Log: database.c,v $
 * Revision 1.2  2000/05/17 22:51:46  marisa
 * More work to support autoconf - now builds clean on Linux
 *
 * Revision 1.1.1.1  2000/05/17 19:22:13  marisa
 * First CVS checkin
 *
 * Revision 1.1.1.7  1997/04/27 23:23:59  marisag
 * patch1: Daily checkin
 *
 * Revision 1.1.1.6  1997/04/27 16:33:42  marisag
 * patch1: Add routines for maintaining sector planet & ship  listings
 *
 * Revision 1.1.1.5  1997/04/24 04:46:01  marisag
 * patch1: Many changes
 *
 * Revision 1.1.1.4  1997/04/21 00:03:20  marisag
 * patch1: Nows really reads/write the sector records
 *
 * Revision 1.1.1.3  1997/04/16 03:07:35  marisag
 * patch1: Daily checkin
 *
 * Revision 1.1.1.2  1997/04/14 16:05:14  marisag
 * patch1: Daily checkin
 *
 * Revision 1.1.1.1  1997/04/12 16:48:07  marisag
 * patch1: Added much functionality - now stores player, race, ship, and
 * patch1: item data. Still needs caching routines & planet & sector stuff.
 *
 * Revision 1.1  1997/04/10 06:27:59  marisag
 * Initial revision
 *
 *
 */

#include "config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
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

#ifndef HAVE_MMAP
"BOGUS - You don't have mmap() which ImpFE needs!"
#endif
#ifndef HAVE_MUNMAP
"BOGUS - You don't have munmap() which ImpFE needs!"
#endif
/*
 * Distance formula: sqrt((abs(x1 - x2)^2) + (abs(y1 - y2)^2))
 */

unsigned long getDistance(unsigned short x1, unsigned short y1,
	unsigned short x2, unsigned short y2)
{
	return((unsigned long) sqrt(
		(double) (abs((long) x1 - (long) x2)^2) +
		(double) (abs((long) y1 - (long) y2)^2)
		));
}

static char ParseBuf[2048];

static char BaseDir[1024];
static char BaseFile[1024];
static char ShipFile[1024];
static char PlanetFile[1024];
static char ItemFile[1024];
static char SectorFile[1024];
static char BitmapFile[1024];
static char PlayerFile[1024];
static char RaceFile[1024];
static FILE *ShipFileHndl;
static FILE *PlanetFileHndl;
static FILE *ItemFileHndl;
static FILE *SectorFileHndl;
static int BitmapFileHndl=-1;
static FILE *PlayerFileHndl;
static FILE *RaceFileHndl;

/*
 * noExt - like strcpy except strips off any extension
 */
void noExt(char *dest, char *src)
{
	char *tmpPos;

	/* Make sure they have an extension */
	if (strchr(src, '.') == NULL)
	{
	    /* Just copy it */
	    strcpy(dest, src);
	    return;
	}
	tmpPos=strrchr(src, '.');
	strcpy(dest, src);
	dest[(unsigned long)tmpPos - (unsigned long)src] = '\0';
}

/*
 * openFeDBs - Attempts to open the binary databases. If createIt is
 *             true, will delete any existing files and recreate them.
 *             Assumes file to be used as a guide has just been selected
 *             via the browser
 */
/*
 * dbName - builds the name of one of the database files from the name
 *          of the config file. Returns FALSE if it would not fit.
 */
static BOOL dbName(char *dest, const char *suffix)
{
	return(snprintf(dest, 1024, "%s-%s", RawFile, suffix) < 1024);
}

BOOL openFeDBs(BOOL createIt)
{
	int tmp;
	Player_t tmpPlayer;
	Race_t tmpRace;
	char locBuf[4096];

	/* Pull selections out of file selector buffer */
	bzero(locBuf, 4096 * sizeof(char));
	if ((snprintf(BaseDir, sizeof(BaseDir), "%s", ui_last_directory()) >=
		 (int) sizeof(BaseDir)) ||
		(snprintf(BaseFile, sizeof(BaseFile), "%s", ui_last_filename()) >=
		 (int) sizeof(BaseFile)))
	{
		fe_puts("*** Database path is too long\n");
		return(FALSE);
	}
	/* Get the file name without an extension */
	noExt(RawFile, BaseFile);
	/* Build the rest of the names */
	if (!dbName(ShipFile, "ships") || !dbName(PlanetFile, "planets") ||
		!dbName(ItemFile, "items") || !dbName(SectorFile, "sectors") ||
		!dbName(BitmapFile, "sectors-bitmap") ||
		!dbName(PlayerFile, "players") || !dbName(RaceFile, "races"))
	{
		fe_puts("*** Database file name is too long\n");
		return(FALSE);
	}
	DirtyData=FALSE;
	/* Set working directory - the database files are relative to it */
	if (chdir(BaseDir) != 0)
	{
		fe_puts("*** Unable to change to the database directory ");
		fe_puts(BaseDir);
		fe_puts(": ");
		fe_puts(strerror(errno));
		fe_puts("\n");
		return(FALSE);
	}
	/* If creating, create/truncate the files */
	if (createIt)
	{
		if ((ShipFileHndl=fopen(ShipFile, "r")) != NULL)
		{
			fclose(ShipFileHndl);
			unlink(ShipFile);
		}
		if ((ShipFileHndl=fopen(ShipFile, "w+")) == NULL)
		{
			fe_puts("*** Unable to create the ships database!\n");
			return(FALSE);
		}
		if ((PlanetFileHndl=fopen(PlanetFile, "r")) != NULL)
		{
			fclose(PlanetFileHndl);
			unlink(PlanetFile);
		}
		if ((PlanetFileHndl=fopen(PlanetFile, "w+")) == NULL)
		{
			fclose(ShipFileHndl);
			ShipFileHndl=NULL;
			fe_puts("*** Unable to create the planets database!\n");
			return(FALSE);
		}
		if ((ItemFileHndl=fopen(ItemFile, "r")) != NULL)
		{
			fclose(ItemFileHndl);
			unlink(ItemFile);
		}
		if ((ItemFileHndl=fopen(ItemFile, "w+")) == NULL)
		{
			fclose(ShipFileHndl);
			fclose(PlanetFileHndl);
			ShipFileHndl=NULL;
			PlanetFileHndl=NULL;
			fe_puts("*** Unable to create the items database!\n");
			return(FALSE);
		}
		if ((SectorFileHndl=fopen(SectorFile, "r")) != NULL)
		{
			fclose(SectorFileHndl);
			unlink(SectorFile);
		}
		if ((SectorFileHndl=fopen(SectorFile, "w+")) == NULL)
		{
			fclose(ShipFileHndl);
			fclose(PlanetFileHndl);
			fclose(ItemFileHndl);
			ShipFileHndl=NULL;
			PlanetFileHndl=NULL;
			ItemFileHndl=NULL;
			fe_puts("*** Unable to create the sectors database!\n");
			return(FALSE);
		}
		if ((BitmapFileHndl=open(BitmapFile, O_RDONLY)) != -1)
		{
			close(BitmapFileHndl);
			unlink(BitmapFile);
		}
		if ((BitmapFileHndl=open(BitmapFile, O_RDWR|O_CREAT,
								 S_IREAD|S_IWRITE|S_IRGRP|S_IWGRP|S_IROTH)) == -1)
		{
			fclose(ShipFileHndl);
			fclose(PlanetFileHndl);
			fclose(ItemFileHndl);
			fclose(SectorFileHndl);
			ShipFileHndl=NULL;
			PlanetFileHndl=NULL;
			ItemFileHndl=NULL;
			SectorFileHndl=NULL;
			fe_puts("*** Unable to create the bitmap sectors database!\n");
			return(FALSE);
		}
		ui_browser_append(fd_ImpFeMain->MainBrowser, ">>> Creating the sectors-bitmap database - this may take a while...\n");
		/* Create the file length */
		for (tmp=0; tmp < (sizeof(FeBitmap_t) / 4096) + 1; tmp++)
		{
		    if (write(BitmapFileHndl, locBuf, 4096 * sizeof(char)) !=
				4096 * sizeof(char))
		    {
				/* A short file would crash us when mapped (disk full?) */
				fe_puts("*** Unable to write the bitmap sectors database: ");
				fe_puts(strerror(errno));
				fe_puts("\n");
				fclose(ShipFileHndl);
				fclose(PlanetFileHndl);
				fclose(ItemFileHndl);
				fclose(SectorFileHndl);
				close(BitmapFileHndl);
				ShipFileHndl=NULL;
				PlanetFileHndl=NULL;
				ItemFileHndl=NULL;
				SectorFileHndl=NULL;
				BitmapFileHndl=-1;
				unlink(BitmapFile);
				return(FALSE);
		    }
		}
		lseek(BitmapFileHndl, 0, SEEK_SET);
		ui_browser_append(fd_ImpFeMain->MainBrowser, ">>> sectors-bitmap database created\n");
		if ((BitMap=mmap(NULL, sizeof(FeBitmap_t),
						 PROT_READ|PROT_WRITE, MAP_SHARED, BitmapFileHndl,
						 0)) ==  (FeBitmap_t *)-1)
		{
			fclose(ShipFileHndl);
			fclose(PlanetFileHndl);
			fclose(ItemFileHndl);
			fclose(SectorFileHndl);
			close(BitmapFileHndl);
			ShipFileHndl=NULL;
			PlanetFileHndl=NULL;
			ItemFileHndl=NULL;
			SectorFileHndl=NULL;
			BitmapFileHndl=-1;
			fe_puts("*** Unable to mmap() the bitmap sectors database!\n");
			return(FALSE);
		}
		if ((PlayerFileHndl=fopen(PlayerFile, "r")) != NULL)
		{
			fclose(PlayerFileHndl);
			unlink(PlayerFile);
		}
		if ((PlayerFileHndl=fopen(PlayerFile, "w+")) == NULL)
		{
			fclose(ShipFileHndl);
			fclose(PlanetFileHndl);
			fclose(ItemFileHndl);
			fclose(SectorFileHndl);
			munmap(BitMap, sizeof(FeBitmap_t));
			BitMap=NULL;
			close(BitmapFileHndl);
			ShipFileHndl=NULL;
			PlanetFileHndl=NULL;
			ItemFileHndl=NULL;
			SectorFileHndl=NULL;
			BitmapFileHndl=-1;
			fe_puts("*** Unable to create the players database!\n");
			return(FALSE);
		}
		if ((RaceFileHndl=fopen(RaceFile, "r")) != NULL)
		{
			fclose(RaceFileHndl);
			unlink(RaceFile);
		}
		if ((RaceFileHndl=fopen(RaceFile, "w+")) == NULL)
		{
			fclose(ShipFileHndl);
			fclose(PlanetFileHndl);
			fclose(ItemFileHndl);
			fclose(SectorFileHndl);
			munmap(BitMap, sizeof(FeBitmap_t));
			BitMap=NULL;
			close(BitmapFileHndl);
			fclose(PlayerFileHndl);
			ShipFileHndl=NULL;
			PlanetFileHndl=NULL;
			ItemFileHndl=NULL;
			SectorFileHndl=NULL;
			BitmapFileHndl=-1;
			PlayerFileHndl=NULL;
			fe_puts("*** Unable to create the races database!\n");
			return(FALSE);
		}
		/* Pre-fill the arrays */
		bzero(&tmpPlayer, sizeof(Player_t));
		bzero(&tmpRace, sizeof(Race_t));
		tmpPlayer.status=ps_notseen;
		tmpPlayer.relation=relation_default;
		ui_browser_append(fd_ImpFeMain->MainBrowser, ">>> Creating players database\n");
		for (tmp=0; tmp < PLAYER_MAX; tmp++)
		{
		    tmpPlayer.number=tmp;
		    Player[tmp] = tmpPlayer;
		    if (fwrite(&tmpPlayer, sizeof(char), sizeof(Player_t),
					   PlayerFileHndl) != sizeof(Player_t))
		    {
				fe_puts("*** Error writing to players file\n");
				tmp=PLAYER_MAX;
		    }
		}
		tmpRace.relation=relation_default;
		ui_browser_append(fd_ImpFeMain->MainBrowser, ">>> Creating races database\n");
		for (tmp=0; tmp < RACE_MAX; tmp++)
		{
		    Race[tmp] = tmpRace;
		    if (fwrite(&tmpRace, sizeof(char), sizeof(Race_t),
					   RaceFileHndl) != sizeof(Race_t))
		    {
				fe_puts("*** Error writing to races file\n");
				tmp=RACE_MAX;
		    }
		}
		ui_browser_append(fd_ImpFeMain->MainBrowser, ">>> All database created\n");
	}
	else
	{
		if ((ShipFileHndl=fopen(ShipFile, "r+")) == NULL)
		{
			fe_puts("*** Unable to modify the ships database!\n");
			return(FALSE);
		}
		if ((PlanetFileHndl=fopen(PlanetFile, "r+")) == NULL)
		{
			fclose(ShipFileHndl);
			ShipFileHndl=NULL;
			fe_puts("*** Unable to modify the planets database!\n");
			return(FALSE);
		}
		if ((ItemFileHndl=fopen(ItemFile, "r+")) == NULL)
		{
			fclose(ShipFileHndl);
			fclose(PlanetFileHndl);
			ShipFileHndl=NULL;
			PlanetFileHndl=NULL;
			fe_puts("*** Unable to modify the items database!\n");
			return(FALSE);
		}
		if ((SectorFileHndl=fopen(SectorFile, "r+")) == NULL)
		{
			fclose(ShipFileHndl);
			fclose(PlanetFileHndl);
			fclose(ItemFileHndl);
			ShipFileHndl=NULL;
			PlanetFileHndl=NULL;
			ItemFileHndl=NULL;
			fe_puts("*** Unable to modify the sectors database!\n");
			return(FALSE);
		}
		if ((BitmapFileHndl=open(BitmapFile, O_RDWR|O_CREAT,
								 S_IREAD|S_IWRITE|S_IRGRP|S_IWGRP|S_IROTH)) == -1)
		{
			fclose(ShipFileHndl);
			fclose(PlanetFileHndl);
			fclose(ItemFileHndl);
			fclose(SectorFileHndl);
			ShipFileHndl=NULL;
			PlanetFileHndl=NULL;
			ItemFileHndl=NULL;
			SectorFileHndl=NULL;
			fe_puts("*** Unable to modify the bitmap sectors database!\n");
			return(FALSE);
		}
		if ((BitMap=mmap(NULL, sizeof(FeBitmap_t),
						 PROT_READ|PROT_WRITE, MAP_SHARED, BitmapFileHndl,
						 0)) ==  (FeBitmap_t *)-1)
		{
			fclose(ShipFileHndl);
			fclose(PlanetFileHndl);
			fclose(ItemFileHndl);
			fclose(SectorFileHndl);
			close(BitmapFileHndl);
			ShipFileHndl=NULL;
			PlanetFileHndl=NULL;
			ItemFileHndl=NULL;
			SectorFileHndl=NULL;
			BitmapFileHndl=-1;
			fe_puts("*** Unable to mmap() the bitmap sectors database!\n");
			return(FALSE);
		}
		if ((PlayerFileHndl=fopen(PlayerFile, "r+")) == NULL)
		{
			fclose(ShipFileHndl);
			fclose(PlanetFileHndl);
			fclose(ItemFileHndl);
			fclose(SectorFileHndl);
			munmap(BitMap, sizeof(FeBitmap_t));
			BitMap=NULL;
			close(BitmapFileHndl);
			ShipFileHndl=NULL;
			PlanetFileHndl=NULL;
			ItemFileHndl=NULL;
			SectorFileHndl=NULL;
			BitmapFileHndl=-1;
			fe_puts("*** Unable to modify the players database!\n");
			return(FALSE);
		}
		/* Read the player structs in */
		for (tmp=0; tmp < PLAYER_MAX; tmp++)
		{
			if (fread(&Player[tmp], sizeof(char), sizeof(Player_t),
					  PlayerFileHndl) != sizeof(Player_t))
			{
				tmp=PLAYER_MAX;
				fputs("*** Error reading player file", stderr);
			}
		}
		if ((RaceFileHndl=fopen(RaceFile, "r+")) == NULL)
		{
			fclose(ShipFileHndl);
			fclose(PlanetFileHndl);
			fclose(ItemFileHndl);
			fclose(SectorFileHndl);
			munmap(BitMap, sizeof(FeBitmap_t));
			BitMap=NULL;
			close(BitmapFileHndl);
			fclose(PlayerFileHndl);
			ShipFileHndl=NULL;
			PlanetFileHndl=NULL;
			ItemFileHndl=NULL;
			SectorFileHndl=NULL;
			BitmapFileHndl=-1;
			PlayerFileHndl=NULL;
			fe_puts("*** Unable to modify the players database!\n");
			return(FALSE);
		}
		/* Read the Race structs in */
		for (tmp=0; tmp < RACE_MAX; tmp++)
		{
			if (fread(&Race[tmp], sizeof(char), sizeof(Race_t),
					  RaceFileHndl) != sizeof(Race_t))
			{
				tmp=RACE_MAX;
				fputs("*** Error reading race file", stderr);
			}
		}
	}
    return(TRUE);
}

/*
 * Flush the caches to disk
 */
void flushCache(void)
{
	int tmp;

	/* Note that there is just one bitmap and it isn't cached */
	/* Do we need to flush data before closing? */
	if (DirtyData)
	{
	    if (PlayerFileHndl != NULL)
	    {
			/* Set to beginning of file */
			rewind(PlayerFileHndl);
			for (tmp=0; tmp < PLAYER_MAX; tmp++)
			{
				if (fwrite(&Player[tmp], sizeof(char), sizeof(Player_t),
						   PlayerFileHndl) != sizeof(Player_t))
				{
					fe_puts("*** Error writing to players file\n");
					tmp=PLAYER_MAX;
				}
			}
	    }
	    if (RaceFileHndl != NULL)
	    {
			/* Set to beginning of file */
			rewind(RaceFileHndl);
			for (tmp=0; tmp < RACE_MAX; tmp++)
			{
				if (fwrite(&Race[tmp], sizeof(char), sizeof(Race_t),
						   RaceFileHndl) != sizeof(Race_t))
				{
					fe_puts("*** Error writing to race file\n");
					tmp=RACE_MAX;
				}
			}
	    }
	}
}

void closeFeDBs(void)
{
	int tmp;

	flushCache();
	if (ShipFileHndl != NULL)
	{
	    fclose(ShipFileHndl);
	    ShipFileHndl = NULL;
	}
	if (PlanetFileHndl != NULL)
	{
	    fclose(PlanetFileHndl);
	    PlanetFileHndl = NULL;
	}
	if (ItemFileHndl != NULL)
	{
	    fclose(ItemFileHndl);
	    ItemFileHndl = NULL;
	}
	if (SectorFileHndl != NULL)
	{
	    fclose(SectorFileHndl);
	    SectorFileHndl = NULL;
	}
	if (BitmapFileHndl != -1)
	{
	    if (BitMap != NULL)
	    {
			munmap(BitMap, sizeof(FeBitmap_t));
			BitMap=NULL;
	    }
	    close(BitmapFileHndl);
	    BitmapFileHndl = -1;
	}
	if (PlayerFileHndl != NULL)
	{
	    fclose(PlayerFileHndl);
	    PlayerFileHndl = NULL;
	}
	if (RaceFileHndl != NULL)
	{
	    fclose(RaceFileHndl);
	    RaceFileHndl = NULL;
	}
	DirtyData=FALSE;
}

/*
 * readShip - This reads the given ship in from disk, initializing the
 *            structure if it didn't exist previously.
 * XXX This should be converted to use a cache
 */
BOOL readShip(FeShip_t *shBuf, unsigned long shNum)
{
	int fileErr;

	/* First handle new ships */
	if (shNum >= next_ship)
	{
	    next_ship = shNum+1;
	    bzero(shBuf, sizeof(FeShip_t));
	    return(TRUE);
	}
	if (fseek(ShipFileHndl, shNum * sizeof(FeShip_t), SEEK_SET) != 0)
	{
	    fprintf(stderr, "*** Error seeking to ship %lu!\n", shNum);
	    return(FALSE);
	}
	/* Attempt to load the ship */
	if (fread(shBuf, sizeof(char), sizeof(FeShip_t), ShipFileHndl) == 0)
	{
	    /* See if it is just due to seeking into a hole */
	    fileErr=ferror(ShipFileHndl);
	    fprintf(stderr, "Got read error %d in ships file\n", fileErr);
	    if (fileErr == 0) /* XXX Make this match the "read in hole" error */
	    {
	    	clearerr(ShipFileHndl);
		bzero(shBuf, sizeof(FeShip_t));
		return(TRUE);
	    }
	    clearerr(ShipFileHndl);
	    return(FALSE);
	}
	return(TRUE);
}

BOOL writeShip(FeShip_t *shBuf, unsigned long shNum)
{
	int fileErr;

	/* Try to seek to the correct spot */
	if (fseek(ShipFileHndl, shNum * sizeof(FeShip_t), SEEK_SET) != 0)
	{
	    fprintf(stderr, "*** Write error seeking to ship %lu!\n", shNum);
	    return(FALSE);
	}
	/* Attempt to write the ship */
	if (fwrite(shBuf, sizeof(char), sizeof(FeShip_t), ShipFileHndl) != sizeof(FeShip_t))
	{
	    fileErr=ferror(ShipFileHndl);
	    fprintf(stderr, "Got write error %d in saving ship %lu to ships file\n",
				fileErr, shNum);
	    clearerr(ShipFileHndl);
	    return(FALSE);
	}
	return(TRUE);
}

/*
 * readPlanet - This reads the given ship in from disk, initializing the
 *            structure if it didn't exist previously.
 * XXX This should be converted to use a cache
 */
BOOL readPlanet(FePlanet_t *plBuf, unsigned long plNum)
{
	int fileErr;

	/* First handle new planets */
	if (plNum >= next_planet)
	{
	    next_planet = plNum+1;
	    bzero(plBuf, sizeof(FePlanet_t));
	    return(TRUE);
	}
	if (fseek(PlanetFileHndl, plNum * sizeof(FePlanet_t), SEEK_SET) != 0)
	{
	    fprintf(stderr, "*** Error seeking to planet %lu!\n", plNum);
	    return(FALSE);
	}
	/* Attempt to load the planet */
	if (fread(plBuf, sizeof(char), sizeof(FePlanet_t), PlanetFileHndl) == 0)
	{
	    /* See if it is just due to seeking into a hole */
	    fileErr=ferror(PlanetFileHndl);
	    fprintf(stderr, "Got read error %d in planets file\n", fileErr);
	    if (fileErr == 0) /* XXX Make this match the "read in hole" error */
	    {
fprintf(stderr, "*** Read in hole\n");
	    	clearerr(PlanetFileHndl);
		bzero(plBuf, sizeof(FePlanet_t));
		return(TRUE);
	    }
	    clearerr(PlanetFileHndl);
	    return(FALSE);
	}
fprintf(stderr, "*** Loaded planet\n");
	return(TRUE);
}

BOOL writePlanet(FePlanet_t *plBuf, unsigned long plNum)
{
	int fileErr;

	/* Try to seek to the correct spot */
	if (fseek(PlanetFileHndl, plNum * sizeof(FePlanet_t), SEEK_SET) != 0)
	{
	    fprintf(stderr, "*** Write error seeking to planet %lu!\n", plNum);
	    return(FALSE);
	}
	/* Attempt to write the planet */
	if (fwrite(plBuf, sizeof(char), sizeof(FePlanet_t), PlanetFileHndl) != sizeof(FePlanet_t))
	{
	    fileErr=ferror(PlanetFileHndl);
	    fprintf(stderr, "Got write error %d in saving ship %lu to planets file\n",
				fileErr, plNum);
	    clearerr(PlanetFileHndl);
	    return(FALSE);
	}
	return(TRUE);
}

BOOL readItem(FeItem_t *itBuf, unsigned long itNum)
{
	int fileErr;

	/* First handle new items */
	if (itNum >= next_item)
	{
	    next_item = itNum+1;
	    bzero(itBuf, sizeof(FeItem_t));
	    return(TRUE);
	}
	if (fseek(ItemFileHndl, itNum * sizeof(FeItem_t), SEEK_SET) != 0)
	{
	    fprintf(stderr, "*** Error seeking to item %lu!\n", itNum);
	    return(FALSE);
	}
	/* Attempt to load the item */
	if (fread(itBuf, sizeof(char), sizeof(FeItem_t), ItemFileHndl) == 0)
	{
	    /* See if it is just due to seeking into a hole */
	    fileErr=ferror(ItemFileHndl);
	    fprintf(stderr, "Got read error %d in items file\n", fileErr);
	    clearerr(ItemFileHndl);
	    if (fileErr == 0) /* XXX Make this match the "read in hole" error */
	    {
			bzero(itBuf, sizeof(FeItem_t));
			return(TRUE);
	    }
	    return(FALSE);
	}
	return(TRUE);
}

BOOL writeItem(FeItem_t *itBuf, unsigned long itNum)
{
	int fileErr;

	/* Try to seek to the correct spot */
	if (fseek(ItemFileHndl, itNum * sizeof(FeItem_t), SEEK_SET) != 0)
	{
	    fprintf(stderr, "*** Write error seeking to item %lu!\n", itNum);
	    return(FALSE);
	}
	/* Attempt to write the item */
	if (fwrite(itBuf, sizeof(char), sizeof(FeItem_t), ItemFileHndl) != sizeof(FeItem_t))
	{
	    fileErr=ferror(ItemFileHndl);
	    fprintf(stderr, "Got write error %d in saving item %lu to items file\n",
				fileErr, itNum);
	    clearerr(ItemFileHndl);
	    return(FALSE);
	}
	return(TRUE);
}

/*
 * readSector - Reads in the given sector. scNum is the sector number
 */
BOOL readSector(FeSector_t *scBuf, USHORT row, USHORT col)
{
	unsigned long scNum;
	int fileErr, tmp;

	/* Convert the row,col to the sector number */
	row /= 10;
	col /= 10;
	scNum = (row * 1000) + col;
#ifdef DEBUG
	printf("Reading sector %u,%u\n", row, col);
#endif

	if (fseek(SectorFileHndl, scNum * sizeof(FeSector_t), SEEK_SET) != 0)
	{
	    fprintf(stderr, "*** Error seeking to sector %u,%u!\n", row, col);
	    return(FALSE);
	}
	/* Attempt to load the sector */
	if (fread(scBuf, sizeof(char), sizeof(FeSector_t), SectorFileHndl) == 0)
	{
	    /* See if it is just due to seeking into a hole */
	    fileErr=ferror(SectorFileHndl);
	    fprintf(stderr, "Got read error %d in sectors file (%u,%u)\n", fileErr, row, col);
	    clearerr(SectorFileHndl);
	    if (fileErr == 0) /* XXX Make this match the "read in hole" error */
	    {
			bzero(scBuf, sizeof(FeSector_t));
			for (tmp=0; tmp < MAX_SECT_PLAN; tmp++)
			{
				scBuf->plan[tmp] = IT_NONE;
			}
			for (tmp=0; tmp < MAX_SECT_SHIP; tmp++)
			{
				scBuf->ship[tmp] = IT_NONE;
			}
			return(TRUE);
	    }
	    return(FALSE);
	}
	else
	{
#ifdef DEBUG
		printf("Read was good\n");
#endif
		if (scBuf->flags == 0)
		{
#ifdef DEBUG
			printf("Flags were zero - clearing\n");
#endif
			bzero(scBuf, sizeof(FeSector_t));
			for (tmp=0; tmp < MAX_SECT_PLAN; tmp++)
			{
				scBuf->plan[tmp] = IT_NONE;
			}
			for (tmp=0; tmp < MAX_SECT_SHIP; tmp++)
			{
				scBuf->ship[tmp] = IT_NONE;
			}
			scBuf->flags |= BF_IS_EMPTY;
			return(TRUE);
		}
	}
	return(TRUE);
}

BOOL writeSector(FeSector_t *scBuf, USHORT row, USHORT col)
{
	unsigned long scNum;
	int fileErr;

	/* Convert the row,col to the sector number */
	row /= 10;
	col /= 10;
	scNum = (row * 1000) + col;
#ifdef DEBUG
	printf("Writing sector %u,%u\n", row, col);
#endif

	/* Try to seek to the correct spot */
	if (fseek(SectorFileHndl, scNum * sizeof(FeSector_t), SEEK_SET) != 0)
	{
	    fprintf(stderr, "*** Write error seeking to sector %u,%u!\n", row,
				col);
	    return(FALSE);
	}
	/* Attempt to write the sector */
	if (fwrite(scBuf, sizeof(char), sizeof(FeSector_t), SectorFileHndl) != sizeof(FeSector_t))
	{
	    fileErr=ferror(SectorFileHndl);
	    fprintf(stderr, "Got write error %d in saving sector %u,%u to sectors file\n",
				fileErr, row, col);
	    clearerr(SectorFileHndl);
	    return(FALSE);
	}
	return(TRUE);
}

/*
 * getSectPlanet - returns the planet located in the given sector, or
 *                 IT_NONE if none there
 */
ULONG getSectPlanet(USHORT row, USHORT col)
{
    ULONG tmp;
	FeSector_t tmpSect;
	FePlanet_t tmpPlan;

	/* Try to read the sector */
#ifdef DEBUG
	printf("getSectPlanet passed %u, %u\n", row, col);
#endif
	if (readSector(&tmpSect, row, col))
	{
		/* If none here, return */
		if (tmpSect.num_plan == 0)
		{
#ifdef DEBUG
			printf("No planets here\n");
#endif
			return(IT_NONE);
		}
		/* First try the cache */
		for (tmp=0; tmp < MAX_SECT_PLAN; tmp++)
		{
			/* This slot used? */
			if (tmpSect.plan[tmp] != IT_NONE)
			{
				/* Try to read the planet */
				if (readPlanet(&tmpPlan, tmpSect.plan[tmp]))
				{
					if (tmpPlan.last_seen != 0)
					{
						/* Check the row & col */
						if ((tmpPlan.pl_row == row) && (tmpPlan.pl_col == col))
						{
							/* A match! */
							return(tmpSect.plan[tmp]);
						}
					}
				}
				else
				{
					/* Planet read failed for a planet we saw before */
					return(IT_NONE);
				}
			}
		}
		/* Ok, we didn't find it in the cache, so do it the long way... */
		for (tmp=0; tmp < next_planet; tmp++)
		{
			if (readPlanet(&tmpPlan, tmp))
			{
				if (tmpPlan.last_seen != 0)
				{
					if ((tmpPlan.pl_row == row) && (tmpPlan.pl_col == col))
					{
						return(tmp);
					}
				}
			}
		}
	}
	return(IT_NONE);
}

/*
 * getSectShip - returns the ship located in the given sector, or
 *                 IT_NONE if none there or more than one.
 */
ULONG getSectShip(USHORT row, USHORT col)
{
    ULONG tmp, lShip, count=0;
	FeSector_t tmpSect;
	FeShip_t tmpShip;

	/* Try to read the sector */
	if (readSector(&tmpSect, row, col))
	{
		/* If none here, return */
		if (tmpSect.num_ship == 0)
		{
			return(IT_NONE);
		}
		if (tmpSect.num_ship < MAX_SECT_SHIP)
		{
			/* Trying the cache */
			for (tmp=0; tmp < MAX_SECT_SHIP; tmp++)
			{
				/* This slot used? */
				if (tmpSect.ship[tmp] != IT_NONE)
				{
					/* Try to read the ship */
					if (readShip(&tmpShip, tmpSect.ship[tmp]))
					{
						if (tmpShip.last_seen != 0)
						{
							/* Check the row & col */
							if ((tmpShip.sh_row == row) &&
								(tmpShip.sh_col == col))
							{
								/* A match! */
								lShip = tmpSect.ship[tmp];
								count++;
							}
						}
					}
					else
					{
						/* Ship read failed for a ship we saw before */
						return(IT_NONE);
					}
				}
			}
			/* If we have found more than one ship, return */
			if (count > 1)
			{
				return(IT_NONE);
			}
			/* If there are less ships than the cache will hold, and we */
			/* only saw it once, then return the number */
			if (count == 1)
			{
				return(lShip);
			}
			return(IT_NONE);
		}
		/* Ok, more ships than the cache will hold, so do it the long way... */
		for (tmp=0; tmp < next_ship; tmp++)
		{
			if (readShip(&tmpShip, tmp))
			{
				if (tmpShip.last_seen != 0)
				{
					if ((tmpShip.sh_row == row) && (tmpShip.sh_col == col))
					{
						lShip = tmp;
						count++;
					}
				}
			}
		}
		if (count == 1)
		{
			return(lShip);
		}
	}
	return(IT_NONE);
}

/*
 * getSectShList - sets buf to be a list of ships located in the given
 *                 sector, or '\0' if none there
 */
void getSectShList(USHORT row, USHORT col, char *buf)
{
}

void addSectShip(USHORT row, USHORT col, ULONG shNum)
{
	int tmp;
	FeSector_t tmpSect;

	(void) readSector(&tmpSect, row, col);
	/* Don't bother with this if none here */
	if (tmpSect.num_ship != 0)
	{
		/* See if we have already seen this one */
		for (tmp=0; tmp < MAX_SECT_SHIP; tmp++)
		{
			if (tmpSect.ship[tmp] == shNum)
			{
				return;
			}
		}
	}
	/* Use first available slot */
	if (tmpSect.num_ship >= MAX_SECT_SHIP)
	{
		/* Can't cache any more, but increment count */
		tmpSect.num_ship++;
		(void) writeSector(&tmpSect, row, col);
		return;
	}
	for (tmp=0; tmp < MAX_SECT_SHIP; tmp++)
	{
		if (tmpSect.ship[tmp] == IT_NONE)
		{
			tmpSect.ship[tmp] = shNum;
			tmpSect.num_ship++;
			(void) writeSector(&tmpSect, row, col);
			return;
		}
	}
}

void delSectShip(USHORT row, USHORT col, ULONG shNum)
{
	int tmp;
	FeSector_t tmpSect;

	(void) readSector(&tmpSect, row, col);
	/* Don't bother with this if none here */
	if (tmpSect.num_ship == 0)
	{
		return;
	}
	/* Check for it in the cache */
	for (tmp=0; tmp < MAX_SECT_SHIP; tmp++)
	{
		if (tmpSect.ship[tmp] == shNum)
		{
			tmpSect.ship[tmp] = IT_NONE;
			tmpSect.num_ship--;
			(void) writeSector(&tmpSect, row, col);
			return;
		}
	}
	/* Didn't find it, so must not be in the cache */
	if (tmpSect.num_ship > 0)
	{
		tmpSect.num_ship--;
		(void) writeSector(&tmpSect, row, col);
		return;
	}
}

void addSectPlan(USHORT row, USHORT col, ULONG plNum)
{
	int tmp;
	FeSector_t tmpSect;

#ifdef DEBUG
	printf("In addSectPlan()\n");
#endif
	(void) readSector(&tmpSect, row, col);
	/* Don't bother with this if none here */
	if (tmpSect.num_plan != 0)
	{
		/* See if we have already seen this one */
		for (tmp=0; tmp < MAX_SECT_PLAN; tmp++)
		{
			if (tmpSect.plan[tmp] == plNum)
			{
#ifdef DEBUG
				printf("Already seen it\n");
#endif
				return;
			}
		}
	}
	/* Use first available slot */
	if (tmpSect.num_plan >= MAX_SECT_PLAN)
	{
		/* Can't cache any more, but increment count */
		tmpSect.num_plan++;
		tmpSect.flags &= ~BF_IS_EMPTY;
		tmpSect.flags |= BF_IS_NORMAL;
		(void) writeSector(&tmpSect, row, col);
		return;
	}
	for (tmp=0; tmp < MAX_SECT_PLAN; tmp++)
	{
		if (tmpSect.plan[tmp] == IT_NONE)
		{
			tmpSect.plan[tmp] = plNum;
			tmpSect.num_plan++;
			tmpSect.flags &= ~BF_IS_EMPTY;
			tmpSect.flags |= BF_IS_NORMAL;
			(void) writeSector(&tmpSect, row, col);
#ifdef DEBUG
			printf("Stored it\n");
			printf("num_plan = %u\n", tmpSect.num_plan);
#endif
			return;
		}
	}
	fprintf(stderr, "*** Oops! Fell through!\n");
}

