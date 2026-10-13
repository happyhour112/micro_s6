#include <avr/io.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <util/delay.h>
#include "lcd.h"
#include "keys.h"

/* EEMEM allocates EEPROM storage; it is not a normal SRAM variable.
   No initializer: a fresh EEPROM byte normally reads 0xFF. */
static uint8_t EEMEM saved_value;
static void screen(uint8_t value, uint8_t saved)
{
    lcd_line(0, "Value:");
    lcd_cursor(0, 7);
    lcd_uint(value);
    lcd_line(1, "Saved:");
    lcd_cursor(1, 7);
    lcd_uint(saved);
}
int main(void)
{
    lcd_init();
    keys_init();
    uint8_t saved = eeprom_read_byte(&saved_value);
    if (saved > 100)
        saved = 10; /* Invalid/erased byte: safe default. */
    uint8_t value = saved;
    screen(value, saved);

    for (;;)
    {
        _delay_ms(10);
        key_event_t event = keys_poll();
        if (event.press == KEY_UP && value < 100)
            ++value;
        else if (event.press == KEY_DOWN && value > 0)
            --value;
        else if (event.press == KEY_SELECT)
        {
            /* Update compares first; an unchanged byte is not rewritten. */
            eeprom_update_byte(&saved_value, value);
            eeprom_busy_wait();
            saved = value;
        }
        if (event.press != KEY_NONE)
            screen(value, saved);
    }
}
