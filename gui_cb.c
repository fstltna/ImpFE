#ifndef LINT
/*@unused@*/static char rcsid[] = "$Id: gui_cb.c,v 1.2 2000/05/24 21:51:38 marisa Exp $";
#endif

/*
 * $Id: gui_cb.c,v 1.2 2000/05/24 21:51:38 marisa Exp $
 *
 * Handles the callbacks for ImpFe
 *
 * $Log: gui_cb.c,v $
 * Revision 1.2  2000/05/24 21:51:38  marisa
 * Fix bugs with TRUE/FALSE testing
 *
 * Revision 1.1.1.1  2000/05/17 19:22:13  marisa
 * First CVS checkin
 *
 * Revision 1.1.1.10  1997/05/03 09:42:35  marisag
 * patch1: Daily checkin
 *
 * Revision 1.1.1.9  1997/04/27 23:23:30  marisag
 * patch1: Daily checkin
 *
 * Revision 1.1.1.8  1997/04/27 16:32:21  marisag
 * patch1: Fix bug with browsers
 *
 * Revision 1.1.1.7  1997/04/25 06:39:10  marisag
 * patch1: Code for the planet & ship detail forms
 *
 * Revision 1.1.1.6  1997/04/24 04:46:06  marisag
 * patch1: Many changes
 *
 * Revision 1.1.1.5  1997/04/16 03:07:02  marisag
 * patch1: New callbacks
 *
 * Revision 1.1.1.4  1997/04/14 02:51:25  marisag
 * patch1: New functions
 *
 * Revision 1.1.1.3  1997/04/12 16:50:14  marisag
 * patch1: Much changes - handle saving data on exit, not asking for file
 * patch1: name when saving already opened data, clearing vars
 * patch1: when making new config...
 *
 * Revision 1.1.1.2  1997/04/10 06:28:48  marisag
 * patch1: Makes use of the new config & database functions.
 *
 * Revision 1.1.1.1  1997/04/09 03:21:52  marisag
 * patch1: New changes...
 *
 * Revision 1.1  1997/04/08 07:07:36  marisag
 * Initial revision
 *
 *
 */

#include "config.h"
#include "ImpFeGui.h"
#include "gui.h"
#include "impfe.h"
#include "version.h"

static char OutBuf[2048];

/* callbacks for form ImpFeMain */

/*
 * MainInputBox_cb - called when the user presses Enter in the input
 *                   line; sends the line on to the server
 */
void MainInputBox_cb(GtkWidget *w, /*@unused@*/gpointer data)
{
	/* Copy out what they typed */
	g_strlcpy(OutBuf, ui_entry_get(fd_ImpFeMain->MainInputBox), sizeof(OutBuf));
	/* Clear the input box */
	ui_entry_set(fd_ImpFeMain->MainInputBox, "");
	/* Send it on to the server */
	if (GameSocket != -1)
	{
		(void) write_data(GameSocket, OutBuf, (int) strlen(OutBuf));
		(void) write_data(GameSocket, "\n", 1);
	}
	ui_browser_append(fd_ImpFeMain->MainBrowser, OutBuf);
	ui_browser_append(fd_ImpFeMain->MainBrowser, "\n");
}

void MainHelpMenu_cb(/*@unused@*/GtkWidget *menu, int item)
{
	switch(item)
	{
	    case MAIN_HELP_MENU_ABOUT:
			ui_set_text(fd_ImpFeAbout->ImpVer, "ImpFE v" FE_VER "." FE_REV "." FE_PATCH);
			ui_set_text(fd_ImpFeAbout->MainAboutText,
								"(C) 1997-2026 Marisa Giancarla\n\n"
								"For more information contact <fstltna@yahoo.com>\n"
								"or go to the Imperium home page at:\n"
								"<https://EmpireDirectory.net>");
			ui_window_show(fd_ImpFeAbout->ImpFeAbout);
			break;
	    case MAIN_HELP_MENU_INDEX:
			cb_help_button(CHAPTER_START);
			break;
	    default:
			break;
	}
}

