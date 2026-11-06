/*
 * ImpFeGui.c - builds the ImpFE windows with GTK 3
 *
 * This replaces the code the XForms form designer used to generate
 * from ImpFeGui.fd. Layouts follow the original forms, but use GTK
 * containers so the windows resize and follow the desktop theme.
 *
 * ImpFE is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 1, or (at your option)
 * any later version.
 */

#include <stdlib.h>
#include "ImpFeGui.h"
#include "gui.h"
#include "smimplogo.xpm"

/* ------------------------------------------------------------------ */
/* Layout helpers                                                     */
/* ------------------------------------------------------------------ */

static GtkWidget *vbox_new(int spacing)
{
	return(gtk_box_new(GTK_ORIENTATION_VERTICAL, spacing));
}

static GtkWidget *hbox_new(int spacing)
{
	return(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, spacing));
}

static GtkWidget *grid_new(void)
{
	GtkWidget *grid;

	grid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
	gtk_grid_set_row_spacing(GTK_GRID(grid), 2);
	return(grid);
}

/* A titled frame holding a grid; the grid is returned via *grid */
static GtkWidget *frame_grid(const char *title, GtkWidget **grid)
{
	GtkWidget *frame;

	frame = gtk_frame_new(title);
	*grid = grid_new();
	gtk_container_set_border_width(GTK_CONTAINER(*grid), 6);
	gtk_container_add(GTK_CONTAINER(frame), *grid);
	return(frame);
}

static GtkWidget *heading_new(const char *text)
{
	GtkWidget *label;

	label = ui_label_new(text);
	gtk_style_context_add_class(gtk_widget_get_style_context(label),
		"heading");
	return(label);
}

/* A monospaced column-heading line to sit above a browser */
static GtkWidget *column_heads(const char *text)
{
	GtkWidget *label;

	label = ui_label_new(text);
	gtk_style_context_add_class(gtk_widget_get_style_context(label), "mono");
	gtk_style_context_add_class(gtk_widget_get_style_context(label),
		"heading");
	gtk_widget_set_margin_start(label, 4);
	return(label);
}

/* Packs a browser beneath its column headings */
static GtkWidget *browser_with_heads(GtkWidget *browser, const char *heads)
{
	GtkWidget *box;

	box = vbox_new(2);
	if (heads != NULL)
	{
		gtk_box_pack_start(GTK_BOX(box), column_heads(heads), FALSE, FALSE, 0);
	}
	gtk_box_pack_start(GTK_BOX(box), ui_browser_widget(browser), TRUE, TRUE, 0);
	return(box);
}

/* A row of buttons along the bottom of a window */
static GtkWidget *button_bar(void)
{
	GtkWidget *bar;

	bar = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(bar), GTK_BUTTONBOX_EDGE);
	gtk_box_set_spacing(GTK_BOX(bar), 6);
	return(bar);
}

/* "Detailed info for ...: ####   Owner: ####   At: ####" */
static GtkWidget *detail_title(const char *what, GtkWidget **num,
	GtkWidget **owner, GtkWidget **rowcol)
{
	GtkWidget *box;

	box = hbox_new(6);
	gtk_box_pack_start(GTK_BOX(box), heading_new(what), FALSE, FALSE, 0);
	*num = ui_label_new("");
	gtk_label_set_width_chars(GTK_LABEL(*num), 9);
	gtk_box_pack_start(GTK_BOX(box), *num, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), heading_new("Owner:"), FALSE, FALSE, 12);
	*owner = ui_label_new("");
	gtk_label_set_width_chars(GTK_LABEL(*owner), 10);
	gtk_box_pack_start(GTK_BOX(box), *owner, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), heading_new("At:"), FALSE, FALSE, 12);
	*rowcol = ui_label_new("");
	gtk_box_pack_start(GTK_BOX(box), *rowcol, FALSE, FALSE, 0);
	return(box);
}

/* ------------------------------------------------------------------ */
/* Main window                                                        */
/* ------------------------------------------------------------------ */

