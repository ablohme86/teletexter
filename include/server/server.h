#ifndef SERVER_H
#define SERVER_H

#include "handle_client.h"
#define BUFFER_SIZE 1024
#define SCK_INTERFACE "NETWORK"

int start_server();
char *get_ip(client_t *cli);
void setuplcd();
void close_server();
void ok_status(client_t *cli, int status_no,char *cust_msg,...);
void bad_status(client_t *cli, int status_no,char *cust_msg,...);
void send_server_ident(client_t *cli);

#endif /* SERVER_H */
