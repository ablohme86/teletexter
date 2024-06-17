//
// Created by Alexander Blohme on 17/06/2024.
//
#ifndef ARGS_H
#define ARGS_H
#define MAX_COMMAND_ARGS 5 // Just an example, adjust as needed
#include "server_commands.h"


typedef struct {
    command_t *command_ptr; // Pointer to the command structure
    char *argument_name[MAX_COMMAND_ARGS]; // Array of argument names
} cmd_args_t;


cmd_args_t *find_command_arguments(command_t *command);

#endif // ARGS_H