FD_ImpFeMain *create_form_ImpFeMain(void)
{
	FD_ImpFeMain *fdui;
	GtkWidget *win, *outer, *menubar, *content, *status, *grid, *lbl;

	fdui = g_new0(FD_ImpFeMain, 1);

	win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(win), "ImpFE");
	gtk_window_set_default_size(GTK_WINDOW(win), 780, 560);
	g_signal_connect(win, "delete-event", G_CALLBACK(close_main_form), NULL);
	fdui->ImpFeMain = win;

	outer = vbox_new(0);
	gtk_container_add(GTK_CONTAINER(win), outer);

	/* Menus */
	menubar = gtk_menu_bar_new();
	gtk_box_pack_start(GTK_BOX(outer), menubar, FALSE, FALSE, 0);
	fdui->MainFileMenu = ui_menu_new(menubar, "_File", MAIN_FILE_MENU,
		MainFileMenu_cb);
	fdui->MainServerMenu = ui_menu_new(menubar, "_Server", MAIN_SERVER_MENU,
		MainServerMenu_cb);
	fdui->MainViewMenu = ui_menu_new(menubar, "_View", MAIN_VIEW_MENU,
		MainViewMenu_cb);
	fdui->MainConfigMenu = ui_menu_new(menubar, "_Configure",
		MAIN_CONFIG_MENU, MainConfigMenu_cb);
	fdui->MainHelpMenu = ui_menu_new(menubar, "_Help", MAIN_HELP_MENU,
		MainHelpMenu_cb);

	content = vbox_new(6);
	gtk_container_set_border_width(GTK_CONTAINER(content), 8);
	gtk_box_pack_start(GTK_BOX(outer), content, TRUE, TRUE, 0);

	/* Status line */
	status = hbox_new(4);
	gtk_box_pack_start(GTK_BOX(content), status, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(status), heading_new("Active scripts"),
		FALSE, FALSE, 0);
	fdui->ScriptDisp = ui_status_new("", 4);
	gtk_box_pack_start(GTK_BOX(status), fdui->ScriptDisp, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(status), heading_new("Cmd queue"),
		FALSE, FALSE, 8);
	fdui->QueueDisp = ui_status_new("", 4);
	gtk_box_pack_start(GTK_BOX(status), fdui->QueueDisp, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(status), heading_new("Status"),
		FALSE, FALSE, 8);
	fdui->StatusBox = ui_status_new("Offline", 10);
	gtk_box_pack_start(GTK_BOX(status), fdui->StatusBox, FALSE, FALSE, 0);
	fdui->TimeDisp = ui_status_new("", 5);
	gtk_box_pack_end(GTK_BOX(status), fdui->TimeDisp, FALSE, FALSE, 0);
	gtk_box_pack_end(GTK_BOX(status), heading_new("Time"), FALSE, FALSE, 0);
	fdui->BtuDisp = ui_status_new("", 5);
	gtk_box_pack_end(GTK_BOX(status), fdui->BtuDisp, FALSE, FALSE, 8);
	gtk_box_pack_end(GTK_BOX(status), heading_new("BTU"), FALSE, FALSE, 0);

	/* Server output */
	fdui->MainBrowser = ui_browser_new(TRUE, NULL);
	ui_browser_set_selectable(fdui->MainBrowser, FALSE);
	gtk_box_pack_start(GTK_BOX(content), ui_browser_widget(fdui->MainBrowser),
		TRUE, TRUE, 0);

	/* Prompt and input line */
	grid = grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
	gtk_box_pack_start(GTK_BOX(content), grid, FALSE, FALSE, 0);

	fdui->PrivMsgBtn = gtk_check_button_new_with_mnemonic("_Priv");
	gtk_grid_attach(GTK_GRID(grid), fdui->PrivMsgBtn, 0, 0, 1, 1);
	lbl = heading_new("Prompt:");
	gtk_grid_attach(GTK_GRID(grid), lbl, 1, 0, 1, 1);
	fdui->MainPrompt = ui_label_new("");
	gtk_style_context_add_class(gtk_widget_get_style_context(fdui->MainPrompt),
		"prompt");
	gtk_label_set_ellipsize(GTK_LABEL(fdui->MainPrompt), PANGO_ELLIPSIZE_END);
	gtk_widget_set_hexpand(fdui->MainPrompt, TRUE);
	gtk_grid_attach(GTK_GRID(grid), fdui->MainPrompt, 2, 0, 1, 1);

	fdui->PrivMsgWho = ui_entry_new(0, 8);
	gtk_grid_attach(GTK_GRID(grid), fdui->PrivMsgWho, 0, 1, 1, 1);
	fdui->MainInputBox = ui_entry_new(257, 0);
	gtk_style_context_add_class(
		gtk_widget_get_style_context(fdui->MainInputBox), "mono");
	gtk_widget_set_hexpand(fdui->MainInputBox, TRUE);
	g_signal_connect(fdui->MainInputBox, "activate",
		G_CALLBACK(MainInputBox_cb), GINT_TO_POINTER(1));
	gtk_grid_attach(GTK_GRID(grid), fdui->MainInputBox, 1, 1, 2, 1);

	gtk_widget_show_all(outer);
	gtk_widget_grab_focus(fdui->MainInputBox);
	return(fdui);
}

/* ------------------------------------------------------------------ */
/* About box                                                          */
/* ------------------------------------------------------------------ */

FD_ImpFeAbout *create_form_ImpFeAbout(void)
{
	FD_ImpFeAbout *fdui;
	GtkWidget *box, *top, *bar;

	fdui = g_new0(FD_ImpFeAbout, 1);
	fdui->ImpFeAbout = ui_window_new("About ImpFE", 360, -1);
	gtk_window_set_resizable(GTK_WINDOW(fdui->ImpFeAbout), FALSE);

	box = vbox_new(12);
	gtk_container_add(GTK_CONTAINER(fdui->ImpFeAbout), box);

	top = hbox_new(16);
	gtk_box_pack_start(GTK_BOX(box), top, FALSE, FALSE, 0);
	fdui->ImpLogo = gtk_image_new_from_pixbuf(
		ui_xpm_pixbuf((const char **) smimplogo));
	gtk_box_pack_start(GTK_BOX(top), fdui->ImpLogo, FALSE, FALSE, 0);
	fdui->ImpVer = ui_label_new("ImpFE");
	gtk_style_context_add_class(gtk_widget_get_style_context(fdui->ImpVer),
		"title");
	gtk_box_pack_start(GTK_BOX(top), fdui->ImpVer, FALSE, FALSE, 0);

	fdui->MainAboutText = ui_label_new("");
	gtk_label_set_line_wrap(GTK_LABEL(fdui->MainAboutText), TRUE);
	gtk_box_pack_start(GTK_BOX(box), fdui->MainAboutText, TRUE, TRUE, 0);

	bar = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(bar), GTK_BUTTONBOX_CENTER);
	fdui->AboutOkButn = ui_button_new("_OK", G_CALLBACK(AboutOk_cb), 0);
	gtk_container_add(GTK_CONTAINER(bar), fdui->AboutOkButn);
	gtk_box_pack_start(GTK_BOX(box), bar, FALSE, FALSE, 0);
	return(fdui);
}

/* ------------------------------------------------------------------ */
/* Map                                                                */
/* ------------------------------------------------------------------ */

/* The navigation pad: position, label and DirBtn_cb code */
static const struct
{
	int x, y;
	const char *label;
	int code;
	const char *tip;
} NavPad[] =
{
	{ 0, 0, "\xe2\x87\x96", 70, "Up-left a full screen" },
	{ 2, 0, "\xe2\x87\x91", 80, "Up a full screen" },
	{ 4, 0, "\xe2\x87\x97", 90, "Up-right a full screen" },
	{ 1, 1, "\xe2\x86\x96", 7, "Up-left one sector" },
	{ 2, 1, "\xe2\x86\x91", 8, "Up one sector" },
	{ 3, 1, "\xe2\x86\x97", 9, "Up-right one sector" },
	{ 0, 2, "\xe2\x87\x90", 40, "Left a full screen" },
	{ 1, 2, "\xe2\x86\x90", 4, "Left one sector" },
	{ 2, 2, "\xe2\x97\x8f", 5, "Centre on home planet" },
	{ 3, 2, "\xe2\x86\x92", 6, "Right one sector" },
	{ 4, 2, "\xe2\x87\x92", 60, "Right a full screen" },
	{ 1, 3, "\xe2\x86\x99", 1, "Down-left one sector" },
	{ 2, 3, "\xe2\x86\x93", 2, "Down one sector" },
	{ 3, 3, "\xe2\x86\x98", 3, "Down-right one sector" },
	{ 0, 4, "\xe2\x87\x99", 10, "Down-left a full screen" },
	{ 2, 4, "\xe2\x87\x93", 20, "Down a full screen" },
	{ 4, 4, "\xe2\x87\x98", 30, "Down-right a full screen" }
};

