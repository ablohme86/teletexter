#ifndef CLIENT_H
#define CLIENT_H

#include <netinet/in.h>


typedef struct {
    int socket;
    char nickname[32];
    int identified;
    struct sockaddr_in address;
} client_t;

void add_client(client_t *cli);
void remove_client(client_t *cli);
void handle_client(client_t *cli);
void *client_handler(void *arg);

#endif // CLIENT_H
