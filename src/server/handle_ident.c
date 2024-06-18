#include "../../include/server/handle_ident.h"
#include "../../include/db/db_handler.h"
#include "../../include/utils.h"
#include "../../include/config/config.h"
#include "../../include/log.h"
#include "../../include/server/server.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>



// Funksjon for å verifisere passordet
int verify_login(const char *username, const char *password, client_t *cli)
{
    char filepath[256];
    strip_newline(username);
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
            cli->user->access_level = atoi(line + 13);
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
    (void)argc;
    // Hent argumenter fra argv
    char *nickname = argv[0];
    char *password = argv[1];
    

    strip_newline(password);
    strip_newline(nickname);

    int db_login = check_user_login(nickname,password,cli->user);
    if (db_login == LOGIN_OK)
    {
        cli->identified = 1;
        log_sys_message("[%s] %s successfully logged in with access level %d", cli->ipv4addr,cli->user->username,cli->user->access_level);
        ok_status(cli,LOGIN_OK,"LOGIN_OK");
        //printf("Login Success: user: %s, password (from db): %s\n",cli->user->username,cli->user->password);
    }
    else
    {
        cli->identified = 0;
        log_sys_message("[%s] Invalid credentials for user %s",cli->ipv4addr, nickname);
        bad_status(cli,INVALID_CREDENTIALS,"INVALID_CREDENTIALS");
    }

}



