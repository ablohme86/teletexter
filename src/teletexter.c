#include <stdio.h>
#include <ncurses.h>

#ifdef DISABLE_LCD
#include "../include/lcd_disp.h"
#endif
#include "../include/server.h"
#include "../include/config.h"
#include "../include/version.h"
#include "../include/console.h"


int main()
{
    init_console(); // Initialize console

    printf("        TeleTexter v%d.%d\nCopyright (c) 2024 Alexander Blohme\n===================================\n", MAJOR, MINOR);
    printf("Loading configuration file...\n");

    loadConfig("./configs/teletexter.cfg"); // last konfigen

#ifdef LINUX
    print_to_console("Initializing LCD display...\n");

    setuplcd();  // sleng inn nødvendige variabler fra konfig til lcd'en

    i2c_init(config.lcdConfig.lcdDeviceFile,config.lcdConfig.lcdAddress); // aktiver / start opp lcd biblioteket med dev fil og addr fra config
    
    // sett velkomstmelding på skjermen
    char welcomeTxt_line1[config.lcdConfig.lcdWidth];
    snprintf(welcomeTxt_line1,sizeof(welcomeTxt_line1),"TeleTexter v%d.%d", MAJOR,MINOR);
    lcd_text(welcomeTxt_line1,1,LEFT);
    lcd_text("By A. Blohme",2,CENTER);
#endif

    // last inn konfigurasjonsfila    

    print_to_console("Server starting...\n");


 
    return start_server();
}
// Put in all nessescary parameters for the lcd
void setuplcd()
{

#ifdef LINUX
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