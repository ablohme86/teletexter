//
// Created by Alexander Blohme on 20/06/2024.
//

#ifndef USER_H
#define USER_H
#define USR_INTERFACE "USER"
#define ALL_USERS 0
#define NORMAL_USER 1
#define MODERATOR_USER 2
#define ADMIN_USER 3

// Strukturer som representerer tabellene.
typedef struct {
    unsigned int id;
    char username[100];
    char password[100];
    unsigned int access_level;
    unsigned int enabled;
} User;

int add_user(User *user, char **errmsg);
int get_user(int id, User *user);
int update_user(User *user);
int delete_user(int id);
int check_user_login(const char *username, const char *pwd, User *user, char **errmsg);
int get_user_by_id(int id, User *user);
int user_callback(void *data, int argc, char **argv, char **azColName);

#endif //USER_H