void MainFileMenu_cb(/*@unused@*/GtkWidget *menu, int item)
{
	switch(item)
	{
	    case MAIN_FILE_MENU_OPEN:
			handleOpenCfg();
			break;
	    case MAIN_FILE_MENU_SAVE:
			handleSaveCfg();
			break;
	    case MAIN_FILE_MENU_CLOSE:
			handleSaveCfg();
			handleCloseCfg();
			break;
	    case MAIN_FILE_MENU_NEW:
			handleNewCfg();
			break;
	    case MAIN_FILE_MENU_EXIT:
			impfeQuit();
			break;
	    default:
			break;
	}
}

void MainServerMenu_cb(GtkWidget *menu, int item)
{
	switch(item)
	{
	    case MAIN_SERVER_MENU_OPEN:
			if (cmd_connect(FALSE))
			{
				ui_menu_enable(menu, MAIN_SERVER_MENU_OPEN, FALSE);
				ui_menu_enable(menu, MAIN_SERVER_MENU_CLOSE, TRUE);
				ui_set_text(fd_ImpFeMain->StatusBox, "CONNECTED");
			}
			break;
	    case MAIN_SERVER_MENU_CLOSE:
			if (cmd_connect(TRUE))
			{
				ui_menu_enable(menu, MAIN_SERVER_MENU_CLOSE, FALSE);
				ui_menu_enable(menu, MAIN_SERVER_MENU_OPEN, TRUE);
				ui_set_text(fd_ImpFeMain->StatusBox, "Offline");
			}
			break;
	    default:
			break;
	}
}

void MainViewMenu_cb(/*@unused@*/GtkWidget *menu, int item)
{
	switch(item)
	{
	    case MAIN_VIEW_MENU_MAP:
			buildMapDisp();
			ui_window_show(fd_MapForm->MapForm);
			break;
		case MAIN_VIEW_MENU_SHIP:
			buildShCensus();
			ui_window_show(fd_ShipCensusForm->ShipCensusForm);
			break;
		case MAIN_VIEW_MENU_PLANET:
			buildPlCensus();
			ui_window_show(fd_PlanetCensusForm->PlanetCensusForm);
			break;
		case MAIN_VIEW_MENU_CMDS:
			buildCmdList();
			ui_window_show(fd_CommandForm->CommandForm);
			break;
	    case MAIN_VIEW_MENU_CLEAR:
			ui_browser_clear(fd_ImpFeMain->MainBrowser);
			break;
	    default:
			break;
	}
}

void ServCfgCancelBtn_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
	ui_window_hide(fd_ServCfgForm->ServCfgForm);
}

void MainConfigMenu_cb(/*@unused@*/GtkWidget *menu, int item)
{
	switch(item)
	{
	    case MAIN_CONFIG_MENU_HOST:
			/* Load in current values */
			ui_entry_set(fd_ServCfgForm->ServName, game_host);
			ui_entry_set(fd_ServCfgForm->ServPort, game_port);
			ui_entry_set(fd_ServCfgForm->PlayerName, game_player);
			ui_entry_set(fd_ServCfgForm->PlayerPswd, game_player_pswd);
			ui_entry_set(fd_ServCfgForm->GameDesc, game_desc);
			/* The window is modal, so the main window waits for it */
			ui_window_show(fd_ServCfgForm->ServCfgForm);
			break;
	    default:
			break;
	}
}

/* callbacks for form ImpFeAbout */
void AboutOk_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
	ui_window_hide(fd_ImpFeAbout->ImpFeAbout);
}

/* callbacks for form MapForm */

/*
 * showPlanetDetail - pulls up a planet in the detailed planet window
 */
static void showPlanetDetail(ULONG plNum)
{
	LastPlanet=plNum;
	buildPlDetDisp();
	ui_window_show(fd_PlanetDetailForm->PlanetDetailForm);
}

