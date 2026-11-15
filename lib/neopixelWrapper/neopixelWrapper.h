#ifndef NEOPIXEL_WRAPPER_H
#define NEOPIXEL_WRAPPER_H
#include <Arduino.h>

#define NEOPIXEL_LED_COUNT 8
#define NEOPIXEL_DATA_PIN 48

void setup_neopixel();

void blink_alternating(uint8_t r, uint8_t g, uint8_t b);
void blink(uint8_t r, uint8_t g, uint8_t b);
void set_bar(uint8_t level);

#endif  // NEOPIXEL_WRAPPER_H