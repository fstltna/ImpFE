#ifndef LINT
static char rcsid[]="$Id: cmnd.c,v 1.2 2000/05/24 21:51:38 marisa Exp $";
#endif
/*
 * ImpFE
 *
 * $Id: cmnd.c,v 1.2 2000/05/24 21:51:38 marisa Exp $
 *
 * This file contains routines for managing command queues
 *
 * $Log: cmnd.c,v $
 * Revision 1.2  2000/05/24 21:51:38  marisa
 * Fix bugs with TRUE/FALSE testing
 *
 * Revision 1.1.1.1  2000/05/17 19:22:13  marisa
 * First CVS checkin
 *
 * Revision 1.1.1.4  1997/05/03 09:42:55  marisag
 * patch1: Daily checkin
 *
 * Revision 1.1.1.3  1997/04/25 06:40:14  marisag
 * patch1: Will now convert embedded NL's in the queued command
 * patch1: to tildes when saving, and reverse when loading
 *
 * Revision 1.1.1.2  1997/04/24 04:45:59  marisag
 * patch1: Many changes
 *
 * Revision 1.1.1.1  1997/04/14 02:52:57  marisag
 * patch1: Many changes
 *
 * Revision 1.1  1997/04/12 16:47:01  marisag
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

#define MAX_CMD_LEN 258
#define MAX_CMDS    100      /* Maximum # of commands in queue */

typedef struct {
	int pri;                 /* Priority */
	char
	    cmdbuf[MAX_CMD_LEN]; /* Holds the command */
} Cmd_t;

static Cmd_t *CmdArray=NULL; /* Pointer to our buffer */

/*
 * addCmd - Adds a command to the queue at the given priority
 *          returns the command "key" or -1 if failed.
 */
int addCmd(const char *line, int priority)
{
	int tmp;
	char locBuf[25];

	if (!QueueOk)
	{
		return(-1);
	}
	/* If they requested level 0, or not queueing, just send it on */
	if (priority == QUE_IMMEDIATE)
	{
		/* Make sure we are at the main prompt */
		if (AtMainPrompt && !StopQueue)
		{
			if (QueueEcho)
			{
				ui_browser_append(fd_ImpFeMain->MainBrowser, "[QUE] ");
				ui_browser_append(fd_ImpFeMain->MainBrowser, line);
			}
			write_str(line);
			return(-1); /* Value is meaningless in this case */
		}
		/* No, so have to be queued */
	}
	for (tmp=0; tmp < MAX_CMDS; tmp++)
	{
		if (CmdArray[tmp].pri == QUE_AVAIL)
		{
			CmdArray[tmp].pri = priority;
			snprintf(CmdArray[tmp].cmdbuf, sizeof(CmdArray[tmp].cmdbuf), "%s", line);
			NumCmds++;
			sprintf(locBuf, "%d", NumCmds);
			ui_set_text(fd_ImpFeMain->QueueDisp, locBuf);
			if (ui_window_visible(fd_CommandForm->CommandForm))
			{
				buildCmdList();
			}
			return(tmp);
		}
	}
	return(-1);
}

/*
 * modCmd - Modifies an existing command
 *          returns the command "key" or -1 if failed.
 */
BOOL modCmd(int pos, const char *line, int priority)
{
	int tmp, len, point;
	char locBuf[MAX_CMD_LEN];

	if (!QueueOk || (pos >= MAX_CMDS))
	{
		return(FALSE);
	}
	if (CmdArray[pos].pri == QUE_AVAIL)
	{
		return(FALSE);
	}
	CmdArray[pos].pri = priority;
	snprintf(locBuf, sizeof(locBuf), "%s", line);
	len = strlen(locBuf);
	/* reconvert the NL characters */
	for (point = 0; point < len; point++)
	{
		if (locBuf[point] == '~')
		{
			locBuf[point] = '\n';
		}
	}
	snprintf(CmdArray[pos].cmdbuf, sizeof(CmdArray[pos].cmdbuf), "%s", locBuf);
	if (ui_window_visible(fd_CommandForm->CommandForm))
	{
		buildCmdList();
	}
	return(TRUE);
}

/*
 * cancelCmd - cancels the given command. Returns TRUE if command was still
 *             in the buffer, FALSE if already executed
 */
