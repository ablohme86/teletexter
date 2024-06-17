#ifndef CLIENT_H
#define CLIENT_H

#include <netinet/in.h>


typedef struct {
    int socket;
    char nickname[32];
    int identified;
    int access_level;
    struct sockaddr_in address;
} client_t;

void add_client(client_t *cli);
void remove_client(client_t *cli);
void handle_client(client_t *cli);
void handle_disconnect_client(client_t *cli, int argc, char **argv);
void *client_handler(void *arg);
void send_client_errmsg(client_t *cli, char *errmsg);

#endif // CLIENT_H
