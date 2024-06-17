#include "../include/server_commands.h"

#include <stddef.h>

#include "../include/ident.h"
#include "../include/messages.h"
#include "../include/client.h"

command_t commands[] = {
    {"IDENT", handle_ident, 2},
    {"MSG", handle_msg, 1},
    {"MSG_CLEAR", handle_clear_display, 0},
    {"BYE", handle_disconnect_client,0},
    {"", NULL, 0}
};
