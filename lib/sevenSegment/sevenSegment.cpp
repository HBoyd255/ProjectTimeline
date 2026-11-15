

#include "sevenSegment.h"

#include <Arduino.h>

#define CELL_A 0b01110000
#define CELL_B 0b10110000
#define CELL_C 0b11010000
#define CELL_D 0b11100000

#define SEG_A 0b10000000
#define SEG_B 0b01000000
#define SEG_C 0b00100000
#define SEG_D 0b00010000
#define SEG_E 0b00001000
#define SEG_F 0b00000100
#define SEG_G 0b00000010
#define SEG_H 0b00000001

#define CHAR_0 SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G
#define CHAR_1 SEG_C | SEG_F
#define CHAR_2 SEG_A | SEG_C | SEG_D | SEG_E | SEG_G
#define CHAR_3 SEG_A | SEG_C | SEG_D | SEG_F | SEG_G
#define CHAR_4 SEG_B | SEG_C | SEG_D | SEG_F
#define CHAR_5 SEG_A | SEG_B | SEG_D | SEG_F | SEG_G
#define CHAR_6 SEG_A | SEG_B | SEG_D | SEG_E | SEG_F | SEG_G
#define CHAR_7 SEG_A | SEG_C | SEG_F
#define CHAR_8 SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G
#define CHAR_9 SEG_A | SEG_B | SEG_C | SEG_D | SEG_F

#define CELL_LIST \
    (uint8_t[]) { CELL_A, CELL_B, CELL_C, CELL_D }

#define CHARACTER_LIST                                                  \
    (uint8_t[]) {                                                       \
        CHAR_0, CHAR_1, CHAR_2, CHAR_3, CHAR_4, CHAR_5, CHAR_6, CHAR_7, \
            CHAR_8, CHAR_9                                              \
    }

volatile uint8_t sevenSegValues[4] = {0, 0, 0, 0};

TaskHandle_t Task2;

void UseShiftReg(uint8_t byteA, uint8_t byteB) {
    uint16_t value = byteA << 8 | byteB;  // Merge the two bytes into one 16-bit

    digitalWrite(SEVEN_SEG_LATCH, 1);  // Standard 595 shift reg.
    for (int i = 0; i < 16; i++) {
        digitalWrite(SEVEN_SEG_DATA, value & 1);
        value >>= 1;
        digitalWrite(SEVEN_SEG_CLOCK, 1);
        digitalWrite(SEVEN_SEG_CLOCK, 0);
    }
    digitalWrite(SEVEN_SEG_LATCH, 0);
}

void multiplexSevenSegDisplayThread(void* pvParameters) {
    while (1) {
        for (uint8_t i = 0; i < 4; i++) {
            UseShiftReg(CELL_LIST[i], sevenSegValues[i]);
        }
    }
}

void setDigitsOnSevenSeg(uint8_t digit_a, uint8_t digit_b, uint8_t digit_c,
                         uint8_t digit_d) {
    // TODO add a check to ensure each value is 0-9.

    bool blinker = (millis() >> 9) & 1;

    sevenSegValues[0] = CHARACTER_LIST[digit_a];
    sevenSegValues[1] = CHARACTER_LIST[digit_b] | (SEG_H & blinker);
    sevenSegValues[2] = CHARACTER_LIST[digit_c];
    sevenSegValues[3] = CHARACTER_LIST[digit_d];
}

void beginSevenSegThread() {
    // https://randomnerdtutorials.com/esp32-dual-core-arduino-ide/
    xTaskCreatePinnedToCore(
        multiplexSevenSegDisplayThread,   /* Task function. */
        "multiplexSevenSegDisplayThread", /* name of task. */
        10000,                            /* Stack size of task */
        NULL,                             /* parameter of the task */
        1,                                /* priority of the task */
        &Task2, /* Task handle to keep track of created task */
        1);     /* pin task to core 1 */
}