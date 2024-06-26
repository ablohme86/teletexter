//
// Created by Alexander Blohme on 26/06/2024.
//

#ifndef HANDLE_LCDTXT_H
#define HANDLE_LCDTXT_H
#include "../server/handle_client.h"


void handle_lcd_line_scroll(client_t *cli, int argc, char **argv);
void handle_clear_lcd_display(client_t *cli, int argc, char **argv);
void handle_clear_lcd_line(client_t *cli, int argc, char **argv);
void handle_lcd_set_text(client_t *cli,int argc, char **argv);


#endif //HANDLE_LCDTXT_H
