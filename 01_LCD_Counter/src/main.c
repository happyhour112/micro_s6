#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include "keys.h"
static const uint8_t heart[8] = {0,10,31,31,14,4,0,0};
static const uint8_t smile[8] = {0,10,10,0,17,14,0,0};
int main(void)
{
    uint16_t count = 0;
    lcd_init(); keys_init(); lcd_glyph(0, heart); lcd_glyph(1, smile);
    lcd_line(0, "AVR LCD DEMO"); lcd_cursor(0, 14); lcd_data(0); lcd_data(1);
    lcd_line(1, "COUNT: 0");
    for (;;) {
        key_event_t event = keys_poll();
        if (event.press == KEY_SELECT) {
            if (count < 9999) ++count;
            lcd_line(1, "COUNT: "); lcd_cursor(1, 7); lcd_uint(count);
        }
        _delay_ms(10); /* sample interval, approximate after LCD work */
    }
}
