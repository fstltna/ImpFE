/*
 * gtkui.c - small helper layer between ImpFE and GTK 3
 *
 * See gtkui.h for an overview.
 *
 * ImpFE is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 1, or (at your option)
 * any later version.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gtkui.h"

/* Main output can run for hours; keep it from growing without bound */
#define UI_BROWSER_MAX_LINES	10000

static const char ui_css[] =
	".mono { font-family: monospace; }\n"
	"label.status { background-color: #f5deb3; color: #000000;"
	"  border: 1px solid #8b7d6b; padding: 1px 6px; }\n"
	"label.status.warn { background-color: #ff6347; color: #000000; }\n"
	"label.status.crit { background-color: #000000; color: #ff0000; }\n"
	"label.prompt { background-color: #cd5c5c; color: #000000;"
	"  border: 1px solid #8b3a3a; padding: 4px 6px; }\n"
	"label.heading { font-weight: bold; }\n"
	"label.title { font-weight: bold; font-size: larger; }\n"
	".map-grid { background-color: #000000; }\n"
	"treeview.browser-heading { background-color: transparent; }\n"
	".map-coord { font-family: monospace; }\n";

/*
 * ui_init - initialise GTK and install ImpFE's style sheet
 */
void ui_init(int *argc, char ***argv)
{
	GtkCssProvider *css;

	gtk_init(argc, argv);
	css = gtk_css_provider_new();
	gtk_css_provider_load_from_data(css, ui_css, -1, NULL);
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
		GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	g_object_unref(css);
}

/* ------------------------------------------------------------------ */
/* Windows                                                            */
/* ------------------------------------------------------------------ */

/*
 * ui_window_new - creates a secondary window. Closing it from the
 *                 window manager just hides it, so it can be reopened.
 */
GtkWidget *ui_window_new(const char *title, int width, int height)
{
	GtkWidget *win;

	win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(win), title);
	if ((width > 0) && (height > 0))
	{
		gtk_window_set_default_size(GTK_WINDOW(win), width, height);
	}
	gtk_container_set_border_width(GTK_CONTAINER(win), 8);
	g_signal_connect(win, "delete-event",
		G_CALLBACK(gtk_widget_hide_on_delete), NULL);
	return(win);
}

void ui_window_show(GtkWidget *win)
{
	gtk_widget_show_all(win);
	gtk_window_present(GTK_WINDOW(win));
}

void ui_window_hide(GtkWidget *win)
{
	gtk_widget_hide(win);
}

gboolean ui_window_visible(GtkWidget *win)
{
	return((win != NULL) && gtk_widget_get_visible(win));
}

/* ------------------------------------------------------------------ */
/* Labels                                                             */
/* ------------------------------------------------------------------ */

GtkWidget *ui_label_new(const char *text)
{
	GtkWidget *label;

	label = gtk_label_new(text);
	gtk_label_set_xalign(GTK_LABEL(label), 0.0);
	return(label);
}

/*
 * ui_status_new - a framed, coloured value box (BTU, TIME, etc.)
 */
GtkWidget *ui_status_new(const char *text, int width_chars)
{
	GtkWidget *label;

	label = ui_label_new(text);
	if (width_chars > 0)
	{
		gtk_label_set_width_chars(GTK_LABEL(label), width_chars);
	}
	gtk_style_context_add_class(gtk_widget_get_style_context(label),
		"status");
	return(label);
}

void ui_set_text(GtkWidget *label, const char *text)
{
	gtk_label_set_text(GTK_LABEL(label), text);
}

void ui_set_level(GtkWidget *label, UiLevel level)
{
	GtkStyleContext *ctx;

	ctx = gtk_widget_get_style_context(label);
	gtk_style_context_remove_class(ctx, "warn");
	gtk_style_context_remove_class(ctx, "crit");
	switch (level)
	{
		case UI_LEVEL_WARN:
			gtk_style_context_add_class(ctx, "warn");
			break;
		case UI_LEVEL_CRIT:
			gtk_style_context_add_class(ctx, "crit");
			break;
		default:
			break;
	}
}

/*
 * ui_grid_value - attaches a "Caption:" label and an (initially empty)
 *                 value label to a grid, returning the value label
 */
