#include "../include/commands.h"
#include "../include/ident.h"
#include "../include/messages.h"

command_t commands[] = {
    {"IDENT", handle_ident},
    {"MSG", handle_msg},
    {"", NULL}
};
