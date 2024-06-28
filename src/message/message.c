//
// Created by Alexander Blohme on 18/06/2024.
//
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "../../include/config/config.h"
#include "../../include/message/message.h"
#include "../../include/db/db_handler.h"
#include "../../include/utils.h" 
#include "../../include/log.h"
#include "../../include/status.h"
#include "../../include/lcd/lcd_txt.h"

Message *currentMessage;   // message on display

int save_message(Message *msg)
{
    char sql[1024];
    int res;
    
    if (msg->id < 1)
    {
        snprintf(sql, sizeof(sql), "INSERT INTO messages (datetime, message, poster_id, status) VALUES (?, ?, ?, ?)");
        log_sys_message("[%s] New message, inserting into db...", MSG_INTERFACE);
        res = execute_sql(sql,NULL, "isii", 4, msg->datetime, msg->message, msg->poster_id, msg->status);
        msg->id = sqlite3_last_insert_rowid(db);
    }
    else
    {
        snprintf(sql, sizeof(sql), "UPDATE messages SET datetime = ?, message = ?, poster_id = ?, status = ? WHERE id = ?");
        log_sys_message("[%s] Updating message id %d",MSG_INTERFACE, msg->id);
        res = execute_sql(sql,NULL, "isiii", 5, msg->datetime, msg->message, msg->poster_id, msg->status, msg->id);
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
   int res = execute_sql(sql,NULL,"i",1,currentMessage->id);
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
        set_lcd_line_text("There's whole 0", 1, "CENTER");
        set_lcd_line_text("messages to show", 2, "CENTER");

        log_err_message("[%s] message object id was 0! Cannot set message!", MSG_INTERFACE);
        return;
    }

    // Get the username of msg poster_id:
    User *user = (User *)calloc(1, sizeof(User));
    get_user_by_id(msg->poster_id, user);

    char top_line_msg[config.lcdConfig.lcdWidth + 1]; // For å unngå scrolling
    char datetime_short[15];
    print_datetime_short(msg->datetime, datetime_short);
    snprintf(top_line_msg, sizeof(top_line_msg), "%s %s:", datetime_short, user->username);

    currentMessage = msg;  // Store the current message into memory

    // instead of clearing the lines we will just overwrite ALL the lines with either the new text or blanks!
    //clear_lcd_lines();
    set_lcd_line_text(top_line_msg, 1, "LEFT");

    int n = 0;
    int msg_len = strlen(msg->message);
    int line_width = config.lcdConfig.lcdWidth;
    char buffer[line_width + 1];  // Buffer for line_width characters + null terminator

    for (int i = 2; i <= config.lcdConfig.lcdHeight; ++i)
    {
        if (n * line_width >= msg_len)
            break;

        memset(buffer, 0, sizeof(buffer)); // Clear buffer
        strncpy(buffer, msg->message + (n * line_width), line_width);
        set_lcd_line_text(buffer, i, "LEFT");
        n++;
    }

    free(user);
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


void print_latest_msg()           // prints the latest message object in db!
{
    Message *msg = (Message *)calloc(sizeof(Message),1);
    get_latest_message_object(msg);
    print_message_object(msg);
}

int get_message_by_id(unsigned int id, Message *message)    // get a "Message" object by id
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

int message_callback(void *data, int argc, char **argv, char **azColName)        // callback to store data into the provided "Message"-object from sqlite
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