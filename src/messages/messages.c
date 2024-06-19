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


typedef struct {
    int line;
    char buff[MAX_LINE_MSG_LENGTH];

} MessageLine;
MessageLine *messageLines[128]; // maximum 129 supported lines...



int new_message(char *msg, User *user)
{
    char weekday_str[5];
    char cur_time[8];
    char top_line_msg[config.lcdConfig.lcdWidth+1]; // For å unngå scrolling
    
    get_short_weekday(weekday_str);
    get_time(cur_time);

    snprintf(top_line_msg, sizeof(top_line_msg), "%s %s %s:", weekday_str,cur_time, user->username);
    Message *new_msg = (Message *)calloc(1, sizeof(Message)); // bruker calloc for å forsikre om at alle variabler er nullstilt

    get_date(new_msg->date);
    get_time(new_msg->time);
    strcpy(new_msg->message, msg);
    new_msg->poster_id = user->id;
    db_save_message(new_msg);



#ifndef DISABLE_LCD
    lcd_clear();
    set_line_text(top_line_msg, 1, LEFT);   
    set_line_text(msg,2,LEFT);
#endif
    return MSG_SET_OK;
}

int scroll_message(int sel_line)    // Scrolls the selected line
{
    if (messageLines[sel_line] == NULL)
    {
        return NO_MESSAGE_SET;
    }
    lcd_scroll(messageLines[sel_line]->buff,sel_line,config.messageConfig.scrollSpeed);
    return 0;
}


int set_line_text(const char *msg, unsigned int line,const char *align)
{
    if (line > config.lcdConfig.lcdWidth)
    {
        return LINE_NOT_AVAILABLE;    // The set line is higher than available lines
    }
    if (messageLines[line] == NULL)
    {
        // allokér minne til linjens peker
        messageLines[line] = (MessageLine *)malloc(sizeof(MessageLine));
        messageLines[line]->line = line;
    
    }

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

    if (line == 0)
    {
        return LINE_CANNOT_BE_ZERO;   // Line cannot be zero!
    }

    strcpy(messageLines[line]->buff, msg);
#ifndef DISABLE_LCD
    lcd_text(msg,line,r_align);
#endif
    if (strlen(msg) > config.lcdConfig.lcdWidth)
    {
        if (config.messageConfig.scrollLongMessages == 1)
        {
#ifndef DISABLE_LCD
            scroll_message(line);
#endif

        }
    }

    return MSG_SET_OK;
}