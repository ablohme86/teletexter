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
#include <string.h>

sqlite3 *db;
char *err_msg;

int execute_sql(const char *sql, int param_count, const void **params, const enum ParamType *param_types, result_callback callback, void *callback_data)
{
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    for (int i = 0; i < param_count; i++)
    {
        switch (param_types[i])
        {
            case PARAM_INT:
                sqlite3_bind_int(stmt, i + 1, *((int *)params[i]));
                break;
            case PARAM_TEXT:
                sqlite3_bind_text(stmt, i + 1, (const char *)params[i], -1, SQLITE_STATIC);
                break;
            case PARAM_LONG:
                sqlite3_bind_int64(stmt, i + 1, *((long *)params[i]));
                break;
        }
    }

    int found = 0; // Track if any row was found
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        int column_count = sqlite3_column_count(stmt);
        char *values[column_count];
        char *columns[column_count];

        for (int i = 0; i < column_count; i++)
        {
            columns[i] = (char *)sqlite3_column_name(stmt, i);
            switch (sqlite3_column_type(stmt, i))
            {
                case SQLITE_INTEGER:
                {
                    int value = sqlite3_column_int(stmt, i);
                    values[i] = (char *)malloc(32); // Allocate space for integer as string
                    snprintf(values[i], 32, "%d", value);
                    break;
                }
                case SQLITE_TEXT:
                {
                    const unsigned char *text = sqlite3_column_text(stmt, i);
                    values[i] = strdup((const char *)text); // Duplicate the text
                    break;
                }
                default:
                    values[i] = NULL;
                    break;
            }
        }

        if (callback)
        {
            callback(callback_data, column_count, values, columns);
        }

        // Free the allocated memory for values
        for (int i = 0; i < column_count; i++)
        {
            if (values[i])
            {
                free(values[i]);
            }
        }

        found = 1; // At least one row was found
    }

    if (rc != SQLITE_DONE && rc != SQLITE_ROW)
    {
        fprintf(stderr, "Failed to execute query: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);

    if (callback) {
        return found ? 1 : 0;
    } else {
        return (rc == SQLITE_DONE) ? 0 : rc;
    }
}


int create_db(const char *sql_commands)
{
    int rc = sqlite3_exec(db, sql_commands, 0, 0, &err_msg);

    if (rc != SQLITE_OK)
    {
        log_err_message("[%s] Failed to create tables of database: %s", DB_INTERFACE);
        sqlite3_free(err_msg);
        return rc;
    }

    return SQLITE_OK;
}

int init_db(char *db_name)
{
    char *errmsg;
    int is_new_db = access(db_name, F_OK) == -1;

    int rc = sqlite3_open(db_name, &db);
    if (rc != SQLITE_OK)
    {
        log_err_message("[%s] Cannot open database: '%s'", DB_INTERFACE,sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }

    if (is_new_db)
    {
        log_sys_message("[%s] Database file %s is new. Initializing...",DB_INTERFACE, db_name);
        const char *sql_commands =
            "BEGIN TRANSACTION;"
            "CREATE TABLE IF NOT EXISTS users ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "username TEXT,"
            "password TEXT,"
            "access_level INTEGER DEFAULT 1,"
            "enabled INTEGER DEFAULT 1"
            ");"
            "CREATE TABLE IF NOT EXISTS messages ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "datetime INTEGER,"
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

        const char *sql_default_values =
            "INSERT INTO users (username,password,access_level) VALUES ('admin','admin',3)";

        rc = create_db(sql_commands);
        execute_sql(sql_default_values,0,NULL,NULL,NULL,NULL);
        log_err_message("Sql error: %s", errmsg);
        if (rc != SQLITE_OK)
        {
            log_err_message( "[%s] Failed to initialize database schema on file %s",DB_INTERFACE, db_name);
            sqlite3_close(db);
            exit(1);
        }
        log_sys_message("[%s] New database %s initialized!",DB_INTERFACE, db_name);
    }
    return rc;
}





