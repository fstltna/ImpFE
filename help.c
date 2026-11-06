#ifndef LINT
static const char rcsid[] = "$Id: help.c,v 1.1.1.1 2000/05/17 19:22:13 marisa Exp $";
#endif
/*
 * Copyright(c) 1995,1996 by Gennady B. Sorokopud (gena@NetVision.net.il)
 *
 * This software can be freely redistributed and modified for 
 * non-commercial purposes as long as above copyright
 * message and this permission notice appear in all
 * copies of distributed source code and included as separate file
 * in binary distribution.
 *
 * Any commercial use of this software requires author's permission.
 *
 * This software is provided "as is" without expressed or implied
 * warranty of any kind.
 * Under no circumstances is the author responsible for the proper
 * functioning of this software, nor does the author assume any
 * responsibility for damages incurred with its use.
 *
 * This file heavily modified by me, Marisa Giancarla for ImpFE
 *
 */

/* $Id: help.c,v 1.1.1.1 2000/05/17 19:22:13 marisa Exp $
 *
 * $Log: help.c,v $
 * Revision 1.1.1.1  2000/05/17 19:22:13  marisa
 * First CVS checkin
 *
 * Revision 1.1.1.2  1997/05/03 09:43:21  marisag
 * patch1: Daily checkin
 *
 * Revision 1.1.1.1  1997/04/24 04:46:08  marisag
 * patch1: Many changes
 *
 * Revision 1.1  1997/04/21 15:44:56  marisag
 * Initial revision
 *
 *
 */

#include "config.h"
#include "ImpFeGui.h"
#include "gui.h"
#include "impfe.h"
#include "help.h"

void help(char *chapter)
{
	int i;
	int chapter_line = 0;
	int line_count = 0;
	BOOL redisp = FALSE; /* Are we just updating an existing display? */

	if (ui_window_visible(fd_HelpForm->HelpForm))
	{
		gtk_window_present(GTK_WINDOW(fd_HelpForm->HelpForm));
		redisp = TRUE;
	}
	else
	{
		/* (Re)load the text from scratch */
		ui_browser_clear(fd_HelpForm->Help_Browser);
	}

	i = 0;
	while (strcmp(help_txt[i], "%END"))
	{
		if (help_txt[i][0] == '%')
		{
			if (chapter && !strcmp(chapter, help_txt[i]))
			{
				chapter_line = line_count;
				if (redisp)
				{
					break;
				}
			}
			i++;
			continue;
		}
		if (!redisp)
		{
			if (strlen(help_txt[i]) < 1)
			{
				ui_browser_add_line(fd_HelpForm->Help_Browser, " ");
			}
			else
			{
				ui_browser_add_line(fd_HelpForm->Help_Browser, help_txt[i]);
			}
		}
		line_count++;
		i++;
	}

	if (!redisp)
	{
		ui_window_show(fd_HelpForm->HelpForm);
	}
	ui_browser_set_topline(fd_HelpForm->Help_Browser, chapter_line+1);
}

void cb_help_button(long param)
{
	if ((param < 0) || (param >= TOTAL_CHAPTERS))
	{
		return;
	}

	help(chapters[param]);
}

void HelpOk_cb(GtkWidget *w, gpointer data)
{
	/* If window not open, just return */
	if (ui_window_visible(fd_HelpForm->HelpForm))
	{
		ui_window_hide(fd_HelpForm->HelpForm);
	}
}
