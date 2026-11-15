#include "neopixelWrapper.h"

#include <Arduino.h>
#include <NeoPixelBus.h>

NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> ledStrip(NEOPIXEL_LED_COUNT,
                                                      NEOPIXEL_DATA_PIN);

/* -- Python3
import math
# Create a sine wave ranging from 2 to 99 over the span of 255 frames.
for x in range(255):
    v = math.floor(
            (math.sin(((x * 2 * math.pi) / 255) + (1.5 * math.pi)) * 50) + 50
        )
        if v == 0:
            v = 2
        if v == 1:
            v = 2
        print(v, end=",")
*/
const uint8_t fade_animation[255] = {
    2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  4,  4,
    5,  5,  6,  7,  7,  8,  9,  9,  10, 11, 12, 13, 13, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 31, 32, 33, 34, 35, 36, 38, 39, 40,
    41, 42, 44, 45, 46, 47, 49, 50, 51, 52, 53, 55, 56, 57, 58, 60, 61, 62, 63,
    64, 66, 67, 68, 69, 70, 71, 72, 73, 74, 76, 77, 78, 79, 80, 81, 82, 82, 83,
    84, 85, 86, 87, 88, 88, 89, 90, 91, 91, 92, 93, 93, 94, 94, 95, 95, 96, 96,
    97, 97, 98, 98, 98, 98, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 98, 98, 98, 98, 97, 97, 96, 96, 95, 95, 94, 94, 93, 93, 92, 91,
    91, 90, 89, 88, 88, 87, 86, 85, 84, 83, 82, 82, 81, 80, 79, 78, 77, 76, 74,
    73, 72, 71, 70, 69, 68, 67, 66, 64, 63, 62, 61, 60, 58, 57, 56, 55, 53, 52,
    51, 50, 49, 47, 46, 45, 44, 42, 41, 40, 39, 38, 36, 35, 34, 33, 32, 31, 29,
    28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 13, 12, 11,
    10, 9,  9,  8,  7,  7,  6,  5,  5,  4,  4,  3,  3,  2,  2,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  2,  2};

void setup_neopixel() { ledStrip.Begin(); }

void set_pixel(uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
    ledStrip.SetPixelColor(index, RgbColor(r, g, b));
}

void blink_alternating(uint8_t r, uint8_t g, uint8_t b) {
    bool toggle = (millis() >> 8) & 1;  // Toggle each 0.25 ish secconds.

    if (toggle) {
        set_pixel(0, r, g, b);
        set_pixel(1, 0, 0, 0);
        set_pixel(2, r, g, b);
        set_pixel(3, 0, 0, 0);
        set_pixel(4, r, g, b);
        set_pixel(5, 0, 0, 0);
        set_pixel(6, r, g, b);
        set_pixel(7, 0, 0, 0);

    } else {
        set_pixel(0, 0, 0, 0);
        set_pixel(1, r, g, b);
        set_pixel(2, 0, 0, 0);
        set_pixel(3, r, g, b);
        set_pixel(4, 0, 0, 0);
        set_pixel(5, r, g, b);
        set_pixel(6, 0, 0, 0);
        set_pixel(7, r, g, b);
    }
    ledStrip.Show();
}

void blink(uint8_t r, uint8_t g, uint8_t b) {
    bool toggle = (millis() >> 8) & 1;  // Toggle each 0.25 ish secconds.

    if (toggle) {
        set_pixel(0, r, g, b);
        set_pixel(1, r, g, b);
        set_pixel(2, r, g, b);
        set_pixel(3, r, g, b);
        set_pixel(4, r, g, b);
        set_pixel(5, r, g, b);
        set_pixel(6, r, g, b);
        set_pixel(7, r, g, b);

    } else {
        set_pixel(0, 0, 0, 0);
        set_pixel(1, 0, 0, 0);
        set_pixel(2, 0, 0, 0);
        set_pixel(3, 0, 0, 0);
        set_pixel(4, 0, 0, 0);
        set_pixel(5, 0, 0, 0);
        set_pixel(6, 0, 0, 0);
        set_pixel(7, 0, 0, 0);
    }
    ledStrip.Show();
}

void set_bar(uint8_t level) {
    uint8_t animation_frame =
        (millis() & 4095) >> 4;  // Toggle each 0.25 ish secconds.

    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;

    uint8_t animation_r = 0;
    uint8_t animation_g = 0;
    uint8_t animation_b = 0;

    if (level > 4) {
        g = 25;
        animation_g = fade_animation[animation_frame];
    } else if (level > 2) {
        r = 25;
        g = 12;
        animation_r = fade_animation[animation_frame];
        animation_g = fade_animation[animation_frame] >> 1;
    } else {
        r = 25;
        animation_r = fade_animation[animation_frame];
    }

    for (uint8_t i = 0; i < 8; i++) {
        if (i == (level - 1)) {
            set_pixel(i, animation_r, animation_g, animation_b);

        } else if (i < (level - 1)) {
            set_pixel(i, r, g, b);
        } else {
            set_pixel(i, 0, 0, 0);
        }
    }

    ledStrip.Show();
}