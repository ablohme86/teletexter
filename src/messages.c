#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include "../include/lcdlib.h"
#include "../include/ident.h"
#include "../include/messages.h"

void get_current_time(char current_time[15])
{
	time_t rawtime;
	struct tm *timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(current_time, 15, "%H:%M %a", timeinfo);
}

void handle_msg(client_t *cli, char *args)
{
	if (strlen(cli->nickname) == 0)
	{
		char *error_msg = "No nickname is set! Please IDENT first!\n";
		send(cli->socket, error_msg, strlen(error_msg), 0);
		return;
	}

	if (args != NULL)
	{
		char current_time_str[20];

		get_current_time(current_time_str);

		strip_newline(args);

		printf("Message from %s: %s\n", cli->nickname, args);

		char lcd_message[190];
		snprintf(lcd_message, sizeof(lcd_message), "%s %s:", current_time_str, cli->nickname);

		lcd_clear();
		lcd_text(lcd_message, 1, LEFT);
		lcd_text(args, 2, LEFT);
	}
}
