#ifndef SEVEN_SEGMENT_H
#define SEVEN_SEGMENT_H

#include <Arduino.h>

// TODO Make this configurable
#define SEVEN_SEG_DATA 47
#define SEVEN_SEG_LATCH 33
#define SEVEN_SEG_CLOCK 34

// Set the 4 digits of the Seven Seg display, values should be between 0 and 9.
void setDigitsOnSevenSeg(uint8_t digit_a, uint8_t digit_b, uint8_t digit_c,
                         uint8_t digit_d);

// Required for multiplexing, add to setup().
void beginSevenSegThread();

#endif  // SEVEN_SEGMENT_H
