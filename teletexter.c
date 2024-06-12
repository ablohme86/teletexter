/******************************************************************************
 *                                                                            *
 *                               TeleTexter                                   *
 *                                                                            *
 *                Developed by: Alexander Blohme                              *
 *                Initial Release: 2024                                       *
 *                                                                            *
 *   teletexter.c - The program itself                                        *
 *                                                                            *
 * This program is free software: you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the               *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program. If not, see <https://www.gnu.org/licenses/>.      *
 *                                                                            *
 ******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "lcdlib.h"

#define PORT 5053
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

typedef struct 
{
    int socket;
    char nickname[50];
} client_t;

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
client_t *clients[MAX_CLIENTS];

typedef struct 
{
    char command[10];
    void (*function)(client_t *, char *);
} command_t;

// Prototyper for funksjoner
void handle_ident(client_t *cli, char *args);
void handle_msg(client_t *cli, char *args);
void handle_client(client_t *cli);

void strip_newline(char *str) 
{
    char *p = str;
    while (*p) {
        if (*p == '\r' || *p == '\n') 
        {
            *p = '\0';
            break;
        }
        p++;
    }
}


// Kommandoer
command_t commands[] = {
    {"IDENT", handle_ident},
    {"MSG", handle_msg},
    {"", NULL}
};
void add_client(client_t *cli) 
{
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) 
    {
        if (clients[i] == NULL) 
        {
            clients[i] = cli;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void remove_client(client_t *cli) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) 
    {
        if (clients[i] == cli) 
        {
            clients[i] = NULL;
            break;
        }
    }
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

        for (int i = 0; commands[i].command[0] != '\0'; ++i) 
        {
            if (strcmp(commands[i].command, cmd) == 0) 
            {
                commands[i].function(cli, args);
                break;
            }
        }
    }
}

void handle_ident(client_t *cli, char *args) 
{
    if (args != NULL) 
    {
        
        strncpy(cli->nickname, args, sizeof(cli->nickname) - 1);
        cli->nickname[sizeof(cli->nickname) - 1] = '\0';
        strip_newline(cli->nickname);   // just for good measure! ;)
        printf("Client identified as: '%s'\n", cli->nickname);
    }
}

void handle_msg(client_t *cli, char *args) 
{
    if (strlen(cli->nickname) == 0) 
    {
        char *error_msg = "No nickname is set! Please IDENT first!\n";
        send(cli->socket, error_msg, strlen(error_msg), 0);
        return;
    }

    if (args != NULL) 
    {
        strip_newline(args);  // Fjern \r\n fra args
        
        printf("Message from %s: %s\n", cli->nickname, args);
        lcd_clear();
        
        lcd_text(cli->nickname, 1, LEFT);
        lcd_text(args, 2, LEFT);
        
    }
}

int main() 
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    const char *device = "/dev/i2c-1";
    i2c_init(device); // initialize our i2c lib

    printf("connected to LCD display!\n");
    
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) 
    {
        perror("Could not create socket");
        return EXIT_FAILURE;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    lcd_text("TeleTexter v0.1",1,LEFT);
    lcd_text("By A. Blohme",2,CENTER);
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
    {
        perror("Bind failed");
        lcd_text("Sockets failed!",2,CENTER);
        close(server_socket);
        return EXIT_FAILURE;
    }

    if (listen(server_socket, 3) < 0) 
    {
        perror("Listen failed");
        close(server_socket);
        return EXIT_FAILURE;
    }

    printf("Server listening on port %d\n", PORT);

    while (1) 
    {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) 
        {
            perror("Accept failed");
            continue;
        }

        client_t *cli = (client_t *)malloc(sizeof(client_t));
        cli->socket = client_socket;
        memset(cli->nickname, 0, sizeof(cli->nickname));

        pthread_t tid;
        if (pthread_create(&tid, NULL, client_handler, (void *)cli) != 0) 
        {
            perror("Could not create thread");
            free(cli);
        } 
        else 
        {
            add_client(cli);
        }
    }

    close(server_socket);
    return EXIT_SUCCESS;
}
