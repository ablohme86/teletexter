#ifndef HANDLE_ADMIN_H
#define HANDLE_ADMIN_H
#include "../../include/server/handle_client.h"


void handle_admin_create_user(client_t *cli, int argc, char **argv);
void handle_admin_delete_user(client_t *cli, int argc, char **argv);
void handle_admin_set_permission(client_t *cli, int argc, char **argv);
void handle_admin_edit_pwd(client_t *cli, int argc, char **argv);

#endif