/* This lets the user click on a sector to get additional info */
gboolean MB_cb(/*@unused@*/GtkWidget *w, GdkEventButton *ev, gpointer data)
{
	ULONG tmp;
	USHORT row, col;
	int code;

	/* data holds the code for the tile they clicked */
	code = GPOINTER_TO_INT(data);
	if ((code == 0) || (ev->type != GDK_BUTTON_PRESS) || (BitMap == NULL))
	{
		return(FALSE);
	}
	code--;
	row = (USHORT) code / 10;
	col = (USHORT) code % 10;
#ifdef DEBUG
printf("Row = %u, col = %u\n", row, col);
#endif
	/* See which mouse button they clicked */
	switch(ev->button)
	{
		case UI_LEFT_MOUSE:
			break;
		case UI_MIDDLE_MOUSE:
#ifdef DEBUG
printf("Middle pressed\n");
#endif
			tmp = getSectPlanet(row + LastY, col + LastX);
			if (tmp != IT_NONE)
			{
#ifdef DEBUG
printf("Selecting planet %u\n", tmp);
#endif
				showPlanetDetail(tmp);
			}
			break;
		case UI_RIGHT_MOUSE:
			break;
		default:
			break;
	}
	return(TRUE);
}

void JmpBtn_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
	int row, col;

	row=atoi(ui_entry_get(fd_MapForm->JmRow));
	col=atoi(ui_entry_get(fd_MapForm->JmCol));
    if (row > ((game_sizey * 10) - 7))
	{
		row = (game_sizey * 10) - 7;
	}
	else if (row < 0)
	{
		row = 0;
	}
    if (col > (((game_sizex - 1) * 10) - 1))
	{
		col = (((game_sizex - 1) * 10) - 1);
	}
	else if (col < 0)
	{
		col = 0;
	}
	/* If coords are the same, don't bother updating map */
	if (((int) LastX != col) || ((int) LastY != row))
	{
	    LastX=(USHORT) col;
	    LastY=(USHORT) row;
	    buildMapDisp();
	}
}

void DirBtn_cb(/*@unused@*/GtkWidget *w, gpointer data)
{
    int mult=1;
    long code = (long) GPOINTER_TO_INT(data);

    if (code > 9)
    {
	mult=10;
	code /= 10;
    }
    switch(code)
    {
	case 1:
	    scrollFeMap(-1 * mult, 1 * mult);
	    break;
	case 2:
	    scrollFeMap(0, 1 * mult);
	    break;
	case 3:
	    scrollFeMap(1 * mult, 1 * mult);
	    break;
	case 4:
	    scrollFeMap(-1 * mult, 0);
	    break;
	case 5:
	    LastX=HomeCol;
	    if (LastX > 5)
	    {
		LastX -= 5;
	    }
	    LastY=HomeRow;
	    if (LastY > 3)
	    {
		LastY -= 3;
	    }
	    buildMapDisp();
	    break;
	case 6:
	    scrollFeMap(1 * mult, 0);
	    break;
	case 7:
	    scrollFeMap(-1 * mult, -1 * mult);
	    break;
	case 8:
	    scrollFeMap(0, -1 * mult);
	    break;
	case 9:
	    scrollFeMap(1 * mult, -1 * mult);
	    break;
	default:
	    fprintf(stderr, "*** Unknown directional button value %ld\n",
		code);
	    break;
    }
}

void MapOk_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
	ui_window_hide(fd_MapForm->MapForm);
}

/* callbacks for form TasksForm */
void TasksUpdtBtn_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
  /* fill-in code for callback */
}

void TasksKillBtn_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
  /* fill-in code for callback */
}

void TasksDoneBtn_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
	ui_window_hide(fd_TasksForm->TasksForm);
}

/* callbacks for form ServCfgForm */
void ServCfgOkBtn_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
	g_strlcpy(game_host, ui_entry_get(fd_ServCfgForm->ServName), sizeof(game_host));
	g_strlcpy(game_port, ui_entry_get(fd_ServCfgForm->ServPort), sizeof(game_port));
	g_strlcpy(game_player, ui_entry_get(fd_ServCfgForm->PlayerName), sizeof(game_player));
	g_strlcpy(game_player_pswd, ui_entry_get(fd_ServCfgForm->PlayerPswd), sizeof(game_player_pswd));
	g_strlcpy(game_desc, ui_entry_get(fd_ServCfgForm->GameDesc), sizeof(game_desc));
	ui_window_hide(fd_ServCfgForm->ServCfgForm);
}

/*
 * addToInput - adds the string to the input box, if NL is TRUE,
 *              it is the end of line and the line should go to the
 *              server
 */
