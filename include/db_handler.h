//
// Created by Alexander Blohme on 18/06/2024.
//

#ifndef DB_HANDLER_H
#define DB_HANDLER_H
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Strukturer som representerer tabellene
typedef struct {
    int id;
    char username[100];
    char password[100];
    int access_level;
    int enabled;
} User;

typedef struct {
    int id;
    char date[11];
    char time[9];
    char message[256];
    int poster_id;
    int status;
} Message;

// Funksjoner for å samhandle med databasen
int add_user(sqlite3 *db, User *user);
int get_user(sqlite3 *db, int id, User *user);
int update_user(sqlite3 *db, User *user);
int delete_user(sqlite3 *db, int id);

int add_message(sqlite3 *db, Message *msg);
int get_message(sqlite3 *db, int id, Message *msg);
int update_message(sqlite3 *db, Message *msg);
int delete_message(sqlite3 *db, int id);

int init_db(char *db_name);




#endif //DB_H
