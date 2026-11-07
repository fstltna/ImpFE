#ifndef LINT
static char rcsid[]="$Id: impcon.c,v 1.2 2000/05/24 21:51:39 marisa Exp $";
#endif
/*************************************************************
 *  $Id: impcon.c,v 1.2 2000/05/24 21:51:39 marisa Exp $
 *************************************************************/

#include "config.h"
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <stdlib.h>
#include "impfe.h"
#include "ImpFeGui.h"
#include "gui.h"

static guint ServWatch=0;	/* GLib watch on the game socket */

/* Connection states, shown in the Server menu and status box */
typedef enum { CONN_OFFLINE, CONN_CONNECTING, CONN_CONNECTED } ConnState_t;
static void setConnState(ConnState_t state);
#define SERV_BUF_LEN	2048
static char ServBuf[SERV_BUF_LEN+1];
static BOOL DoingLogin=FALSE;
static BOOL LoseNextChar=FALSE;

/*
 * The server asks some questions (creating a new player, for example)
 * with a prompt that has no newline after it. If part of a line arrives
 * and then nothing more for this long, it is shown as a question.
 */
#define PROMPT_DELAY_MS 250
static guint PromptTimer=0;

static void stopPromptTimer(void)
{
	if (PromptTimer != 0)
	{
		g_source_remove(PromptTimer);
		PromptTimer=0;
	}
}

/*
 * showPartialPrompt - the server has gone quiet part way through a line,
 *                     so it must be waiting for an answer
 */
static gboolean showPartialPrompt(gpointer unused)
{
	PromptTimer=0;
	/* Lines starting with '!' are for ImpFE and always end in a newline */
	if ((ServBuf[0] != '\0') && (ServBuf[0] != '!'))
	{
		ui_browser_append(fd_ImpFeMain->MainBrowser, ServBuf);
		showServerQuestion(ServBuf);
		ServBuf[0]='\0';
	}
	return(G_SOURCE_REMOVE);
}

/* These get set from our config files
char game_host[]="localhost";
int game_port=DEF_PORT;
char game_player[]="unknown";
char game_player_pswd[]="unknown";
char game_desc[]="unknown";
int game_time=0;
int game_btus=0; */

/*
 * write_data - writes data to the socket...
 */
int write_data(int sock, const char *buf, int len)
{
	int bcount; /* Bytes written */
	int bw;     /* Bytes written this pass */

	bcount = 0;
	bw=0;
	while (bcount < len)
	{
		if ((bw=write(sock, buf, len-bcount)) > 0)
		{
			bcount += bw;
			buf += bw;
		}
		if (bw < 0)
		{
			return(-1);
		}
	}
	return(bcount);
}

/*
 * write_str - writes a C string to the game socket
 */
void write_str(const char *buf)
{
    if (write_data(GameSocket, buf, strlen(buf) * sizeof(char)) == -1)
    {
        fe_puts("*** error returned from write_data\n");
    }
}

/*
 * read_data - reads data from the game socket
 */
int read_data(int sock, char *buf, int len)
{
	int bcount; /* Bytes read */
	int br;     /* Bytes read this pass */

	bcount = 0;
	br=0;
	while (bcount < len)
	{
		if ((br=read(sock, buf, len-bcount)) > 0)
		{
			bcount += br;
			buf += br;
		}
		if (br < 0)
		{
			return(-1);
		}
	}
	return(bcount);
}

/*
 * stopServWatch - stops watching the game socket for input
 */
static void stopServWatch(void)
{
	if (ServWatch != 0)
	{
		g_source_remove(ServWatch);
		ServWatch=0;
	}
}

/*
 * handleServInp - called from the main loop when the game socket has
 *                 something for us. Returns FALSE once the connection
 *                 has gone, which removes the watch.
 */
