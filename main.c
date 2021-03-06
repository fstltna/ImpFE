#ifndef lint
static char rcsid[] = "$Id: main.c,v 1.3 2000/05/24 21:51:39 marisa Exp $";
#endif
/*
 * $Id: main.c,v 1.3 2000/05/24 21:51:39 marisa Exp $
 * (C) 1997, Marisa Giancarla
 *
 * This file was originally generated by the XForms GUI builder, fdesign
 *
 * $COPYRIGHT
 *
 * $Log: main.c,v $
 * Revision 1.3  2000/05/24 21:51:39  marisa
 * Fix bugs with TRUE/FALSE testing
 *
 * Revision 1.2  2000/05/24 20:40:39  marisa
 * Disable some menu options until config file created or opened.
 *
 * Revision 1.1.1.1  2000/05/17 19:22:13  marisa
 * First CVS checkin
 *
 * Revision 1.1.1.10  1997/05/03 09:42:36  marisag
 * patch1: Daily checkin
 *
 * Revision 1.1.1.9  1997/04/27 23:23:33  marisag
 * patch1: Daily checkin
 *
 * Revision 1.1.1.8  1997/04/25 06:39:13  marisag
 * patch1: Code for the planet & ship detail forms
 *
 * Revision 1.1.1.7  1997/04/24 04:46:11  marisag
 * patch1: Many changes
 *
 * Revision 1.1.1.6  1997/04/16 03:07:18  marisag
 * patch1: Daily checkin
 *
 * Revision 1.1.1.5  1997/04/14 16:04:55  marisag
 * patch1: Daily checkin
 *
 * Revision 1.1.1.4  1997/04/14 02:51:42  marisag
 * patch1: New functions
 *
 * Revision 1.1.1.3  1997/04/12 16:50:43  marisag
 * patch1: Many changes
 *
 * Revision 1.1.1.2  1997/04/10 06:30:19  marisag
 * patch1: Make use of the new config file stuff
 *
 * Revision 1.1.1.1  1997/04/09 03:21:54  marisag
 * patch1: New changes...
 *
 * Revision 1.1  1997/04/08 07:07:40  marisag
 * Initial revision
 *
 */

#define BUILDING_MAIN
#include "config.h"
#include "forms.h"
#include "ImpFeGui.h"
#include "gui.h"
#include "impfe.h"
#include "smimplogo.xpm"
#include "smimplogo.xbm"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/* #include <ao/ao.h>*/	/* audio library */
/* #include <mpg123.h> */	/* mp3 decoder library */

/* our pid */
    pid_t pid;

/*
 * Simple routine to display a string in the main text area
 */
void fe_puts(const char *mesg)
{
	fl_addto_browser_chars(fd_ImpFeMain->MainBrowser, mesg);
}

/*
 * initImpFeMain_menu - initialize the menus on the ImpFeMain form
 */
