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
    log_sys_message("[%s] Connected! Awaiting IDENT", get_ip(cli));
    pthread_mutex_unlock(&clients_mutex);
}

void handle_disconnect_client(client_t *cli,int argc, char **argv)
{


    (void) argc; // Suppress compiler warning
    (void) argv;
    char exit_msg[BUFFER_SIZE];

    if (strcmp(cli->nickname,"") != 0)
    {
        snprintf(exit_msg,sizeof(exit_msg),"See ya later %s! Bye for now!\n", cli->nickname);
        log_sys_message("[%s] %s disconnected!", get_ip(cli), cli->nickname);
    }
    else
    {
        snprintf(exit_msg,sizeof(exit_msg),"Hmmf! You left without telling me who you are?!\n");
        log_sys_message("[%s] Disconnected!", get_ip(cli));
    }
    // Send a last message before cleanup work begins!
    send(cli->socket, exit_msg, strlen(exit_msg), 0);
    remove_client(cli);
    //  remove_client(cli);
}

void remove_client(client_t *cli)
{
    if (cli == NULL) {
        return; // Sjekk om cli er gyldig
    }

    pthread_mutex_lock(&clients_mutex);

    // Lukk cli->socket hvis det er gyldig
    if (cli->socket != -1) {
        close(cli->socket);
    }

    // Finn og merk cli som NULL i clients-arrayet
    for (int i = 0; i < config.serverConfig.maxClients; ++i)
    {
        if (clients[i] == cli)
        {
            clients[i] = NULL;
            break;
        }
    }

    pthread_mutex_unlock(&clients_mutex);
    if (cli == NULL) {
        printf("cli is NULL now, cannot free memory :O");
        return; // Sjekk om cli er gyldig
    }

    // Frigjør cli
    //free(cli);
}

int check_access_lvl(client_t *cli, int access_required)
{
    if (cli->identified < access_required)
    {
        bad_status(cli,ACCESS_DENIED,"Your access is not high enough for this operation!");
        return 0;
    }
    return 1;
}
int is_identified(client_t *cli)
{
    if (cli->identified == 0)
    {
        bad_status(cli,MISSING_IDENT,"You are not identified!");
        return 0;
    }
    return 1;
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
        int cmd_found = 0;

        if (strcmp(buffer,"") == 0)
        {

            log_sys_message("[%s] Found invalid characters in command!",get_ip(cli));
            bad_status(cli,INVALID_CMD,"Found invalid  characters in command!");

        }
        else
        {
            // Split command and arguments from buffer
            char *cmd = strtok(buffer, " ");

            char *args = strtok(NULL, "\n"); // Capture rest of string including spaces


            for (int i = 0; commands[i].command[0] != '\0'; ++i)
            {
                if (strcmp(commands[i].command, cmd) == 0) {
                    cmd_found = 1;

                    if (commands[i].requires_args)
                    {
                        if (args == NULL || strcmp(args, "") == 0)
                        {
                            char missing_args_msg[BUFFER_SIZE];
                            snprintf(missing_args_msg, sizeof(missing_args_msg), "MISSING_ARGS: %s needs %d arguments...\n", strip_newline_return(cmd), commands[i].requires_args);
                            log_sys_message("[%s] Missing arguments on command %s, requires: %d", get_ip(cli), cmd, commands[i].requires_args);
                            send(cli->socket, missing_args_msg, strlen(missing_args_msg), 0);
                        }
                        else
                        {
                            int argc;
                            char **argv = split_args(args, &argc);

                            if (argc < commands[i].requires_args)
                            {
                                char missing_args_msg[BUFFER_SIZE];
                                snprintf(missing_args_msg, sizeof(missing_args_msg), "MISSING_ARGS: %s needs %d arguments, but got %d.\n", strip_newline_return(cmd), commands[i].requires_args, argc);
                                log_sys_message("[%s] Missing arguments on command %s, requires: %d, got: %d", get_ip(cli), cmd, commands[i].requires_args, argc);
                                send(cli->socket, missing_args_msg, strlen(missing_args_msg), 0);
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
                bad_status(cli,INVALID_CMD,NULL);
                log_sys_message("[%s] Invalid command: %s", get_ip(cli), cmd);
            }
        }

    }
}
