//
// Created by Alexander Blohme on 20/06/2024.
//
#include "../../include/user/user.h"

#include <stdlib.h>
#include <string.h>

#include "../../include/db/db_handler.h"
#include "../../include/log.h"


int get_user_by_id(int id, User *user)
{
    char sql[256];
    snprintf(sql, sizeof(sql), "SELECT * FROM users WHERE id = %d", id);
    return select_from_db(sql, user_callback, user);
}
int check_user_login(const char *username, const char *pwd, User *user)
{
    char sql[256];
    snprintf(sql, sizeof(sql), "SELECT * FROM users WHERE username = '%s' AND password = '%s'", username, pwd);
    return select_from_db(sql, user_callback, user);
}
int user_callback(void *data, int argc, char **argv, char **azColName)
{
    User *user = (User *)data;
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(azColName[i], "id") == 0)
        {
            user->id = atoi(argv[i]);
        }
        else if (strcmp(azColName[i], "username") == 0)
        {
            strncpy(user->username, argv[i], sizeof(user->username));
        }
        else if (strcmp(azColName[i], "access_level") == 0)
        {
            user->access_level = atoi(argv[i]);
        }
        else if (strcmp(azColName[i], "enabled") == 0)
        {
            user->enabled = atoi(argv[i]);
        }
        else if (strcmp(azColName[i], "password") == 0)
        {
            strncpy(user->password, argv[i], sizeof(user->password));
        }
    }
    return 0;
}