void addToInput(const char *str, BOOL NL)
{
	char work[1024];

	g_strlcpy(work, ui_entry_get(fd_ImpFeMain->MainInputBox), sizeof(work));
	g_strlcat(work, str, sizeof(work));
	work[256]='\0';
	ui_entry_set(fd_ImpFeMain->MainInputBox, work);
	gtk_editable_set_position(GTK_EDITABLE(fd_ImpFeMain->MainInputBox), -1);
	if (NL)
	{
		MainInputBox_cb(fd_ImpFeMain->MainInputBox, GINT_TO_POINTER(1));
	}
}

/*
 * censusNumber - pulls the ship/planet/item number from the start of a
 *                browser line, returning it in lBuff with a leading space
 */
static ULONG censusNumber(GtkWidget *browser, int line, char *lBuff, size_t len)
{
	ULONG num;

	g_strlcpy(lBuff, ui_browser_line(browser, line), len);
	if (len > 9)
	{
		lBuff[9]='\0';
	}
	num=(ULONG) atol(lBuff);
	/* Put it back in the buffer to chop off leading space */
	g_snprintf(lBuff, len, " %lu", num);
	return(num);
}

/* callbacks for form ShipCensusForm */
/* ShBrowse_cb - called when the user clicks on one of the ships in the
 *               census window
 */
void ShBrowse_cb(GtkWidget *browser, int line, int button)
{
	ULONG shNum;
	char lBuff[85];

	/* Get the ship number from the line they clicked on */
	shNum = censusNumber(browser, line, lBuff, sizeof(lBuff));

	switch(button)
	{
		case UI_LEFT_MOUSE:
			/* Add ship number to line */
			addToInput(lBuff, FALSE);
			break;
		case UI_MIDDLE_MOUSE:
			/* Add ship number to line, and send it */
			addToInput(lBuff, TRUE);
			break;
		case UI_RIGHT_MOUSE:
			/* Pull up ship in detailed ship display window */
			LastShip=shNum;
			buildShDetDisp();
			ui_window_show(fd_ShipDetailForm->ShipDetailForm);
			break;
		default:
			break;
	}
}

void ShCenUpdate_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
	buildShCensus();
}

void ShCenOk_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
	ui_window_hide(fd_ShipCensusForm->ShipCensusForm);
}

/* callbacks for form PlanetCensusForm */
/* PlBrowse_cb - called when the user clicks on one of the planets in the
 *               census window
 */
void PlBrowse_cb(GtkWidget *browser, int line, int button)
{
	ULONG plNum;
	char lBuff[120];

	/* Get the planet number from the line they clicked on */
	plNum = censusNumber(browser, line, lBuff, sizeof(lBuff));

	switch(button)
	{
		case UI_LEFT_MOUSE:
			/* Add planet number to line */
			addToInput(lBuff, FALSE);
			break;
		case UI_MIDDLE_MOUSE:
			/* Add planet number to line, and send it */
			addToInput(lBuff, TRUE);
			break;
		case UI_RIGHT_MOUSE:
			/* Pull up planet in detailed planet display window */
			showPlanetDetail(plNum);
			break;
		default:
			break;
	}
}

void PlCenUpdate_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
	buildPlCensus();
}

void PlCenOk_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
	ui_window_hide(fd_PlanetCensusForm->PlanetCensusForm);
}

/* callbacks for form ShipDetailForm */
void ShDetUpdBtn_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
	buildShDetDisp();
}

void ShDetStatBtn_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
	char workBuf[256];

	sprintf(workBuf, "ship status %lu\n", LastShip);
	(void) addCmd(workBuf, QUE_IMMEDIATE);
}

void ShDetCargoBtn_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
	char workBuf[256];

	sprintf(workBuf, "ship cargo %lu\n", LastShip);
	(void) addCmd(workBuf, QUE_IMMEDIATE);
}

void ShDetBigBtn_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
	char workBuf[256];

	sprintf(workBuf, "ship big %lu\n", LastShip);
	(void) addCmd(workBuf, QUE_IMMEDIATE);
}

/*
 * ShDetActionBtn_cb - called when the user presses the install,
 *                     uninstall, or unload action buttons
 */