BOOL cancelCmd(int cmdKey)
{
	char locBuf[25];

	/* Are we using the queue? */
	if (!QueueOk)
	{
		/* Just act like it worked */
		return(TRUE);
	}
#ifdef DEBUG
	if (cmdKey >= MAX_CMDS)
	{
		fprintf(stderr, "*** cancelCmd called with invalid command key %u\n",
				cmdKey);
		return(FALSE);
	}
#endif
	if (CmdArray[cmdKey].pri == QUE_AVAIL)
	{
		fe_puts("*** Command already complete\n");
		return(FALSE);
	}
	CmdArray[cmdKey].pri = QUE_AVAIL;
	NumCmds--;
	sprintf(locBuf, "%d", NumCmds);
	if (NumCmds == 0)
	{
	    ui_set_text(fd_ImpFeMain->QueueDisp, "");
	}
	else
	{
	    ui_set_text(fd_ImpFeMain->QueueDisp, locBuf);
	}
	if (ui_window_visible(fd_CommandForm->CommandForm))
	{
		buildCmdList();
	}
	return(TRUE);
}

/*
 * runImmQueue - this runs the first immediate command it finds
 */
void runImmQueue(void)
{
	int tmp, fMed=-1, fLow=-1;
	char locBuf[25];

	/* Make sure we are using the queue, that we are at the main
	 * prompt, and that they are not pausing the queue
	 */
	if (!QueueOk || !AtMainPrompt || StopQueue ||
		(NumCmds == 0))
	{
		return;
	}
	/* Check for immediate mode commands */
	for (tmp=0; tmp < MAX_CMDS; tmp++)
	{
		switch (CmdArray[tmp].pri)
		{
			/* If priority is immediate, run it */
			case QUE_IMMEDIATE:
				/* Echo it? */
				if (QueueEcho)
				{
					ui_browser_append(fd_ImpFeMain->MainBrowser, "[QUE] ");
					ui_browser_append(fd_ImpFeMain->MainBrowser,
										   CmdArray[tmp].cmdbuf);
				}
				write_str(CmdArray[tmp].cmdbuf);
				CmdArray[tmp].pri = QUE_AVAIL;
				NumCmds--;
				sprintf(locBuf, "%d", NumCmds);
				if (NumCmds == 0)
				{
	    			    ui_set_text(fd_ImpFeMain->QueueDisp, "");
				}
				else
				{
	    			    ui_set_text(fd_ImpFeMain->QueueDisp, locBuf);
				}
				if (ui_window_visible(fd_CommandForm->CommandForm))
				{
					buildCmdList();
				}
				return;
				break; /* Never get here */
			default:
				/* Just skip this command */
				break;
		}
	}
}

/*
 * runQueue_cb - This function is called from the GLib main loop
 *               every 20 seconds to run the queued commands
 */
static gboolean runQueue_cb(gpointer unused)
{
    runCmdQueue();
    return(G_SOURCE_CONTINUE);
}

/*
 * runImmQueue_cb - called every 5 seconds to send any immediate
 *                  commands that had to wait for the main prompt
 */
static gboolean runImmQueue_cb(gpointer unused)
{
    runImmQueue();
    return(G_SOURCE_CONTINUE);
}

/*
 * initCmdQueues - initializes the command queues. Sets QueueOk = TRUE if
 *                 succeeds
 */
void initCmdQueues(void)
{
	/* Check if already run */
	if (QueueOk)
	{
		return;
	}
	QueueEcho=TRUE;
	StopQueue = FALSE;
	NumCmds=0;
	if ((CmdArray=(Cmd_t *)malloc(MAX_CMDS * sizeof(Cmd_t))) == NULL)
	{
		QueueOk=FALSE;
	}
	else
	{
		QueueOk=TRUE;
		bzero(CmdArray, MAX_CMDS * sizeof(Cmd_t));
		(void) g_timeout_add(20000, runQueue_cb, NULL);
		(void) g_timeout_add(5000, runImmQueue_cb, NULL);
	}
}

/*
 * freeCmdQueues - frees up the command queues
 */
void freeCmdQueues(void)
{
	if (QueueOk)
	{
		free(CmdArray);
		CmdArray=NULL;
		QueueOk=FALSE;
	}
}

/*
 * runCmdQueue - this runs the first command it finds, in priority
 *               order
 */
