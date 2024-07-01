//
// Created by Alexander Blohme on 26/06/2024.
//
#include "../../include/server/handle_lcdtxt.h"
#include "../../include/lcd/lcd_txt.h"
#include "../../include/lcd/lcd_disp.h"
#include "../../include/server/handle_msg.h"
#include "../../include/log.h"
#include "../../include/config/config.h"
#include "../../include/server/server.h"
#include "../../include/utils.h"
#include "../../include/server/handle_client.h"
#include "../../include/status.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

void handle_lcd_set_text(client_t *cli,int argc, char **argv)
{
    (void) argc;
    (void) argv;
    char *inc_align = argv[0];
    char *inc_line = argv[1];
    int p_line;
    if (is_numerical_cpy(inc_line,&p_line) != 1)       // pass p_line's pointer addr to function so it will edit it!
    {
        log_sys_message("[%s] %s Specified LINE number '%s' is not specified or not a number",SCK_INTERFACE,get_ip(cli) , inc_line);
        bad_status(cli,INVALID_LINE,"Provided LINE was not a number!");
        return;
    }
    char *inc_msg = argv[2];
    if (is_identified(cli) == 1)
    {
    #ifndef DISABLE_LCD
        int set_msg_status = set_lcd_line_text(inc_msg,p_line,inc_align);
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
        #endif

    }
    else
    {
        bad_status(cli,ACCESS_DENIED,"Please IDENT first!");
        log_sys_message("[%s] %s Attempted to use MSG_CUSTOM without being identified",SCK_INTERFACE,get_ip(cli));

    }
}
void handle_lcd_line_scroll(client_t *cli, int argc, char **argv)
{
    (void)argc;
    unsigned int p_line;

    if (!is_numerical_cpy(argv[0], (int*)&p_line) || p_line < 1 || p_line > config.lcdConfig.lcdHeight)
    {
        bad_status(cli,INVALID_LINE,"Invalid line number!");
        return;
    }
    #ifndef DISABLE_LCD
    scroll_lcd_line(p_line);
    ok_status(cli,MESSAGE_SET,"Scrolling line");
    #endif
    return;
}


void handle_clear_lcd_display(client_t *cli, int argc, char **argv)
{
    (void) argc;
    (void) argv;
    #ifndef DISABLE_LCD
    clear_lcd_lines();
    #endif
    log_sys_message("[%s] %s %s cleared display",SCK_INTERFACE,cli->ipv4addr, cli->user->username);
    ok_status(cli,CLEAR_DISPLAY,"LCD is clean as a whistle!");

}
void handle_clear_lcd_line(client_t *cli, int argc, char **argv)
{
    (void)argc;

    char *inc_line = argv[0];
    int p_line;

    if (is_numerical_cpy(inc_line,&p_line) != 1)
    {
        bad_status(cli,INVALID_LINE,"Cannot clear line, invalid line number!");
        log_sys_message("[%s] %s %s provided invalid line for clearing",SCK_INTERFACE, get_ip(cli), cli->user->username);
        return;
    }
    #ifndef DISABLE_LCD
    clear_lcd_line(p_line);
    #endif
    char replybuff[100];
    snprintf(replybuff,sizeof(replybuff),"Line %d was successfully cleared!", p_line);
    ok_status(cli,LINE_CLEARED,replybuff);

}