GtkWidget *ui_grid_value(GtkWidget *grid, int col, int row,
	const char *caption, int width_chars)
{
	GtkWidget *value;

	gtk_grid_attach(GTK_GRID(grid), ui_label_new(caption), col, row, 1, 1);
	value = ui_label_new("");
	gtk_label_set_selectable(GTK_LABEL(value), TRUE);
	gtk_widget_set_can_focus(value, FALSE);
	if (width_chars > 0)
	{
		gtk_label_set_width_chars(GTK_LABEL(value), width_chars);
	}
	gtk_grid_attach(GTK_GRID(grid), value, col + 1, row, 1, 1);
	return(value);
}

/* ------------------------------------------------------------------ */
/* Inputs                                                             */
/* ------------------------------------------------------------------ */

GtkWidget *ui_entry_new(int max_chars, int width_chars)
{
	GtkWidget *entry;

	entry = gtk_entry_new();
	if (max_chars > 0)
	{
		gtk_entry_set_max_length(GTK_ENTRY(entry), max_chars);
	}
	if (width_chars > 0)
	{
		gtk_entry_set_width_chars(GTK_ENTRY(entry), width_chars);
	}
	return(entry);
}

const char *ui_entry_get(GtkWidget *entry)
{
	return(gtk_entry_get_text(GTK_ENTRY(entry)));
}

void ui_entry_set(GtkWidget *entry, const char *text)
{
	gtk_entry_set_text(GTK_ENTRY(entry), (text != NULL) ? text : "");
}

GtkWidget *ui_choice_new(void)
{
	return(gtk_combo_box_text_new());
}

void ui_choice_add(GtkWidget *choice, const char *text)
{
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(choice), text);
}

int ui_choice_get(GtkWidget *choice)
{
	return(gtk_combo_box_get_active(GTK_COMBO_BOX(choice)) + 1);
}

void ui_choice_set(GtkWidget *choice, int item)
{
	gtk_combo_box_set_active(GTK_COMBO_BOX(choice), item - 1);
}

GtkWidget *ui_button_new(const char *label, GCallback cb, int data)
{
	GtkWidget *btn;

	btn = gtk_button_new_with_mnemonic(label);
	if (cb != NULL)
	{
		g_signal_connect(btn, "clicked", cb, GINT_TO_POINTER(data));
	}
	return(btn);
}

/* ------------------------------------------------------------------ */
/* Menus                                                              */
/* ------------------------------------------------------------------ */

static void ui_menu_activate(GtkMenuItem *mi, gpointer top)
{
	UiMenuCb cb;
	int item;

	cb = (UiMenuCb) g_object_get_data(G_OBJECT(top), "ui-menu-cb");
	item = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(mi), "ui-menu-item"));
	if (cb != NULL)
	{
		cb(GTK_WIDGET(top), item);
	}
}

/*
 * ui_menu_new - adds a pull-down menu to a menu bar. Items are given
 *               as "First|Second|Third" and are numbered from 1.
 */
GtkWidget *ui_menu_new(GtkWidget *menubar, const char *title,
	const char *items, UiMenuCb cb)
{
	GtkWidget *top, *menu, *mi;
	gchar **names;
	int i;

	top = gtk_menu_item_new_with_mnemonic(title);
	menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(top), menu);
	g_object_set_data(G_OBJECT(top), "ui-menu-cb", (gpointer) cb);
	names = g_strsplit(items, "|", -1);
	for (i = 0; names[i] != NULL; i++)
	{
		mi = gtk_menu_item_new_with_label(names[i]);
		g_object_set_data(G_OBJECT(mi), "ui-menu-item",
			GINT_TO_POINTER(i + 1));
		g_signal_connect(mi, "activate", G_CALLBACK(ui_menu_activate), top);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), mi);
	}
	g_strfreev(names);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), top);
	return(top);
}

void ui_menu_enable(GtkWidget *menu, int item, gboolean enabled)
{
	GtkWidget *sub;
	GList *kids;
	gpointer mi;

	sub = gtk_menu_item_get_submenu(GTK_MENU_ITEM(menu));
	if (sub == NULL)
	{
		return;
	}
	kids = gtk_container_get_children(GTK_CONTAINER(sub));
	mi = g_list_nth_data(kids, (guint) (item - 1));
	if (mi != NULL)
	{
		gtk_widget_set_sensitive(GTK_WIDGET(mi), enabled);
	}
	g_list_free(kids);
}