FD_MapForm *create_form_MapForm(void)
{
	FD_MapForm *fdui;
	GtkWidget *box, *map, *side, *pad, *btn, *jump, *ev, *bar;
	unsigned int i;
	int row, col;

	fdui = g_new0(FD_MapForm, 1);
	fdui->MapForm = ui_window_new("ImpFE Map Display", -1, -1);

	box = vbox_new(8);
	gtk_container_add(GTK_CONTAINER(fdui->MapForm), box);
	ev = hbox_new(12);
	gtk_box_pack_start(GTK_BOX(box), ev, TRUE, TRUE, 0);

	/* Sector grid with coordinate labels */
	map = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(map), 0);
	gtk_grid_set_row_spacing(GTK_GRID(map), 0);
	gtk_box_pack_start(GTK_BOX(ev), map, FALSE, FALSE, 0);
	for (col = 0; col < MAP_COLS; col++)
	{
		fdui->ColLbl[col] = ui_label_new("");
		gtk_label_set_xalign(GTK_LABEL(fdui->ColLbl[col]), 0.5);
		gtk_style_context_add_class(
			gtk_widget_get_style_context(fdui->ColLbl[col]), "map-coord");
		gtk_grid_attach(GTK_GRID(map), fdui->ColLbl[col], col + 1, 0, 1, 1);
	}
	for (row = 0; row < MAP_ROWS; row++)
	{
		fdui->RowLbl[row] = ui_label_new("");
		gtk_widget_set_margin_end(fdui->RowLbl[row], 4);
		gtk_style_context_add_class(
			gtk_widget_get_style_context(fdui->RowLbl[row]), "map-coord");
		gtk_grid_attach(GTK_GRID(map), fdui->RowLbl[row], 0, row + 1, 1, 1);
		for (col = 0; col < MAP_COLS; col++)
		{
			/* Event box so the cell can tell which mouse button was used */
			btn = gtk_event_box_new();
			gtk_style_context_add_class(gtk_widget_get_style_context(btn),
				"map-grid");
			fdui->Cell[row][col] = gtk_image_new();
			gtk_widget_set_size_request(fdui->Cell[row][col], 50, 50);
			gtk_container_add(GTK_CONTAINER(btn), fdui->Cell[row][col]);
			g_signal_connect(btn, "button-press-event", G_CALLBACK(MB_cb),
				GINT_TO_POINTER((row * 10) + col + 1));
			gtk_grid_attach(GTK_GRID(map), btn, col + 1, row + 1, 1, 1);
		}
	}

	/* Navigation pad and jump-to controls */
	side = vbox_new(8);
	gtk_box_pack_start(GTK_BOX(ev), side, FALSE, FALSE, 0);
	pad = gtk_grid_new();
	gtk_grid_set_row_homogeneous(GTK_GRID(pad), TRUE);
	gtk_grid_set_column_homogeneous(GTK_GRID(pad), TRUE);
	gtk_box_pack_start(GTK_BOX(side), pad, FALSE, FALSE, 0);
	for (i = 0; i < G_N_ELEMENTS(NavPad); i++)
	{
		btn = ui_button_new(NavPad[i].label, G_CALLBACK(DirBtn_cb),
			NavPad[i].code);
		gtk_widget_set_tooltip_text(btn, NavPad[i].tip);
		gtk_grid_attach(GTK_GRID(pad), btn, NavPad[i].x, NavPad[i].y, 1, 1);
		if (NavPad[i].code == 5)
		{
			fdui->HomeBtn = btn;
		}
	}

	jump = grid_new();
	gtk_box_pack_start(GTK_BOX(side), jump, FALSE, FALSE, 8);
	gtk_grid_attach(GTK_GRID(jump), ui_label_new("Row"), 0, 0, 1, 1);
	fdui->JmRow = ui_entry_new(5, 5);
	gtk_grid_attach(GTK_GRID(jump), fdui->JmRow, 1, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(jump), ui_label_new("Col"), 2, 0, 1, 1);
	fdui->JmCol = ui_entry_new(5, 5);
	gtk_grid_attach(GTK_GRID(jump), fdui->JmCol, 3, 0, 1, 1);
	fdui->JmpBtn = ui_button_new("_Jump!", G_CALLBACK(JmpBtn_cb), 0);
	gtk_grid_attach(GTK_GRID(jump), fdui->JmpBtn, 0, 1, 4, 1);
	g_signal_connect(fdui->JmRow, "activate", G_CALLBACK(JmpBtn_cb), NULL);
	g_signal_connect(fdui->JmCol, "activate", G_CALLBACK(JmpBtn_cb), NULL);

	gtk_box_pack_start(GTK_BOX(side),
		ui_label_new("Middle-click a planet\nfor its details."),
		FALSE, FALSE, 0);

	bar = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(bar), GTK_BUTTONBOX_CENTER);
	fdui->MapOkButn = ui_button_new("_OK", G_CALLBACK(MapOk_cb), 0);
	gtk_container_add(GTK_CONTAINER(bar), fdui->MapOkButn);
	gtk_box_pack_start(GTK_BOX(box), bar, FALSE, FALSE, 0);
	return(fdui);
}

/* ------------------------------------------------------------------ */
/* Tasks                                                              */
/* ------------------------------------------------------------------ */

FD_TasksForm *create_form_TasksForm(void)
{
	FD_TasksForm *fdui;
	GtkWidget *box, *bar;

	fdui = g_new0(FD_TasksForm, 1);
	fdui->TasksForm = ui_window_new("ImpFE Tasks", 420, 280);
	box = vbox_new(8);
	gtk_container_add(GTK_CONTAINER(fdui->TasksForm), box);

	fdui->TasksBrowser = ui_browser_new(TRUE, NULL);
	gtk_box_pack_start(GTK_BOX(box), browser_with_heads(fdui->TasksBrowser,
		"Num  Description                     State"), TRUE, TRUE, 0);

	bar = button_bar();
	fdui->TasksUpdtBtn = ui_button_new("_Update", G_CALLBACK(TasksUpdtBtn_cb), 0);
	fdui->TasksDoneBtn = ui_button_new("_OK", G_CALLBACK(TasksDoneBtn_cb), 0);
	fdui->TasksKillBtn = ui_button_new("_Kill", G_CALLBACK(TasksKillBtn_cb), 0);
	gtk_container_add(GTK_CONTAINER(bar), fdui->TasksUpdtBtn);
	gtk_container_add(GTK_CONTAINER(bar), fdui->TasksDoneBtn);
	gtk_container_add(GTK_CONTAINER(bar), fdui->TasksKillBtn);
	gtk_box_pack_start(GTK_BOX(box), bar, FALSE, FALSE, 0);
	return(fdui);
}

