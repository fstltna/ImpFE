/*
 * gtkui.h - small helper layer between ImpFE and GTK 3
 *
 * ImpFE was originally written against the XForms toolkit.  These
 * helpers provide the handful of widget behaviours the rest of the
 * code relies on (line-oriented "browsers", pull-down menus addressed
 * by item number, coloured status boxes, file selection) on top of
 * plain GTK 3 widgets.
 *
 * ImpFE is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 1, or (at your option)
 * any later version.
 */

#ifndef IMPFE_GTKUI_H
#define IMPFE_GTKUI_H

#include <gtk/gtk.h>

/* Mouse buttons, as reported to browser and map callbacks */
#define UI_LEFT_MOUSE	1
#define UI_MIDDLE_MOUSE	2
#define UI_RIGHT_MOUSE	3

/* Highlight levels for the status boxes on the main window */
typedef enum
{
	UI_LEVEL_NORMAL,	/* black on wheat */
	UI_LEVEL_WARN,		/* black on tomato */
	UI_LEVEL_CRIT		/* red on black */
} UiLevel;

/* Called when a menu item is picked; item numbers start at 1 */
typedef void (*UiMenuCb)(GtkWidget *menu, int item);

/* Called when a browser line is clicked; line numbers start at 1 */
typedef void (*UiBrowserCb)(GtkWidget *browser, int line, int button);

/* Startup */
extern void ui_init(int *argc, char ***argv);

/* Windows */
extern GtkWidget *ui_window_new(const char *title, int width, int height);
extern void ui_window_show(GtkWidget *win);
extern void ui_window_hide(GtkWidget *win);
extern gboolean ui_window_visible(GtkWidget *win);

/* Labels / text boxes */
extern GtkWidget *ui_label_new(const char *text);
extern GtkWidget *ui_status_new(const char *text, int width_chars);
extern void ui_set_text(GtkWidget *label, const char *text);
extern void ui_set_level(GtkWidget *label, UiLevel level);
extern GtkWidget *ui_grid_value(GtkWidget *grid, int col, int row,
	const char *caption, int width_chars);

/* Single line text input */
extern GtkWidget *ui_entry_new(int max_chars, int width_chars);
extern const char *ui_entry_get(GtkWidget *entry);
extern void ui_entry_set(GtkWidget *entry, const char *text);

/* Drop-down choice (items numbered from 1, 0 = nothing selected) */
extern GtkWidget *ui_choice_new(void);
extern void ui_choice_add(GtkWidget *choice, const char *text);
extern int ui_choice_get(GtkWidget *choice);
extern void ui_choice_set(GtkWidget *choice, int item);

/* Buttons */
extern GtkWidget *ui_button_new(const char *label, GCallback cb, int data);

/* Pull-down menus */
extern GtkWidget *ui_menu_new(GtkWidget *menubar, const char *title,
	const char *items, UiMenuCb cb);
extern void ui_menu_enable(GtkWidget *menu, int item, gboolean enabled);

/* Browsers - scrolling lists of text lines */
extern GtkWidget *ui_browser_new(gboolean monospace, UiBrowserCb cb);
extern GtkWidget *ui_browser_widget(GtkWidget *browser);
extern void ui_browser_set_selectable(GtkWidget *browser, gboolean selectable);
extern void ui_browser_set_wrap(GtkWidget *browser);
extern void ui_browser_append(GtkWidget *browser, const char *chars);
extern void ui_browser_add_line(GtkWidget *browser, const char *line);
extern void ui_browser_clear(GtkWidget *browser);
extern int ui_browser_selected(GtkWidget *browser);
extern const char *ui_browser_line(GtkWidget *browser, int line);
extern void ui_browser_set_topline(GtkWidget *browser, int line);

/* Images */
extern GdkPixbuf *ui_xpm_pixbuf(const char **xpm);

/* File selection - returns the full path, or NULL if cancelled */
extern const char *ui_file_select(GtkWidget *parent, const char *title,
	gboolean create, const char *pattern, const char *def_name);
extern const char *ui_last_directory(void);
extern const char *ui_last_filename(void);

#endif /* IMPFE_GTKUI_H */
