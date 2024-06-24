#include <ctype.h>
#include "../../include/server/handle_client.h"
#include "../../include/server/server_commands.h"
#include "../../include/server/server.h"
#include "../../include/config/config.h"
#include "../../include/log.h"
#include "../../include/utils.h"
#include "../../include/status.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <ctype.h>

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
    log_sys_message("[%s] %s Connected ", SCK_INTERFACE, cli->ipv4addr);
    pthread_mutex_unlock(&clients_mutex);
}
void handle_list_cmd(client_t *cli,  int argc,  char **argv)
{
(void)argc;
(void)argv;
    char help_msg[BUFFER_SIZE];
    snprintf(help_msg,sizeof(help_msg),"Available commands:\n"
                                       "MSG [message] - Creates a new message to show on LCD Display\n"
                                       "MSG_PREV - Displays previous message\n"
                                       "MSG_NEXT - Displays next message\n"
                                       "MSG_DEL - Deletes the shown message\n"
                                       "Custom LCD display commands:\n"
                                       "MSG_CUSTOM [align: LEFT/RIGHT/CENTER] [line: 1-4] [message] - Shows a custom text that will not be stored in database\n"
                                       "CLEAR_DISPLAY - Clears every line on the display\n"
                                       "CLEAR_LINE [line: 1-4] - Clears the specified line\n"
                                       "Other commands:\n"
                                       "IDENT [username] [password] - Logs you into the TeleTexter\n"
                                       "BYE - Disconnect from the TeleTexter server");
    ok_status(cli,250,help_msg);


}

void handle_disconnect_client(client_t *cli, int argc,  char **argv)
{


    (void) argc; // Suppress compiler warning
    (void) argv;
    char exit_msg[BUFFER_SIZE];

    if (cli->identified > 0)
    {
        snprintf(exit_msg,sizeof(exit_msg),"See ya later %s (%s)! Bye for now!\n", cli->user->username,cli->ipv4addr);
        log_sys_message("[%s] %s (%s) disconnected!", SCK_INTERFACE, cli->user->username,cli->ipv4addr);
    }
    else
    {
        snprintf(exit_msg,sizeof(exit_msg),"Hmmf! You left without telling me who you are?!\n");
        log_sys_message("[%s] %s disconnected!", SCK_INTERFACE,cli->ipv4addr);
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
    if (cli == NULL) 
    {
        log_sys_message("cli is NULL, cannot free pointer!");    
        
        return; // Sjekk om cli er gyldig
    }
}

int check_access_lvl(client_t *cli, int access_required)
{
    if (cli->identified < access_required)
    {
        bad_status(cli,ACCESS_DENIED,"Your access level is not high enough for this operation!");
        return 0;
    }
    return 1;
}

int is_identified(client_t *cli)
{
    if (cli->identified == 0)
    {
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
            bad_status(cli,INVALID_COMMAND,"No command given!");
            log_sys_message("[%s] %s Invalid command: %s",get_ip(cli), SCK_INTERFACE, buffer);
        }
        else
        {
            // Split command and arguments from buffer
            char *cmd = strtok(buffer, " ");
            if (cmd != NULL)
            {
                strtoupper(cmd);
            }

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
                            snprintf(missing_args_msg, sizeof(missing_args_msg), "%s needs %d arguments", strip_newline_return(cmd), commands[i].requires_args);
                            log_sys_message("[%s] %s Missing arguments on command %s, requires: %d", SCK_INTERFACE,cli->ipv4addr, cmd, commands[i].requires_args);
                            bad_status(cli,MISSING_ARGS,missing_args_msg);
                        }
                        else
                        {
                            int argc;
                            char **argv = split_args(args, &argc);

                            if (argc < commands[i].requires_args)
                            {
                                char missing_args_msg[BUFFER_SIZE];
                                snprintf(missing_args_msg, sizeof(missing_args_msg), "%s needs %d arguments, but got %d", strip_newline_return(cmd), commands[i].requires_args, argc);
                                log_sys_message("[%s] %s Missing arguments on command %s, requires: %d, got: %d", SCK_INTERFACE,get_ip(cli), cmd, commands[i].requires_args, argc);
                                bad_status(cli,MISSING_ARGS,missing_args_msg);
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
                bad_status(cli,INVALID_COMMAND,"Invalid command!");
                log_sys_message("[%s] Invalid command: %s", SCK_INTERFACE, cmd);
            }
        }

    }
}
