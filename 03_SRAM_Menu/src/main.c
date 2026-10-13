#include <util/delay.h>
#include "lcd.h"
#include "keys.h"
static void render(uint8_t page, uint16_t setting, uint8_t automatic)
{
    if (page == 0) {
        lcd_line(0, "LIMIT SETTING"); lcd_line(1, "Value: ");
        lcd_cursor(1, 7); lcd_uint(setting);
    } else {
        lcd_line(0, "OPERATING MODE");
        lcd_line(1, automatic ? "AUTO" : "MANUAL");
    }
}
int main(void)
{
    uint16_t setting = 300; uint8_t page = 0, automatic = 1;
    lcd_init(); keys_init(); render(page, setting, automatic);
    for (;;) {
        key_event_t event = keys_poll(); uint8_t changed = 0;
        if (event.press == KEY_RIGHT || event.press == KEY_LEFT) {
            page ^= 1; changed = 1;
        }
        if (page == 0 && event.press == KEY_UP && setting < 1000) {
            setting += 10; changed = 1;
        }
        if (page == 0 && event.press == KEY_DOWN && setting > 100) {
            setting -= 10; changed = 1;
        }
        if (page == 1 && event.press == KEY_SELECT) {
            automatic ^= 1; changed = 1;
        }
        if (changed) render(page, setting, automatic);
        _delay_ms(10);
    }
}