void runCmdQueue(void)
{
	int tmp, fMed=-1, fLow=-1;
	char locBuf[25];

	/* Make sure we are using the queue, that we are at the main
	 * prompt, and that they are not pausing the queue
	 */
	if (!QueueOk || !AtMainPrompt || StopQueue ||
		(NumCmds == 0))
	{
		return;
	}
	/* Loop for each possible command */
	for (tmp=0; tmp < MAX_CMDS; tmp++)
	{
		switch (CmdArray[tmp].pri)
		{
			/* If priority is high, just run it */
			case QUE_HIGH:
				/* Echo it? */
				if (QueueEcho)
				{
					ui_browser_append(fd_ImpFeMain->MainBrowser, "[QUE] ");
					ui_browser_append(fd_ImpFeMain->MainBrowser,
										   CmdArray[tmp].cmdbuf);
				}
				write_str(CmdArray[tmp].cmdbuf);
				CmdArray[tmp].pri = QUE_AVAIL;
				NumCmds--;
				sprintf(locBuf, "%d", NumCmds);
				if (NumCmds == 0)
				{
	    			    ui_set_text(fd_ImpFeMain->QueueDisp, "");
				}
				else
				{
	    			    ui_set_text(fd_ImpFeMain->QueueDisp, locBuf);
				}
				if (ui_window_visible(fd_CommandForm->CommandForm))
				{
					buildCmdList();
				}
				return;
				break; /* Never get here */
			case QUE_MED:
				if (fMed < 0)
				{
					fMed=tmp;
				}
				break;
			case QUE_LOW:
				if (fLow < 0)
				{
					fLow=tmp;
				}
				break;
			default:
				break;
		}
	}
	/* Look for a medium command */
	if (fMed != -1)
	{
		/* Echo it? */
		if (QueueEcho)
		{
			ui_browser_append(fd_ImpFeMain->MainBrowser, "[QUE] ");
			ui_browser_append(fd_ImpFeMain->MainBrowser,
								   CmdArray[fMed].cmdbuf);
		}
		write_str(CmdArray[fMed].cmdbuf);
		CmdArray[fMed].pri = QUE_AVAIL;
		NumCmds--;
		sprintf(locBuf, "%d", NumCmds);
		if (NumCmds == 0)
		{
	    	    ui_set_text(fd_ImpFeMain->QueueDisp, "");
		}
		else
		{
	    	    ui_set_text(fd_ImpFeMain->QueueDisp, locBuf);
		}
		if (ui_window_visible(fd_CommandForm->CommandForm))
		{
			buildCmdList();
		}
		return;
	}
	if (fLow != -1)
	{
		/* Echo it? */
		if (QueueEcho)
		{
			ui_browser_append(fd_ImpFeMain->MainBrowser, "[QUE] ");
			ui_browser_append(fd_ImpFeMain->MainBrowser,
								   CmdArray[fLow].cmdbuf);
		}
		write_str(CmdArray[fLow].cmdbuf);
		CmdArray[fLow].pri = QUE_AVAIL;
		NumCmds--;
		sprintf(locBuf, "%d", NumCmds);
		if (NumCmds == 0)
		{
	    	    ui_set_text(fd_ImpFeMain->QueueDisp, "");
		}
		else
		{
	    	    ui_set_text(fd_ImpFeMain->QueueDisp, locBuf);
		}
		if (ui_window_visible(fd_CommandForm->CommandForm))
		{
			buildCmdList();
		}
		return;
	}
}

/*
 * loadCmdQueue - Tries to load in any existing commands into the
 *                queue
 */
void loadCmdQueue(void)
{
	char qfName[1025], line[1025];
	FILE *qFile;
	int priority;
#ifndef DONT_CONVERT_COMMAND
	int len, pos;
#endif

	/* Build up the config file name */
	if (snprintf(qfName, sizeof(qfName), "%s-commands", RawFile) >=
		(int) sizeof(qfName))
	{
		fe_puts("*** Command queue file name is too long\n");
		return;
	}
	/* See if we can load the file */
	if ((qFile=fopen(qfName, "r")) != NULL)
	{
		while (fgets(line, 1024, qFile) != NULL)
		{
			priority = ((int) line[0]) - 'A';
#ifndef DONT_CONVERT_COMMAND  /* XXX */
			len=strlen(&line[1]) - 1;
			for (pos=0; pos < len; pos++)
			{
				if (line[pos + 1] == '~')
				{
					line[pos + 1] = '\n';
				}
			}
			line[len + 1] = '\0';
#else
			strcat(line, "\n");
#endif
			addCmd(&line[1], priority);
		}
		fclose(qFile);
		unlink(qfName);
	}
}

