#include "../../include/server/server_commands.h"

#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>

#include "../../include/server/handle_ident.h"
#include "../../include/server/handle_msg.h"
#include "../../include/server/handle_client.h"

command_t commands[] = {
    {"IDENT", handle_ident, 2},
    {"MSG", handle_msg, 1},
    {"MSG_CUSTOM",handle_msg_custom,3},
    {"CLEAR_DISPLAY", handle_clear_display, 0},
    {"BYE", handle_disconnect_client,0},
    {"CLEAR_LINE", handle_clear_line,1},
    {"MSG_NEXT", handle_next_msg,0},
    {"MSG_PREV", handle_prev_msg, 0},
    {"MSG_LATEST", handle_latest_msg,0},
    {"LIST",handle_list_cmd,0},
    {"", NULL, 0}
};


char **split_args(char *args, int *argc)
{
    char **argv = malloc(MAX_ARGS * sizeof(char *));
   // char *arg;
    int count = 0;
    int in_quote = 0;
    char *start = NULL;

    while (*args != '\0' && count < MAX_ARGS)
    {
        while (isspace(*args))
        {
            args++;
        }

        if (*args == '\0')
        {
            break;
        }

        if (*args == '\"')
        {
            in_quote = 1;
            start = ++args;
            while (*args != '\0' && (in_quote && *args != '\"'))
            {
                args++;
            }
            if (*args == '\"')
            {
                *args = '\0';
                in_quote = 0;
            }
            argv[count++] = start;
            args++;
        }
        else
        {
            start = args;
            while (*args != '\0' && !isspace(*args))
            {
                args++;
            }
            if (*args != '\0')
            {
                *args++ = '\0';
            }
            argv[count++] = start;
        }
    }

    argv[count] = NULL;
    *argc = count;

    return argv;
}
