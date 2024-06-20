//
// Created by Alexander Blohme on 18/06/2024.
//
#ifndef DB_HANDLER_H
#define DB_HANDLER_H
#include <sqlite3.h>
#define DB_MESSAGE_IS_NOT_NEW 700
#define DB_SAVE_OK SQLITE_OK




// Funksjoner for å samhandle med databasen


typedef int (*select_callback)(void*, int, char**, char**);
int init_db(char *db_name);
int execute_sql(const char *sql, const char *param_types, int param_count, ...);
int create_db(const char *sql_commands);
int query_sql(const char *sql);
int select_from_db(const char *sql, select_callback callback, void *data);

extern sqlite3 *db;

#endif //DB_H