/* ------------------------------------------------------------------ */
/* Server configuration                                               */
/* ------------------------------------------------------------------ */

FD_ServCfgForm *create_form_ServCfgForm(void)
{
	FD_ServCfgForm *fdui;
	GtkWidget *box, *row, *frame, *grid, *bar;

	fdui = g_new0(FD_ServCfgForm, 1);
	fdui->ServCfgForm = ui_window_new("ImpFE Host Config", -1, -1);
	gtk_window_set_modal(GTK_WINDOW(fdui->ServCfgForm), TRUE);

	box = vbox_new(10);
	gtk_container_add(GTK_CONTAINER(fdui->ServCfgForm), box);
	row = hbox_new(10);
	gtk_box_pack_start(GTK_BOX(box), row, FALSE, FALSE, 0);

	frame = frame_grid("Server Data", &grid);
	gtk_box_pack_start(GTK_BOX(row), frame, TRUE, TRUE, 0);
	gtk_grid_attach(GTK_GRID(grid), ui_label_new("Server name"), 0, 0, 1, 1);
	fdui->ServName = ui_entry_new(0, 24);
	gtk_grid_attach(GTK_GRID(grid), fdui->ServName, 1, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), ui_label_new("Port"), 0, 1, 1, 1);
	fdui->ServPort = ui_entry_new(0, 6);
	gtk_widget_set_halign(fdui->ServPort, GTK_ALIGN_START);
	gtk_entry_set_input_purpose(GTK_ENTRY(fdui->ServPort),
		GTK_INPUT_PURPOSE_DIGITS);
	gtk_grid_attach(GTK_GRID(grid), fdui->ServPort, 1, 1, 1, 1);

	frame = frame_grid("Player Data", &grid);
	gtk_box_pack_start(GTK_BOX(row), frame, TRUE, TRUE, 0);
	gtk_grid_attach(GTK_GRID(grid), ui_label_new("Name"), 0, 0, 1, 1);
	fdui->PlayerName = ui_entry_new(0, 24);
	gtk_grid_attach(GTK_GRID(grid), fdui->PlayerName, 1, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), ui_label_new("Password"), 0, 1, 1, 1);
	fdui->PlayerPswd = ui_entry_new(0, 24);
	gtk_entry_set_visibility(GTK_ENTRY(fdui->PlayerPswd), FALSE);
	gtk_entry_set_input_purpose(GTK_ENTRY(fdui->PlayerPswd),
		GTK_INPUT_PURPOSE_PASSWORD);
	gtk_grid_attach(GTK_GRID(grid), fdui->PlayerPswd, 1, 1, 1, 1);

	frame = frame_grid("World Data", &grid);
	gtk_box_pack_start(GTK_BOX(box), frame, FALSE, FALSE, 0);
	gtk_grid_attach(GTK_GRID(grid), ui_label_new("Game description"),
		0, 0, 1, 1);
	fdui->GameDesc = ui_entry_new(0, 40);
	gtk_widget_set_hexpand(fdui->GameDesc, TRUE);
	gtk_grid_attach(GTK_GRID(grid), fdui->GameDesc, 1, 0, 1, 1);

	bar = button_bar();
	fdui->ServCfgOkBtn = ui_button_new("_OK", G_CALLBACK(ServCfgOkBtn_cb), 0);
	fdui->ServCfgCancelBtn = ui_button_new("_Cancel",
		G_CALLBACK(ServCfgCancelBtn_cb), 0);
	gtk_container_add(GTK_CONTAINER(bar), fdui->ServCfgOkBtn);
	gtk_container_add(GTK_CONTAINER(bar), fdui->ServCfgCancelBtn);
	gtk_box_pack_start(GTK_BOX(box), bar, FALSE, FALSE, 0);
	return(fdui);
}

/* ------------------------------------------------------------------ */
/* Ship and planet census                                             */
/* ------------------------------------------------------------------ */

FD_ShipCensusForm *create_form_ShipCensusForm(void)
{
	FD_ShipCensusForm *fdui;
	GtkWidget *box, *bar;
	gchar *heads;

	fdui = g_new0(FD_ShipCensusForm, 1);
	fdui->ShipCensusForm = ui_window_new("Ship Census", 700, 360);
	box = vbox_new(8);
	gtk_container_add(GTK_CONTAINER(fdui->ShipCensusForm), box);

	/* Same layout as the lines built in buildShCensus() */
	heads = g_strdup_printf("%8s  %8s   %8s    %c   %-10s  %3s %3s %s",
		"Number", "Name", "Owner", 'C', "Location", "Crw", "Eff",
		"Last Seen");
	fdui->ShBrowse = ui_browser_new(TRUE, ShBrowse_cb);
	gtk_box_pack_start(GTK_BOX(box), browser_with_heads(fdui->ShBrowse, heads),
		TRUE, TRUE, 0);
	g_free(heads);
	gtk_box_pack_start(GTK_BOX(box), ui_label_new(
		"Click: add ship number to input    Middle-click: add and send    "
		"Right-click: details"), FALSE, FALSE, 0);

	bar = button_bar();
	fdui->ShCenOkButn = ui_button_new("_OK", G_CALLBACK(ShCenOk_cb), 0);
	fdui->Update = ui_button_new("_Update", G_CALLBACK(ShCenUpdate_cb), 0);
	gtk_container_add(GTK_CONTAINER(bar), fdui->ShCenOkButn);
	gtk_container_add(GTK_CONTAINER(bar), fdui->Update);
	gtk_box_pack_start(GTK_BOX(box), bar, FALSE, FALSE, 0);
	return(fdui);
}

