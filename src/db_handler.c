//
// Created by Alexander Blohme on 18/06/2024.
//

#include "../include/db.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

extern sqlite3 *db;
char *err_msg;


int init_db(char *db_name)
{
    printf("Initializing GPIO ports from db...\n");
    int rc = sqlite3_open(db_name, &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }


}