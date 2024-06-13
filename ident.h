#ifndef IDENT_H
#define IDENT_H

#include "client.h"

void handle_ident(client_t *cli, char *args);
void strip_newline(char *str);

#endif /* IDENT_H */