/* ------------------------------------------------------------------ */
/* Browsers                                                           */
/* ------------------------------------------------------------------ */

enum
{
	BR_TEXT,
	BR_WEIGHT,
	BR_STYLE,
	BR_SCALE,
	BR_XALIGN,
	BR_FG,
	BR_FAMILY,
	BR_UNDERLINE,
	BR_NCOLS
};

static void br_insert(GtkListStore *store, const char *text, int weight,
	PangoStyle style, double scale, float xalign, const char *fg,
	const char *family, PangoUnderline underline);

static GtkListStore *br_store(GtkWidget *browser)
{
	return(GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(browser))));
}

static gboolean br_button_press(GtkWidget *view, GdkEventButton *ev,
	gpointer unused)
{
	UiBrowserCb cb;
	GtkTreePath *path;
	int line;

	if (ev->type != GDK_BUTTON_PRESS)
	{
		return(FALSE);
	}
	if (!gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(view), (gint) ev->x,
		(gint) ev->y, &path, NULL, NULL, NULL))
	{
		return(FALSE);
	}
	gtk_widget_grab_focus(view);
	gtk_tree_view_set_cursor(GTK_TREE_VIEW(view), path, NULL, FALSE);
	line = gtk_tree_path_get_indices(path)[0] + 1;
	gtk_tree_path_free(path);
	cb = (UiBrowserCb) g_object_get_data(G_OBJECT(view), "ui-br-cb");
	if (cb != NULL)
	{
		cb(view, line, (int) ev->button);
	}
	return(TRUE);
}

/*
 * ui_browser_new - creates a list of text lines. The returned widget is
 *                  the list itself; pack ui_browser_widget() of it.
 */
/*
 * br_view_new - the list widget behind a browser (and its heading line,
 *               which must be drawn identically so the columns line up)
 */
static GtkWidget *br_view_new(gboolean monospace)
{
	GtkListStore *store;
	GtkWidget *view;
	GtkCellRenderer *cell;
	GtkTreeViewColumn *col;

	store = gtk_list_store_new(BR_NCOLS, G_TYPE_STRING, G_TYPE_INT,
		PANGO_TYPE_STYLE, G_TYPE_DOUBLE, G_TYPE_FLOAT, G_TYPE_STRING,
		G_TYPE_STRING, PANGO_TYPE_UNDERLINE);
	view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
	g_object_unref(store);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(view), FALSE);
	gtk_tree_view_set_enable_search(GTK_TREE_VIEW(view), FALSE);
	cell = gtk_cell_renderer_text_new();
	g_object_set(cell, "ypad", 0, NULL);
	col = gtk_tree_view_column_new_with_attributes("", cell,
		"text", BR_TEXT, "weight", BR_WEIGHT, "style", BR_STYLE,
		"scale", BR_SCALE, "xalign", BR_XALIGN, "foreground", BR_FG,
		"family", BR_FAMILY, "underline", BR_UNDERLINE, NULL);
	gtk_tree_view_column_set_expand(col, TRUE);
	gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);
	if (monospace)
	{
		gtk_style_context_add_class(gtk_widget_get_style_context(view),
			"mono");
	}
	return(view);
}

GtkWidget *ui_browser_new(gboolean monospace, UiBrowserCb cb)
{
	GtkWidget *view, *scroll;

	view = br_view_new(monospace);
	if (cb != NULL)
	{
		g_object_set_data(G_OBJECT(view), "ui-br-cb", (gpointer) cb);
		g_signal_connect(view, "button-press-event",
			G_CALLBACK(br_button_press), NULL);
	}
	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll),
		GTK_SHADOW_IN);
	gtk_widget_set_hexpand(scroll, TRUE);
	gtk_widget_set_vexpand(scroll, TRUE);
	gtk_container_add(GTK_CONTAINER(scroll), view);
	return(view);
}

/* Keeps a browser's heading scrolled sideways along with its list */
static void br_heading_follow(GtkAdjustment *list, gpointer heading)
{
	gtk_adjustment_set_value(GTK_ADJUSTMENT(heading),
		gtk_adjustment_get_value(list));
}

/*
 * ui_browser_heading - a column-heading line for a browser. It is drawn
 *                      with the same kind of list, font and padding as
 *                      the browser's lines, so the columns line up under
 *                      any theme. Pack the returned widget above the
 *                      browser.
 */
