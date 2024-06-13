/******************************************************************************
 *                                                                            *
 *                               TeleTexter                                   *
 *                                                                            *
 *                Developed by: Alexander Blohme                              *
 *                Initial Release: 2024                                       *
 *                                                                            *
 *   lcdlib.c - For handling the i2c lcd display                              *
 *                                                                            *
 * This program is free software: you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the               *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program. If not, see <https://www.gnu.org/licenses/>.      *
 *                                                                            *
 ******************************************************************************/


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "../include/lcdlib.h"

int i2c_bus;

void delay(int milliseconds) 
{
    usleep(milliseconds * 1000);
}

void i2c_init(const char *device) 
{
    i2c_bus = open(device, O_RDWR);
    if (i2c_bus < 0) 
    {
        fprintf(stderr, "Failed to open the i2c bus\n");
        exit(1);
    }
    if (ioctl(i2c_bus, I2C_SLAVE, LCD_ADDRESS) < 0) 
    {
        perror("Failed to acquire bus access and/or talk to slave");
        exit(1);
    }
    
    // Send init params
    write_command(0);
    write_command(0x33);
    write_command(0x32);
    write_command(0x06);
    write_command(0x0C);
    write_command(0x28);
    
    // Clear LCD screen
    lcd_clear();
    
    delay(10); // Optional delay for stabilization
}


void i2c_write_byte(uint8_t byte) 
{
    if (write(i2c_bus, &byte, 1) != 1) 
    {
        fprintf(stderr, "Failed to write to the i2c bus\n");
        exit(1);
    }
}

void write_command(uint8_t byte) 
{
    i2c_write_byte(byte);
    i2c_write_byte(byte | ENABLE_BIT);
    delay(1);
    i2c_write_byte(byte & ~ENABLE_BIT);
    delay(10);
}

void lcd_write(uint8_t byte, uint8_t mode) 
{
    uint8_t backlight_mode = LCD_BACKLIGHT;
    if (mode == 0) 
    {
        backlight_mode = LCD_NOBACKLIGHT;
    }
    write_command(mode | (byte & 0xF0) | backlight_mode);
    write_command(mode | ((byte << 4) & 0xF0) | backlight_mode);
   
}

void lcd_text(const char *text, uint8_t line, uint8_t align) 
{

     uint8_t lcd_line;
     switch(line) 
     {
         case 1:
             lcd_line = LINE_1;
             break;
         case 2:
             lcd_line = LINE_2;
             break;
         case 3:
             lcd_line = LINE_3;
             break;
         case 4:
             lcd_line = LINE_4;
             break;
         default:
             lcd_line = LINE_1;
             break;
     }
     
     lcd_write(lcd_line, 0);
     
     // Oppdater tekstlinjen på LCD-skjermen
     static char lcd_lines[LCD_ROWS][LCD_WIDTH + 1] = {0}; // Lagrer teksten på hver linje
     if (align == LEFT) 
     {
         snprintf(lcd_lines[line - 1], LCD_WIDTH + 1, "%-.*s", LCD_WIDTH, text);
     } 
     else if (align == RIGHT) 
     {
         snprintf(lcd_lines[line - 1], LCD_WIDTH + 1, "%*.*s", LCD_WIDTH, LCD_WIDTH, text);
     } 
     else if (align == CENTER) 
     {
         int padding = (LCD_WIDTH - strlen(text)) / 2;
         snprintf(lcd_lines[line - 1], LCD_WIDTH + 1, "%*.*s%s%*.*s", padding, padding, "", text, LCD_WIDTH - padding - strlen(text), LCD_WIDTH - padding - strlen(text), "");
     }
     
     // Fyll resten av linjen med mellomrom
     int text_len = strlen(lcd_lines[line - 1]);
     for (int i = text_len; i < LCD_WIDTH; i++) 
     {
         lcd_lines[line - 1][i] = ' ';
     }
     
     for (int i = 0; i < LCD_WIDTH; i++) 
     {
         lcd_write(lcd_lines[line - 1][i], 1);
     }
}

void lcd_backlight(uint8_t turn_on) 
{
    if (turn_on) 
    {
        write_command(0);
    } 
    else 
    {
        write_command(LCD_NOBACKLIGHT);
    }
}

void lcd_clear() 
{
    //lcd_write(CLEAR_DISPLAY,1);
   
}
