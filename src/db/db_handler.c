//
// Created by Alexander Blohme on 18/06/2024.
//
#include "../../include/db/db_handler.h"
#include "../../include/log.h"
#include "../../include/server/server.h"
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


int db_create_message(Message *msg)
{
    if (msg->id != 0)
    {
        return DB_MESSAGE_IS_NOT_NEW;
    }
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO messages (date,time,message,poster_id,status) VALUES (?,?,?,?,?)";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        log_err_message("[DATABASE] Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_free(stmt);
        return rc;
    }

    // Bind parameters
    sqlite3_bind_text(stmt, 0, msg->date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 1, msg->time, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2,msg->message,-1,SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3,msg->poster_id);
    sqlite3_bind_int(stmt,4,msg->status);

     rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        log_err_message("[DATABASE] Cannot create a new message in database, SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_free(err_msg);
        return rc;
    }


}


int check_user_login(const char *username, const char *pwd, User *user)
{
    sqlite3_stmt *stmt;
    const char *sql = "SELECT * FROM users WHERE username = ? AND password = ?";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        log_err_message("[DATABASE] Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    // Bind parameters

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, pwd, -1, SQLITE_STATIC);

    // Execute statement
    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW)
    {
        if (user == NULL)
        {
            log_err_message("[MEMORY] For some reason the User pointer was not initialized during client-connect! I will now fail miserably :´-(! Bye...");
        }


        user->id = sqlite3_column_int(stmt,0);
        strcpy(user->username, (const char *)sqlite3_column_text(stmt, 1));
        strcpy(user->password, (const char *)sqlite3_column_text(stmt, 2));
        user->access_level = sqlite3_column_int(stmt, 3);
        user->enabled = sqlite3_column_int(stmt, 4);
    }
    else if (rc == SQLITE_DONE)
    {
        return INVALID_CREDENTIALS;

    }
    else
    {
        log_err_message("[DATABASE] Failed to fetch data from SQL table: %s", sqlite3_errmsg(db));
        return DATABASE_QUERY_ERROR;
    }

    sqlite3_finalize(stmt);
    return LOGIN_OK;
}