GtkWidget *ui_browser_heading(GtkWidget *browser, const char *text)
{
	GtkWidget *view, *scroll;
	GtkAdjustment *listAdj;

	view = br_view_new(gtk_style_context_has_class(
		gtk_widget_get_style_context(browser), "mono"));
	gtk_style_context_add_class(gtk_widget_get_style_context(view),
		"browser-heading");
	br_insert(br_store(view), text, PANGO_WEIGHT_BOLD, PANGO_STYLE_NORMAL,
		1.0, 0.0f, NULL, NULL, PANGO_UNDERLINE_NONE);
	gtk_tree_selection_set_mode(
		gtk_tree_view_get_selection(GTK_TREE_VIEW(view)), GTK_SELECTION_NONE);
	gtk_widget_set_can_focus(view, FALSE);

	/* Same frame as the list, with no scrollbars of its own */
	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
		GTK_POLICY_EXTERNAL, GTK_POLICY_NEVER);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll),
		GTK_SHADOW_NONE);
	gtk_widget_set_hexpand(scroll, TRUE);
	gtk_container_add(GTK_CONTAINER(scroll), view);

	/* Scroll sideways with the list */
	listAdj = gtk_scrolled_window_get_hadjustment(
		GTK_SCROLLED_WINDOW(ui_browser_widget(browser)));
	g_signal_connect(listAdj, "value-changed",
		G_CALLBACK(br_heading_follow),
		gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(scroll)));
	return(scroll);
}

/* Plain output browsers don't need a selection highlight */
void ui_browser_set_selectable(GtkWidget *browser, gboolean selectable)
{
	gtk_tree_selection_set_mode(
		gtk_tree_view_get_selection(GTK_TREE_VIEW(browser)),
		selectable ? GTK_SELECTION_SINGLE : GTK_SELECTION_NONE);
	gtk_widget_set_can_focus(browser, selectable);
}

/* Keeps the wrap width of a wrapping browser matched to its width */
static void br_wrap_allocate(GtkWidget *view, GdkRectangle *alloc,
	gpointer cell)
{
	GtkTreeViewColumn *col;
	gint width, old;

	width = alloc->width - 16;
	if (width < 100)
	{
		width = 100;
	}
	g_object_get(cell, "wrap-width", &old, NULL);
	if (old != width)
	{
		g_object_set(cell, "wrap-width", width, NULL);
		col = gtk_tree_view_get_column(GTK_TREE_VIEW(view), 0);
		gtk_tree_view_column_queue_resize(col);
	}
}

/* Wraps long lines to the browser's width instead of scrolling sideways */
void ui_browser_set_wrap(GtkWidget *browser)
{
	GtkTreeViewColumn *col;
	GList *cells;

	col = gtk_tree_view_get_column(GTK_TREE_VIEW(browser), 0);
	cells = gtk_cell_layout_get_cells(GTK_CELL_LAYOUT(col));
	if (cells != NULL)
	{
		g_object_set(cells->data, "wrap-mode", PANGO_WRAP_WORD_CHAR,
			"wrap-width", 400, NULL);
		g_signal_connect(browser, "size-allocate",
			G_CALLBACK(br_wrap_allocate), cells->data);
	}
	g_list_free(cells);
	gtk_scrolled_window_set_policy(
		GTK_SCROLLED_WINDOW(ui_browser_widget(browser)),
		GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
}

/* The scrolled window holding a browser, for packing into a layout */
GtkWidget *ui_browser_widget(GtkWidget *browser)
{
	return(gtk_widget_get_parent(browser));
}

static void br_insert(GtkListStore *store, const char *text, int weight,
	PangoStyle style, double scale, float xalign, const char *fg,
	const char *family, PangoUnderline underline)
{
	GtkTreeIter iter;

	gtk_list_store_insert_with_values(store, &iter, -1,
		BR_TEXT, text, BR_WEIGHT, weight, BR_STYLE, style,
		BR_SCALE, scale, BR_XALIGN, xalign, BR_FG, fg,
		BR_FAMILY, family, BR_UNDERLINE, underline, -1);
}

static void br_insert_plain(GtkListStore *store, const char *text)
{
	br_insert(store, text, PANGO_WEIGHT_NORMAL, PANGO_STYLE_NORMAL, 1.0,
		0.0f, NULL, NULL, PANGO_UNDERLINE_NONE);
}

static void br_scroll_to_end(GtkWidget *browser)
{
	GtkTreeModel *model;
	GtkTreePath *path;
	int rows;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(browser));
	rows = gtk_tree_model_iter_n_children(model, NULL);
	if (rows < 1)
	{
		return;
	}
	path = gtk_tree_path_new_from_indices(rows - 1, -1);
	gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(browser), path, NULL,
		FALSE, 0.0, 0.0);
	gtk_tree_path_free(path);
}

