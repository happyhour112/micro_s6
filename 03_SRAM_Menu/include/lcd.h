#ifndef LCD_H
#define LCD_H
#include <stdint.h>
void lcd_init(void);
void lcd_command(uint8_t value);
void lcd_data(uint8_t value);
void lcd_cursor(uint8_t row, uint8_t column); /* zero based */
void lcd_puts(const char *text);
void lcd_line(uint8_t row, const char *text); /* pad to 16 columns */
void lcd_uint(uint16_t value);
void lcd_glyph(uint8_t slot, const uint8_t rows[8]);
#endif
