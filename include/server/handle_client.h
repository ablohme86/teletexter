#ifndef CLIENT_H
#define CLIENT_H

#include <netinet/in.h>
#include "../db/db_handler.h"


typedef struct {
    int socket;
    User *user;
    int identified;
    struct sockaddr_in address;
    char *ipv4addr;
} client_t;

void add_client(client_t *cli);
void remove_client(client_t *cli);
void handle_client(client_t *cli);
void handle_disconnect_client(client_t *cli, int argc, char **argv);
void *client_handler(void *arg);
int is_identified(client_t *cli);
int check_access_lvl(client_t *cli, int a_l);

#endif // CLIENT_H