void initImpFeMain_menu(FD_ImpFeMain *MainForm)
{
    /* File Menu */
    fl_set_menu(MainForm->MainFileMenu, MAIN_FILE_MENU);
    fl_set_menu_item_mode(MainForm->MainFileMenu, MAIN_FILE_MENU_CLOSE,
						  FL_PUP_GREY);
    fl_set_menu_item_mode(MainForm->MainFileMenu, MAIN_FILE_MENU_SAVE,
						  FL_PUP_GREY);

    /* Server Menu */
    fl_set_menu(MainForm->MainServerMenu, MAIN_SERVER_MENU);
    fl_set_menu_item_mode(MainForm->MainServerMenu, MAIN_SERVER_MENU_OPEN,
						  FL_PUP_GREY);
    fl_set_menu_item_mode(MainForm->MainServerMenu, MAIN_SERVER_MENU_CLOSE,
						  FL_PUP_GREY);

    /* View Menu */
    fl_set_menu(MainForm->MainViewMenu, MAIN_VIEW_MENU);
    fl_set_menu_item_mode(MainForm->MainViewMenu, MAIN_VIEW_MENU_MAP,
						  FL_PUP_GREY);
    fl_set_menu_item_mode(MainForm->MainViewMenu, MAIN_VIEW_MENU_PLANET,
						  FL_PUP_GREY);
    fl_set_menu_item_mode(MainForm->MainViewMenu, MAIN_VIEW_MENU_SHIP,
						  FL_PUP_GREY);
    fl_set_menu_item_mode(MainForm->MainViewMenu, MAIN_VIEW_MENU_SCRIPTS,
						  FL_PUP_GREY);
    fl_set_menu_item_mode(MainForm->MainViewMenu, MAIN_VIEW_MENU_CMDS,
						  FL_PUP_GREY);

    /* Configure Menu */
    fl_set_menu(MainForm->MainConfigMenu, MAIN_CONFIG_MENU);
    fl_set_menu_item_mode(MainForm->MainConfigMenu, MAIN_CONFIG_MENU_HOST,
						  FL_PUP_GREY);
    fl_set_menu_item_mode(MainForm->MainConfigMenu, MAIN_CONFIG_MENU_SETTINGS,
						  FL_PUP_GREY);

    /* Help Menu */
    fl_set_menu(MainForm->MainHelpMenu, MAIN_HELP_MENU);

    /* Set up the input boxes */
    fl_set_input_return(MainForm->MainInputBox, FL_RETURN_END);
    fl_set_input_hscrollbar(MainForm->MainInputBox, FL_OFF);
    fl_set_input_vscrollbar(MainForm->MainInputBox, FL_OFF);
    fl_set_input_maxchars(MainForm->MainInputBox, 257);
    fl_set_input_maxchars(fd_CommandForm->CmdsCmd, 256);

    /* Set up the browsers */
    /* Main browser */
    fl_set_browser_fontsize(fd_ImpFeMain->MainBrowser, FL_NORMAL_FONT);
    fl_set_browser_fontstyle(fd_ImpFeMain->MainBrowser, FL_FIXED_STYLE);
    /* Help browser */
    fl_set_browser_fontsize(fd_HelpForm->Help_Browser, FL_NORMAL_FONT);
    /* Ship census browser */
    fl_set_browser_fontsize(fd_ShipCensusForm->ShBrowse, FL_NORMAL_FONT);
    fl_set_browser_fontstyle(fd_ShipCensusForm->ShBrowse, FL_FIXED_STYLE);
    /* Ship detail browser */
    fl_set_browser_fontsize(fd_ShipDetailForm->ShDetItem, FL_NORMAL_FONT);
    fl_set_browser_fontstyle(fd_ShipDetailForm->ShDetItem, FL_FIXED_STYLE);
    /* Planet census browser */
    fl_set_browser_fontsize(fd_PlanetCensusForm->PlBrowse, FL_NORMAL_FONT);
    fl_set_browser_fontstyle(fd_PlanetCensusForm->PlBrowse, FL_FIXED_STYLE);
    /* Planet detail browser */
    fl_set_browser_fontsize(fd_PlanetDetailForm->PlDetItem, FL_NORMAL_FONT);
    fl_set_browser_fontstyle(fd_PlanetDetailForm->PlDetItem, FL_FIXED_STYLE);
    fl_set_browser_fontsize(fd_PlanetDetailForm->PlProdBrowse, FL_NORMAL_FONT);
    fl_set_browser_fontstyle(fd_PlanetDetailForm->PlProdBrowse, FL_FIXED_STYLE);
    /* Commands browser */
    fl_set_browser_fontsize(fd_CommandForm->CmdsBrowser, FL_NORMAL_FONT);
    fl_set_browser_fontstyle(fd_CommandForm->CmdsBrowser, FL_FIXED_STYLE);

    /* Choice lists */
    fl_addto_choice(fd_CommandForm->CmdsPriorBtn, "LOW");
    fl_addto_choice(fd_CommandForm->CmdsPriorBtn, "MEDIUM");
    fl_addto_choice(fd_CommandForm->CmdsPriorBtn, "HIGH");
    fl_addto_choice(fd_CommandForm->CmdsPriorBtn, "IMMED");
    fl_set_choice(fd_CommandForm->CmdsPriorBtn, 4);
}

void initImpFeServCfg(void)
{
	/* Set the field maximums */
	fl_set_input_maxchars(fd_ServCfgForm->ServName, MAX_HOST_LEN -1);
	fl_set_input_maxchars(fd_ServCfgForm->ServPort, 6);
	fl_set_input_maxchars(fd_ServCfgForm->PlayerName, MAX_PLAYER_LEN -1);
	fl_set_input_maxchars(fd_ServCfgForm->PlayerPswd, MAX_PLAYER_LEN -1);
	fl_set_input_maxchars(fd_ServCfgForm->GameDesc, 80);
}

/*
 * close_main_form - generic call-back for closing a form
 */
int close_main_form(FL_FORM *form, void *unused)
{
	(void) cmd_connect(TRUE);
	handleSaveCfg();
	handleCloseCfg();
	freeCmdQueues();
	impfeCleanup();
	fl_finish();
	exit(0);
}