void ShDetActionBtn_cb(/*@unused@*/GtkWidget *w, gpointer data)
{
	int line;
	ULONG itNum;
	char lBuff[85];

	/* Get the line # they selected */
	line = ui_browser_selected(fd_ShipDetailForm->ShDetItem);
	/* Make sure they have an item selected */
	if (line == 0)
	{
		/* Nope */
		return;
	}
	/* Get the item number */
	itNum = censusNumber(fd_ShipDetailForm->ShDetItem, line, lBuff,
		sizeof(lBuff));
	switch (GPOINTER_TO_INT(data))
	{
		case 1:
			sprintf(lBuff, "install %lu %lu\n", LastShip, itNum);
			(void) addCmd(lBuff, QUE_IMMEDIATE);
			break;
		case 2:
			sprintf(lBuff, "remove %lu %lu\n", LastShip, itNum);
			(void) addCmd(lBuff, QUE_IMMEDIATE);
			break;
		case 3:
			sprintf(lBuff, "unload big %lu %lu\n", LastShip, itNum);
			(void) addCmd(lBuff, QUE_IMMEDIATE);
			break;
		default:
			break;
	}
}

/*
 * PlDetActionBtn_cb - called when the user presses the refurbish
 *                     action button
 */
void PlDetActionBtn_cb(/*@unused@*/GtkWidget *w, gpointer data)
{
	int line;
	ULONG itNum;
	char lBuff[85];

	/* Get the line # they selected */
	line = ui_browser_selected(fd_PlanetDetailForm->PlDetItem);
	/* Make sure they have an item selected */
	if (line == 0)
	{
		/* Nope */
		return;
	}
	/* Get the item number */
	itNum = censusNumber(fd_PlanetDetailForm->PlDetItem, line, lBuff,
		sizeof(lBuff));
	switch (GPOINTER_TO_INT(data))
	{
		case 1:
			sprintf(lBuff, "refurbish item %lu\n", itNum);
			(void) addCmd(lBuff, QUE_IMMEDIATE);
			break;
		default:
			break;
	}
}

void ShDetOk_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
	ui_window_hide(fd_ShipDetailForm->ShipDetailForm);
}

/* callbacks for form PlanetDetailForm */
void PlDetUpdBtn_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
	buildPlDetDisp();
}

void PlDetDumpBtn_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
	char workBuf[256];

	sprintf(workBuf, "dump %lu\n", LastPlanet);
	(void) addCmd(workBuf, QUE_IMMEDIATE);
}

void PlDetProdBtn_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
	char workBuf[256];

	sprintf(workBuf, "census product %lu\n", LastPlanet);
	(void) addCmd(workBuf, QUE_IMMEDIATE);
}

void PlDetGeoBtn_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
	char workBuf[256];

	sprintf(workBuf, "census geolog %lu\n", LastPlanet);
	(void) addCmd(workBuf, QUE_IMMEDIATE);
}

void PlDetPopBtn_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
	char workBuf[256];

	sprintf(workBuf, "census population %lu\n", LastPlanet);
	(void) addCmd(workBuf, QUE_IMMEDIATE);
}

void PlDetBigBtn_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
	char workBuf[256];

	sprintf(workBuf, "census big %lu\n", LastPlanet);
	(void) addCmd(workBuf, QUE_IMMEDIATE);
}

/*
 * NameUpdate_cb - updates the database with the name requested
 */
void NameUpdate_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
	char workBuf[256], plName[PL_NAME_LEN];
	FePlanet_t tmpPlanet;

	/* Read the planet in */
	(void) readPlanet(&tmpPlanet, LastPlanet);
	/* Copy out the value of the field */
	g_strlcpy(plName, ui_entry_get(fd_PlanetDetailForm->Name), sizeof(plName));
	/* Put the name into the planet struct */
	strcpy(tmpPlanet.name, plName);
	/* Write it back to the database */
	(void) writePlanet(&tmpPlanet, LastPlanet);
	/* If we don't own the planet, no point in sending command to host */
	if ((USHORT) tmpPlanet.owner != player_num)
	{
		return;
	}
	/* Add this command to the queue */
	sprintf(workBuf, "name %lup\n%s\n", LastPlanet, plName);
	(void) addCmd(workBuf, QUE_IMMEDIATE);
}

/*
 * CheckpointUpdate_cb - updates the database with the checkpoint requested
 */
