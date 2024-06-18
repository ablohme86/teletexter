//
// Created by Alexander Blohme on 18/06/2024.
//

#ifndef MESSAGES_H
#define MESSAGES_H
#include "../server/server.h"
#include "../server/handle_client.h"

#endif //MESSAGES_H




int new_message(char *msg, client_t *sender);
int set_text(const char *align, const char *msg, unsigned int line, client_t *sender);
