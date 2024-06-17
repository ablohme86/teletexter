#ifndef MESSAGING_H
#define MESSAGING_H
#include "client.h"


void handle_msg(client_t *cli, int argc, char **argv);
void get_current_time(char current_time[15]);
void handle_clear_display(client_t *cli, int argc, char **argv);
void handle_msg_custom(client_t *cli, int argc, char **argv);

#endif /* MESSAGING_H */
