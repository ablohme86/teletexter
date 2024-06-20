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




int select_from_db(const char *sql, select_callback callback, void *data)
{
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        log_err_message("[%s] Failed to prepare statement: %s",DB_INTERFACE, sqlite3_errmsg(db));
        return rc;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int column_count = sqlite3_column_count(stmt);
        char **values = (char **)malloc(column_count * sizeof(char *));
        char **columns = (char **)malloc(column_count * sizeof(char *));

        for (int i = 0; i < column_count; i++) {
            values[i] = (char *)sqlite3_column_text(stmt, i);
            columns[i] = (char *)sqlite3_column_name(stmt, i);
        }

        callback(data, column_count, values, columns);

        free(values);
        free(columns);
    }

    if (rc != SQLITE_DONE) {
        log_err_message("[%s] Failed to execute query: %s",DB_INTERFACE, sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    if (rc == SQLITE_OK || SQLITE_DONE)
    {
        return 1;
    }
    return 0;
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

int execute_sql(const char *sql, const char *param_types, int param_count, ...)
{
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        log_err_message("[%s] Failed to prepare statement: %s",DB_INTERFACE, sqlite3_errmsg(db));
        return rc;
    }

    va_list args;
    va_start(args, param_count);

    int param_index = 1;
    const char *p = param_types;

    while (*p)
    {
        if (*p == 'i')
        {
            int param = va_arg(args, int);
            sqlite3_bind_int(stmt, param_index, param);
        }
        else if (*p == 's')
        {
            const char *param = va_arg(args, const char *);
            sqlite3_bind_text(stmt, param_index, param, -1, SQLITE_STATIC);
        }
        else if (*p == 'd')
        {
            double param = va_arg(args, double);
            sqlite3_bind_double(stmt, param_index, param);
        }
        else if (*p == 'n')
        {
            sqlite3_bind_null(stmt, param_index);
        }
        else
        {
            log_err_message("[%s] Unknown parameter type: %c",DB_INTERFACE, *p);
            va_end(args);
            sqlite3_finalize(stmt);
            return SQLITE_ERROR;
        }
        param_index++;
        p++;
    }

    va_end(args);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        log_err_message("[%s] Cannot perform SQL operation, SQL error: %s", DB_INTERFACE,sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    sqlite3_finalize(stmt);

    return SQLITE_OK;
}

int init_db(char *db_name)
{
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
        rc = create_db(sql_commands);
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





