#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/lcd.h"
#include "../include/client.h"
#include "../include/commands.h"
#include "../include/messages.h"
#include "../include/ident.h"
#include "../include/server.h"
#include "../include/config.h"
#include "../include/version.h"

int main()
{
    printf("        TeleTexter v%d.%d\nCopyright (c) 2024 Alexander Blohme\n===================================\n",MAJOR,MINOR);
        
    loadConfig("./configs/teletexter.cfg");

    i2c_init(config.lcdConfig.lcdDeviceFile,config.lcdConfig.lcdAddress); // Initialize our i2c lib
    

    // sett app-info på skjermen & for litt error-sjekking samtidig..
    char welcomeTxt_line1[config.lcdConfig.lcdRows];
    snprintf(welcomeTxt_line1,sizeof(welcomeTxt_line1),"TeleTexter v%d.%d", MAJOR,MINOR);
    lcd_text(welcomeTxt_line1,1,LEFT);
    lcd_text("By A. Blohme",2,CENTER);

    // last inn konfigurasjonsfila    


 
    return start_server();
}
