#include "../include/pwd.h"
#include "../include/config.h"
#include "../include/ident.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

int verify_password(const char *username, const char *password)
{
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "users/%s", username);

    FILE *file = fopen(filepath, "r");
    if (file == NULL)
    {
        // Sender tilbake melding til klienten om at brukeren ikke kunne bli funnet
        fprintf(stderr, "User could not be found: %s @ %s!\n", username, filepath);
        return 0; // User file does not exist
    }

    char stored_password[256];
    int enabled = 0;

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        if (strncmp(line, "PWD ", 4) == 0)
        {
            strncpy(stored_password, line + 4, sizeof(stored_password) - 1);
            stored_password[sizeof(stored_password) - 1] = '\0';
            strip_newline(stored_password);
        }
        else if (strncmp(line, "ENABLED ", 8) == 0)
        {
            enabled = atoi(line + 8);
        }
    }

    fclose(file);

    if (!enabled)
    {
        return -1; // User is blocked
    }

    return strcmp(password, stored_password) == 0;
}

void handle_pwd(client_t *cli, char *args)
{
    if (strlen(cli->nickname) == 0)
    {
        char *error_msg = "Please IDENT first.\n";
        send(cli->socket, error_msg, strlen(error_msg), 0);
        return;
    }

    if (args != NULL)
    {
        char *password = args;

        int verify_result = verify_password(cli->nickname, password);
        if (verify_result == 0)
        {
            char *error_msg = "INVALID_IDENT\n";
            send(cli->socket, error_msg, strlen(error_msg), 0);
        }
        else if (verify_result == -1)
        {
            char *error_msg = "USER_BLOCKED\n";
            send(cli->socket, error_msg, strlen(error_msg), 0);
        }
        else
        {
            char *success_msg = "IDENTIFIED\n";
            send(cli->socket, success_msg, strlen(success_msg), 0);
            cli->identified = 1;
        }
    }
    else
    {
        char *error_msg = "User could not be found\n";  // Melding når args er NULL
        send(cli->socket, error_msg, strlen(error_msg), 0);
    }
}
