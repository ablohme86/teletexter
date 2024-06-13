#ifndef CLIENT_H
#define CLIENT_H

#include <pthread.h>

#define NICKNAME_MAXLENGTH 6
#define MAX_CLIENTS 10

typedef struct client_t {
    int socket;
    char nickname[NICKNAME_MAXLENGTH];
} client_t;

extern pthread_mutex_t clients_mutex;
extern client_t *clients[MAX_CLIENTS];

void add_client(client_t *cli);
void remove_client(client_t *cli);
void *client_handler(void *arg);
void handle_client(client_t *cli);

#endif /* CLIENT_H */
