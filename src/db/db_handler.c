//
// Created by Alexander Blohme on 18/06/2024.
//
#include "../../include/db/db_handler.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

sqlite3 *db;
char *err_msg;


int execute_sql_commands(const char *sql_commands)
{
    int rc = sqlite3_exec(db, sql_commands, 0, 0, &err_msg);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to execute SQL commands: %s\n", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }

    return SQLITE_OK;
}

int init_db(char *db_name)
{
    printf("Loading database from file %s\n", db_name);

    int is_new_db = access(db_name, F_OK) == -1;

    int rc = sqlite3_open(db_name, &db);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }

    if (is_new_db)
    {
        printf("Database file %s is new. Initializing...\n", db_name);
        const char *sql_commands =
            "BEGIN TRANSACTION;"
            "CREATE TABLE IF NOT EXISTS users ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "username TEXT,"
            "password TEXT,"
            "access_level INTEGER,"
            "enabled INTEGER"
            ");"
            "CREATE TABLE IF NOT EXISTS messages ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "date TEXT,"
            "time TEXT,"
            "message TEXT,"
            "poster_id INTEGER,"
            "status INTEGER,"
            "FOREIGN KEY(poster_id) REFERENCES users(id)"
            ");"
            "CREATE TABLE IF NOT EXISTS gpio_a ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "value INTEGER,"
            "name TEXT,"
            "gpio_pin INTEGER,"
            "io INTEGER"
            ");"
            "CREATE TABLE IF NOT EXISTS gpio_d ("
            "gpio_pin INTEGER,"
            "io INTEGER,"
            "name TEXT,"
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "value INTEGER"
            ");"
            "COMMIT;";
        rc = execute_sql_commands(sql_commands);
        if (rc != SQLITE_OK)
        {
            fprintf(stderr, "Failed to initialize database schema on file %s\n", db_name);
            sqlite3_close(db);
            exit(1);
        }
        printf("New database %s initialized!\n", db_name);
    }
    else
    {
        //printf("Database already exists. Skipping table creation.\n");
    }

    return rc;
}
int add_user(User *user)
{
    char *err_msg = 0;
    char sql[256];

    snprintf(sql, sizeof(sql), "INSERT INTO users (username, password, access_level, enabled) VALUES ('%s', '%s', %d, %d);", user->username, user->password, user->access_level, user->enabled);

    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }

    return SQLITE_OK;
}


int get_user(int id, User *user)
{
    sqlite3_stmt *stmt;
    char sql[256];

    snprintf(sql, sizeof(sql), "SELECT * FROM users WHERE id = %d;", id);

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW)
    {
        user->id = sqlite3_column_int(stmt, 0);
        strcpy(user->username, (const char *)sqlite3_column_text(stmt, 1));
        strcpy(user->password, (const char *)sqlite3_column_text(stmt, 2));
        user->access_level = sqlite3_column_int(stmt, 3);
        user->enabled = sqlite3_column_int(stmt, 4);
    }

    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int check_user_login( const char *username, const char *pwd, User *user)
{
    sqlite3_stmt *stmt;
    char sql[256];

    snprintf(sql, sizeof(sql), "SELECT * FROM users WHERE username = %s AND password = %s", username,pwd);

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW)
    {
        user->id = sqlite3_column_int(stmt, 0);
        strcpy(user->username, (const char *)sqlite3_column_text(stmt, 1));
        strcpy(user->password, (const char *)sqlite3_column_text(stmt, 2));
        user->access_level = sqlite3_column_int(stmt, 3);
        user->enabled = sqlite3_column_int(stmt, 4);
    }

    sqlite3_finalize(stmt);
    return SQLITE_OK;
}


int update_user(User *user) {
    char *err_msg = 0;
    char sql[256];

    snprintf(sql, sizeof(sql), "UPDATE users SET username = '%s', password = '%s', access_level = %d, enabled = %d WHERE id = %d;",
             user->username, user->password, user->access_level, user->enabled, user->id);

    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }

    return SQLITE_OK;
}

int delete_user(int id) {
    char *err_msg = 0;
    char sql[256];

    snprintf(sql, sizeof(sql), "DELETE FROM users WHERE id = %d;", id);

    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }

    return SQLITE_OK;
}
