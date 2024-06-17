#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../include/lcd_disp.h"
#include "../include/messages.h"
#include "../include/log.h"
#include "../include/server.h" // Include server.h for get_ip function
#include "../include/utils.h"

void handle_msg_custom(client_t *cli, char *args)
{
    char r_msg[BUFFER_SIZE];

    if (cli->identified)
    {
        
    }
    else
    {
        snprintf(r_msg,sizeof(r_msg), "ACCESS_DENIED MSG_CUSTOM\n");
        log_sys_message("[%s] tried to set custom message without proper access", get_ip(cli));
        send(cli->socket,r_msg,strlen(r_msg), 0);
    }
}

void handle_clear_display(client_t *cli, char *args)
{
    (void) args;
    char r_msg[BUFFER_SIZE];

    if (cli->identified)
    {
#ifndef DISABLE_LCD
        lcd_clear();
#endif

        snprintf(r_msg,sizeof(r_msg), "DISPLAY_CLEARED\n");
        log_sys_message("[%s] %s cleared display", get_ip(cli), cli->nickname);
        send(cli->socket,r_msg,strlen(r_msg), 0);
    }
    else
    {
        snprintf(r_msg,sizeof(r_msg), "ACCESS_DENIED CLEAR_DISPLAY\n");
        log_sys_message("[%s] tried to clear display without proper access", get_ip(cli));
        send(cli->socket,r_msg,strlen(r_msg), 0);
    }

}

void handle_msg(client_t *cli, char *args)
{
    if (cli->identified == 0)
    {
        char *error_msg = "NOT_IDENTIFIED\n";
        log_sys_message("[%s] Client cannot send MSG, not identified yet!", get_ip(cli));
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

#ifndef DISABLE_LCD
        snprintf(lcd_message, sizeof(lcd_message), "%s %s %s:", weekday_str,cur_time, cli->nickname);

        lcd_clear();
        lcd_text(lcd_message, 1, LEFT);
        lcd_text(args, 2, LEFT);
#endif
    }
}
