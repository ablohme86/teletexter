#include "commands.h"
#include "ident.h"
#include "messaging.h"

command_t commands[] = {
    {"IDENT", handle_ident},
    {"MSG", handle_msg},
    {"", NULL}
};
