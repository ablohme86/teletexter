#include "../include/ident.h"
#include "../include/utils.h"
#include "../include/pwd.h"
#include "../include/console.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

// Funksjon for å verifisere passordet
int verify_login(const char *username, const char *password)
{
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "users/%s", username);

    FILE *file = fopen(filepath, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Userfile for %s could not be found at %s!\n", username, filepath);
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

// Håndter IDENT-kommandoen
void handle_ident(client_t *cli, char *args)
{
    if (strlen(args) == 0)
    {
        char *error_msg = "IDENT_MISSING_ARGS\n";
        send(cli->socket, error_msg, strlen(error_msg), 0);
        return;
    }

    char nickname[256];
    char password[256];
    int args_count = sscanf(args, "%255s %255s", nickname, password);

    if (args_count < 2) {
        // Handle error
        printf("Not enough arguments provided.\n");

    }

    // Find the position in the original args string where the password starts
    char *password_pos = strchr(args, ' ');
    if (password_pos != NULL)
    {
        // Move past the first space
        password_pos++;
        // Find the start of the actual password (second space)
        password_pos = strchr(password_pos, ' ');
        if (password_pos != NULL)
        {
            // Move past the second space to get the start of the full password
            password_pos++;
            // Copy the rest of the string as the password
            strncpy(password, password_pos, sizeof(password) - 1);
            password[sizeof(password) - 1] = '\0'; // Ensure null-terminated string
        }
    }

    int verify_result = verify_login(nickname, password);
    if (verify_result == 0)
    {
        char *error_msg = "LOGIN_ERROR\n";
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
        strncpy(cli->nickname, nickname, sizeof(cli->nickname) - 1);
        cli->nickname[sizeof(cli->nickname) - 1] = '\0';
        cli->identified = 1;
    }
}


