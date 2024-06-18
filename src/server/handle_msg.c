#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../../include/lcd/lcd_disp.h"
#include "../../include/server/handle_msg.h"
#include "../../include/log.h"
#include "../../include/server/server.h"
#include "../../include/utils.h"
#include "../../include/server/handle_client.h"
#include "../../include/config/config.h"
#include "../../include/messages/messages.h"

void handle_msg_custom(client_t *cli,int argc, char **argv)
{
    (void) argc;
    (void) argv;
    char *inc_align = argv[0];
    char *inc_line = argv[1];
    int p_line;
    if (is_numerical_cpy(inc_line,&p_line) != 1)       // pass p_line's pointer addr to function so it will edit it!
    {
        log_sys_message("[%s] Specified LINE number '%s' is not specified or not a number",get_ip(cli) , inc_line);
        bad_status(cli,LINE_ARG_NOT_NUMBER,"Provided LINE was not a number!");
        return;
    }
    char *inc_msg = argv[2];
    if (is_identified(cli) == 1)
    {
        int set_msg_status = set_text(inc_align,inc_msg,p_line,cli);
        switch (set_msg_status)
        {
            case LINE_NOT_AVAILABLE:
                bad_status(cli,LINE_NOT_AVAILABLE,"LINE_NOT_AVAILABLE");
                break;
            case INVALID_ALIGN:
                break;
                bad_status(cli,INVALID_ALIGN,"INVALID_ALIGN");
            case LINE_CANNOT_BE_ZERO:
                bad_status(cli,LINE_CANNOT_BE_ZERO,"LINE_CANNOT_BE_ZERO");
                break;
            case MSG_SET_OK:
                ok_status(cli,MSG_SET_OK,"MSG_SET_OK");
                break;
        };

    }
    else
    {
        log_sys_message("[%s] Attempted to use MSG_CUSTOM without being identified",get_ip(cli));
        return;
    }
}

void handle_clear_display(client_t *cli, int argc, char **argv)
{
    (void) argc;
    (void) argv;

    if (cli->identified)
    {
#ifndef DISABLE_LCD
        lcd_clear();
#endif
        log_sys_message("[%s] %s cleared display",cli->ipv4addr, cli->user->username);
        ok_status(cli,LCD_CLEARED,"LCD is clean as a whistle!");
    }
    else
    {
        log_sys_message("[%s] tried to clear display without proper access", get_ip(cli));
        bad_status(cli,ACCESS_DENIED,NULL);
    }

}

void handle_msg(client_t *cli, int argc, char **argv)
{
    if (is_identified(cli) == 0)
    {
        log_sys_message("[%s] Client cannot send MSG, not identified yet!", get_ip(cli));
        return;
    }


    if (argc > 0)
    {
        char full_message[100];


        strncpy(full_message,argv[0], sizeof(full_message));
        strip_newline(full_message);
        new_message(full_message,cli);
    }
}
