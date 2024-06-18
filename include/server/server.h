#ifndef SERVER_H
#define SERVER_H

#include "handle_client.h"
#define BUFFER_SIZE 1024

// Server replies & status messages:
#define INVALID_ALIGN 100
#define LINE_NOT_AVAILABLE 101
#define LINE_CANNOT_BE_ZERO 102
#define MSG_SET_OK 200
#define GENERAL_ERR 300
#define INVALID_CMD 301
#define DATABASE_QUERY_ERROR 340
#define INVALID_ARG 302
#define MISSING_ARGS 303
#define USER_BLOCKED 310
#define INVALID_INPUT 304
#define LOGIN_FAILED 305
#define MISSING_IDENT 306
#define INVALID_CREDENTIALS 307
#define ACCESS_DENIED 308
#define LINE_ARG_NOT_NUMBER 309
#define MESSAGE_SENT 200
#define LOGIN_OK 201

// LCD commands reply
#define LCD_CLEARED 100
#define MESSAGE_SET 101



int start_server();
char *get_ip(client_t *cli);
void setuplcd();
void close_server();
void ok_status(client_t *cli, int status_no,char *cust_msg,...);
void bad_status(client_t *cli, int status_no,char *cust_msg,...);
void send_server_ident(client_t *cli);

#endif /* SERVER_H */
