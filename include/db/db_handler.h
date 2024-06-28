//
// Created by Alexander Blohme on 18/06/2024.
//
#ifndef DB_HANDLER_H
#define DB_HANDLER_H
#include <sqlite3.h>
#define DB_INTERFACE "DATABASE"

// Funksjoner for å samhandle med databasen

enum ParamType {
    PARAM_INT,
    PARAM_TEXT   ,
    PARAM_LONG
};
typedef int (*result_callback)(void *data, int argc, char **argv, char **col_names);
int init_db(char *db_name);
int execute_sql(const char *sql, int param_count, const void **params, const enum ParamType *param_types, result_callback callback, void *callback_data);
int create_db(const char *sql_commands);
int query_sql(const char *sql);

extern sqlite3 *db;

#endif //DB_H
