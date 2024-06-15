#ifndef PWD_H
#define PWD_H

#include "client.h"

void handle_pwd(client_t *cli, char *args);
int verify_password(const char *username, const char *password);

#endif // PWD_H
