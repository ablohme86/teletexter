#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../../include/server/handle_msg.h"
#include "../../include/log.h"
#include "../../include/server/server.h"
#include "../../include/utils.h"
#include "../../include/server/handle_client.h"
#include "../../include/status.h"
#include "../../include/message/message.h"


void handle_msg_del(client_t *cli, int argc, char **argv)
{
   (void)argc;
   (void)argv;
    int res = delete_message();
    if (res == -1)
    {
        bad_status(cli,NO_MESSAGE_SHOWN,"Cannot delete, no message is currently on display!");
        return;
    }
    ok_status(cli,MESSAGE_DELETED,"Message deleted!");
}


void handle_latest_msg(client_t *cli, int argc, char **argv)   // handle LATEST_MSG call from client
{
    (void)argc;
    (void)argv;
    print_latest_msg();
    ok_status(cli,MESSAGE_SET,"Message set!");
}
void handle_next_msg(client_t *cli, int argc, char **argv) // handle NEXT_MSG from client
{
    (void)argc;
    (void)argv; // suppress compiler warnings, this is a 0 argument command
    Message *msg = (Message *)calloc(sizeof(Message),1);
    get_next_message_object(msg);
    if (msg->id == 0)
    {
        bad_status(cli,330,"No more messages!");
    }
    else
    {
        print_message_object(msg);
        ok_status(cli,MESSAGE_SET,"Next message set!");
    }
}
void handle_set_msg_no(client_t *cli, int argc, char **argv)
{
    (void)cli;
    (void)argc;
    (void)argv;
}
void handle_prev_msg(client_t *cli, int argc, char **argv)
{
    (void)argc;
    (void)argv;
    Message *msg = (Message *)calloc(sizeof(Message),1);
    get_prev_message_object(msg);

    if (msg->id == 0)
    {
        bad_status(cli,NO_MORE_MESSAGES,"No more messages!");
    }
    else
    {
        print_message_object(msg);
        ok_status(cli,MESSAGE_SET,"Previous message set!");
    }
}

void handle_msg(client_t *cli, int argc, char **argv)
{
    if (is_identified(cli) == 0)
    {
        log_sys_message("[%s] %s Client cannot send MSG, not identified yet!",SCK_INTERFACE, get_ip(cli));
        bad_status(cli,ACCESS_DENIED,NULL);
        return;
    }
    if (argc > 0)
    {
        char full_message[100];


        strncpy(full_message,argv[0], sizeof(full_message));
        strip_newline(full_message);
        new_message(full_message,cli->user);
        ok_status(cli,MESSAGE_SET,"Message created!");
    }
}