/*
 * saveCmdQueue - Tries to save any unexecuted commands to the
 *                recover file
 */
void saveCmdQueue(void)
{
	char qfName[1025], line[1025];
	FILE *qFile;
	int priority, cmd;
#ifndef DONT_CONVERT_COMMAND
	int len, pos;
#endif

	/* If no commands, just return */
	if (NumCmds == 0)
	{
		return;
	}
	/* Build up the config file name */
	if (snprintf(qfName, sizeof(qfName), "%s-commands", RawFile) >=
		(int) sizeof(qfName))
	{
		fe_puts("*** Command queue file name is too long\n");
		return;
	}
	/* See if we can create the file */
	if ((qFile=fopen(qfName, "w")) != NULL)
	{
		for (cmd=0; cmd < MAX_CMDS; cmd++)
		{
			switch (CmdArray[cmd].pri)
			{
				case QUE_AVAIL:
					/* Skip it */
					break;
				default:
					/* Save all others */
#ifndef DONT_CONVERT_COMMAND  /* XXX */
					len=strlen(CmdArray[cmd].cmdbuf);
					for (pos=0; pos < len; pos++)
					{
						if (CmdArray[cmd].cmdbuf[pos] == '\n')
						{
							CmdArray[cmd].cmdbuf[pos] = '~';
						}
					}
					fprintf(qFile, "%c%s\n", CmdArray[cmd].pri + 'A',
							CmdArray[cmd].cmdbuf);
#else
					fprintf(qFile, "%c%s", CmdArray[cmd].pri + 'A',
							CmdArray[cmd].cmdbuf);
#endif
					break;
			}
		}
		fclose(qFile);
	}
}

/*
 * buildCmdList - builds up the command list browser
 */
void buildCmdList(void)
{
	int tmp, len, pos;
	char lineBuf[95], cmnd[35];

	/* Clear the existing contents, if any */
	ui_browser_clear(fd_CommandForm->CmdsBrowser);
	if (NumCmds != 0)
	{
		/* Loop for each possible command */
		for (tmp=0; tmp < MAX_CMDS; tmp++)
		{
			/* Is it in use? */
			if (CmdArray[tmp].pri != QUE_AVAIL)
			{
				strncpy(cmnd, CmdArray[tmp].cmdbuf, 30 * sizeof(char));
				cmnd[31]='\0';
				/* Strip out NL's */
				len = strlen(cmnd);
				for (pos=0; pos < len; pos++)
				{
					if (cmnd[pos] == '\n')
					{
						cmnd[pos] = '~';
					}
				}
				/* Set the string */
				switch (CmdArray[tmp].pri)
				{
					case QUE_IMMEDIATE:
						sprintf(lineBuf, "%3u  %-30s  IMMED", tmp, cmnd);
						break;
					case QUE_HIGH:
						sprintf(lineBuf, "%3u  %-30s  HIGH", tmp, cmnd);
						break;
					case QUE_MED:
						sprintf(lineBuf, "%3u  %-30s  MEDIUM", tmp, cmnd);
						break;
					case QUE_LOW:
						sprintf(lineBuf, "%3u  %-30s  LOW", tmp, cmnd);
						break;
					default:
						sprintf(lineBuf, "*** unknown queue type %u",
								CmdArray[tmp].pri);
						break;
				}
				ui_browser_add_line(fd_CommandForm->CmdsBrowser,
									lineBuf);
			}
		}
	}
}

/*
 * getCmd - returns the command for the given queue position, or
 *          NULL if error
 */
const char *getCmd(int pos)
{
	int tmp, len, point;
	char locBuf[MAX_CMD_LEN];

	if (!QueueOk || (pos >= MAX_CMDS))
	{
		return(NULL);
	}
	if (CmdArray[pos].pri == QUE_AVAIL)
	{
		return(NULL);
	}
	return(CmdArray[pos].cmdbuf);
}

/*
 * getPrior - returns the priority for the given queue position, or
 *          -1 if error
 */
int getPrior(int pos)
{
	int tmp, len, point;
	char locBuf[MAX_CMD_LEN];

	if (!QueueOk || (pos >= MAX_CMDS))
	{
		return(-1);
	}
	return(CmdArray[pos].pri);
}

