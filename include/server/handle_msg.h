#ifndef MESSAGING_H
#define MESSAGING_H
#include "handle_client.h"


void handle_msg(client_t *cli, int argc, char **argv);
void handle_clear_display(client_t *cli, int argc, char **argv);
void handle_msg_custom(client_t *cli, int argc, char **argv);

#endif /* MESSAGING_H */
