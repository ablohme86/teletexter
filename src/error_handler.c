//
// Created by Alexander Blohme on 17/06/2024.
//

#include "../include/error_handler.h"

#include <stdlib.h>

#include "../include/console.h"

void error_exit(char *msg)
{
    fprintf(stderr,"Error: %s. Qutting...\n", msg);
    exit(1);
}