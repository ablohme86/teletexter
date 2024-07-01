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
    if (res != MESSAGE_DELETED)
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
    Message *msg = (Message *)calloc(sizeof(Message),1);
    int res = get_latest_message_object(msg);
    switch (res)
    {
        case MESSAGE_SET:
            print_message_object(msg);
            ok_status(cli,MESSAGE_SET,"Next message set!");
            break;
        case NO_MESSAGES:
            bad_status(cli,NO_MORE_MESSAGES,"There is no message to show!");
            break;
    };
    ok_status(cli,MESSAGE_SET,"Message set!");
}
void handle_next_msg(client_t *cli, int argc, char **argv) // handle NEXT_MSG from client
{
    (void)argc;
    (void)argv; // suppress compiler warnings, this is a 0 argument command
    Message *msg = (Message *)calloc(sizeof(Message),1);
    int res = get_next_message_object(msg);
    switch (res)
    {
        case NO_MESSAGE_SET:
            bad_status(cli,NO_MESSAGE_SET,"There is no current message on display to go from!");
            break;
        case MESSAGE_SET:
            print_message_object(msg);
            ok_status(cli,MESSAGE_SET,"Next message set!");
            break;
        case NO_MORE_MESSAGES:
            bad_status(cli,NO_MORE_MESSAGES,"There are no more messages to show!");
            break;
    };

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
    int res = get_prev_message_object(msg);
    switch (res)
    {
        case NO_MESSAGE_SET:
            bad_status(cli,NO_MESSAGE_SET,"There is no current message on display to go from!");
            break;
        case MESSAGE_SET:
            print_message_object(msg);
            ok_status(cli,MESSAGE_SET,"Next message set!");
            break;
        case NO_MORE_MESSAGES:
            bad_status(cli,NO_MORE_MESSAGES,"You are already on the first message!");
            break;
    };

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