FD_PlanetCensusForm *create_form_PlanetCensusForm(void)
{
	FD_PlanetCensusForm *fdui;
	GtkWidget *box, *bar;
	gchar *heads;

	fdui = g_new0(FD_PlanetCensusForm, 1);
	fdui->PlanetCensusForm = ui_window_new("Planet Census", 900, 380);
	box = vbox_new(8);
	gtk_container_add(GTK_CONTAINER(fdui->PlanetCensusForm), box);

	/* Same layout as the lines built in buildPlCensus() */
	heads = g_strdup_printf(
		"%8s  %8s   %8s    %c   %-10s  %3s  %3s  %3s %3s %3s %3s %3s  %s",
		"Number", "Name", "Owner", 'C', "Location", "Pop", "Eff", "Air",
		"Wat", "Ore", "Gld", "Pol", "Last Seen");
	fdui->PlBrowse = ui_browser_new(TRUE, PlBrowse_cb);
	gtk_box_pack_start(GTK_BOX(box), browser_with_heads(fdui->PlBrowse, heads),
		TRUE, TRUE, 0);
	g_free(heads);
	gtk_box_pack_start(GTK_BOX(box), ui_label_new(
		"Click: add planet number to input    Middle-click: add and send    "
		"Right-click: details"), FALSE, FALSE, 0);

	bar = button_bar();
	fdui->PlCenOkButn = ui_button_new("_OK", G_CALLBACK(PlCenOk_cb), 0);
	fdui->Update = ui_button_new("_Update", G_CALLBACK(PlCenUpdate_cb), 0);
	gtk_container_add(GTK_CONTAINER(bar), fdui->PlCenOkButn);
	gtk_container_add(GTK_CONTAINER(bar), fdui->Update);
	gtk_box_pack_start(GTK_BOX(box), bar, FALSE, FALSE, 0);
	return(fdui);
}

/* ------------------------------------------------------------------ */
/* Ship detail                                                        */
/* ------------------------------------------------------------------ */

FD_ShipDetailForm *create_form_ShipDetailForm(void)
{
	FD_ShipDetailForm *fdui;
	GtkWidget *box, *top, *right, *frame, *grid, *items, *ibox, *btns, *bar;
	gchar *heads;

	fdui = g_new0(FD_ShipDetailForm, 1);
	fdui->ShipDetailForm = ui_window_new("Detailed Ship Information", -1, -1);
	box = vbox_new(8);
	gtk_container_add(GTK_CONTAINER(fdui->ShipDetailForm), box);

	gtk_box_pack_start(GTK_BOX(box), detail_title("Detailed info for ship:",
		&fdui->ShNum, &fdui->Owner, &fdui->RowCol), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box),
		gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 0);

	top = hbox_new(8);
	gtk_box_pack_start(GTK_BOX(box), top, FALSE, FALSE, 0);

	/* Status */
	frame = frame_grid("Status", &grid);
	gtk_box_pack_start(GTK_BOX(top), frame, TRUE, TRUE, 0);
	fdui->Fuel = ui_grid_value(grid, 0, 0, "Fuel:", 6);
	fdui->Energy = ui_grid_value(grid, 0, 1, "Energy:", 6);
	fdui->Armor = ui_grid_value(grid, 0, 2, "Armor:", 6);
	fdui->Shields = ui_grid_value(grid, 0, 3, "Shields:", 6);
	fdui->ShKeep = ui_grid_value(grid, 0, 4, "(keep):", 6);
	fdui->Air = ui_grid_value(grid, 0, 5, "Air:", 6);
	fdui->Effic = ui_grid_value(grid, 2, 0, "Effic:", 4);
	fdui->TF = ui_grid_value(grid, 2, 1, "TF:", 4);
	fdui->HullTF = ui_grid_value(grid, 2, 2, "Hull TF:", 4);
	fdui->EngTF = ui_grid_value(grid, 2, 3, "Eng TF:", 4);
	fdui->EngEff = ui_grid_value(grid, 2, 4, "Eng Eff:", 4);
	fdui->Plague = ui_grid_value(grid, 2, 5, "Plague:", 4);
	gtk_grid_attach(GTK_GRID(grid), ui_label_new("Last seen:"), 0, 6, 1, 1);
	fdui->Seen = ui_label_new("");
	gtk_grid_attach(GTK_GRID(grid), fdui->Seen, 1, 6, 3, 1);
	fdui->Where = ui_label_new("");
	gtk_grid_attach(GTK_GRID(grid), fdui->Where, 0, 7, 4, 1);

	right = vbox_new(8);
	gtk_box_pack_start(GTK_BOX(top), right, TRUE, TRUE, 0);

	/* Passengers */
	frame = frame_grid("Passengers", &grid);
	gtk_box_pack_start(GTK_BOX(right), frame, FALSE, FALSE, 0);
	fdui->Civ = ui_grid_value(grid, 0, 0, "Civ:", 5);
	fdui->Mil = ui_grid_value(grid, 0, 1, "Mil:", 5);
	fdui->Sci = ui_grid_value(grid, 2, 0, "Sci:", 5);
	fdui->Ofc = ui_grid_value(grid, 2, 1, "Ofc:", 5);

	/* Small cargo */
	frame = frame_grid("Small Cargo", &grid);
	gtk_box_pack_start(GTK_BOX(right), frame, FALSE, FALSE, 0);
	fdui->Torp = ui_grid_value(grid, 0, 0, "Torp:", 5);
	fdui->Ore = ui_grid_value(grid, 0, 1, "Ore:", 5);
	fdui->Bars = ui_grid_value(grid, 0, 2, "Bars:", 5);
	fdui->AirTnk = ui_grid_value(grid, 2, 0, "Air tnk:", 6);
	fdui->FuelTnk = ui_grid_value(grid, 2, 1, "Fuel tnk:", 6);
	fdui->Cargo = ui_grid_value(grid, 2, 2, "Available:", 6);

	/* Big items */
	items = hbox_new(8);
	gtk_box_pack_start(GTK_BOX(box), items, TRUE, TRUE, 0);
	frame = gtk_frame_new("Big Items");
	gtk_box_pack_start(GTK_BOX(items), frame, TRUE, TRUE, 0);
	ibox = hbox_new(8);
	gtk_container_set_border_width(GTK_CONTAINER(ibox), 6);
	gtk_container_add(GTK_CONTAINER(frame), ibox);
	/* Same layout as the lines built in buildShDetDisp() */
	heads = g_strdup_printf("%8s  %c  %3s  %3s  %5s %s", "Number", 'T',
		"TF", "Eff", "Wght", "Inst");
	fdui->ShDetItem = ui_browser_new(TRUE, NULL);
	gtk_widget_set_size_request(ui_browser_widget(fdui->ShDetItem), 300, 120);
	gtk_box_pack_start(GTK_BOX(ibox), browser_with_heads(fdui->ShDetItem,
		heads), TRUE, TRUE, 0);
	g_free(heads);
	btns = vbox_new(4);
	gtk_box_pack_start(GTK_BOX(ibox), btns, FALSE, FALSE, 0);
	fdui->ShDetInstallBtn = ui_button_new("_Install",
		G_CALLBACK(ShDetActionBtn_cb), 1);
	fdui->ShDetUninstallBtn = ui_button_new("U_ninstall",
		G_CALLBACK(ShDetActionBtn_cb), 2);
	fdui->ShDetUnloadBtn = ui_button_new("Un_load",
		G_CALLBACK(ShDetActionBtn_cb), 3);
	gtk_box_pack_start(GTK_BOX(btns), fdui->ShDetInstallBtn, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(btns), fdui->ShDetUninstallBtn, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(btns), fdui->ShDetUnloadBtn, FALSE, FALSE, 0);

	/* Requests to the server */
	btns = vbox_new(4);
	gtk_box_pack_start(GTK_BOX(items), btns, FALSE, FALSE, 0);
	fdui->ShDetUpdBtn = ui_button_new("_Update", G_CALLBACK(ShDetUpdBtn_cb), 0);
	fdui->ShDetStatBtn = ui_button_new("_Status",
		G_CALLBACK(ShDetStatBtn_cb), 0);
	fdui->ShDetCargoBtn = ui_button_new("_Cargo",
		G_CALLBACK(ShDetCargoBtn_cb), 0);
	fdui->ShDetBigBtn = ui_button_new("_Big", G_CALLBACK(ShDetBigBtn_cb), 0);
	gtk_box_pack_start(GTK_BOX(btns), fdui->ShDetUpdBtn, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(btns), fdui->ShDetStatBtn, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(btns), fdui->ShDetCargoBtn, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(btns), fdui->ShDetBigBtn, FALSE, FALSE, 0);

	bar = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(bar), GTK_BUTTONBOX_END);
	fdui->ShDetOkButn = ui_button_new("_OK", G_CALLBACK(ShDetOk_cb), 0);
	gtk_container_add(GTK_CONTAINER(bar), fdui->ShDetOkButn);
	gtk_box_pack_start(GTK_BOX(box), bar, FALSE, FALSE, 0);
	return(fdui);
}

