#include "lcd.h"
#include <avr/io.h>
#include <util/delay.h>
/* DFR0009-compatible shield: RS=D8/PB0, E=D9/PB1,
   LCD DB4..DB7=Uno D4..D7/PD4..PD7. R/W is wired to GND.
   D10/PB2 belongs to the backlight circuit. We leave it high impedance.
   UART pins PD0/PD1 and I2C pins PC4/PC5 remain free. */
#define RS PB0
#define EN PB1
static void nibble(uint8_t value)
{
    PORTD = (uint8_t)((PORTD & 0x0F) | ((value & 0x0F) << 4));
    _delay_us(1);                   /* data and RS setup */
    PORTB |= (1 << EN);
    _delay_us(1);                   /* E high pulse */
    PORTB &= (uint8_t)~(1 << EN);
    _delay_us(1);                   /* data hold */
}
static void byte_write(uint8_t value, uint8_t data)
{
    if (data) PORTB |= (1 << RS);
    else PORTB &= (uint8_t)~(1 << RS);
    nibble(value >> 4);             /* high nibble first */
    nibble(value & 0x0F);
    if (!data && (value == 0x01 || value == 0x02)) _delay_ms(2);
    else _delay_us(100);            /* conservative classroom delays */
}
void lcd_command(uint8_t value) { byte_write(value, 0); }
void lcd_data(uint8_t value) { byte_write(value, 1); }
void lcd_init(void)
{
    PORTB &= (uint8_t)~((1 << RS) | (1 << EN) | (1 << PB2));
    DDRB &= (uint8_t)~(1 << PB2);    /* safe default for unknown clone */
    DDRB |= (1 << RS) | (1 << EN);
    PORTD &= 0x0F;
    DDRD |= 0xF0;
    _delay_ms(50);
    nibble(3); _delay_ms(5);
    nibble(3); _delay_us(150);
    nibble(3); _delay_us(150);
    nibble(2); _delay_us(150);       /* establish four-bit communication */
    lcd_command(0x28);              /* four-bit, two lines, 5x8 font */
    lcd_command(0x08);
    lcd_command(0x01);
    lcd_command(0x06);              /* increment address, no display shift */
    lcd_command(0x0C);
}
void lcd_cursor(uint8_t row, uint8_t column)
{
    if (row < 2 && column < 16)
        lcd_command((uint8_t)(0x80 | (row ? 0x40 : 0) | column));
}
void lcd_puts(const char *text)
{
    while (*text) lcd_data((uint8_t)*text++);
}
void lcd_line(uint8_t row, const char *text)
{
    uint8_t column = 0;
    if (row > 1) return;
    lcd_cursor(row, 0);
    while (*text && column < 16) { lcd_data((uint8_t)*text++); ++column; }
    while (column++ < 16) lcd_data(' '); /* erase old trailing characters */
}
void lcd_uint(uint16_t value)
{
    char digits[5]; uint8_t count = 0;
    do { digits[count++] = (char)('0' + value % 10); value /= 10; }
    while (value);
    while (count) lcd_data((uint8_t)digits[--count]);
}
void lcd_glyph(uint8_t slot, const uint8_t rows[8])
{
    if (slot > 7) return;
    lcd_command((uint8_t)(0x40 | (slot << 3)));
    for (uint8_t row = 0; row < 8; ++row) lcd_data(rows[row] & 0x1F);
    lcd_cursor(0, 0);               /* restore DDRAM after CGRAM writes */
}
