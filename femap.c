#ifndef LINT
static char rcsid[]="$Id: femap.c,v 1.1.1.1 2000/05/17 19:22:13 marisa Exp $";
#endif
/*
 * ImpFE
 *
 * $Id: femap.c,v 1.1.1.1 2000/05/17 19:22:13 marisa Exp $
 *
 * This file contains routines for managing the map window
 *
 * $Log: femap.c,v $
 * Revision 1.1.1.1  2000/05/17 19:22:13  marisa
 * First CVS checkin
 *
 * Revision 1.1.1.2  1997/04/24 04:46:04  marisag
 * patch1: Many changes
 *
 * Revision 1.1.1.1  1997/04/17 06:51:19  marisag
 * patch1: Was using planc.xpm instead of pland.xpm
 *
 * Revision 1.1  1997/04/16 03:08:31  marisag
 * Initial revision
 *
 *
 */

#include "config.h"
#include <sys/types.h>
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

/* Include our Pixmaps */
#include "btile.xpm"
#include "unknown.xpm"
#include "genplan.xpm"
#include "star.xpm"
#include "bhole.xpm"
#include "snova.xpm"
#include "plana.xpm"
#include "planb.xpm"
#include "planc.xpm"
#include "pland.xpm"
#include "planh.xpm"
#include "planm.xpm"
#include "plann.xpm"
#include "plano.xpm"
#include "planq.xpm"

/*
 * setPm - sets the image shown in a map cell
 */
static void setPm(GtkWidget *cell, const char **bits)
{
	gtk_image_set_from_pixbuf(GTK_IMAGE(cell), ui_xpm_pixbuf(bits));
}

/*
 * sectPixmap - returns the image to show for a given sector type
 */
static const char **sectPixmap(int type)
{
	switch (type)
	{
		case BM_UNKNOWN:
			return((const char **) unknown_xpm);
		case BM_PLAN:
			return((const char **) genplan_xpm);
		case BM_STAR:
			return((const char **) star_xpm);
		case BM_BH:
			return((const char **) bhole_xpm);
		case BM_SN:
			return((const char **) snova_xpm);
		case BM_PC_A:
			return((const char **) plana_xpm);
		case BM_PC_B:
			return((const char **) planb_xpm);
		case BM_PC_C:
			return((const char **) planc_xpm);
		case BM_PC_D:
			return((const char **) pland_xpm);
		case BM_PC_M:
			return((const char **) planm_xpm);
		case BM_PC_N:
			return((const char **) plann_xpm);
		case BM_PC_O:
			return((const char **) plano_xpm);
		case BM_PC_Q:
			return((const char **) planq_xpm);
		case BM_PC_H:
			return((const char **) planh_xpm);
		case BM_EMPTY:
			return((const char **) btile_xpm);
		default:
			fprintf(stderr, "*** Unknown sector type\n");
			return((const char **) unknown_xpm);
	}
}

/*
 * buildMapDisp - Builds up the map display based on LastX and LastY
 *                Used only for initial building, scrollFeMap() does
 *                the maintenance once the window is open
 *                Depends on LastX & LastY never being within 9 of the
 *                edge!
 */
void buildMapDisp(void)
{
	int row, col;
	char posit[25];

	if (BitMap == NULL)
	{
		return;
	}
	/* Set the column coord fields */
	for (col=0; col < MAP_COLS; col++)
	{
		sprintf(posit, "%04u", col + LastX);
		ui_set_text(fd_MapForm->ColLbl[col], posit);
	}
	/* Loop for each row */
	for (row=0; row < MAP_ROWS; row++)
	{
		/* Set the row coord field */
		sprintf(posit, "%04u", row + LastY);
		ui_set_text(fd_MapForm->RowLbl[row], posit);

		/* Loop for each column, showing the sector type */
		for (col=0; col < MAP_COLS; col++)
		{
			setPm(fd_MapForm->Cell[row][col], sectPixmap(
				(int) BitMap->coord[row + LastY][col + LastX].type));
		}
	}
	sprintf(posit, "%u", LastY);
	ui_entry_set(fd_MapForm->JmRow, posit);
	sprintf(posit, "%u", LastX);
	ui_entry_set(fd_MapForm->JmCol, posit);
}

/*
 * scrollFeMap - Scrolls the map via the given stepX and stepY,
 *               and updates LastX and LastY
 *               Will not let LastX & LastY never be within 9 of the
 *               bottom/right edge!
 */
void scrollFeMap(int stepX, int stepY)
{
	BOOL doMove=FALSE;

	/* Handle simple cases */
	if ((stepX == 0) && (stepY == 0))
	{
		/* Do nothing */
		return;
	}
	if (stepX != 0)
	{
		if (abs(stepX) < 10)
		{
			/* Moving X < 10 */
			if (stepX < 0)
			{
				if (LastX > 0)
				{
					LastX--;
					doMove=TRUE;
				}
			}
			else
			{
				if (LastX < ((game_sizex - 1) * 10))
				{
					LastX++;
					doMove=TRUE;
				}
				else
				{
					LastX=(game_sizex - 1) * 10;
				}
			}
		}
		else
		{
			/* Moving X >= 10 */
			if (stepX < 0)
			{
				if (LastX > 9)
				{
					LastX -= 10;
					doMove=TRUE;
				}
				else
				{
					LastX = 0;
					doMove=TRUE;
				}
			}
			else
			{
				if (LastX < ((game_sizex - 2) * 10))
				{
					LastX += 10;
					doMove=TRUE;
				}
				else
				{
					LastX = (game_sizex - 1) * 10;
					doMove=TRUE;
				}
			}
		}
	}

	/* Now handle Y movement */
	if (stepY != 0)
	{
		if (abs(stepY) < 7)
		{
			/* Moving Y < 7 */
			if (stepY < 0)
			{
				if (LastY > 0)
				{
					LastY--;
					doMove=TRUE;
				}
			}
			else
			{
				if (LastY < ((game_sizey * 10) - 7))
				{
					LastY++;
					doMove=TRUE;
				}
			}
		}
		else
		{
			/* Moving Y >= 7 */
			if (stepY < 0)
			{
				if (LastY > 7)
				{
					LastY -= 7;
					doMove=TRUE;
				}
				else
				{
					LastY = 0;
					doMove=TRUE;
				}
			}
			else
			{
				if (LastY < (((game_sizey - 1) * 10) - 7))
				{
					LastY += 7;
					doMove=TRUE;
				}
				else
				{
					LastY = ((game_sizey * 10) - 7);
					doMove=TRUE;
				}
			}
		}
	}

	/* Show the new map display */
	if (doMove)
	{
		buildMapDisp();
	}
}

/*
 * updateMaps - This function gets called when another function thinks
 *              the map(s) might need to be updated.
 */
void updateMaps(void)
{
	if (ui_window_visible(fd_MapForm->MapForm))
	{
		buildMapDisp();
	}
}

