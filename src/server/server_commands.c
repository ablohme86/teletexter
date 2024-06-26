#include "../../include/server/server_commands.h"

#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>

#include "../../include/server/handle_ident.h"
#include "../../include/server/handle_msg.h"
#include "../../include/server/handle_admin.h"
#include "../../include/server/handle_lcdtxt.h"
#include "../../include/server/handle_client.h"
#include "../../include/user/user.h"

command_t commands[] = {
// command, function, required arguments, access level required.
// i framtia vil jeg legge til at man skal kunne endre disse i en fil kanskje? 
    
    // handle_ident.h
    {"IDENT", handle_ident, 2,ALL_USERS},
    
    // handle_msg.h
    {"MSG", handle_msg, 1,NORMAL_USER},
    {"MSG_CUSTOM",handle_lcd_set_text,3,ADMIN_USER},
    {"CLEAR_DISPLAY", handle_clear_lcd_display, 0,MODERATOR_USER},
    {"CLEAR_LINE", handle_clear_lcd_line,1,MODERATOR_USER},
    {"MSG_NEXT", handle_next_msg,0,NORMAL_USER},
    {"MSG_PREV", handle_prev_msg, 0,NORMAL_USER},
    {"MSG_LATEST", handle_latest_msg,0,NORMAL_USER},
    {"MSG_SCROLL",handle_lcd_line_scroll,1,NORMAL_USER},
    {"MSG_DEL",handle_msg_del,0,MODERATOR_USER},
     
    // handle_client.h
    {"BYE", handle_disconnect_client,0,ALL_USERS},
    {"LIST",handle_list_cmd,0,ALL_USERS},
    
    
    // Admin Commands:
    
    {"CREATE_USER",handle_admin_create_user,3,ADMIN_USER},
    {"DELETE_USER",handle_admin_delete_user,1,ADMIN_USER},
    
    // Nullator
    {"", NULL, 0,0}

    
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
