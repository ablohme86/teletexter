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
    char sql[256];
    char *errmsg;
    snprintf(sql,sizeof(sql), "SELECT username FROM users WHERE username = ?");
    int res = execute_sql(sql,&errmsg, "s", 1, user->username);
    if (res == 0)
    {
        // User does not exist, go create
        snprintf(sql,sizeof(sql),"INSERT INTO users (username,password,access_level) VALUES (?,?,?)");
        int aures = execute_sql(sql,&errmsg,"ssi",3,user->username,user->password,user->access_level);
        if (aures == SQLITE_OK)
        {
            return 0;
        }
        log_err_message("[DATABASE] Could not query %s: %s",sql,errmsg);
        return -1;
    }
    if (res == SQLITE_ERROR)
    {
        log_err_message("[DATABASE] Could not query %s: %s",sql,errmsg);
    }
    return -1;
}

int get_user_by_id(int id, User *user)
{
    char sql[256];
    snprintf(sql, sizeof(sql), "SELECT * FROM users WHERE id = %d", id);
    return select_from_db(sql, user_callback, user);
}
int check_user_login(const char *username, const char *pwd, User *user)
{
    const char *sql = "SELECT * FROM users WHERE username = ? AND password = ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) 
    {
        log_err_message("[DATABASE] Failed to prepare statement: %s", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, pwd, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) 
    {
        int column_count = sqlite3_column_count(stmt);
        char *values[column_count];
        char *columns[column_count];

        for (int i = 0; i < column_count; i++) 
        {
            values[i] = (char *)sqlite3_column_text(stmt, i);
            columns[i] = (char *)sqlite3_column_name(stmt, i);
        }

        user_callback(user, column_count, values, columns);
        sqlite3_finalize(stmt);
        return 1;  // User found
    } 
    else 
    {
        if (rc != SQLITE_DONE) 
        {
            log_err_message("[DATABASE] Failed to execute query: %s", sqlite3_errmsg(db));
        }
        sqlite3_finalize(stmt);
        return 0;  // User not found
    }
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