/*
 * ui_browser_append - appends raw text to the browser. Text up to a
 *                     newline continues the last line; text after the
 *                     last newline starts a line that later calls
 *                     will continue.
 */
void ui_browser_append(GtkWidget *browser, const char *chars)
{
	GtkListStore *store;
	GtkTreeModel *model;
	GtkTreeIter iter;
	const char *start, *nl;
	gboolean partial;
	gchar *seg, *old, *joined;
	int rows;

	if ((chars == NULL) || (chars[0] == '\0'))
	{
		return;
	}
	store = br_store(browser);
	model = GTK_TREE_MODEL(store);
	partial = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(browser),
		"ui-br-partial"));
	start = chars;
	while (*start != '\0')
	{
		nl = strchr(start, '\n');
		if (nl == NULL)
		{
			nl = start + strlen(start);
		}
		seg = g_strndup(start, (gsize) (nl - start));
		g_strdelimit(seg, "\r", ' ');
		rows = gtk_tree_model_iter_n_children(model, NULL);
		if (partial && (rows > 0) &&
			gtk_tree_model_iter_nth_child(model, &iter, NULL, rows - 1))
		{
			gtk_tree_model_get(model, &iter, BR_TEXT, &old, -1);
			joined = g_strconcat((old != NULL) ? old : "", seg, NULL);
			gtk_list_store_set(store, &iter, BR_TEXT, joined, -1);
			g_free(joined);
			g_free(old);
		}
		else
		{
			br_insert_plain(store, seg);
		}
		g_free(seg);
		if (*nl == '\n')
		{
			partial = FALSE;
			start = nl + 1;
		}
		else
		{
			partial = TRUE;
			start = nl;
		}
	}
	g_object_set_data(G_OBJECT(browser), "ui-br-partial",
		GINT_TO_POINTER(partial));
	/* Trim old lines */
	while (gtk_tree_model_iter_n_children(model, NULL) > UI_BROWSER_MAX_LINES)
	{
		if (!gtk_tree_model_get_iter_first(model, &iter))
		{
			break;
		}
		gtk_list_store_remove(store, &iter);
	}
	br_scroll_to_end(browser);
}

/* XForms browser colour indices used by the help text */
static const char *br_colour(long idx)
{
	static const char *colours[] = { "black", "red", "green", "yellow",
		"blue", "magenta", "cyan", "white" };

	if ((idx < 0) || (idx >= (long) G_N_ELEMENTS(colours)))
	{
		return(NULL);
	}
	return(colours[idx]);
}

/*
 * ui_browser_add_line - adds a complete line. Leading XForms-style
 *                       format codes (@b bold, @i italic, @l large,
 *                       @c centred, @C<n> colour, ...) are honoured.
 */
