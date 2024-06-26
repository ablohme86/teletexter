//
// Created by Alexander Blohme on 26/06/2024.
//
// The main functions for sending / setting / getting text from the lcd panel!
//
#include "../../include/lcd/lcd_txt.h"
#include "../../include/lcd/lcd_disp.h"
#include "../../include/status.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    unsigned int line;
    char align[10];
    char buff[200];

} LCD_Line;
LCD_Line *lcdLines[128]; // stores all lines string data thats on display


void clear_lcd_line(unsigned int line)           // clears the specified line
{
    set_lcd_line_text(" ",line,"LEFT");
}
void clear_lcd_lines()
{
    for (unsigned int i = 1; i <= lcd_height; i++)
    {
        clear_lcd_line(i);
    }
}

int scroll_lcd_line(unsigned int sel_line)    // Scrolls the selected line
{
    if (lcdLines[sel_line] == NULL)
    {
        return 0;
    }
    if (lcd_scroll_enabled != 1)
    {
        return -1;      // scrolling is not enabled
    }
#ifndef DISABLE_LCD
    lcd_scroll(lcdLines[sel_line]->buff,sel_line,lcd_scroll_speed);
#endif
    return 0;
}


int set_lcd_line_text(const char *msg, unsigned int line,const char *align)
{
    if (line > lcd_height)
    {
        return INVALID_LINE;    // The set line is higher than available lines
    }
    if (lcdLines[line] == NULL)
    {
        // allokér minne til linjens peker
        lcdLines[line] = (LCD_Line *)malloc(sizeof(LCD_Line));
        lcdLines[line]->line = line;
    }
    int r_align = 0;

    if (strcmp(align,"LEFT") == 0)
    {
        r_align = LEFT;
    }
    else if (strcmp(align,"RIGHT") == 0)
    {
        r_align = RIGHT;
    }
    else if (strcmp(align,"CENTER") == 0)
    {
        r_align = CENTER;
    }
    else
    {
        return INVALID_ALIGN;
    }
    strncpy(lcdLines[line]->buff,msg, sizeof(lcdLines[line]->buff));
    strncpy(lcdLines[line]->align,align, sizeof(lcdLines[line]->align));

    if (strlen(msg) > lcd_width && lcd_scroll_speed == 1)
    {
        // Automatically set align to LEFT since it will bug the hell out of the display if we are trying to scroll
        // a centered message that cant be centered anyways!

#ifndef DISABLE_LCD
        lcd_text(msg,line,LEFT);
        scroll_lcd_line(line);
#endif

    }
    else // Una problema, vi kan skrive ut som vi vil
    {
#ifndef DISABLE_LCD
        lcd_text(msg,line,r_align);
#endif
    }
    return MESSAGE_SET;
}

