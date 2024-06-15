#include "../include/commands.h"

#include <stddef.h>

#include "../include/ident.h"
#include "../include/messages.h"
//#include "../include/pwd.h"

command_t commands[] = {
    {"IDENT", handle_ident},
   // {"PWD", handle_pwd},
    {"MSG", handle_msg},
    {"", NULL}
};
