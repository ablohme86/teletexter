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
#include "../include/lcd_disp.h"
 

int i2c_bus;

uint8_t bglight_bit = LCD_BACKLIGHT;    // Sett disse til #DEFINE parameterene i lcd_disp.h hvis ingenting er definert i konfigurasjonsfilene
uint8_t no_bglight_bit = LCD_NOBACKLIGHT;
uint8_t cleardisp_bit = CLEAR_DISPLAY;
uint8_t line1_addr = LINE_1;
uint8_t line2_addr = LINE_2;
uint8_t line3_addr = LINE_3;
uint8_t line4_addr = LINE_4;
uint8_t enable_bit = ENABLE_BIT;

 int lcd_height;
 int lcd_width;
 int lcd_msg_maxlen;
void delay(int milliseconds) 
 {
     usleep(milliseconds * 1000);
 }
 
 void i2c_init(const char *device,uint8_t i2caddr) 
 {
    if (lcd_height == 0)
    {
        fprintf(stderr, "lcd_height (%d) is not set! Exiting",lcd_height);
        exit(1);
    }
    else if (lcd_width == 0)
    {
        fprintf(stderr, "lcd_width (%d) is not set! Exiting\n",lcd_width);
        exit(1);
    }
     i2c_bus = open(device, O_RDWR);
     if (i2c_bus < 0) 
     {
         fprintf(stderr, "Failed to open the i2c bus %s on address %d\n", device, i2caddr);
         exit(1);
     }
     if (ioctl(i2c_bus, I2C_SLAVE, LCD_ADDRESS) < 0) 
     {
     fprintf(stderr, "Failed to aquire bus access from %s on address %d\n", device, i2caddr);
         perror("Failed to acquire bus access and/or talk to slave");
         exit(1);
     }
     printf("Connected to I2C port %s on address %u\n", device, i2caddr);
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
     char limited_text[lcd_msg_maxlen + 1];  // +1 for null-terminatoren
      strncpy(limited_text, text, lcd_msg_maxlen);
      limited_text[lcd_msg_maxlen] = '\0';  // Sørg for at det er null-terminert
       
      uint8_t lcd_line;
      switch(line) 
      {
          case 1:
              lcd_line = line1_addr;
              break;
          case 2:
              lcd_line = line2_addr;
              break;
          case 3:
              lcd_line = line3_addr;
              break;
          case 4:
              lcd_line = line4_addr;
              break;
          default:
              lcd_line = line1_addr;
              break;
      }
      
      lcd_write(lcd_line, 0);
      
      // Oppdater tekstlinjen på LCD-skjermen
     char **lcd_lines = malloc(lcd_height * sizeof(char *));
     for (int i = 0; i < lcd_height; ++i)
     {
         lcd_lines[i] = malloc((lcd_width + 1) * sizeof(char));
     }
      if (align == LEFT) 
      {
          snprintf(lcd_lines[line - 1], lcd_width + 1, "%-.*s", lcd_width, limited_text);
      } 
      else if (align == RIGHT) 
      {
          snprintf(lcd_lines[line - 1], -lcd_width + 1, "%*.*s", lcd_width, lcd_width, limited_text);
      } 
      else if (align == CENTER) 
      {
          int padding = (lcd_width - strlen(limited_text)) / 2;
          snprintf(lcd_lines[line - 1], lcd_width + 1, "%*.*s%s%*.*s", padding, padding, "", limited_text, lcd_width - padding - strlen(limited_text), lcd_width - padding - strlen(limited_text), "");
      }
      
      // Fyll resten av linjen med mellomrom
      int text_len = strlen(lcd_lines[line - 1]);
      for (int i = text_len; i < lcd_width; i++) 
      {
          lcd_lines[line - 1][i] = ' ';
      }
      
      for (int i = 0; i < lcd_width; i++) 
      {
          lcd_write(lcd_lines[line - 1][i], 1);
      }
 }
 
 void lcd_backlight(uint8_t turn_on) 
 {
     if (turn_on) 
     {
         write_command(bglight_bit);
     } 
     else 
     {
         write_command(no_bglight_bit);
     }
 }
 

 
 void lcd_clear() 
 {
     //lcd_write(CLEAR_DISPLAY,1);
    
 }