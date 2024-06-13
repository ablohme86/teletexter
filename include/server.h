#ifndef SERVER_H
#define SERVER_H

#include "client.h"
#define BUFFER_SIZE 1024

int start_server();
char *get_ip(client_t *cli);

#endif /* SERVER_H */
