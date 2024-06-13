#include "../include/ident.h"
#include <stdio.h>
#include <string.h>

void handle_ident(client_t *cli, char *args)
{
	if (args != NULL)
	{
		strncpy(cli->nickname, args, sizeof(cli->nickname) - 1);
		cli->nickname[sizeof(cli->nickname) - 1] = '\0';
		strip_newline(cli->nickname);
		printf("Client identified as: '%s'\n", cli->nickname);
	}
}

void strip_newline(char *str)
{
	char *p = str;
	while (*p)
	{
		if (*p == '\r' || *p == '\n')
		{
			*p = '\0';
			break;
		}
		p++;
	}
}
