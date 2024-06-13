#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <time.h>
#include "../include/lcdlib.h"
#include "../include/client.h"
#include "../include/commands.h"
#include "../include/messages.h"
#include "../include/ident.h"
#include "../include/server.h"

int main()
{
    const char *device = "/dev/i2c-1";
    i2c_init(device); // Initialize our i2c lib
    printf("TeleTexter v1.0\nCopyright (c) 2024 Alexander Blohme\n==============\n");
    printf("connected to LCD display!\n");

    lcd_text("TeleTexter v0.1",1,LEFT);
    lcd_text("By A. Blohme",2,CENTER);

    return start_server();
}