void ui_browser_add_line(GtkWidget *browser, const char *line)
{
	const char *p, *fg = NULL, *family = NULL;
	char *end;
	int weight = PANGO_WEIGHT_NORMAL;
	PangoStyle style = PANGO_STYLE_NORMAL;
	PangoUnderline underline = PANGO_UNDERLINE_NONE;
	double scale = 1.0;
	float xalign = 0.0f;
	gboolean done = FALSE, separator = FALSE;

	p = (line != NULL) ? line : "";
	while (!done && (p[0] == '@') && (p[1] != '\0'))
	{
		switch (p[1])
		{
			case 'b': weight = PANGO_WEIGHT_BOLD; break;
			case 'i': style = PANGO_STYLE_ITALIC; break;
			case 'L': scale = PANGO_SCALE_X_LARGE; break;
			case 'l': scale = PANGO_SCALE_LARGE; break;
			case 'm': scale = PANGO_SCALE_MEDIUM; break;
			case 's': scale = PANGO_SCALE_SMALL; break;
			case 'S': scale = PANGO_SCALE_X_SMALL; break;
			case 'c': xalign = 0.5f; break;
			case 'r': xalign = 1.0f; break;
			case 'f': family = "monospace"; break;
			case 't': family = "serif"; break;
			case '_': underline = PANGO_UNDERLINE_SINGLE; break;
			case '-': separator = TRUE; break;
			case '@':
				/* "@@" - a literal '@' starts the text */
				p++;
				done = TRUE;
				continue;
			case 'C':
				fg = br_colour(strtol(&p[2], &end, 10));
				p = end;
				continue;
			default:
				break;
		}
		p += 2;
	}
	if (separator)
	{
		p = "\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80"
			"\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80";
	}
	br_insert(br_store(browser), p, weight, style, scale, xalign, fg,
		family, underline);
	g_object_set_data(G_OBJECT(browser), "ui-br-partial",
		GINT_TO_POINTER(FALSE));
}

void ui_browser_clear(GtkWidget *browser)
{
	gtk_list_store_clear(br_store(browser));
	g_object_set_data(G_OBJECT(browser), "ui-br-partial",
		GINT_TO_POINTER(FALSE));
}

/* Returns the selected line number (from 1), or 0 if none */
int ui_browser_selected(GtkWidget *browser)
{
	GtkTreeSelection *sel;
	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkTreePath *path;
	int line;

	sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(browser));
	if (!gtk_tree_selection_get_selected(sel, &model, &iter))
	{
		return(0);
	}
	path = gtk_tree_model_get_path(model, &iter);
	line = gtk_tree_path_get_indices(path)[0] + 1;
	gtk_tree_path_free(path);
	return(line);
}

/*
 * ui_browser_line - returns the text of a line (numbered from 1). The
 *                   result is valid until the next call.
 */
const char *ui_browser_line(GtkWidget *browser, int line)
{
	static char buf[1024];
	GtkTreeModel *model;
	GtkTreeIter iter;
	gchar *text = NULL;

	buf[0] = '\0';
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(browser));
	if ((line > 0) && gtk_tree_model_iter_nth_child(model, &iter, NULL,
		line - 1))
	{
		gtk_tree_model_get(model, &iter, BR_TEXT, &text, -1);
		if (text != NULL)
		{
			g_strlcpy(buf, text, sizeof(buf));
			g_free(text);
		}
	}
	return(buf);
}

/* Scrolls so the given line (from 1) is at the top */
void ui_browser_set_topline(GtkWidget *browser, int line)
{
	GtkTreeModel *model;
	GtkTreePath *path;
	int rows;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(browser));
	rows = gtk_tree_model_iter_n_children(model, NULL);
	if (rows < 1)
	{
		return;
	}
	if (line < 1)
	{
		line = 1;
	}
	if (line > rows)
	{
		line = rows;
	}
	path = gtk_tree_path_new_from_indices(line - 1, -1);
	gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(browser), path, NULL,
		TRUE, 0.0, 0.0);
	gtk_tree_path_free(path);
}

/* ------------------------------------------------------------------ */
/* Images                                                             */
/* ------------------------------------------------------------------ */

/*
 * xpm_colour - parses the colour part of an XPM colour line, e.g.
 *              "c #FF0000" or "c None", into *rgba (0xRRGGBBAA)
 */
static gboolean xpm_colour(const char *spec, guint32 *rgba)
{
	gchar **words;
	GString *value;
	GdkRGBA colour;
	gboolean ok = FALSE, inColour = FALSE;
	int i;

	/* Use the "c" (colour display) value; it can be several words */
	words = g_strsplit_set(spec, " \t", -1);
	value = g_string_new(NULL);
	for (i = 0; words[i] != NULL; i++)
	{
		if (words[i][0] == '\0')
		{
			continue;
		}
		if ((strcmp(words[i], "c") == 0) || (strcmp(words[i], "m") == 0) ||
			(strcmp(words[i], "g") == 0) || (strcmp(words[i], "g4") == 0) ||
			(strcmp(words[i], "s") == 0))
		{
			if (inColour && (value->len > 0))
			{
				break;
			}
			inColour = (strcmp(words[i], "c") == 0);
			continue;
		}
		if (inColour)
		{
			if (value->len > 0)
			{
				g_string_append_c(value, ' ');
			}
			g_string_append(value, words[i]);
		}
	}
	if (g_ascii_strcasecmp(value->str, "None") == 0)
	{
		*rgba = 0;	/* transparent */
		ok = TRUE;
	}
	else if (gdk_rgba_parse(&colour, value->str))
	{
		*rgba = ((guint32) (colour.red * 255.0 + 0.5) << 24) |
			((guint32) (colour.green * 255.0 + 0.5) << 16) |
			((guint32) (colour.blue * 255.0 + 0.5) << 8) | 0xFF;
		ok = TRUE;
	}
	g_string_free(value, TRUE);
	g_strfreev(words);
	return(ok);
}

