//
// Created by Alexander Blohme on 17/06/2024.
//

#ifndef APP_ARGS_H
#define APP_ARGS_H
#include <stdio.h>
#include "utils.h"


char *configPath = "/etc/teletexter/teletexter.cfg";
int arg_srv_port = 0;
char *arg_bind_ip = NULL;
char *arg_usr_path = NULL;



void print_usage(const char *program_name)
{
    printf("Usage: %s --config <config_file> [--port <port>] [--bind <ip/host>] [--users <users_path>]\n", program_name);
}


int manage_startup_args(int argc, char **argv)
{
    char *config_file = NULL;
    if (argc < 3)
    {
        print_usage(argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; ++i)
    {
        if ((strcmp(argv[i], "--config") == 0 || strcmp(argv[i], "-c") == 0) && (i + 1 < argc))
        {
            config_file = argv[++i];
        }
        else if ((strcmp(argv[i], "--port") == 0 || strcmp(argv[i], "-p") == 0) && (i + 1 < argc))
        {
            if (is_numerical(argv[i + 1]))
            {
                arg_srv_port = atoi(argv[++i]);
            }
            else
            {
                printf("Invalid port number: %s\n", argv[i + 1]);
                return 1;
            }
        }
        else if ((strcmp(argv[i], "--bind") == 0 || strcmp(argv[i], "-b") == 0) && (i + 1 < argc))
        {
            arg_bind_ip = argv[++i];
        }
        else if ((strcmp(argv[i], "--users") == 0 || strcmp(argv[i], "-u") == 0) && (i + 1 < argc))
        {
            arg_usr_path = argv[++i];
        }
        else
        {
            printf("Unknown or incomplete argument: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }

    if (config_file == NULL)
    {
        printf("No config file specified...\n");
        print_usage(argv[0]);
        return 1;
    }

    if (arg_srv_port != 0)
    {
        printf("Port: %d\n", arg_srv_port);
    }
    if (arg_bind_ip != NULL)
    {
        printf("Bind IP: %s\n", arg_bind_ip);
    }
    if (arg_usr_path != NULL)
    {
        printf("Users path: %s\n", arg_usr_path);
    }

    configPath = config_file;
    return 0;
}


#endif //APP_ARGS_H
