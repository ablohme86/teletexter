#ifndef DISABLE_LCD
#include "../include/lcd/lcd_disp.h"
#endif
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include "../include/server/server.h"
#include "../include/message/message.h"
#include "../include/config/config.h"
#include "../include/version.h"
#include "../include/db/db_handler.h"
#include "../include/app_args.h"
#include "../include/log.h"


void handle_signal(int signal) 
{
    char past_or_present_tense[20];
    char action[50];
    switch (signal)
    {
        case SIGTERM:
            snprintf(past_or_present_tense,sizeof(past_or_present_tense), "TeleTexter was");
            snprintf(action,sizeof(action), "terminated x(");
            log_sys_message("[SYSTEM] Service is stopping due to SIGTERM signal");
        break;
        
        case SIGHUP:
            snprintf(past_or_present_tense,sizeof(past_or_present_tense), "TeleTexter is");
            snprintf(action,sizeof(action), "restarting...");
            log_sys_message("[SYSTEM] Service is restarting due to SIGHUP signal");
        break;
        
        case SIGINT:
            snprintf(past_or_present_tense,sizeof(past_or_present_tense), "TeleTexter was");
            snprintf(action,sizeof(action), "terminated x(");
            log_sys_message("[SYSTEM] Service is stopping due to SIGINT signal");
        break;
        
        default:
            snprintf(past_or_present_tense,sizeof(past_or_present_tense), "TeleTexter recvd");
            snprintf(action,sizeof(action), "unknown signal!");
            log_sys_message("[SYSTEM] Service is stopping due to some strange unknown signal in the ether...");
            
        break;
    };
    lcd_text(past_or_present_tense,1,CENTER);
    lcd_text(action,2,CENTER);    
    close_server();
    exit(0);
}


int main(const int argc, char **argv)
{
    signal(SIGTERM, handle_signal);
    signal(SIGHUP, handle_signal);
    signal(SIGINT, handle_signal);


    printf("\n\n        TeleTexter v%d.%d\n", MAJOR, MINOR);
    printf("Copyright (c) 2024 Alexander Blohme\n-----------------------------------\n\n");
    if (manage_startup_args(argc, argv) == 0)
    {
        log_sys_message("Loading configuration file %s...",configPath);
        loadConfig(configPath);
        if (arg_srv_port != 0)
        {
            config.serverConfig.port = arg_srv_port;
        }
        if (strlen(config.serverConfig.blackListFile) > 0 && strlen(config.serverConfig.whiteListFile) > 0)
        {
            log_err_message("Both BlackList and WhiteList has been set in %s! Please remove one of them!", configPath);
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

    // sett velkomstmelding på skjermen
    char welcomeTxt_line1[config.lcdConfig.lcdWidth];
    char welcometxt_line2[30];
    snprintf(welcomeTxt_line1, sizeof(welcomeTxt_line1), "TeleTexter v%d.%d", MAJOR, MINOR);
    snprintf(welcomeTxt_line2,sizeof(welcomeTxt_line2), "Host: %s", config.serverConfig.serverHost);

    set_line_text(welcomeTxt_line1, 1, "LEFT");
    set_line_text(welcomeTxt_line2, 2, "CENTER");
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
    no_bglight_bit = config.lcdConfig.lcdNoBacklight;
    line1_addr = config.lcdConfig.lcdLine1Addr;
    line2_addr = config.lcdConfig.lcdLine2Addr;
    line3_addr = config.lcdConfig.lcdLine3Addr;
    line4_addr = config.lcdConfig.lcdLine4Addr;
    enable_bit = config.lcdConfig.lcdEnableBit;
#endif
}
