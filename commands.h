#ifndef COMMANDS_H
#define COMMANDS_H

#include "client.h"

typedef struct command_t {
    char command[10];
    void (*function)(client_t *, char *);
} command_t;

extern command_t commands[];

#endif /* COMMANDS_H */
