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
Message *currentMessage;   // message on display

int save_message(Message *msg)
{
    char sql[1024];
    int res;

    if (msg->id < 1)
    {
        snprintf(sql, sizeof(sql), "INSERT INTO messages (datetime, message, poster_id, status) VALUES (?, ?, ?, ?)");
        log_sys_message("[%s] New message, inserting into db...", MSG_INTERFACE);
        res = execute_sql(sql, "isii", 4, msg->datetime, msg->message, msg->poster_id, msg->status);
        msg->id = sqlite3_last_insert_rowid(db);
    }
    else
    {
        snprintf(sql, sizeof(sql), "UPDATE messages SET datetime = ?, message = ?, poster_id = ?, status = ? WHERE id = ?");
        log_sys_message("[%s] Updating message id %d",MSG_INTERFACE, msg->id);
        res = execute_sql(sql, "isiii", 5, msg->datetime, msg->message, msg->poster_id, msg->status, msg->id);
    }

    return res;
}
int delete_message() // Deletes the current message
{
   if (currentMessage == NULL)
   {
      return -1;
   }

   if (currentMessage->id < 1)
   {
      return -1;
   }
   char sql[1024];
   snprintf(sql,sizeof(sql),"DELETE FROM messages WHERE id = ?");
   int res = execute_sql(sql,"i",1,currentMessage->id);
   if (res == SQLITE_OK)
   {
      currentMessage = NULL;  // reset currentMessage pointer
      print_latest_msg();  // re-draw the new latest message in db
   }
   
   return res;
}

void print_message_object(Message *msg)           // Prints a "Message" object to screen
{
   if (msg->id == 0)
   {
      log_err_message("[%s] message object id was 0! Cannot set message!", MSG_INTERFACE);
      return;
   }
    // get the username of msg poster_id:
    User *user = (User *)calloc(sizeof(User), 1);
    get_user_by_id(msg->poster_id,user);

    char top_line_msg[config.lcdConfig.lcdWidth+1]; // For å unngå scrolling

    char datetime_short[15];
    print_datetime_short(msg->datetime,datetime_short);
    snprintf(top_line_msg, sizeof(top_line_msg), "%s %s:",datetime_short, user->username);
    currentMessage = msg;  // store the current message into memory


#ifndef DISABLE_LCD
    lcd_clear();
    set_line_text(top_line_msg, 1, "LEFT");
    set_line_text(msg->message,2,"LEFT");
#endif

}

int new_message(char *msg, User *user)    // Creates and stores a new message to database and prints it
{

   Message *new_msg = (Message *)calloc(1,sizeof(Message)); // bruker calloc for å forsikre om at alle variabler er nullstilt

    new_msg->datetime = get_unixtime();
    strcpy(new_msg->message, msg);
    new_msg->poster_id = user->id;
    save_message(new_msg);
    print_message_object(new_msg);
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
    set_line_text(" ",line,"LEFT");
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


        strncpy(messageLines[line]->buff,msg, sizeof(messageLines[line]->buff));
  
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

    return MESSAGE_SET;
}

void print_latest_msg()           // prints the latest message object in db!
{
    Message *msg = (Message *)calloc(sizeof(Message),1);
    get_latest_message_object(msg);
    print_message_object(msg);
}

int get_message_by_id(int id, Message *message)    // get a "Message" object by id
{
    char sql[256];
    snprintf(sql, sizeof(sql), "SELECT * FROM messages WHERE id = %d", id);
    return select_from_db(sql, message_callback, message);
}

int get_next_message_object(Message *message)
{
   if (currentMessage == NULL)
   {
      return -1;
   }
   else
   {
      char sql[256];
      snprintf(sql,sizeof(sql),"SELECT * FROM messages WHERE id > %d ORDER BY id ASC LIMIT 1",currentMessage->id);
      return select_from_db(sql,message_callback,message);
   }
}


int get_prev_message_object(Message *message)
{
   if (currentMessage == NULL)
   {
      return -1;
   }
   else
   {
      
      char sql[256];
      snprintf(sql,sizeof(sql),"SELECT * FROM messages WHERE id < %d ORDER BY id DESC LIMIT 1",currentMessage->id);
      return select_from_db(sql,message_callback,message);
   }
}


int get_latest_message_object(Message *message)      // gets the latest "Message" object in db!
{
    char sql[256];
    snprintf(sql, sizeof(sql), "SELECT * FROM messages ORDER BY id DESC LIMIT 1");
    return select_from_db(sql, message_callback, message);
}



int message_callback(void *data, int argc, char **argv, char **azColName)        // callback for "Message"-object
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
