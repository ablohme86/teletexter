//
// Created by Alexander Blohme on 18/06/2024.
//

#ifndef MESSAGES_H
#define MESSAGES_H
#include "../user/user.h"

#define NO_MESSAGE_SET 600
#define MAX_LINE_MSG_LENGTH 100
#define MSG_INTERFACE "MESSAGE"



typedef struct {
    unsigned int id;
    long datetime;
    char time[9];
    char message[256];
    unsigned int poster_id;
    unsigned int status;    // 0 for active, 1 for archived
} Message;

int save_message( Message *msg);
int new_message(char *msg, User *user);
int set_line_text(const char *msg, unsigned int line,const char *align);
int scroll_message(int line);
void get_line_msg(int line, char*out_msg);
void clear_line(int line);
int message_callback(void *data, int argc, char **argv, char **azColName);  // for sql cb
void print_latest_msg();
void print_next_msg();
void print_prev_msg();
int get_message_by_id(int id, Message *message);
int get_latest_message_object(Message *message);
void print_message_object(Message *message);
int get_next_message_object(Message *message);
int get_prev_message_object(Message *message);


#endif //MESSAGES_H




