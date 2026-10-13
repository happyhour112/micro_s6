#ifndef KEYS_H
#define KEYS_H
#include <stdint.h>
typedef enum { KEY_NONE, KEY_RIGHT, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_SELECT } key_t;
typedef struct { key_t press, release, long_press; } key_event_t;
void keys_init(void);
uint16_t adc0_read(void);
key_t keys_decode(uint16_t raw);
const char *key_name(key_t key);
key_event_t keys_poll(void); /* call about once per 10 ms loop */
#endif
