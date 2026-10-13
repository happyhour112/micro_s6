#include "keys.h"
#include <avr/io.h>
/* Starter thresholds match common older DFR shields. Measure your board
   with project 02 before class and replace these cutoffs if required.
   V1.1 often uses 50,250,450,650,850 instead. No simultaneous-key promise. */
#define RIGHT_LIMIT 50
#define UP_LIMIT 195
#define DOWN_LIMIT 380
#define LEFT_LIMIT 555
#define SELECT_LIMIT 790
static key_t candidate, stable;
static uint8_t samples, armed = 1, long_sent;
static uint16_t held;
void keys_init(void)
{
    DDRC &= (uint8_t)~(1 << PC0);
    PORTC &= (uint8_t)~(1 << PC0);   /* ladder already provides a pull-up */
    ADMUX = (1 << REFS0);           /* AVCC reference, ADC0, right adjusted */
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    DIDR0 |= (1 << ADC0D);          /* disable digital input buffer */
    candidate = stable = KEY_NONE; samples = held = long_sent = 0; armed = 1;
    (void)adc0_read();              /* discard first conversion after enable */
}
uint16_t adc0_read(void)
{
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC)) { } /* ADC peripheral clears ADSC */
    return ADC;                    /* avr-gcc reads low then high byte */
}
key_t keys_decode(uint16_t raw)
{
    if (raw < RIGHT_LIMIT) return KEY_RIGHT;
    if (raw < UP_LIMIT) return KEY_UP;
    if (raw < DOWN_LIMIT) return KEY_DOWN;
    if (raw < LEFT_LIMIT) return KEY_LEFT;
    if (raw < SELECT_LIMIT) return KEY_SELECT;
    return KEY_NONE;
}
const char *key_name(key_t key)
{
    switch (key) {
    case KEY_RIGHT: return "RIGHT"; case KEY_UP: return "UP";
    case KEY_DOWN: return "DOWN"; case KEY_LEFT: return "LEFT";
    case KEY_SELECT: return "SELECT"; default: return "NONE";
    }
}
key_event_t keys_poll(void)
{
    key_event_t event = { KEY_NONE, KEY_NONE, KEY_NONE };
    key_t raw = keys_decode(adc0_read());
    if (raw != candidate) { candidate = raw; samples = 1; }
    else if (samples < 3) ++samples;
    if (samples >= 3 && candidate != stable) {
        if (candidate == KEY_NONE) {
            if (stable != KEY_NONE && !long_sent) event.release = stable;
            stable = KEY_NONE; held = 0; long_sent = 0; armed = 1;
        } else if (armed) {
            stable = candidate; armed = 0; held = 0; long_sent = 0;
            event.press = stable;
        }
        /* A different key while held is ignored until all keys release.
           This reduces false events from analog transitions and chords. */
    }
    if (stable != KEY_NONE && candidate == stable && !long_sent) {
        if (++held >= 80) { event.long_press = stable; long_sent = 1; }
    }
    return event;
}
