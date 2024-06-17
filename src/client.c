#include "../include/client.h"

#include <ctype.h>
#include <log.h>

#include "../include/server_commands.h"
#include "../include/server.h"
#include "../include/config.h"
#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <utils.h>
#include <arpa/inet.h>

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
client_t *clients[100]; // total max capacity

void add_client(client_t *cli)
{
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < config.serverConfig.maxClients; ++i)
    {
        if (clients[i] == NULL)
        {
            clients[i] = cli;
            break;
        }
    }
    printf("%s connected... Awaiting IDENT\n", get_ip(cli));
    log_sys_message("[%s] Connected! Awaiting IDENT", get_ip(cli));
    pthread_mutex_unlock(&clients_mutex);
}

void handle_disconnect_client(client_t *cli, char *args)
{

    (void) args; // Suppress compiler warning
    remove_client(cli);
}

void remove_client(client_t *cli)
{
    pthread_mutex_lock(&clients_mutex);

    // Finn og merk cli som NULL i clients-arrayet
    for (int i = 0; i < config.serverConfig.maxClients; ++i)
    {
        if (clients[i] == cli)
        {
            printf("Found client, setting NULL\n");
            clients[i] = NULL;
            break;
        }
    }

    pthread_mutex_unlock(&clients_mutex);

    // Lukk cli->socket og frigjør cli
    if (cli->socket != -1) {
        close(cli->socket);
    }
    free(cli);
}


void *client_handler(void *arg)
{
    client_t *cli = (client_t *)arg;
    handle_client(cli);
    remove_client(cli);
    return NULL;
}
void handle_client(client_t *cli)
{
    char buffer[BUFFER_SIZE];
    int nbytes;
    while ((nbytes = recv(cli->socket, buffer, sizeof(buffer), 0)) > 0)
    {
        buffer[nbytes] = '\0';

        // Trim trailing whitespace (if any)
        int len = strlen(buffer);
        while (len > 0 && isspace(buffer[len - 1]))
        {
            buffer[len - 1] = '\0';
            len--;
        }

        // Split command and arguments from buffer
        char *cmd = strtok(buffer, " ");
        char *args = strtok(NULL, "\n"); // Capture rest of string including spaces

        int cmd_found = 0;
        for (int i = 0; commands[i].command[0] != '\0'; ++i)
        {
            if (strcmp(commands[i].command, cmd) == 0) {
                cmd_found = 1;

                // Check if the command requires arguments and if args is NULL or empty
                if (commands[i].requires_args && (args == NULL || strcmp(args, "") == 0))
                {
                    char missing_args_msg[BUFFER_SIZE];
                    snprintf(missing_args_msg, sizeof(missing_args_msg), "MISSING_ARGS: %s needs %d arguments...\n", strip_newline_return(cmd), commands[i].requires_args);
                    log_sys_message("[%s] Missing arguments on command %s, requires: %d", get_ip(cli), cmd, commands[i].requires_args);
                    send(cli->socket, missing_args_msg, strlen(missing_args_msg), 0);
                }
                else
                {
                    strip_newline(args);
                    commands[i].function(cli, args);
                    break;
                }
            }
        }

        if (!cmd_found)
        {
            char invalid_cmd_msg[BUFFER_SIZE];
            snprintf(invalid_cmd_msg, sizeof(invalid_cmd_msg), "INVALID_COMMAND %s\n", strip_newline_return(cmd));
            log_sys_message("[%s] Invalid command: %s", get_ip(cli), cmd);
            send(cli->socket, invalid_cmd_msg, strlen(invalid_cmd_msg), 0);
        }
    }
}