/*
 * xpm_decode - decodes XPM image data compiled into the program.
 *
 *              GTK's own XPM support is an optional plug-in that newer
 *              systems (and the GNOME runtime snaps use) leave out, so
 *              ImpFE reads its images itself. Returns NULL if the data
 *              is malformed.
 */
static GdkPixbuf *xpm_decode(const char **xpm)
{
	GdkPixbuf *pb;
	GHashTable *colours;
	guint32 rgba, lookup1[256];
	gboolean have1[256];
	int width, height, ncolours, cpp, i, x, y;
	guchar *pixels, *p;
	int stride;
	gpointer found;

	if ((xpm == NULL) || (xpm[0] == NULL) ||
		(sscanf(xpm[0], "%d %d %d %d", &width, &height, &ncolours, &cpp) != 4) ||
		(width < 1) || (height < 1) || (width > 4096) || (height > 4096) ||
		(ncolours < 1) || (ncolours > 65536) || (cpp < 1) || (cpp > 4))
	{
		return(NULL);
	}

	/* Colour table: one-character codes use a quick array */
	colours = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
	memset(have1, 0, sizeof(have1));
	for (i = 0; i < ncolours; i++)
	{
		const char *line = xpm[1 + i];

		if ((line == NULL) || (strlen(line) < (size_t) cpp) ||
			!xpm_colour(line + cpp, &rgba))
		{
			g_hash_table_destroy(colours);
			return(NULL);
		}
		if (cpp == 1)
		{
			lookup1[(guchar) line[0]] = rgba;
			have1[(guchar) line[0]] = TRUE;
		}
		else
		{
			g_hash_table_insert(colours, g_strndup(line, (gsize) cpp),
				GUINT_TO_POINTER(rgba));
		}
	}

	pb = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, width, height);
	if (pb == NULL)
	{
		g_hash_table_destroy(colours);
		return(NULL);
	}
	pixels = gdk_pixbuf_get_pixels(pb);
	stride = gdk_pixbuf_get_rowstride(pb);
	for (y = 0; y < height; y++)
	{
		const char *row = xpm[1 + ncolours + y];
		char key[5];

		if ((row == NULL) || (strlen(row) < (size_t) (width * cpp)))
		{
			g_object_unref(pb);
			g_hash_table_destroy(colours);
			return(NULL);
		}
		p = pixels + (y * stride);
		for (x = 0; x < width; x++)
		{
			if (cpp == 1)
			{
				if (!have1[(guchar) row[x]])
				{
					g_object_unref(pb);
					g_hash_table_destroy(colours);
					return(NULL);
				}
				rgba = lookup1[(guchar) row[x]];
			}
			else
			{
				memcpy(key, &row[x * cpp], (size_t) cpp);
				key[cpp] = '\0';
				if (!g_hash_table_lookup_extended(colours, key, NULL, &found))
				{
					g_object_unref(pb);
					g_hash_table_destroy(colours);
					return(NULL);
				}
				rgba = GPOINTER_TO_UINT(found);
			}
			*p++ = (guchar) (rgba >> 24);
			*p++ = (guchar) (rgba >> 16);
			*p++ = (guchar) (rgba >> 8);
			*p++ = (guchar) rgba;
		}
	}
	g_hash_table_destroy(colours);
	return(pb);
}

/*
 * ui_xpm_pixbuf - converts compiled-in XPM data to a pixbuf. Results
 *                 are cached, so this is cheap to call repeatedly.
 */
