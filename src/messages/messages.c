//
// Created by Alexander Blohme on 18/06/2024.
//
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "../../include/config/config.h"
#include "../../include/messages/messages.h"
#include "../../include/lcd/lcd_disp.h"
#include "../../include/db/db_handler.h"
#include "../../include/utils.h" 
#include "../../include/server/handle_client.h"



int new_message(char *msg, client_t *sender)
{
    char weekday_str[5];
    char cur_time[8];
    char top_line_msg[config.lcdConfig.lcdWidth];

    get_short_weekday(weekday_str);
    get_time(cur_time);

    snprintf(top_line_msg, sizeof(top_line_msg), "%s %s %s:", weekday_str,cur_time, sender->user->username);
    Message *new_msg = (Message *)malloc(sizeof(Message)); // allocate memory for the new User object

    get_date(new_msg->date);
    get_time(new_msg->time);
    strcpy(new_msg->message, msg);
    new_msg->poster_id = sender->user->id;

    db_create_message(new_msg);



#ifndef DISABLE_LCD
    lcd_clear();
    lcd_text(top_line_msg, 1, LEFT);
#endif


    if (strlen(msg) > config.lcdConfig.lcdWidth)
    {
        if (config.messageConfig.scrollLongMessages == 1)
        {
#ifndef DISABLE_LCD
            lcd_scroll(msg,2, config.messageConfig.scrollSpeed);
#endif

        }
    }
#ifndef DISABLE_LCD
    lcd_text(msg,2,LEFT);
#endif
    return 0;
}


int set_text(const char *align, const char *msg, unsigned int line, client_t *sender)
{
    int r_align = 0;

    if (strcmp(align,"LEFT") == 0)
    {
        r_align = LEFT;
    }
    else if (strcmp(align,"RIGHT") == 0)
    {
        r_align = RIGHT;
    }
    else if (strcmp(align,"CENTER") == 0)
    {
        r_align = CENTER;
    }
    else
    {
        return INVALID_ALIGN;
    }
    if (line > config.lcdConfig.lcdWidth)
    {
        return LINE_NOT_AVAILABLE;    // The set line is higher than available lines
    }
    if (line == 0)
    {
        return LINE_CANNOT_BE_ZERO;   // Line cannot be zero!
    }

    if (strlen(msg) > config.lcdConfig.lcdWidth)
    {
        if (config.messageConfig.scrollLongMessages == 1)
        {
#ifndef DISABLE_LCD
            lcd_scroll(msg,line, config.messageConfig.scrollSpeed);
#endif

        }
    }
    else
    {
#ifndef DISABLE_LCD
        lcd_text(msg,line,r_align);
#endif
    }


    return MSG_SET_OK;


}