/* ------------------------------------------------------------------ */
/* Planet detail                                                      */
/* ------------------------------------------------------------------ */

FD_PlanetDetailForm *create_form_PlanetDetailForm(void)
{
	FD_PlanetDetailForm *fdui;
	GtkWidget *box, *row, *col, *frame, *grid, *ibox, *btns, *bar, *hb;
	gchar *heads;

	fdui = g_new0(FD_PlanetDetailForm, 1);
	fdui->PlanetDetailForm = ui_window_new("Detailed Planet Information",
		-1, -1);
	box = vbox_new(8);
	gtk_container_add(GTK_CONTAINER(fdui->PlanetDetailForm), box);

	gtk_box_pack_start(GTK_BOX(box), detail_title("Detailed info for planet:",
		&fdui->PlNum, &fdui->Owner, &fdui->RowCol), FALSE, FALSE, 0);

	/* Name and checkpoint code */
	row = hbox_new(6);
	gtk_box_pack_start(GTK_BOX(box), row, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(row), ui_label_new("Name:"), FALSE, FALSE, 0);
	fdui->Name = ui_entry_new(0, 16);
	gtk_box_pack_start(GTK_BOX(row), fdui->Name, FALSE, FALSE, 0);
	fdui->NameUpdate = ui_button_new("Upd", G_CALLBACK(NameUpdate_cb), 0);
	gtk_widget_set_tooltip_text(fdui->NameUpdate, "Update the planet name");
	gtk_box_pack_start(GTK_BOX(row), fdui->NameUpdate, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(row), ui_label_new("Checkpoint:"),
		FALSE, FALSE, 12);
	fdui->Checkpoint = ui_entry_new(0, 16);
	gtk_box_pack_start(GTK_BOX(row), fdui->Checkpoint, FALSE, FALSE, 0);
	fdui->CheckpointUpdate = ui_button_new("Upd",
		G_CALLBACK(CheckpointUpdate_cb), 0);
	gtk_widget_set_tooltip_text(fdui->CheckpointUpdate,
		"Update the checkpoint code");
	gtk_box_pack_start(GTK_BOX(row), fdui->CheckpointUpdate, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box),
		gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 0);

	row = hbox_new(8);
	gtk_box_pack_start(GTK_BOX(box), row, FALSE, FALSE, 0);

	/* Population */
	frame = frame_grid("Population", &grid);
	gtk_box_pack_start(GTK_BOX(row), frame, TRUE, TRUE, 0);
	fdui->Civ = ui_grid_value(grid, 0, 0, "Civ:", 5);
	fdui->Mil = ui_grid_value(grid, 0, 1, "Mil:", 5);
	fdui->Tech = ui_grid_value(grid, 0, 2, "Tech lv:", 5);
	fdui->TF = ui_grid_value(grid, 0, 3, "TF:", 5);
	fdui->PF = ui_grid_value(grid, 0, 4, "PF:", 5);
	fdui->Sci = ui_grid_value(grid, 2, 0, "Sci:", 5);
	fdui->Ofc = ui_grid_value(grid, 2, 1, "Ofc:", 5);
	fdui->Research = ui_grid_value(grid, 2, 2, "Res lv:", 5);
	fdui->RF = ui_grid_value(grid, 2, 3, "RF:", 5);
	fdui->PopPct = ui_grid_value(grid, 2, 4, "Pop %:", 5);
	gtk_grid_attach(GTK_GRID(grid), ui_label_new("Prev own:"), 0, 5, 1, 1);
	fdui->PrevOwn = ui_label_new("");
	gtk_grid_attach(GTK_GRID(grid), fdui->PrevOwn, 1, 5, 3, 1);
	gtk_grid_attach(GTK_GRID(grid), ui_label_new("Transfer:"), 0, 6, 1, 1);
	fdui->Transfer = ui_label_new("");
	gtk_grid_attach(GTK_GRID(grid), fdui->Transfer, 1, 6, 3, 1);
	gtk_grid_attach(GTK_GRID(grid), ui_label_new("Owner relat:"), 0, 7, 1, 1);
	fdui->OwnerRel = ui_label_new("");
	gtk_grid_attach(GTK_GRID(grid), fdui->OwnerRel, 1, 7, 3, 1);
	gtk_grid_attach(GTK_GRID(grid), ui_label_new("Last seen:"), 0, 8, 1, 1);
	fdui->Seen = ui_label_new("");
	gtk_grid_attach(GTK_GRID(grid), fdui->Seen, 1, 8, 3, 1);

	col = vbox_new(8);
	gtk_box_pack_start(GTK_BOX(row), col, TRUE, TRUE, 0);

	/* Commodities */
	frame = frame_grid("Commodities", &grid);
	gtk_box_pack_start(GTK_BOX(col), frame, FALSE, FALSE, 0);
	fdui->Misl = ui_grid_value(grid, 0, 0, "Misl:", 5);
	fdui->Plane = ui_grid_value(grid, 0, 1, "Plane:", 5);
	fdui->Ore = ui_grid_value(grid, 0, 2, "Ore:", 5);
	fdui->AirTnk = ui_grid_value(grid, 2, 0, "Air tnk:", 5);
	fdui->FuelTnk = ui_grid_value(grid, 2, 1, "Fuel tnk:", 5);
	fdui->Bars = ui_grid_value(grid, 2, 2, "Bars:", 5);

	/* Geographic */
	frame = frame_grid("Geographic", &grid);
	gtk_box_pack_start(GTK_BOX(col), frame, FALSE, FALSE, 0);
	fdui->Size = ui_grid_value(grid, 0, 0, "Size:", 5);
	fdui->Effic = ui_grid_value(grid, 0, 1, "Effic:", 5);
	fdui->Minerals = ui_grid_value(grid, 0, 2, "Minrl:", 5);
	fdui->Water = ui_grid_value(grid, 0, 3, "Water:", 5);
	fdui->Mobil = ui_grid_value(grid, 0, 4, "Mobil:", 5);
	fdui->Class = ui_grid_value(grid, 2, 0, "Class:", 5);
	fdui->Polut = ui_grid_value(grid, 2, 1, "Polut:", 5);
	fdui->Gold = ui_grid_value(grid, 2, 2, "Gold:", 5);
	fdui->Gas = ui_grid_value(grid, 2, 3, "Gas:", 5);
	fdui->Plague = ui_grid_value(grid, 2, 4, "Plague:", 5);

	col = vbox_new(8);
	gtk_box_pack_start(GTK_BOX(row), col, TRUE, TRUE, 0);

	/* Production */
	frame = gtk_frame_new("Production");
	gtk_box_pack_start(GTK_BOX(col), frame, TRUE, TRUE, 0);
	ibox = vbox_new(4);
	gtk_container_set_border_width(GTK_CONTAINER(ibox), 6);
	gtk_container_add(GTK_CONTAINER(frame), ibox);
	hb = hbox_new(6);
	gtk_box_pack_start(GTK_BOX(ibox), hb, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hb), ui_label_new("BTU:"), FALSE, FALSE, 0);
	fdui->BTU = ui_label_new("");
	gtk_box_pack_start(GTK_BOX(hb), fdui->BTU, FALSE, FALSE, 0);
	/* Same layout as the lines built in buildPlDetDisp() */
	heads = g_strdup_printf("%11s   %5s   %3s", "Type", "Avail", "Pct");
	fdui->PlProdBrowse = ui_browser_new(TRUE, NULL);
	ui_browser_set_selectable(fdui->PlProdBrowse, FALSE);
	gtk_widget_set_size_request(ui_browser_widget(fdui->PlProdBrowse),
		240, 120);
	gtk_box_pack_start(GTK_BOX(ibox), browser_with_heads(fdui->PlProdBrowse,
		heads), TRUE, TRUE, 0);
	g_free(heads);

	/* Requests to the server */
	btns = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(btns), 4);
	gtk_grid_set_row_spacing(GTK_GRID(btns), 4);
	gtk_grid_set_column_homogeneous(GTK_GRID(btns), TRUE);
	gtk_box_pack_start(GTK_BOX(col), btns, FALSE, FALSE, 0);
	fdui->PlDetUpdBtn = ui_button_new("_Update", G_CALLBACK(PlDetUpdBtn_cb), 0);
	fdui->PlDetGeoBtn = ui_button_new("_Geo", G_CALLBACK(PlDetGeoBtn_cb), 0);
	fdui->PlDetPopBtn = ui_button_new("_Popu", G_CALLBACK(PlDetPopBtn_cb), 0);
	fdui->PlDetBigBtn = ui_button_new("_Big", G_CALLBACK(PlDetBigBtn_cb), 0);
	fdui->PlDetDumpBtn = ui_button_new("_Dump", G_CALLBACK(PlDetDumpBtn_cb), 0);
	fdui->PlDetProdBtn = ui_button_new("P_rod", G_CALLBACK(PlDetProdBtn_cb), 0);
	gtk_grid_attach(GTK_GRID(btns), fdui->PlDetUpdBtn, 0, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(btns), fdui->PlDetGeoBtn, 1, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(btns), fdui->PlDetPopBtn, 2, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(btns), fdui->PlDetBigBtn, 3, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(btns), fdui->PlDetDumpBtn, 0, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(btns), fdui->PlDetProdBtn, 1, 1, 1, 1);

	/* Big items */
	row = hbox_new(8);
	gtk_box_pack_start(GTK_BOX(box), row, TRUE, TRUE, 0);
	frame = gtk_frame_new("Big Items");
	gtk_box_pack_start(GTK_BOX(row), frame, TRUE, TRUE, 0);
	ibox = hbox_new(8);
	gtk_container_set_border_width(GTK_CONTAINER(ibox), 6);
	gtk_container_add(GTK_CONTAINER(frame), ibox);
	/* Same layout as the lines built in buildPlDetDisp() */
	heads = g_strdup_printf("%8s  %c  %3s  %3s  %5s", "Number", 'T', "TF",
		"Eff", "Wght");
	fdui->PlDetItem = ui_browser_new(TRUE, NULL);
	gtk_widget_set_size_request(ui_browser_widget(fdui->PlDetItem), 300, 120);
	gtk_box_pack_start(GTK_BOX(ibox), browser_with_heads(fdui->PlDetItem,
		heads), TRUE, TRUE, 0);
	g_free(heads);
	btns = vbox_new(4);
	gtk_box_pack_start(GTK_BOX(ibox), btns, FALSE, FALSE, 0);
	fdui->PlDetRefurbBtn = ui_button_new("Re_furb",
		G_CALLBACK(PlDetActionBtn_cb), 1);
	gtk_box_pack_start(GTK_BOX(btns), fdui->PlDetRefurbBtn, FALSE, FALSE, 0);

	bar = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(bar), GTK_BUTTONBOX_END);
	fdui->PlDetOkButn = ui_button_new("_OK", G_CALLBACK(PlDetOk_cb), 0);
	gtk_container_add(GTK_CONTAINER(bar), fdui->PlDetOkButn);
	gtk_box_pack_start(GTK_BOX(box), bar, FALSE, FALSE, 0);
	return(fdui);
}

