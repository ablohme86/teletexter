#include "../include/client.h"
#include "../include/server_commands.h"
#include "../include/server.h"
#include "../include/config.h"
#include "../include/console.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
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
    pthread_mutex_unlock(&clients_mutex);
}

void remove_client(client_t *cli)
{
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < config.serverConfig.maxClients; ++i)
    {
        if (clients[i] == cli)
        {
            clients[i] = NULL;
            break;
        }
    }
    printf("[%s] %s has disconnected\n", get_ip(cli), cli->nickname);
    pthread_mutex_unlock(&clients_mutex);
    close(cli->socket);
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
        char *cmd = strtok(buffer, " ");
        char *args = strtok(NULL, "\0");
        int cmd_found = 0;
        for (int i = 0; commands[i].command[0] != '\0'; ++i)
        {
            if (strcmp(commands[i].command, cmd) == 0)
            {
                cmd_found = 1;
                commands[i].function(cli, args);
                break;
            }
        }
        if (cmd_found == 0)
        {
            char invalid_cmd_msg[BUFFER_SIZE];
            sprintf(invalid_cmd_msg,"INVALID_COMMAND\n");
            send(cli->socket, invalid_cmd_msg,strlen(invalid_cmd_msg),0);
        }
    }
}

