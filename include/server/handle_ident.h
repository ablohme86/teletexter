#ifndef IDENT_H
#define IDENT_H

#include "handle_client.h"

void handle_ident(client_t *cli,int argc, char **argv);
int verify_login(const char *username, const char *password, client_t *cli);

#endif /* IDENT_H */
