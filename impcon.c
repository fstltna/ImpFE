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
static BOOL DoingLogin=FALSE;
static BOOL LoseNextChar=FALSE;

/* These get set from our config files
char game_host[]="localhost";
int game_port=DEF_PORT;
char game_player[]="unknown";
char game_player_pswd[]="unknown";
char game_desc[]="unknown";
int game_time=0;
int game_btus=0; */

/*
 * call_cocket - Attempts to connect to a remote socket
 *               returns the socket number if succeeds, or -1 if fails
 */
int call_socket(char *hostname, int portnum)
{
	struct sockaddr_in sa;
	struct hostent *hp;
	int a, s;

	if ((hp=gethostbyname(hostname)) == NULL)
	{
		errno=ECONNREFUSED;
		return(-1);
	}

	bzero(&sa, sizeof(sa));
	bcopy(hp->h_addr, (char *)&sa.sin_addr, hp->h_length * sizeof(char));
	sa.sin_family=hp->h_addrtype;
	sa.sin_port=htons((unsigned short) portnum);

	if ((s=socket(hp->h_addrtype, SOCK_STREAM, 0)) < 0) /* Get socket */
	{
		return(-1);
	}
	if (connect(s, (struct sockaddr *) &sa, sizeof(sa)) < 0)
	{
		close(s);
		return(-1);
	}
	return(s);
}

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

#define SERV_BUF_LEN	2048
static char ServBuf[SERV_BUF_LEN+1];
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
		    /* Parse it */
		    fe_parse(ServBuf);
		    ServBuf[0]='\0';
		}
		else {
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
		/* Close the socket */
		close(GameSocket);
		GameSocket=-1;
		ServBuf[0]='\0';
		ui_menu_enable(fd_ImpFeMain->MainServerMenu,
							  MAIN_SERVER_MENU_CLOSE, FALSE);
		ui_menu_enable(fd_ImpFeMain->MainServerMenu,
							  MAIN_SERVER_MENU_OPEN, TRUE);
		ui_set_text(fd_ImpFeMain->StatusBox, "Offline");
		fe_puts("*** Connection to server terminated...\n");
		AtMainPrompt=FALSE;
		return(FALSE);
	}
	return(TRUE);
}

/*
 * cmd_connect
 */
BOOL cmd_connect(BOOL kill_it)
{
	/* Are we trying to stop the server? */
	if (kill_it)
	{
		/* Make sure the socket was valid */
		if (GameSocket != -1)
		{
			/* Remove the callback */
			stopServWatch();
			/* Close the socket */
			close(GameSocket);
			GameSocket=-1;
			ServBuf[0]='\0';
			fe_puts("*** Connection to server closed\n");
		        AtMainPrompt=FALSE;
		}
		return(TRUE);
	}
	ui_browser_append(fd_ImpFeMain->MainBrowser, "*** Attempting to connect to ");
	ui_browser_append(fd_ImpFeMain->MainBrowser, game_host);
	ui_browser_append(fd_ImpFeMain->MainBrowser, ", port ");
	ui_browser_append(fd_ImpFeMain->MainBrowser, game_port);
	ui_browser_append(fd_ImpFeMain->MainBrowser, "\n");
	if ((GameSocket=call_socket(game_host, atoi(game_port))) == -1)
	{
		fe_puts("**** Connection failed\n");
		AtMainPrompt=FALSE;
		return(FALSE);
	}
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
	return(TRUE);
}