/* ------------------------------------------------------------------ */
/* Help                                                               */
/* ------------------------------------------------------------------ */

FD_HelpForm *create_form_HelpForm(void)
{
	FD_HelpForm *fdui;
	GtkWidget *box, *bar;

	fdui = g_new0(FD_HelpForm, 1);
	fdui->HelpForm = ui_window_new("Help for ImpFE", 680, 480);
	box = vbox_new(8);
	gtk_container_add(GTK_CONTAINER(fdui->HelpForm), box);
	fdui->Help_Browser = ui_browser_new(FALSE, NULL);
	ui_browser_set_selectable(fdui->Help_Browser, FALSE);
	ui_browser_set_wrap(fdui->Help_Browser);
	gtk_widget_set_size_request(ui_browser_widget(fdui->Help_Browser), 400, 200);
	gtk_box_pack_start(GTK_BOX(box), ui_browser_widget(fdui->Help_Browser),
		TRUE, TRUE, 0);
	bar = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(bar), GTK_BUTTONBOX_CENTER);
	fdui->OkBtn = ui_button_new("_OK", G_CALLBACK(HelpOk_cb), 0);
	gtk_container_add(GTK_CONTAINER(bar), fdui->OkBtn);
	gtk_box_pack_start(GTK_BOX(box), bar, FALSE, FALSE, 0);
	return(fdui);
}

