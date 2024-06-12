#define LCD_ADDRESS 0x27
#define LCD_WIDTH 16
#define LCD_ROWS 2


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



void delay(int ms);
void i2c_init(const char *dev);
void i2c_write_byte(uint8_t byte);
void write_command(uint8_t byte);
void lcd_write(uint8_t byte, uint8_t mode);
void lcd_text(const char *text, uint8_t line, uint8_t align);
void lcd_backlight(uint8_t turn_on);
void lcd_clear();