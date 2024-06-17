#ifndef COMMANDS_H
#define COMMANDS_H

#include "client.h"

typedef struct command {
    char command[10];
    void (*function)(client_t *cli, char *args);
    int requires_args;
} command_t;


extern command_t commands[];

#endif /* COMMANDS_H */
