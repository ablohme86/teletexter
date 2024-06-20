//
// Created by Alexander Blohme on 20/06/2024.
//

#ifndef USER_H
#define USER_H

// Strukturer som representerer tabellene.
typedef struct {
    unsigned int id;
    char username[100];
    char password[100];
    unsigned int access_level;
    unsigned int enabled;
} User;

int add_user(User *user);
int get_user(int id, User *user);
int update_user(User *user);
int delete_user(int id);
int check_user_login(const char *username, const char *pwd, User *user);
int get_user_by_id(int id, User *user);
int user_callback(void *data, int argc, char **argv, char **azColName);

#endif //USER_H
