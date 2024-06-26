#ifndef DISABLE_LCD
#include "../include/lcd/lcd_disp.h"
#include "../include/lcd/lcd_txt.h"
#endif
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "../include/signals.h"
#include <string.h>
#include <stdlib.h>
#include "../include/server/server.h"
#include "../include/config/config.h"
#include "../include/version.h"
#include "../include/db/db_handler.h"
#include "../include/app_args.h"
#include "../include/log.h"

int main(const int argc, char **argv)
{
    signal(SIGTERM, handle_sigs);
    signal(SIGHUP, handle_sigs);
    signal(SIGINT, handle_sigs);

    printf("\n\n        TeleTexter v%d.%d\n", MAJOR, MINOR);
    printf("Copyright (c) 2024 Alexander Blohme\n-----------------------------------\n\n");
    if (manage_startup_args(argc, argv) == 0)
    {
        loadConfig(configPath);
        if (arg_srv_port != 0)
        {
            config.serverConfig.port = arg_srv_port;
        }
        if (strlen(config.serverConfig.blackListFile) > 0 && strlen(config.serverConfig.whiteListFile) > 0)
        {
            fprintf(stderr,"Both BlackList and WhiteList has been set in %s! Please remove one of them!", configPath);
            exit(1);
        }
    }
    else
    {
        log_sys_message("Exiting...");
        exit(1);
    }
    init_db(config.serverConfig.dbFile);

#ifndef DISABLE_LCD
    log_sys_message("Initializing LCD display...");
    setuplcd();  // sleng inn nødvendige variabler fra konfig til lcd'en

    i2c_init(config.lcdConfig.lcdDeviceFile, config.lcdConfig.lcdAddress); // aktiver / start opp lcd biblioteket med dev fil og addr fra config
    char welcomeTxt_line1[config.lcdConfig.lcdWidth+1];
    char welcometxt_line2[config.lcdConfig.lcdWidth+1];
    char tt_title[config.lcdConfig.lcdWidth+1];
        
    clear_lines();
    
    snprintf(welcomeTxt_line1, sizeof(welcomeTxt_line1), "%s Ready",config.serverConfig.serverIdentifier);
    snprintf(welcometxt_line2,sizeof(welcometxt_line2), "%s:%d", config.serverConfig.serverHost,config.serverConfig.port);
    snprintf(tt_title,sizeof(tt_title),"TeleTexter v%d.%d",MAJOR,MINOR);

    set_lcd_line_text("Welcome to",1,"CENTER");
    set_lcd_line_text(tt_title, 2, "CENTER");
    sleep(1.5);
    set_lcd_line_text("(c) 2024 by",1,"CENTER");
    set_lcd_line_text("Alexander Blohme",2,"CENTER");
    sleep(2.3);
    set_lcd_line_text(welcomeTxt_line1,1,"CENTER");
    set_lcd_line_text(welcometxt_line2, 2, "CENTER");
#endif

    return start_server();
}

// Put in all nessescary parameters for the lcd
void setuplcd()
{
#ifndef DISABLE_LCD
    lcd_msg_maxlen = config.messageConfig.maxMessageLength;
    bglight_bit = config.lcdConfig.lcdBacklight;
    lcd_width = config.lcdConfig.lcdWidth;
    lcd_height = config.lcdConfig.lcdHeight;
    lcd_scroll_enabled = config.messageConfig.scrollLongMessages;
    lcd_scroll_speed = config.messageConfig.scrollSpeed;
    no_bglight_bit = config.lcdConfig.lcdNoBacklight;
    line1_addr = config.lcdConfig.lcdLine1Addr;
    line2_addr = config.lcdConfig.lcdLine2Addr;
    line3_addr = config.lcdConfig.lcdLine3Addr;
    line4_addr = config.lcdConfig.lcdLine4Addr;
    enable_bit = config.lcdConfig.lcdEnableBit;
#endif
}
