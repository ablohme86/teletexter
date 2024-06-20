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
#include "../../include/status.h"
#include "../../include/message/message.h"

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
        bad_status(cli,INVALID_LINE,"Provided LINE was not a number!");
        return;
    }
    char *inc_msg = argv[2];
    if (is_identified(cli) == 1)
    {
        int set_msg_status = set_line_text(inc_msg,p_line,inc_align);
        switch (set_msg_status)
        {
            case INVALID_LINE:
                bad_status(cli,INVALID_LINE,"LINE_NOT_AVAILABLE");
                break;
            case INVALID_ALIGN:
                bad_status(cli,INVALID_ALIGN,"INVALID_ALIGN");
            break;
            case MESSAGE_SET:
                ok_status(cli,MESSAGE_SET,"MSG_SET_OK");
                break;
        };


    }
    else
    {
        bad_status(cli,ACCESS_DENIED,"Please IDENT first!");
        log_sys_message("[%s] Attempted to use MSG_CUSTOM without being identified",get_ip(cli));

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
        ok_status(cli,CLEAR_DISPLAY,"LCD is clean as a whistle!");
    }
    else
    {
        log_sys_message("[%s] tried to clear display without proper access", get_ip(cli));
        bad_status(cli,ACCESS_DENIED,NULL);
    }
}
void handle_clear_line(client_t *cli, int argc, char **argv)
{
    char *inc_line = argv[0];
    int p_line;
    if (cli->identified)
    {
        if (is_numerical_cpy(inc_line,&p_line) != 1)
        {
            bad_status(cli,INVALID_LINE,"Cannot clear line, invalid line number!");
            log_sys_message("[%s] %s provided invalid line for clearing", get_ip(cli), cli->user->username);
            return;
        }
        clear_line(p_line);
        ok_status(cli,LINE_CLEARED,"Line %d was successfully cleared!", p_line);
    }
    else
    {
        bad_status(cli,ACCESS_DENIED,"You must IDENT first!");
    }
}

void handle_latest_msg(client_t *cli)   // handle LATEST_MSG call from client
{

}
void handle_next_msg(client_t *cli) // handle NEXT_MSG from client
{
}
void handle_set_msg_no(client_t *cli, int msg_no)
{
}
void handle_prev_msg(client_t cli)
{
}

void handle_msg(client_t *cli, int argc, char **argv)
{
    if (is_identified(cli) == 0)
    {
        log_sys_message("[%s] Client cannot send MSG, not identified yet!", get_ip(cli));
        bad_status(cli,ACCESS_DENIED,NULL);
        return;
    }
    if (argc > 0)
    {
        char full_message[100];


        strncpy(full_message,argv[0], sizeof(full_message));
        strip_newline(full_message);
        new_message(full_message,cli->user);
        ok_status(cli,MESSAGE_SET,"Message created!");
    }
}