GdkPixbuf *ui_xpm_pixbuf(const char **xpm)
{
	static GHashTable *cache = NULL;
	GdkPixbuf *pb;

	if (cache == NULL)
	{
		cache = g_hash_table_new(g_direct_hash, g_direct_equal);
	}
	pb = (GdkPixbuf *) g_hash_table_lookup(cache, xpm);
	if (pb == NULL)
	{
		pb = xpm_decode(xpm);
		if (pb != NULL)
		{
			g_hash_table_insert(cache, (gpointer) xpm, pb);
		}
		else
		{
			g_warning("ImpFE: could not decode a built-in image");
		}
	}
	return(pb);
}

/* ------------------------------------------------------------------ */
/* File selection                                                     */
/* ------------------------------------------------------------------ */

static char LastDir[4096] = "";
static char LastFile[1024] = "";

/*
 * ui_file_select - asks the user to pick an existing file (create is
 *                  FALSE) or to name a new one (create is TRUE).
 *                  Returns the full path, or NULL if they cancelled.
 */
const char *ui_file_select(GtkWidget *parent, const char *title,
	gboolean create, const char *pattern, const char *def_name)
{
	static char path[4096];
	GtkWidget *dlg;
	GtkFileFilter *filter;
	gchar *fname, *dir, *base, *cwd, *guess;
	const char *result = NULL;

	dlg = gtk_file_chooser_dialog_new(title,
		(parent != NULL) ? GTK_WINDOW(parent) : NULL,
		create ? GTK_FILE_CHOOSER_ACTION_SAVE : GTK_FILE_CHOOSER_ACTION_OPEN,
		"_Cancel", GTK_RESPONSE_CANCEL,
		create ? "C_reate" : "_Open", GTK_RESPONSE_ACCEPT,
		NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(dlg), GTK_RESPONSE_ACCEPT);
	if (pattern != NULL)
	{
		filter = gtk_file_filter_new();
		gtk_file_filter_set_name(filter, "ImpFE databases");
		gtk_file_filter_add_pattern(filter, pattern);
		gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dlg), filter);
		filter = gtk_file_filter_new();
		gtk_file_filter_set_name(filter, "All files");
		gtk_file_filter_add_pattern(filter, "*");
		gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dlg), filter);
	}
	if (LastDir[0] != '\0')
	{
		cwd = g_strdup(LastDir);
	}
	else if (g_getenv("SNAP_REAL_HOME") != NULL)
	{
		/* In a snap, $HOME is the snap's private folder - start in the
		   user's real home folder instead */
		cwd = g_strdup(g_getenv("SNAP_REAL_HOME"));
	}
	else
	{
		cwd = g_get_current_dir();
	}
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dlg), cwd);
	if (create)
	{
		gtk_file_chooser_set_do_overwrite_confirmation(
			GTK_FILE_CHOOSER(dlg), TRUE);
		if (def_name != NULL)
		{
			gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dlg),
				def_name);
		}
	}
	else if (def_name != NULL)
	{
		guess = g_build_filename(cwd, def_name, NULL);
		if (g_file_test(guess, G_FILE_TEST_IS_REGULAR))
		{
			gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dlg), guess);
		}
		g_free(guess);
	}
	g_free(cwd);

	if (gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_ACCEPT)
	{
		fname = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
		if (fname != NULL)
		{
			/* New databases are expected to end in the pattern's suffix */
			if (create && (pattern != NULL) && (pattern[0] == '*') &&
				!g_str_has_suffix(fname, &pattern[1]))
			{
				gchar *tmp = g_strconcat(fname, &pattern[1], NULL);
				g_free(fname);
				fname = tmp;
			}
			dir = g_path_get_dirname(fname);
			base = g_path_get_basename(fname);
			g_strlcpy(LastDir, dir, sizeof(LastDir));
			g_strlcpy(LastFile, base, sizeof(LastFile));
			g_strlcpy(path, fname, sizeof(path));
			g_free(dir);
			g_free(base);
			g_free(fname);
			result = path;
		}
	}
	gtk_widget_destroy(dlg);
	return(result);
}

/* Directory of the last file selected (or "." if none yet) */
const char *ui_last_directory(void)
{
	return((LastDir[0] != '\0') ? LastDir : ".");
}

/* Name (without directory) of the last file selected */
const char *ui_last_filename(void)
{
	return(LastFile);
}