static gboolean handleServInp(GIOChannel *chan, GIOCondition cond, gpointer data)
{
	int br;
	char locBuf[]="\0\0";

	if ((br=read(GameSocket, &locBuf[0], 1)) == 1)
	{
		if (LoseNextChar)
		{
			LoseNextChar=FALSE;
			return(TRUE);
		}
		/* Don't overrun the buffer on absurdly long lines */
		if (strlen(ServBuf) >= SERV_BUF_LEN)
		{
			fe_parse(ServBuf);
			ServBuf[0]='\0';
		}
		strcat(&ServBuf[0], &locBuf[0]);
		/* Check for Newline */
		if (locBuf[0] == '\n')
		{
		    stopPromptTimer();
		    /* The first ImpFE prompt means we are logged in */
		    if ((ServBuf[0] == '!') && (ServBuf[1] == '('))
		    {
				DoingLogin=FALSE;
		    }
		    /* Parse it */
		    fe_parse(ServBuf);
		    ServBuf[0]='\0';
		}
		else {
			/* Show it as a question if nothing more comes for a moment */
			stopPromptTimer();
			PromptTimer=g_timeout_add(PROMPT_DELAY_MS, showPartialPrompt, NULL);
			/* Check for a prompt line */
			if (DoingLogin)
			{
				if ((locBuf[0] == ':') && (ServBuf[0] == ':') &&
					(ServBuf[1] != '\0'))
				{
					ui_browser_append(fd_ImpFeMain->MainBrowser, ServBuf);
					if (strcmp(ServBuf, ":Enter player name:") == 0)
					{
						LoseNextChar=TRUE;
						if (game_player[0] != '\0')
						{
							write_str("!");
							write_str(game_player);
							write_str("\n");
							ui_browser_append(fd_ImpFeMain->MainBrowser,
												   game_player);
							ui_browser_append(fd_ImpFeMain->MainBrowser,
												   "\n");
						}
						else
						{
							/* They have to type it in */
							setInputMode(INPUT_PLAYER_NAME);
							showServerQuestion(ServBuf);
						}
					}
					else if (strcmp(ServBuf, ":Enter player password:") == 0)
					{
						LoseNextChar=TRUE;
						if (game_player_pswd[0] != '\0')
						{
							write_str(game_player_pswd);
							write_str("\n");
							/* Don't echo the password itself */
							ui_browser_append(fd_ImpFeMain->MainBrowser,
												   "********\n");
						}
						else
						{
							/* They have to type it in */
							showServerQuestion(ServBuf);
						}
						DoingLogin=FALSE;
					}
					ServBuf[0]='\0';
				}
			}
		}
	}
	else
	{
		/* The watch is removed when we return FALSE */
		ServWatch=0;
		stopPromptTimer();
		setInputMode(INPUT_NORMAL);
		/* Close the socket */
		close(GameSocket);
		GameSocket=-1;
		ServBuf[0]='\0';
		setConnState(CONN_OFFLINE);
		fe_puts("*** Connection to server terminated...\n");
		AtMainPrompt=FALSE;
		return(FALSE);
	}
	return(TRUE);
}

/* Give up on a connection attempt (name lookup + connect) after this */
#define CONNECT_TIMEOUT_SECS 20

static GCancellable *ConnCancel=NULL;	/* set while a connect is in progress */
static guint ConnTimer=0;		/* timeout for that attempt */
static BOOL ConnTimedOut=FALSE;

/*
 * setConnState - updates the Server menu and the status box
 */
static void setConnState(ConnState_t state)
{
	ui_menu_enable(fd_ImpFeMain->MainServerMenu, MAIN_SERVER_MENU_OPEN,
		state == CONN_OFFLINE);
	/* Close also cancels a connection attempt */
	ui_menu_enable(fd_ImpFeMain->MainServerMenu, MAIN_SERVER_MENU_CLOSE,
		state != CONN_OFFLINE);
	switch (state)
	{
		case CONN_CONNECTING:
			ui_set_text(fd_ImpFeMain->StatusBox, "Connecting...");
			break;
		case CONN_CONNECTED:
			ui_set_text(fd_ImpFeMain->StatusBox, "CONNECTED");
			break;
		default:
			ui_set_text(fd_ImpFeMain->StatusBox, "Offline");
			break;
	}
}

static gboolean connectTimeout(gpointer unused)
{
	ConnTimer=0;
	ConnTimedOut=TRUE;
	if (ConnCancel != NULL)
	{
		g_cancellable_cancel(ConnCancel);
	}
	return(G_SOURCE_REMOVE);
}

/*
 * connectDone - called when a connection attempt finishes, one way or
 *               the other
 */
