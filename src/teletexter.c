#ifndef DISABLE_LCD
#include "../include/lcd/lcd_disp.h"
#endif
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include "../include/server/server.h"
#include "../include/config/config.h"
#include "../include/version.h"
#include "../include/db/db_handler.h"
#include "../include/app_args.h"

volatile sig_atomic_t sigint_received = 0;

// Signalhåndteringsfunksjon
void sigint_handler(int sig)
{

    printf("sigint %d received! Now, HANDLE I-T!\n", sig);
    sigint_received = 1;
    
    close_server();    // properly close the server...
    // TODO:  close all remainign clients...
        
    exit(1);  // ...and get TFO'utta here!
}


int main(const int argc, char **argv)
{
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    printf("\n\n        TeleTexter v%d.%d\n", MAJOR, MINOR);
    printf("Copyright (c) 2024 Alexander Blohme\n-----------------------------------\n\n");
    if (manage_startup_args(argc, argv) == 0)
    {
        printf("Loading configuration file %s...\n",configPath);
        loadConfig(configPath);
        if (arg_srv_port != 0)
        {
            config.serverConfig.port = arg_srv_port;
        }
    }
    else
    {
        printf("Exiting...\n");
        exit(1);
    }
    init_db(config.serverConfig.dbFile);

#ifndef DISABLE_LCD
    printf("Initializing LCD display...\n");
    setuplcd();  // sleng inn nødvendige variabler fra konfig til lcd'en
    i2c_init(config.lcdConfig.lcdDeviceFile, config.lcdConfig.lcdAddress); // aktiver / start opp lcd biblioteket med dev fil og addr fra config

    // sett velkomstmelding på skjermen
    char welcomeTxt_line1[config.lcdConfig.lcdWidth];
    snprintf(welcomeTxt_line1, sizeof(welcomeTxt_line1), "TeleTexter v%d.%d", MAJOR, MINOR);
    lcd_text(welcomeTxt_line1, 1, LEFT);
    lcd_text("By A. Blohme", 2, CENTER);
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
