#include <wiringPi.h>
#include <lcd.h>
#include <stdio.h>
// Tilpass disse verdiene basert på ditt LCD-display og koblingsoppsett
#define I2C_ADDR 0x27 // I2C-adressen til ditt LCD
#define LCD_CHARS 16 // Antall kolonner i LCD-en
#define LCD_ROWS 2   // Antall rader i LCD-en

int main() {
    wiringPiSetup();
    
    int lcd;
    if ((lcd = lcdInit(LCD_ROWS, LCD_CHARS, 4, 11, 10, 0, 1, 2, 3, 0, 0, 0, 0)) < 0) {
        fprintf(stderr, "lcdInit failed\n");
        return -1;
    }
    
    lcdPosition(lcd, 0, 0); // Set position to first column, first row
    lcdPuts(lcd, "Hello, world!"); // Print text to LCD

    return 0;
}