void setDefaults(void)
{
    strcpy(&game_host[0], "empiredirectory.net");
    sprintf(game_port, "%d", DEF_PORT);
    strcpy(&game_player[0], "");
    strcpy(&game_player_pswd[0], "");
    strcpy(&game_desc[0], "none");
    game_sizex = 10;
    game_sizey = 10;
    next_ship=0;
    next_planet=0;
    next_item=0;
    player_num=0;
    home_planet=0;
    AtMainPrompt=FALSE;
    BitMap=NULL;
    LastX=0;
    LastY=0;
    HomeRow=0;
    HomeCol=0;
}

int file_exists(const char * filename)
{
	FILE *file;
    if (file = fopen(filename, "r"))
    {
        fclose(file);
        return(1);
    }
    return(0);
}

int fe_idle_callback(XEvent *xev, void *user_data)
{
	return 0;
}

/*
 * playMusic - play the mp3 file(s)
 */

void playMusic()
{
#ifdef WANTMUSIC
    mpg123_handle *mh;
    unsigned char *buffer;
    size_t buffer_size;
    size_t done;
    int err;
#define BITS 8

    int driver;
    ao_device *dev;

    ao_sample_format format;
    int channels, encoding;
    long rate;

    /* initializations */
    ao_initialize();
    driver = ao_default_driver_id();
    mpg123_init();
    mh = mpg123_new(NULL, &err);
    buffer_size = mpg123_outblock(mh);
    buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));

    /* open the file and get the decoding format */
    mpg123_open(mh, "/bin/impfe.music.mp3");
    mpg123_getformat(mh, &rate, &channels, &encoding);

    /* set the output format and open the output device */
    format.bits = mpg123_encsize(encoding) * BITS;
    format.rate = rate;
    format.channels = channels;
    format.byte_format = AO_FMT_NATIVE;
    format.matrix = 0;
    dev = ao_open_live(driver, &format, NULL);

    /* decode and play */
    while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK)
        ao_play(dev, buffer, done);

    /* clean up */
    free(buffer);
    ao_close(dev);
    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();
    ao_shutdown();
#endif
}

/*
 * Main entry point
 */

