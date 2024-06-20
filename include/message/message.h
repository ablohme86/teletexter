//
// Created by Alexander Blohme on 18/06/2024.
//

#ifndef MESSAGES_H
#define MESSAGES_H
#include "../user/user.h"

#define NO_MESSAGE_SET 600
#define MAX_LINE_MSG_LENGTH 100

typedef struct {
    unsigned int id;
    char date[11];
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

#endif //MESSAGES_H




