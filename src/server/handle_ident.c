#include "../../include/server/handle_ident.h"
#include "../../include/status.h"
#include "../../include/utils.h"
#include "../../include/user/user.h" // for user object
#include "../../include/log.h"
#include "../../include/server/server.h"

void handle_ident(client_t *cli, int argc, char **argv)
{
    (void)argc;
    // Hent argumenter fra argv
    char *nickname = argv[0];
    char *password = argv[1];
    

    strip_newline(password);
    strip_newline(nickname);

    int db_login = check_user_login(nickname,password,cli->user);
    if (db_login > 0)
    {
        cli->identified = 1;
        log_sys_message("[%s] %s (%d) successfully logged in with access level %d", cli->ipv4addr,cli->user->username,cli->user->id,cli->user->access_level);
        ok_status(cli,LOGIN_OK,"LOGIN_OK");
    }
    else
    {
        cli->identified = 0;
        log_sys_message("[%s] Invalid credentials for user %s",cli->ipv4addr, nickname);
        bad_status(cli,LOGIN_FAILED,"Invalid credentials!");
    }

}



