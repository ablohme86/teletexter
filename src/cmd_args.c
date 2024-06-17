//
// Created by Alexander Blohme on 17/06/2024.
//
#include "../include/cmd_args.h"
#include <stddef.h>
#include "../include/server_commands.h"

cmd_args_t cmd_arguments[] = {
    {&commands[0], {"USERNAME", "PASSWORD"}}, // Example initialization for IDENT command
    {&commands[2], {"LINE", "ALIGN", "MSG"}},  // Example initialization for MSG_CUSTOM command
    // Add more entries as needed for other commands
};
cmd_args_t *find_command_arguments(command_t *command)
{
    for (int i = 0; i < sizeof(cmd_arguments) / sizeof(cmd_arguments[0]); ++i)
    {
        if (cmd_arguments[i].command_ptr == command)
        {
            return &cmd_arguments[i];
        }
    }
    return NULL; // Return NULL if no arguments are found for the command
}