#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <netinet/in.h>
#include "../include/lcd_disp.h"
#include "../include/ident.h"
#include "../include/messages.h"
#include "../include/log.h"
#include "../include/server.h" // Include server.h for get_ip function
#include "../include/utils.h"




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
        char weekday_str[20];
        char cur_time[10];
        
        get_short_weekday(weekday_str);
        get_time(cur_time);
        strip_newline(args);

        char *ip;
        ip = get_ip(cli); // Call get_ip to retrieve client's IP address

        log_message(ip, cli->nickname, args);

        char lcd_message[190];
        snprintf(lcd_message, sizeof(lcd_message), "%s %s %s:", weekday_str,cur_time, cli->nickname);

        lcd_clear();
        lcd_text(lcd_message, 1, LEFT);
        lcd_text(args, 2, LEFT);
    }
}
