#ifndef MESSAGING_H
#define MESSAGING_H
#include "handle_client.h"


void handle_msg(client_t *cli, int argc, char **argv);
void handle_clear_display(client_t *cli, int argc, char **argv);
void handle_msg_custom(client_t *cli, int argc, char **argv);
void handle_latest_msg(client_t *cli);
void handle_next_msg(client_t *cli);
void handle_set_msg_no(client_t *cli, int msg_no);
void handle_prev_msg(client_t cli);
#endif /* MESSAGING_H */
