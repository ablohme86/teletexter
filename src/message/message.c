//
// Created by Alexander Blohme on 18/06/2024.
//
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "../../include/config/config.h"
#include "../../include/message/message.h"
#include "../../include/lcd/lcd_disp.h"
#include "../../include/db/db_handler.h"
#include "../../include/utils.h" 
#include "../../include/log.h"
#include "../../include/status.h"


typedef struct {
    int line;
    char buff[MAX_LINE_MSG_LENGTH];

} MessageLine;
MessageLine *messageLines[128]; // stores all lines string data thats on display
Message currentMessage;   // message on display

int save_message(Message *msg)
{
    char sql[1024];
    int res;

    if (msg->id < 1)
    {
        snprintf(sql, sizeof(sql), "INSERT INTO messages (datetime, message, poster_id, status) VALUES (?, ?, ?, ?)");
        log_sys_message("[%s] New message, inserting into db...", MSG_INTERFACE);
        res = execute_sql(sql, "isii", 4, msg->datetime, msg->message, msg->poster_id, msg->status);
    }
    else
    {
        snprintf(sql, sizeof(sql), "UPDATE messages SET datetime = ?, message = ?, poster_id = ?, status = ? WHERE id = ?");
        log_sys_message("[%s] Updating message id %d",MSG_INTERFACE, msg->id);
        res = execute_sql(sql, "isiii", 5, msg->datetime, msg->message, msg->poster_id, msg->status, msg->id);
    }

    return res;
}


int new_message(char *msg, User *user)
{
    char weekday_str[5];
    char cur_time[8];
    char top_line_msg[config.lcdConfig.lcdWidth+1]; // For å unngå scrolling
    
    get_short_weekday(weekday_str);
    get_time(cur_time);

    snprintf(top_line_msg, sizeof(top_line_msg), "%s %s %s:", weekday_str,cur_time, user->username);
    Message *new_msg = (Message *)calloc(1,sizeof(Message)); // bruker calloc for å forsikre om at alle variabler er nullstilt

    new_msg->datetime = get_unixtime();
    printf("Current time is: %d", new_msg->datetime);
    strcpy(new_msg->message, msg);
    new_msg->poster_id = user->id;
    save_message(new_msg);

    currentMessage = *new_msg;
    //free(new_msg);
#ifndef DISABLE_LCD
    lcd_clear();
    set_line_text(top_line_msg, 1, "LEFT");   
    set_line_text(msg,2,"LEFT");
#endif
    return MESSAGE_SET;
}

int scroll_message(int sel_line)    // Scrolls the selected line
{
    if (messageLines[sel_line] == NULL)
    {
        return 0;
    }
#ifndef DISABLE_LCD
    lcd_scroll(messageLines[sel_line]->buff,sel_line,config.messageConfig.scrollSpeed);
#endif
    return 0;
}

void clear_line(int line)
{
    set_line_text("",line,LEFT);
}

int set_line_text(const char *msg, unsigned int line,const char *align)
{
    if (line > config.lcdConfig.lcdWidth)
    {
        return INVALID_LINE;    // The set line is higher than available lines
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
    


    strcpy(messageLines[line]->buff, msg);

  
#ifndef DISABLE_LCD
#ifdef DEBUG
    fprintf(stdout,"Running lcd_text!... ");
#endif

    lcd_text(msg,line,r_align);
    #ifdef DEBUG
        fprintf(stdout,"DONE!\n");
    #endif

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

    return MESSAGE_SET;
}



int message_callback(void *data, int argc, char **argv, char **azColName)
{
    Message *message = (Message *)data;
    for (int i = 0; i < argc; i++) 
    {
        if (strcmp(azColName[i], "id") == 0) 
        {
            message->id = atoi(argv[i]);
        }
        else if (strcmp(azColName[i], "datetime") == 0)
        {
            message->datetime = atoi(argv[i]);
        }
        else if (strcmp(azColName[i], "poster_id") == 0)
        {
            message->poster_id = atoi(argv[i]);
        }
        else if (strcmp(azColName[i], "status") == 0)
        {
            message->status = atoi(argv[i]);
        }
        else if (strcmp(azColName[i], "message") == 0)
        {
            strncpy(message->message, argv[i], sizeof(message->message) - 1);
            message->message[sizeof(message->message) - 1] = '\0';  // Ensure null-termination
        }
    }
    return 0;
}
