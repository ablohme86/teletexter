//
// Created by Alexander Blohme on 20/06/2024.
//
#include "../../include/user/user.h"

#include "../../include/db/db_handler.h"
#include "../../include/log.h"

#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>


int add_user(User *user)
{
    const char *sql = "SELECT username FROM users WHERE username = ?";
    const void *params[] = {user->username};
    const enum ParamType param_types[] = {PARAM_TEXT};

    int res = execute_sql(sql, 1, params, param_types, user_callback, user);

    if (res == 0) // User does not exist, go create
    {
        sql = "INSERT INTO users (username, password, access_level) VALUES (?, ?, ?)";
        const void *insert_params[] = {user->username, user->password, &(user->access_level)};
        const enum ParamType insert_param_types[] = {PARAM_TEXT, PARAM_TEXT, PARAM_INT};

        int aures = execute_sql(sql, 3, insert_params, insert_param_types, NULL, NULL);

        if (aures == 0)
        {
            return 0;
        }
        fprintf(stderr, "[DATABASE] Could not query %s: %s\n", sql, sqlite3_errmsg(db));
        return -1;
    }

    if (res == SQLITE_ERROR)
    {
        fprintf(stderr, "[DATABASE] Could not query %s: %s\n", sql, sqlite3_errmsg(db));
    }

    return -1;
}

int get_user_by_id(int user_id, User *user) {
    const char *sql = "SELECT * FROM users WHERE id=?";
    const void *params[] = {&user_id};
    const enum ParamType param_types[] = {PARAM_INT};

    int rc = execute_sql( sql, 1, params, param_types, user_callback, user);
    printf("Get user by id result is: %d\n", rc);
    return rc == 0;
}
int check_user_login(const char *username, const char *pwd, User *user)
{
    const char *sql = "SELECT * FROM users WHERE username=? AND password=?";
    printf("Checking user '%s' with pwd '%s'\n",username,pwd);
    const void *params[] = {username, pwd};
    const enum ParamType param_types[] = {PARAM_TEXT, PARAM_TEXT};

    int rc = execute_sql( sql, 2, params, param_types, user_callback, user);
    printf("User login result is: %d\n", rc);
    if (rc == 0)
    {
        return 0;
    }
    return 1;
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
            strncpy(user->username, argv[i], sizeof(user->username) - 1);
            user->username[sizeof(user->username) - 1] = '\0';  // Ensure null-termination
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
            strncpy(user->password, argv[i], sizeof(user->password) - 1);
            user->password[sizeof(user->password) - 1] = '\0';  // Ensure null-termination
        }
    }
    return 0;
}
