#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../include/lcd_disp.h"
#include "../include/messages.h"
#include "../include/log.h"
#include "../include/server.h" // Include server.h for get_ip function
#include "../include/utils.h"
#include "../include/client.h"
#include "../include/config.h"

void handle_msg_custom(client_t *cli,int argc, char **argv)
{
    (void) argc;
    (void) argv;
    char *inc_align = argv[0];
    char *inc_line = argv[1];
    int align = 0;
    int p_line;
    if (is_numerical_cpy(inc_line,&p_line))       // pass p_line's pointer addr to function so it will edit it!
    {
            
    }
    else
    {
        log_sys_message("[%s] Specified LINE number '%s' is not specified or not a number",get_ip(cli) , inc_line);
        bad_status(cli,LINE_ARG_NOT_NUMBER,"Provided LINE was not a number!");
        return;
    }


    char *inc_msg = argv[2];


    if (is_identified(cli) == 1)
    {

        if (strcmp(inc_align,"LEFT") == 0)
        {
            align = LEFT;
        }
        else if (strcmp(inc_align,"RIGHT") == 0)
        {
            align = RIGHT;
        }
        else if (strcmp(inc_align,"CENTER") == 0)
        {
            align = CENTER;
        }
        else
        {
            bad_status(cli,WRONG_ALIGNMENT,"Use 'LEFT', 'RIGHT' or 'CENTER'");
            return;
        }
    }
    else
    {
        log_sys_message("[%s] Attempted to use MSG_CUSTOM without being identified",get_ip(cli));
        return;
    }

#ifndef DISABLE_LCD
    lcd_text(inc_msg,p_line,align);
    #endif
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
        log_sys_message("[%s] %s cleared display", get_ip(cli), cli->nickname);
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
        char weekday_str[20];
        char cur_time[10];
        char full_message[100];
        char top_line_msg[100];

        strncpy(full_message,argv[0], sizeof(full_message));

        get_short_weekday(weekday_str);
        get_time(cur_time);
        strip_newline(argv[0]);
        printf("Sending this message to LcD display: %s\n", argv[0]);

        char *ip;
        ip = get_ip(cli); // Call get_ip to retrieve client's IP address

        log_message(ip, cli->nickname, argv[0]);



#ifndef DISABLE_LCD
        snprintf(top_line_msg, sizeof(top_line_msg), "%s %s %s:", weekday_str,cur_time, cli->nickname);

        lcd_clear();
        lcd_text(top_line_msg, 1, LEFT);
        if (strlen(full_message) > lcd_width)
        {
            if (config.messageConfig.scrollLongMessages == 1)
            {
                lcd_scroll(full_message,2, config.messageConfig.scrollSpeed);

            }
        }
        lcd_text(full_message,2,LEFT);


#endif
    }
}
