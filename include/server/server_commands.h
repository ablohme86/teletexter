#ifndef COMMANDS_H
#define COMMANDS_H
#define MAX_ARGS 10 // maks antall argumenter

#include "handle_client.h"

typedef struct {
    char command[50];
    void (*function)(client_t *, int argc, char **argv);
    int requires_args;
} command_t;

char **split_args(char *args, int *argc);

extern command_t commands[];

#endif /* COMMANDS_H */