/* ------------------------------------------------------------------ */
/* Command queue                                                      */
/* ------------------------------------------------------------------ */

FD_CommandForm *create_form_CommandForm(void)
{
	FD_CommandForm *fdui;
	GtkWidget *box, *grid, *bar;
	gchar *heads;

	fdui = g_new0(FD_CommandForm, 1);
	fdui->CommandForm = ui_window_new("Queued Commands", 480, 400);
	box = vbox_new(8);
	gtk_container_add(GTK_CONTAINER(fdui->CommandForm), box);

	/* Same layout as the lines built in buildCmdList() */
	heads = g_strdup_printf("%3s  %-30s  %s", "Num", "Command", "Priority");
	fdui->CmdsBrowser = ui_browser_new(TRUE, CmdsBrowser_cb);
	gtk_box_pack_start(GTK_BOX(box), browser_with_heads(fdui->CmdsBrowser,
		heads), TRUE, TRUE, 0);
	g_free(heads);

	grid = grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
	gtk_box_pack_start(GTK_BOX(box), grid, FALSE, FALSE, 0);
	gtk_grid_attach(GTK_GRID(grid), ui_label_new("Command"), 0, 0, 1, 1);
	fdui->CmdsCmd = ui_entry_new(256, 0);
	gtk_style_context_add_class(gtk_widget_get_style_context(fdui->CmdsCmd),
		"mono");
	gtk_widget_set_tooltip_text(fdui->CmdsCmd,
		"Use ~ to separate the lines of a multi-line command");
	gtk_widget_set_hexpand(fdui->CmdsCmd, TRUE);
	gtk_grid_attach(GTK_GRID(grid), fdui->CmdsCmd, 1, 0, 3, 1);
	gtk_grid_attach(GTK_GRID(grid), ui_label_new("Priority"), 0, 1, 1, 1);
	fdui->CmdsPriorBtn = ui_choice_new();
	gtk_grid_attach(GTK_GRID(grid), fdui->CmdsPriorBtn, 1, 1, 1, 1);
	fdui->CmdsAddBtn = ui_button_new("_Add", G_CALLBACK(CmdsAddBtn_cb), 0);
	gtk_grid_attach(GTK_GRID(grid), fdui->CmdsAddBtn, 2, 1, 1, 1);
	fdui->CmdsModBtn = ui_button_new("_Modify", G_CALLBACK(CmdsModBtn_cb), 0);
	gtk_grid_attach(GTK_GRID(grid), fdui->CmdsModBtn, 3, 1, 1, 1);

	bar = button_bar();
	fdui->CmdsDoneBtn = ui_button_new("_OK", G_CALLBACK(CmdsDoneBtn_cb), 0);
	fdui->CmdsKillBtn = ui_button_new("_Kill", G_CALLBACK(CmdsKillBtn_cb), 0);
	gtk_widget_set_tooltip_text(fdui->CmdsKillBtn,
		"Remove the selected command from the queue");
	fdui->CmdsPauseBtn = ui_button_new("_Pause", G_CALLBACK(CmdsPauseBtn_cb), 0);
	gtk_widget_set_tooltip_text(fdui->CmdsPauseBtn,
		"Stop or restart running queued commands");
	gtk_container_add(GTK_CONTAINER(bar), fdui->CmdsDoneBtn);
	gtk_container_add(GTK_CONTAINER(bar), fdui->CmdsKillBtn);
	gtk_container_add(GTK_CONTAINER(bar), fdui->CmdsPauseBtn);
	gtk_box_pack_start(GTK_BOX(box), bar, FALSE, FALSE, 0);
	return(fdui);
}
