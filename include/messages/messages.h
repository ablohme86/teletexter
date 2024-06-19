//
// Created by Alexander Blohme on 18/06/2024.
//

#ifndef MESSAGES_H
#define MESSAGES_H
#include "../server/server.h"
#include "../server/handle_client.h"

#define NO_MESSAGE_SET 600
#define MAX_LINE_MSG_LENGTH 100

int new_message(char *msg, User *user);
int set_line_text(const char *msg, unsigned int line,const char *align);
int scroll_message(int line);
void get_line_msg(int line, char*out_msg);

#endif //MESSAGES_H




