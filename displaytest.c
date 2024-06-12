#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define LCD_ADDRESS 0x27
#define LCD_WIDTH 20
#define LCD_ROWS 4

#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define CLEAR_DISPLAY 0x01
#define ENABLE_BIT 0b00000100

#define LEFT 0
#define RIGHT 1
#define CENTER 2

#define LINE_1 0x80
#define LINE_2 0xC0
#define LINE_3 0x94
#define LINE_4 0xD4

int i2c_bus;

void delay(int milliseconds) {
    usleep(milliseconds * 1000);
}

void i2c_init(const char *device) {
    i2c_bus = open(device, O_RDWR);
    if (i2c_bus < 0) {
        fprintf(stderr, "Failed to open the i2c bus\n");
        exit(1);
    }
}



void i2c_write_byte(int byte) 
{
    unsigned char buffer[2];
    buffer[0] = byte;
    buffer[1] = byte | ENABLE_BIT;
    write(i2c_bus, buffer, 2);
    buffer[1] = byte & ~ENABLE_BIT;
    write(i2c_bus, buffer, 2);
    usleep(500);
}


void write_command(uint8_t byte) {
    i2c_write_byte(byte);
    i2c_write_byte(byte | ENABLE_BIT);
    delay(1);
    i2c_write_byte(byte & ~ENABLE_BIT);
    delay(1);
}

void lcd_write(uint8_t byte, uint8_t mode) {
    uint8_t backlight_mode = LCD_BACKLIGHT;
    if (mode == 0) {
        backlight_mode = LCD_NOBACKLIGHT;
    }
    write_command(mode | (byte & 0xF0) | backlight_mode);
    write_command(mode | ((byte << 4) & 0xF0) | backlight_mode);
}

void lcd_text(const char *text, uint8_t line, uint8_t align) {
    uint8_t lcd_line;
    switch(line) {
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
    int text_len = strlen(text);
    char *aligned_text = (char *)malloc(LCD_WIDTH + 1);
    if (aligned_text == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    if (align == LEFT) {
        snprintf(aligned_text, LCD_WIDTH + 1, "%-*.*s", LCD_WIDTH, LCD_WIDTH, text);
    } else if (align == RIGHT) {
        snprintf(aligned_text, LCD_WIDTH + 1, "%*.*s", LCD_WIDTH, LCD_WIDTH, text);
    } else if (align == CENTER) {
        int padding = (LCD_WIDTH - text_len) / 2;
        snprintf(aligned_text, LCD_WIDTH + 1, "%*.*s%s%*.*s", padding, padding, "", text, padding, padding, "");
    }
    for (int i = 0; i < LCD_WIDTH; i++) {
        lcd_write(aligned_text[i], 1);
    }
    free(aligned_text);
}

void lcd_backlight(uint8_t turn_on) {
    if (turn_on) {
        lcd_write(0, 1);
    } else {
        lcd_write(LCD_NOBACKLIGHT, 0);
    }
}

void lcd_clear() {
    write_command(CLEAR_DISPLAY);
}

int main() {
    const char *device = "/dev/i2c-1";
    i2c_init(device);
	delay(1000);
    write_command(0x33);
    write_command(0x32);
    write_command(0x06);
    write_command(0x0C);
    write_command(0x28);
    lcd_clear();

    lcd_text("Hello, World!", 1, LEFT);
    lcd_text("This is line 2", 2, CENTER);
//    lcd_text("Line 3", 3, RIGHT);
//    lcd_text("Line 4", 4, LEFT);

    return 0;
}
