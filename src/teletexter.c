#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/lcd_disp.h"
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
        
    loadConfig("./configs/teletexter.cfg"); // last konfigen
    setuplcd();  // sleng inn nødvendige variabler fra konfig til lcd'en
    
    i2c_init(config.lcdConfig.lcdDeviceFile,config.lcdConfig.lcdAddress); // aktiver / start opp lcd biblioteket med dev fil og addr fra config
    
    // sett velkomstmelding på skjermen
    char welcomeTxt_line1[config.lcdConfig.lcdWidth];
    snprintf(welcomeTxt_line1,sizeof(welcomeTxt_line1),"TeleTexter v%d.%d", MAJOR,MINOR);
    lcd_text(welcomeTxt_line1,1,LEFT);
    lcd_text("By A. Blohme",2,CENTER);

    // last inn konfigurasjonsfila    


 
    return start_server();
}
// Put in all nessescary parameters for the lcd
void setuplcd()
{
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
    
                    
}