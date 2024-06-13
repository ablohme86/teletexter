#ifndef SERVER_H
#define SERVER_H

#include "client.h"
#define PORT 5053

int start_server();
void get_ip(client_t *cli);

#endif /* SERVER_H */