static void connectDone(GObject *client, GAsyncResult *res, gpointer unused)
{
	GSocketConnection *conn;
	GError *err=NULL;
	int fd, flags;

	conn=g_socket_client_connect_to_host_finish(G_SOCKET_CLIENT(client), res,
		&err);
	if (ConnTimer != 0)
	{
		g_source_remove(ConnTimer);
		ConnTimer=0;
	}
	g_clear_object(&ConnCancel);

	if (conn == NULL)
	{
		if (ConnTimedOut)
		{
			char msg[64];

			g_snprintf(msg, sizeof(msg),
				"**** Connection failed: no answer after %d seconds\n",
				CONNECT_TIMEOUT_SECS);
			fe_puts(msg);
		}
		else if (g_error_matches(err, G_IO_ERROR, G_IO_ERROR_CANCELLED))
		{
			fe_puts("*** Connection attempt cancelled\n");
		}
		else
		{
			fe_puts("**** Connection failed: ");
			fe_puts(err->message);
			fe_puts("\n");
		}
		g_clear_error(&err);
		AtMainPrompt=FALSE;
		setConnState(CONN_OFFLINE);
		return;
	}

	/*
	 * The rest of ImpFE uses a plain blocking file descriptor, so take
	 * our own copy of it and let GLib's socket objects go.
	 */
	fd=dup(g_socket_get_fd(g_socket_connection_get_socket(conn)));
	g_object_unref(conn);
	if (fd == -1)
	{
		fe_puts("**** Connection failed: ");
		fe_puts(strerror(errno));
		fe_puts("\n");
		setConnState(CONN_OFFLINE);
		return;
	}
	flags=fcntl(fd, F_GETFL);
	if (flags != -1)
	{
		(void) fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
	}
	GameSocket=fd;

	/* Add in the callback for the socket */
	{
		GIOChannel *chan = g_io_channel_unix_new(GameSocket);

		ServWatch = g_io_add_watch(chan, G_IO_IN | G_IO_HUP | G_IO_ERR,
			handleServInp, NULL);
		g_io_channel_unref(chan);
	}
	ServBuf[0]='\0';
	LoseNextChar=FALSE;
	DoingLogin=TRUE;
	setConnState(CONN_CONNECTED);
}

/*
 * cmd_connect - starts connecting to the server (kill_it FALSE), or
 *               closes the connection or cancels the attempt (kill_it
 *               TRUE). Connecting happens in the background; the
 *               result is reported in the main window.
 */
BOOL cmd_connect(BOOL kill_it)
{
	GSocketClient *client;
	long port;
	char *end;

	/* Are we trying to stop the server? */
	if (kill_it)
	{
		/* Still trying to connect? */
		if (ConnCancel != NULL)
		{
			g_cancellable_cancel(ConnCancel);
			return(TRUE);
		}
		/* Make sure the socket was valid */
		if (GameSocket != -1)
		{
			/* Remove the callback */
			stopServWatch();
			stopPromptTimer();
			setInputMode(INPUT_NORMAL);
			/* Close the socket */
			close(GameSocket);
			GameSocket=-1;
			ServBuf[0]='\0';
			fe_puts("*** Connection to server closed\n");
		        AtMainPrompt=FALSE;
		}
		setConnState(CONN_OFFLINE);
		return(TRUE);
	}

	/* Already connected, or on the way? */
	if ((GameSocket != -1) || (ConnCancel != NULL))
	{
		return(FALSE);
	}
	if (game_host[0] == '\0')
	{
		fe_puts("**** No server set - use Configure / Host first\n");
		return(FALSE);
	}
	port=strtol(game_port, &end, 10);
	if ((end == game_port) || (*end != '\0') || (port < 1) || (port > 65535))
	{
		fe_puts("**** The server port '");
		fe_puts(game_port);
		fe_puts("' is not valid - use Configure / Host to fix it\n");
		return(FALSE);
	}

	ui_browser_append(fd_ImpFeMain->MainBrowser, "*** Attempting to connect to ");
	ui_browser_append(fd_ImpFeMain->MainBrowser, game_host);
	ui_browser_append(fd_ImpFeMain->MainBrowser, ", port ");
	ui_browser_append(fd_ImpFeMain->MainBrowser, game_port);
	ui_browser_append(fd_ImpFeMain->MainBrowser, "\n");

	ConnCancel=g_cancellable_new();
	ConnTimedOut=FALSE;
	ConnTimer=g_timeout_add_seconds(CONNECT_TIMEOUT_SECS, connectTimeout, NULL);
	client=g_socket_client_new();
	g_socket_client_connect_to_host_async(client, game_host, (guint16) port,
		ConnCancel, connectDone, NULL);
	/* The pending operation holds its own reference */
	g_object_unref(client);
	setConnState(CONN_CONNECTING);
	return(TRUE);
}
