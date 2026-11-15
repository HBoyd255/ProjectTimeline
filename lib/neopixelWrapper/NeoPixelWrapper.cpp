#include "neopixelWrapper.h"

#include <Arduino.h>
#include <NeoPixelBus.h>

NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> ledStrip(NEOPIXEL_LED_COUNT,
                                                      NEOPIXEL_DATA_PIN);

/* -- Python3
import math
# Create a sine wave ranging from 2 to 99 over the span of 256 frames.
for x in range(256):
    v = math.floor(
            (math.sin(((x * 2 * math.pi) / 256) + (1.5 * math.pi)) * 50) + 50
        )
    if v == 0:
        v = 2
    if v == 1:
        v = 2
    print(v, end=",")
*/
const uint8_t fadeAnimation[256] = {
    2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,   3,  3,  4,  4,
    5,  5,  6,  7,  7,  8,  9,  9,  10, 11, 12, 12, 13, 14, 15,  16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 32, 33, 34, 35,  36, 37, 39, 40,
    41, 42, 43, 45, 46, 47, 48, 49, 51, 52, 53, 54, 56, 57, 58,  59, 60, 62, 63,
    64, 65, 66, 67, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,  80, 81, 82, 83,
    84, 85, 86, 87, 87, 88, 89, 90, 90, 91, 92, 92, 93, 94, 94,  95, 95, 96, 96,
    97, 97, 97, 98, 98, 98, 99, 99, 99, 99, 99, 99, 99, 99, 100, 99, 99, 99, 99,
    99, 99, 99, 99, 98, 98, 98, 97, 97, 97, 96, 96, 95, 95, 94,  94, 93, 92, 92,
    91, 90, 90, 89, 88, 87, 87, 86, 85, 84, 83, 82, 81, 80, 79,  78, 77, 76, 75,
    74, 73, 72, 71, 70, 69, 67, 66, 65, 64, 63, 62, 60, 59, 58,  57, 56, 54, 53,
    52, 51, 50, 48, 47, 46, 45, 43, 42, 41, 40, 39, 37, 36, 35,  34, 33, 32, 30,
    29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15,  14, 13, 12, 12,
    11, 10, 9,  9,  8,  7,  7,  6,  5,  5,  4,  4,  3,  3,  2,   2,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  2,  2,  2};

void setupNeopixel() { ledStrip.Begin(); }

void setPixel(uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
    ledStrip.SetPixelColor(index, RgbColor(r, g, b));
}

void blinkAlternating(uint8_t r, uint8_t g, uint8_t b) {
    bool toggle = (millis() >> 8) & 1;  // Toggle each 0.25 ish secconds.

    if (toggle) {
        setPixel(0, r, g, b);
        setPixel(1, 0, 0, 0);
        setPixel(2, r, g, b);
        setPixel(3, 0, 0, 0);
        setPixel(4, r, g, b);
        setPixel(5, 0, 0, 0);
        setPixel(6, r, g, b);
        setPixel(7, 0, 0, 0);

    } else {
        setPixel(0, 0, 0, 0);
        setPixel(1, r, g, b);
        setPixel(2, 0, 0, 0);
        setPixel(3, r, g, b);
        setPixel(4, 0, 0, 0);
        setPixel(5, r, g, b);
        setPixel(6, 0, 0, 0);
        setPixel(7, r, g, b);
    }
    ledStrip.Show();
}

void blink(uint8_t r, uint8_t g, uint8_t b) {
    bool toggle = (millis() >> 8) & 1;  // Toggle each 0.25 ish secconds.

    if (toggle) {
        setPixel(0, r, g, b);
        setPixel(1, r, g, b);
        setPixel(2, r, g, b);
        setPixel(3, r, g, b);
        setPixel(4, r, g, b);
        setPixel(5, r, g, b);
        setPixel(6, r, g, b);
        setPixel(7, r, g, b);

    } else {
        setPixel(0, 0, 0, 0);
        setPixel(1, 0, 0, 0);
        setPixel(2, 0, 0, 0);
        setPixel(3, 0, 0, 0);
        setPixel(4, 0, 0, 0);
        setPixel(5, 0, 0, 0);
        setPixel(6, 0, 0, 0);
        setPixel(7, 0, 0, 0);
    }
    ledStrip.Show();
}

void setBar(uint8_t level) {
    uint8_t animationFrame =
        (millis() & 4095) >> 4;  // Toggle each 0.25 ish secconds.

    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;

    uint8_t animationR = 0;
    uint8_t animationG = 0;
    uint8_t animationB = 0;

    if (level > 4) {
        g = 25;
        animationG = fadeAnimation[animationFrame];
    } else if (level > 2) {
        r = 25;
        g = 12;
        animationR = fadeAnimation[animationFrame];
        animationG = fadeAnimation[animationFrame] >> 1;
    } else {
        r = 25;
        animationR = fadeAnimation[animationFrame];
    }
    Serial.print("F:");
    Serial.print(animationFrame);
    Serial.print("V:");
    Serial.println(animationR);

    for (uint8_t i = 0; i < 8; i++) {
        if (i == (level - 1)) {
            setPixel(i, animationR, animationG, animationB);

        } else if (i < (level - 1)) {
            setPixel(i, r, g, b);
        } else {
            setPixel(i, 0, 0, 0);
        }
    }

    ledStrip.Show();
}