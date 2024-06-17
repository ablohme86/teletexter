#include "../include/ident.h"

#include <log.h>
#include <server.h>

#include "../include/utils.h"
#include "../include/pwd.h"
#include "../include/config.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>



// Funksjon for å verifisere passordet
int verify_login(const char *username, const char *password, client_t *cli)
{
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/%s",config.userConfig.userFilePath, username);

    FILE *file = fopen(filepath, "r");
    if (file == NULL)
    {
        log_sys_message( "Userfile for %s could not be found at %s!\n", username, filepath);
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
        else if (strncmp(line,"ACCESS_LEVEL ", 13) == 0)
        {
            cli->access_level = atoi(line + 13);
        }
    }

    fclose(file);

    if (!enabled)
    {
        return -1; // User is blocked
    }

    return strcmp(password, stored_password) == 0;
}
void handle_ident(client_t *cli, int argc, char **argv)
{
    // Hent argumenter fra argv
    char *nickname = argv[0];
    char *password = argv[1];

    int verify_result = verify_login(nickname, password, cli);
    if (verify_result == 0)
    {
        log_sys_message( "[%s] Wrong credentials: username %s!", get_ip(cli), nickname);
        char *error_msg = "LOGIN_ERROR\n";
        send(cli->socket, error_msg, strlen(error_msg), 0);
    }
    else if (verify_result == -1)
    {
        log_sys_message("[%s] The user '%s' is blocked from access!", get_ip(cli), nickname);
        char *error_msg = "USER_BLOCKED\n";
        send(cli->socket, error_msg, strlen(error_msg), 0);
    }
    else
    {
        char *success_msg = "IDENTIFIED\n";
        log_sys_message( "[%s] Successfully identified as %s!", get_ip(cli), nickname);
        send(cli->socket, success_msg, strlen(success_msg), 0);
        strncpy(cli->nickname, nickname, sizeof(cli->nickname) - 1);
        cli->nickname[sizeof(cli->nickname) - 1] = '\0';
        cli->identified = 1;
    }
}