int main(int argc, char *argv[])
{
	int item;
	Pixmap icon_pixmap; /* Icon for closed windows */
	pid = fork();
	if (pid == 0)
	{
		/* child process */
		playMusic();
		exit(0);
	}
	else if (pid > 0)
	{
		/* parent process */
		/*printf("Music fork passed\n");*/
	}
	else
	{
		printf("Music fork failed\n");
	}

	/* Set fonts to 75dpi */
	fl_set_font_name(FL_NORMAL_STYLE,            "-*-helvetica-medium-r-*-*-*-?-*-75-*-*-*-*");
	fl_set_font_name(FL_BOLD_STYLE,            "-*-helvetica-bold-r-*-*-*-?-*-75-p-*-*-*");
	fl_set_font_name(FL_ITALIC_STYLE,            "-*-helvetica-medium-o-*-*-*-?-*-75-p-*-*-*");
	fl_set_font_name(FL_BOLDITALIC_STYLE,        "-*-helvetica-bold-o-no*-*-*-?-*-75-p-*-*-*");
	fl_set_font_name(FL_FIXED_STYLE,            "-adobe-courier-medium-r-*-*-*-?-*-75-m-*-*-*");
	fl_set_font_name(FL_FIXEDBOLD_STYLE,        "-adobe-courier-bold-r-*-*-*-?-*-75-m-*-*-*");
	fl_set_font_name(FL_FIXEDITALIC_STYLE,        "-adobe-courier-medium-o-*-*-*-?-*-75-m-*-*-*");
	fl_set_font_name(FL_FIXEDBOLDITALIC_STYLE,    "-adobe-courier-bold-o-*-*-*-?-*-75-m-*-*-*");
	fl_set_font_name(FL_TIMES_STYLE,            "-*-times-medium-r-*-*-*-?-*-75-p-*-*-*");
	fl_set_font_name(FL_TIMESBOLD_STYLE,        "-*-times-bold-r-*-*-*-?-*-75-p-*-*-*");
	fl_set_font_name(FL_TIMESITALIC_STYLE,        "-*-times-medium-i-*-*-*-?-*-75-p-*-*-*");
	fl_set_font_name(FL_TIMESBOLDITALIC_STYLE,    "-*-times-bold-i-*-*-*-?-*-75-p-*-*-*");
	fl_set_font_name(FL_MISC_STYLE,            "-*-charter-medium-r-*-*-*-?-*-75-*-*-*-*");
	fl_set_font_name(FL_MISCBOLD_STYLE,        "-*-charter-bold-r-*-*-*-?-*-75-*-*-*-*");
	fl_set_font_name(FL_MISCITALIC_STYLE,        "-*-charter-medium-i-*-*-*-?-*-75-*-*-*-*");
	fl_set_font_name(FL_SYMBOL_STYLE,            "-*-symbol-medium-r-*-*-*-?-*-75-*-*-*-*");

	/*
	 * Set up the Xforms interface
	 */
	fl_initialize(&argc, argv, 0, 0, 0);

    /* Attatch icon */
    icon_pixmap = fl_create_from_bitmapdata(fl_default_win(),
	smimplogo_bits, smimplogo_width, smimplogo_height);

	/*
	 * Now create the various object instances
	 */
	fd_ImpFeMain = create_form_ImpFeMain();
	fl_set_form_icon(fd_ImpFeMain->ImpFeMain, icon_pixmap, 0);
	fd_ImpFeAbout = create_form_ImpFeAbout();
	fl_set_form_icon(fd_ImpFeAbout->ImpFeAbout, icon_pixmap, 0);
	fd_MapForm = create_form_MapForm();
	fl_set_form_icon(fd_MapForm->MapForm, icon_pixmap, 0);
	fd_TasksForm = create_form_TasksForm();
	fl_set_form_icon(fd_TasksForm->TasksForm, icon_pixmap, 0);
	fd_ServCfgForm = create_form_ServCfgForm();
	fl_set_form_icon(fd_ServCfgForm->ServCfgForm, icon_pixmap, 0);
	fd_ShipCensusForm = create_form_ShipCensusForm();
	fl_set_form_icon(fd_ShipCensusForm->ShipCensusForm, icon_pixmap, 0);
	fd_PlanetCensusForm = create_form_PlanetCensusForm();
	fl_set_form_icon(fd_PlanetCensusForm->PlanetCensusForm, icon_pixmap, 0);
	fd_ShipDetailForm = create_form_ShipDetailForm();
	fl_set_form_icon(fd_ShipDetailForm->ShipDetailForm, icon_pixmap, 0);
	fd_PlanetDetailForm = create_form_PlanetDetailForm();
	fl_set_form_icon(fd_PlanetDetailForm->PlanetDetailForm, icon_pixmap, 0);
	fd_PowerRptForm = create_form_PowerRptForm();
	fd_MinerCensusForm = create_form_MinerCensusForm();
	fd_DebugOutputForm = create_form_DebugOutputForm();
	fd_LocalEditorForm = create_form_LocalEditorForm();
	fd_HelpForm = create_form_HelpForm();
	fl_set_form_icon(fd_HelpForm->HelpForm, icon_pixmap, 0);
	fd_CommandForm = create_form_CommandForm();
	fl_set_form_icon(fd_CommandForm->CommandForm, icon_pixmap, 0);
	fd_Dummy3Form = create_form_Dummy3Form();
	fd_Dummy4Form = create_form_Dummy4Form();
	fd_Dummy5Form = create_form_Dummy5Form();

    /* Set defaults */
    setDefaults();
    initCmdQueues();
    DirtyData = FALSE;
    /* Attatch icon */
    icon_pixmap = fl_create_from_bitmapdata(fl_default_win(),
	smimplogo_bits, smimplogo_width, smimplogo_height);

	/* fill-in form initialization code */
	fl_set_form_atclose(fd_ImpFeMain->ImpFeMain, close_main_form,
						&item);
	initImpFeMain_menu(fd_ImpFeMain);
	initImpFeServCfg();

	/* Set the pixmaps */
	fl_free_pixmap_pixmap(fd_ImpFeAbout->ImpLogo);
	fl_set_pixmap_data(fd_ImpFeAbout->ImpLogo, smimplogo);

	/* show the first form */
	fl_set_form_title(fd_ImpFeMain->ImpFeMain, "ImpFE");
	fl_show_form(fd_ImpFeMain->ImpFeMain,FL_PLACE_CENTER,FL_FULLBORDER,
				 "ImpFeMain");
	fl_set_form_title(fd_ImpFeMain->ImpFeMain, "ImpFE");

	(void) fl_set_idle_callback(fe_idle_callback, NULL);

	/*
	 * Start the main processing loop - this never returns!
	 */
	fl_do_forms();
	/*
	 * Never really get here
	 */
	return 0;
}

/*
 * impfeCleanup - this does whatever housecleaning is required before
 * exiting. Can use Xforms calls
 */
void impfeCleanup(void)
{
}

/*
 * eShowInMain - displays "error" messages in the main text output area
 */
void eShowInMain()
{
}

/*
 * ShowInMain - displays messages in the main text output area
 */
void ShowInMain()
{
}

