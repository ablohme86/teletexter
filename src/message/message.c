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
#ifndef DISABLE_LCD
#include "../../include/lcd/lcd_txt.h"
#endif

Message *currentMessage;   // message on display
int save_message(Message *msg)
{
    char sql[1024];
    int res;

    if (msg->id < 1)
    {
        snprintf(sql, sizeof(sql), "INSERT INTO messages (datetime, message, poster_id, status) VALUES (?, ?, ?, ?)");
        log_sys_message("[%s] New message, inserting into db...", MSG_INTERFACE);

        const void *params[4];
        enum ParamType param_types[4] = {PARAM_INT, PARAM_TEXT, PARAM_INT, PARAM_INT};

        params[0] = &(msg->datetime);  // Convert integer to pointer
        params[1] = msg->message;
        params[2] = &(msg->poster_id);  // Convert integer to pointer
        params[3] = &(msg->status);     // Convert integer to pointer

        res = execute_sql( sql, 4, params, param_types, NULL, NULL);
        msg->id = sqlite3_last_insert_rowid(db);
    }
    else
    {
        snprintf(sql, sizeof(sql), "UPDATE messages SET datetime = ?, message = ?, poster_id = ?, status = ? WHERE id = ?");
        log_sys_message("[%s] Updating message id %d", MSG_INTERFACE, msg->id);

        const void *params[5];
        enum ParamType param_types[5] = {PARAM_INT, PARAM_TEXT, PARAM_INT, PARAM_INT, PARAM_INT};

        params[0] = &(msg->datetime);  // Convert integer to pointer
        params[1] = msg->message;
        params[2] = &(msg->poster_id);  // Convert integer to pointer
        params[3] = &(msg->status);     // Convert integer to pointer
        params[4] = &(msg->id);         // Convert integer to pointer

        res = execute_sql( sql, 5, params, param_types, NULL, NULL);
    }

    return res;
}

int delete_message()
{
    if (currentMessage == NULL || currentMessage->id < 1)
    {
        return NO_MESSAGE_SET;
    }

    const char *sql = "DELETE FROM messages WHERE id = ?";
    const void *params[] = { &(currentMessage->id) };
    enum ParamType param_types[] = { PARAM_INT };

    int res = execute_sql( sql, 1, params, param_types, NULL, NULL);

    if (res == SQLITE_OK)
    {
        currentMessage = NULL;  // reset currentMessage pointer
        Message *msg = (Message *)calloc(sizeof(Message),1);
        get_latest_message_object(msg);
        if (msg->id == 0)
        {
            set_lcd_line_text("No more messages!",1,"CENTER");
            free(msg);
        }
        else
        {
            print_message_object(msg);
        }

        return MESSAGE_DELETED;
    }

    return res;
}

void print_message_object(Message *msg)           // Prints a "Message" object to screen
{

    if (msg->id == 0)
    {
#ifndef DISABLE_LCD
        set_lcd_line_text("There's whole 0", 1, "CENTER");
        set_lcd_line_text("messages to show", 2, "CENTER");
#endif
        log_err_message("[%s] message object id was 0! Cannot set message!", MSG_INTERFACE);
        return;
    }

    // Get the username of msg poster_id:
    User *user = (User *)calloc(1, sizeof(User));
    get_user_by_id(msg->poster_id, user);

    char top_line_msg[config.lcdConfig.lcdWidth + 1]; // For å unngå scrolling
    char datetime_short[15];
#ifndef DISABLE_LCD
    print_datetime_short(msg->datetime, datetime_short);
#endif
    snprintf(top_line_msg, sizeof(top_line_msg), "%s %s:", datetime_short, user->username);

    currentMessage = msg;  // Store the current message into memory

#ifndef DISABLE_LCD
    // instead of clearing the lines we will just overwrite ALL the lines with either the new text or blanks!
    //clear_lcd_lines();
    set_lcd_line_text(top_line_msg, 1, "LEFT");
#endif

    int n = 0;
    int msg_len = strlen(msg->message);
    int line_width = config.lcdConfig.lcdWidth;
    char buffer[line_width + 1];  // Buffer for line_width characters + null terminator
#ifndef DISABLE_LCD

    for (int i = 2; i <= (int)config.lcdConfig.lcdHeight; ++i)
    {
        if (n * line_width >= msg_len)
        {
            if (i <= (int)config.lcdConfig.lcdHeight)
            {
                // print rrest of the lines with blanks
                set_lcd_line_text(" ",i,"LEFT");
            }
            if (i == (int)config.lcdConfig.lcdHeight)
            {
                // We were on the last line, break it!
                break;
            }
        }
        memset(buffer, 0, sizeof(buffer)); // Clear buffer
        strncpy(buffer, msg->message + (n * line_width), line_width);
        set_lcd_line_text(buffer, i, "LEFT");
        n++;
    }
#endif

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

int get_message_by_id(unsigned int id, Message *message)
{
    const char *sql = "SELECT * FROM messages WHERE id = ?";
    const void *params[] = { &id };
    enum ParamType param_types[] = { PARAM_INT };
    return execute_sql( sql, 1, params, param_types, message_callback, message);
}

int get_next_message_object(Message *message)
{
    if (currentMessage == NULL)
    {
        return NO_CURRENT_MESSAGE_SET;
    }
    else
    {
        const char *sql = "SELECT * FROM messages WHERE id > ? ORDER BY id ASC LIMIT 1";
        const void *params[] = { &(currentMessage->id) };
        enum ParamType param_types[] = { PARAM_INT };
        execute_sql( sql, 1, params, param_types, message_callback, message);
        if (message->id == 0)
        {
            return NO_MORE_MESSAGES;
        }
        return MESSAGE_SET;
    }
}

int get_prev_message_object(Message *message)
{
    if (currentMessage == NULL) {
        return NO_CURRENT_MESSAGE_SET;
    }
    else
    {
        const char *sql = "SELECT * FROM messages WHERE id < ? ORDER BY id DESC LIMIT 1";
        const void *params[] = { &(currentMessage->id) };
        enum ParamType param_types[] = { PARAM_INT };
        execute_sql( sql, 1, params, param_types, message_callback, message);
        if (message->id == 0)
        {
            return NO_MORE_MESSAGES;
        }
        return MESSAGE_SET;
    }
}

int get_latest_message_object(Message *message) {
    const char *sql = "SELECT * FROM messages ORDER BY id DESC LIMIT 1";
    execute_sql(sql, 0, NULL, NULL, message_callback, message);
    if (message->id == 0)
    {
        return NO_MESSAGES;
    }
    return MESSAGE_SET;
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