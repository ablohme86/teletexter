//
// Created by Alexander Blohme on 25/06/2024.
//
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

#include "../include/signals.h"
#include "../include/log.h"
#include "../include/message/message.h"
#include "../include/server/server.h"
#include "../include/lcd/lcd_txt.h"

#define SIGTERM 15
#define SIGINT 2
#define SIGHUP 1

void handle_sigs(int signal)
{
    char past_or_present_tense[20];
    char action[50];
    switch (signal)
    {
        case SIGTERM:
            snprintf(past_or_present_tense,sizeof(past_or_present_tense), "TeleTexter is");
        snprintf(action,sizeof(action), "not running");
        log_sys_message("[SYSTEM] Service is stopping due to SIGTERM signal");
        break;

        case SIGHUP:
            snprintf(past_or_present_tense,sizeof(past_or_present_tense), "TeleTexter is");
        snprintf(action,sizeof(action), "restarting...");
        log_sys_message("[SYSTEM] Service is restarting due to SIGHUP signal");
        break;

        case SIGINT:
            snprintf(past_or_present_tense,sizeof(past_or_present_tense), "TeleTexter is");
        snprintf(action,sizeof(action), "not running");
        log_sys_message("[SYSTEM] Service is stopping due to SIGINT signal");
        break;

        default:
            snprintf(past_or_present_tense,sizeof(past_or_present_tense), "TeleTexter recvd");
        snprintf(action,sizeof(action), "unknown signal!");
        log_sys_message("[SYSTEM] Service is stopping due to some strange unknown signal in the ether...");

        break;
    };
    set_lcd_line_text(past_or_present_tense,1,"CENTER");
    set_lcd_line_text(action,2,"CENTER");
    close_server();
    exit(0);
}