void CheckpointUpdate_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
	char workBuf[256], plCode[PL_PSWD_LEN];
	FePlanet_t tmpPlanet;

	/* Read the planet in */
	(void) readPlanet(&tmpPlanet, LastPlanet);
	/* Copy out the value of the field */
	g_strlcpy(plCode, ui_entry_get(fd_PlanetDetailForm->Checkpoint), sizeof(plCode));
	/* Put the name into the planet struct */
	strcpy(tmpPlanet.chkpoint, plCode);
	/* Write it back to the database */
	(void) writePlanet(&tmpPlanet, LastPlanet);
	/* If we don't own the planet, no point in sending command to host */
	if ((USHORT) tmpPlanet.owner != player_num)
	{
		return;
	}
	return; /* Seems to be broken for now XXX */
	/* Add these command to the queue */
	/* XXX Note this will break if they aren't done sequentially! */
	/*@notreached@*/sprintf(workBuf, "checkpoint %lu\n%s\n%s\n", LastPlanet, game_player_pswd,
			plCode);
	/*@notreached@*/(void) addCmd(workBuf, QUE_IMMEDIATE);
}

void PlDetOk_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
	ui_window_hide(fd_PlanetDetailForm->PlanetDetailForm);
}

/* Callbacks for form CommandForm */
void CmdsDoneBtn_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
	ui_window_hide(fd_CommandForm->CommandForm);
}

void CmdsPauseBtn_cb(GtkWidget *w, /*@unused@*/gpointer data)
{
	if (StopQueue)
	{
		StopQueue=FALSE;
		ui_set_level(fd_ImpFeMain->QueueDisp, UI_LEVEL_NORMAL);
		gtk_button_set_label(GTK_BUTTON(w), "_Pause");
	}
	else
	{
		StopQueue=TRUE;
		ui_set_level(fd_ImpFeMain->QueueDisp, UI_LEVEL_CRIT);
		gtk_button_set_label(GTK_BUTTON(w), "_Resume");
	}
}

/*
 * selectedCmd - returns the queue key of the selected command, or -1
 */
static int selectedCmd(void)
{
	int line;

	/* Try and get selected line */
	line=ui_browser_selected(fd_CommandForm->CmdsBrowser);
	/* Make sure they have a line selected */
	if (line == 0)
	{
		return(-1);
	}
	return(atoi(ui_browser_line(fd_CommandForm->CmdsBrowser, line)));
}

void CmdsKillBtn_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
	int tmp;

	if ((tmp = selectedCmd()) < 0)
	{
		return;
	}
	(void) cancelCmd(tmp);
}

/*
 * CmdsBrowser_cb - called when they click on a line in the commands
 *                  browser
 */
void CmdsBrowser_cb(/*@unused@*/GtkWidget *browser, /*@unused@*/int line,
	/*@unused@*/int button)
{
	int tmp, prior, len, pos;
	char cmnd[259];

	if ((tmp = selectedCmd()) < 0)
	{
		return;
	}
	g_strlcpy(cmnd, getCmd(tmp), sizeof(cmnd));
	prior = getPrior(tmp);
	len=(int) strlen(cmnd);
	/* Convert the NL characters */
	for (pos=0; pos < len; pos++)
	{
		if (cmnd[pos] == '\n')
		{
			cmnd[pos]='~';
		}
	}
	ui_entry_set(fd_CommandForm->CmdsCmd, cmnd);
	ui_choice_set(fd_CommandForm->CmdsPriorBtn, prior);
}

void CmdsAddBtn_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
	int len, pos;
	char cmnd[259];

	g_strlcpy(cmnd, ui_entry_get(fd_CommandForm->CmdsCmd), sizeof(cmnd));
	len=(int) strlen(cmnd);
	/* Convert the NL characters */
	for (pos=0; pos < len; pos++)
	{
		if (cmnd[pos] == '~')
		{
			cmnd[pos]='\n';
		}
	}
	(void) addCmd(cmnd, ui_choice_get(fd_CommandForm->CmdsPriorBtn));
}

void CmdsModBtn_cb(/*@unused@*/GtkWidget *w, /*@unused@*/gpointer data)
{
	int tmp;

	if ((tmp = selectedCmd()) < 0)
	{
		return;
	}
	(void) modCmd(tmp, ui_entry_get(fd_CommandForm->CmdsCmd),
		   ui_choice_get(fd_CommandForm->CmdsPriorBtn));
}
