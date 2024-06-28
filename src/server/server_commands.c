#include "../../include/server/server_commands.h"

#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "../../include/server/handle_client.h"
#include "../../include/server/handle_ident.h"
#include "../../include/server/handle_msg.h"
#include "../../include/server/handle_admin.h"
#include "../../include/server/handle_lcdtxt.h"
#include "../../include/server/server.h"
#include "../../include/log.h"
#include "../../include/status.h"
#include "../../include/utils.h"
#include "../../include/user/user.h"

command_t commands[] = {
// command, function, required arguments, access level required.
// i framtia vil jeg legge til at man skal kunne endre disse i en fil kanskje? 
    
    // handle_ident.h
    {"IDENT", handle_ident, 2,ALL_USERS,"[username] [password] -- Logs you into the server "},
    {"MSG_CREATE", handle_msg, 1,NORMAL_USER,"[message] -- Create & displays a new message to the LCD panel"},
    {"MSG_NEXT", handle_next_msg,0,NORMAL_USER, "-- Shows the next stored message"},
    {"MSG_PREV", handle_prev_msg, 0,NORMAL_USER, "-- Goes back 1 stored message"},
    {"MSG_LATEST", handle_latest_msg,0,NORMAL_USER,"-- Displays the latest stored message"},
    {"MSG_SCROLL",handle_lcd_line_scroll,1,NORMAL_USER,"[line number] -- Scrolls selected line"},
    {"MSG_DEL",handle_msg_del,0,MODERATOR_USER, "-- Deletes current message"},
    // handle_client.h
    {"BYE", handle_disconnect_client,0,ALL_USERS,""},
    {"LIST",handle_list_cmd,0,ALL_USERS,""},
    // handle_lcdtxt.h:  to control the lcd display "customly"
    {"SET_TEXT",handle_lcd_set_text,3,ADMIN_USER,"[align] [line number] [text] -- Sets a custom text on the provided line"},
    {"CLEAR_DISPLAY", handle_clear_lcd_display, 0,MODERATOR_USER,"-- Clears all lines on display"},
    {"CLEAR_LINE", handle_clear_lcd_line,1,MODERATOR_USER,"[line number] -- Clears the provided line number of text"},
    // Admin Commands:
    {"CREATE_USER",handle_admin_create_user,3,ADMIN_USER,"[username] [password] [access level] - Creates a new user with the provided access level"},
    {"DELETE_USER",handle_admin_delete_user,1,ADMIN_USER,"[username] -- Deletes the provided username"},
    
    // Nullator
    {"", NULL, 0,0,""}

    
};

void handle_list_cmd(client_t *cli, int argc, char **argv)
{
    (void)argc;
    (void)argv;
    char help_msg[BUFFER_SIZE];
    char command_info[256]; // Buffer for each command info
    int offset = 0; // Offset to keep track of the current position in help_msg

    // Start with the header
    offset += snprintf(help_msg + offset, sizeof(help_msg) - offset, "Available commands:\n");

    for (int i = 0; commands[i].command[0] != '\0'; ++i)
    {
        // Format each command's information
        snprintf(command_info, sizeof(command_info), "%s %s (%d args)\n", commands[i].command,commands[i].description, commands[i].requires_args);

        // Ensure we don't overflow help_msg
        if (offset + strlen(command_info) < sizeof(help_msg))
        {
            offset += snprintf(help_msg + offset, sizeof(help_msg) - offset, "%s", command_info);
        }
        else
        {
            // If the help message is too long, truncate it and break
            snprintf(help_msg + offset - 4, sizeof(help_msg) - offset + 4, "...\n");
            break;
        }
    }

    ok_status(cli, 250, help_msg);
}


void process_command(client_t *cli, char *buffer)
{
    // Trim trailing whitespace (if any)
    int len = strlen(buffer);
    while (len > 0 && isspace(buffer[len - 1]))
    {
        buffer[len - 1] = '\0';
        len--;
    }

    if (len == 0)
    {
        bad_status(cli, INVALID_COMMAND, "No command given!");
        log_sys_message("[%s] %s Invalid command: %s", get_ip(cli), SCK_INTERFACE, buffer);
        return;
    }

    int cmd_found = 0;
    // Split command and arguments from buffer
    char *cmd = strtok(buffer, " ");
    if (cmd != NULL)
    {
        strtoupper(cmd);
    }

    char *args = strtok(NULL, "\n"); // Capture rest of string including spaces

    for (int i = 0; commands[i].command[0] != '\0'; ++i)
    {
        if (strcmp(commands[i].command, cmd) == 0)
        {
            cmd_found = 1;
            if (cli->user->access_level < commands[i].access_level)
            {
                char access_denied_msg[100];
                snprintf(access_denied_msg, sizeof(access_denied_msg), "Your access level is too low for %s", cmd);
                bad_status(cli, ACCESS_DENIED, access_denied_msg);
                break;
            }
            if (commands[i].requires_args)
            {
                if (args == NULL || strcmp(args, "") == 0)
                {
                    char missing_args_msg[BUFFER_SIZE];
                    snprintf(missing_args_msg, sizeof(missing_args_msg), "%s needs %d arguments", strip_newline_return(cmd), commands[i].requires_args);
                    log_sys_message("[%s] %s Missing arguments on command %s, requires: %d", SCK_INTERFACE, cli->ipv4addr, cmd, commands[i].requires_args);
                    bad_status(cli, MISSING_ARGS, missing_args_msg);
                }
                else
                {
                    int argc;
                    char **argv = split_args(args, &argc);

                    if (argc < commands[i].requires_args)
                    {
                        char missing_args_msg[BUFFER_SIZE];
                        snprintf(missing_args_msg, sizeof(missing_args_msg), "%s needs %d arguments, but got %d", strip_newline_return(cmd), commands[i].requires_args, argc);
                        log_sys_message("[%s] %s Missing arguments on command %s, requires: %d, got: %d", SCK_INTERFACE, get_ip(cli), cmd, commands[i].requires_args, argc);
                        bad_status(cli, MISSING_ARGS, missing_args_msg);
                    }
                    else
                    {
                        commands[i].function(cli, argc, argv);
                    }
                    free(argv);
                }
            }
            else
            {
                commands[i].function(cli, 0, NULL);
            }
            break;
        }
    }
    if (!cmd_found)
    {
        bad_status(cli, INVALID_COMMAND, "Invalid command!");
        log_sys_message("[%s] Invalid command: %s", SCK_INTERFACE, cmd);
    }
}

char **split_args(char *args, int *argc)
{
    char **argv = malloc(MAX_ARGS * sizeof(char *));
   // char *arg;
    int count = 0;
    int in_quote = 0;
    char *start = NULL;

    while (*args != '\0' && count < MAX_ARGS)
    {
        while (isspace(*args))
        {
            args++;
        }

        if (*args == '\0')
        {
            break;
        }

        if (*args == '\"')
        {
            in_quote = 1;
            start = ++args;
            while (*args != '\0' && (in_quote && *args != '\"'))
            {
                args++;
            }
            if (*args == '\"')
            {
                *args = '\0';
                in_quote = 0;
            }
            argv[count++] = start;
            args++;
        }
        else
        {
            start = args;
            while (*args != '\0' && !isspace(*args))
            {
                args++;
            }
            if (*args != '\0')
            {
                *args++ = '\0';
            }
            argv[count++] = start;
        }
    }

    argv[count] = NULL;
    *argc = count;

    return argv;
}
