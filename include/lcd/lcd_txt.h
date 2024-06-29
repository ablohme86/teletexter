//
// Created by Alexander Blohme on 26/06/2024.
//

#ifndef LCD_TXT_H
#define LCD_TXT_H

int set_lcd_line_text(const char *msg, unsigned int line,const char *align);
int set_lcd_text(const char *mmsg, unsigned int line);
int scroll_lcd_line(unsigned int line);
void clear_lcd_line(unsigned int line);
void clear_lcd_lines();

#endif //LCD_TXT_H
