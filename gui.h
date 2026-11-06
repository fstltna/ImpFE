
/*
 * $Id: gui.h,v 1.1.1.1 2000/05/17 19:22:13 marisa Exp $
 *
 * Defines for the menu items for the main form
 *
 * $Log: gui.h,v $
 * Revision 1.1.1.1  2000/05/17 19:22:13  marisa
 * First CVS checkin
 *
 * Revision 1.1.1.6  1997/05/03 09:42:46  marisag
 * patch1: Daily checkin
 *
 * Revision 1.1.1.5  1997/04/24 04:47:07  marisag
 * patch1: Many changes
 *
 * Revision 1.1.1.4  1997/04/14 02:52:33  marisag
 * patch1: Many changes
 *
 * Revision 1.1.1.3  1997/04/12 16:52:54  marisag
 * patch1: Menu changes
 *
 * Revision 1.1.1.2  1997/04/10 06:31:40  marisag
 * patch1: Added in defs for the database stuff
 *
 * Revision 1.1.1.1  1997/04/09 03:21:57  marisag
 * patch1: New changes...
 *
 * Revision 1.1  1997/04/08 07:16:20  marisag
 * Initial revision
 *
 *
 */

#define MAIN_FILE_MENU "Open DB|Close DB|Save DB|New DB|Quit"
#define MAIN_FILE_MENU_OPEN	1
#define MAIN_FILE_MENU_CLOSE	2
#define MAIN_FILE_MENU_SAVE	3
#define MAIN_FILE_MENU_NEW	4
#define MAIN_FILE_MENU_EXIT	5

#define MAIN_SERVER_MENU "Open Connection|Close Connection"
#define MAIN_SERVER_MENU_OPEN	1
#define MAIN_SERVER_MENU_CLOSE	2

#define MAIN_VIEW_MENU "Map|Planet|Ship|Scripts|Commands|Clear Display"
#define MAIN_VIEW_MENU_MAP	1
#define MAIN_VIEW_MENU_PLANET	2
#define MAIN_VIEW_MENU_SHIP	3
#define MAIN_VIEW_MENU_SCRIPTS  4
#define MAIN_VIEW_MENU_CMDS	5
#define MAIN_VIEW_MENU_CLEAR	6

#define MAIN_CONFIG_MENU "Host|Settings"
#define MAIN_CONFIG_MENU_HOST	1
#define MAIN_CONFIG_MENU_SETTINGS	2

#define MAIN_HELP_MENU "Index|About"
#define MAIN_HELP_MENU_INDEX	1
#define MAIN_HELP_MENU_ABOUT	2

/* Form Defs */
#ifdef BUILDING_MAIN
#define GUI_EXTERN
#else
#define GUI_EXTERN extern
#endif
GUI_EXTERN FD_ImpFeMain *fd_ImpFeMain;
GUI_EXTERN FD_ImpFeAbout *fd_ImpFeAbout;
GUI_EXTERN FD_MapForm *fd_MapForm;
GUI_EXTERN FD_TasksForm *fd_TasksForm;
GUI_EXTERN FD_ServCfgForm *fd_ServCfgForm;
GUI_EXTERN FD_ShipCensusForm *fd_ShipCensusForm;
GUI_EXTERN FD_PlanetCensusForm *fd_PlanetCensusForm;
GUI_EXTERN FD_ShipDetailForm *fd_ShipDetailForm;
GUI_EXTERN FD_PlanetDetailForm *fd_PlanetDetailForm;
GUI_EXTERN FD_HelpForm *fd_HelpForm;
GUI_EXTERN FD_CommandForm *fd_CommandForm;
