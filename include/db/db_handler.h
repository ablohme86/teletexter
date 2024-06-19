//
// Created by Alexander Blohme on 18/06/2024.
//

#ifndef DB_HANDLER_H
#define DB_HANDLER_H
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DB_MESSAGE_IS_NOT_NEW 700
#define DB_SAVE_OK SQLITE_OK

// Strukturer som representerer tabellene.
typedef struct {
    unsigned int id;
    char username[100];
    char password[100];
    unsigned int access_level;
    unsigned int enabled;
} User;

typedef struct {
    unsigned int id;
    char date[11];
    char time[9];
    char message[256];
    unsigned int poster_id;
    unsigned int status;    // 0 for active, 1 for archived
} Message;

// Funksjoner for å samhandle med databasen
int add_user(User *user);
int get_user(int id, User *user);
int update_user(User *user);
int delete_user(int id);

int db_save_message( Message *msg);
int db_get_message( int id, Message *msg);
int update_message( Message *msg);
int delete_message( int id);
int check_user_login( const char *username, const char *pwd, User *user);
int init_db(char *db_name);




#endif //DB_H
