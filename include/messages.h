#ifndef MESSAGING_H
#define MESSAGING_H
#include "client.h"


void handle_msg(client_t *cli, char *args);
void get_current_time(char current_time[15]);
void handle_clear_display(client_t *cli, char *args);
void handle_msg_custom(client_t *cli, char *args);

#endif /* MESSAGING_H */
